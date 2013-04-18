#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.GamerServices;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
using Microsoft.Xna.Framework.Media;
using WarGame.GameObjects;
#endregion

namespace WarGame
{
    /// <summary>
    /// Main Scene of the game.
    /// </summary>
    public class MainScene : Scene
    {
        #region Fields
        /// <summary>
        /// List of all game objects in the Scene.
        /// </summary>
        List<GameObject> sceneObjects = new List<GameObject>();

        /// <summary>
        /// List of all fighters in the Scene.
        /// </summary>
        List<Fighter> fighters = new List<Fighter>();

        /// <summary>
        /// List of all alien crafts in the Scene.
        /// </summary>
        List<AlienCraft> alienCrafts = new List<AlienCraft>();

        /// <summary>
        /// List of the currently active fighters.
        /// </summary>
        List<Fighter> activeFighters = new List<Fighter>();

        /// <summary>
        /// Ground surface.
        /// </summary>
        GroundElement groundElement;

        /// <summary>
        /// Command center of the human player.
        /// </summary>
        public Building HumanBase { get; set; }

        /// <summary>
        /// Command center of the computer player
        /// </summary>
        public AlienBuilding AlienBase { get; set; }

        /// <summary>
        /// Rectangle used to select fighters.
        /// </summary>
        public SelectionRectangle SelectionRectangle { get; private set; }

        /// <summary>
        /// Position in the reference to the y-axis at which the ground is placed.
        /// </summary>
        float groundYAxisPosition = 2.0f;

        /// <summary>
        /// Maximal number of fighters in the Scene.
        /// </summary>
        int maxNumberOfFighters = 10;

        /// <summary>
        /// Number of alien crafts in the Scene.
        /// </summary>
        int numberOfAliens = 10;

        /// <summary>
        /// Determines if the status of the debug mode.
        /// </summary>
        public bool IsInDebugMode { get; set; }

        /// <summary>
        /// Position of the alien base.
        /// </summary>
        Vector3 alienBasePosition;

        /// <summary>
        /// Post processing render target.
        /// </summary>
        RenderTarget2D postTarget;

        /// <summary>
        /// Post processing screen texture.
        /// </summary>
        Texture2D postTexture;

        /// <summary>
        /// Render target of the alien base's environment.
        /// </summary>
        RenderTargetCube alienBaseEnvironmentRenTarget;

        /// <summary>
        /// Texture cube representing the alien base environment.
        /// </summary>
        TextureCube alienBaseEnvironmentTextureCube;
        #endregion

        #region Getters and Setters
        public List<GameObject> SceneObjects
        {
            get { return sceneObjects; }
        }
        public List<Fighter> Fighters
        {
            get { return fighters; }
        }
        public List<Fighter> ActiveFighters
        {
            get { return activeFighters; }
        }
        public List<AlienCraft> AlienCrafts
        {
            get { return alienCrafts; }
        }
        public GroundElement GroundElement
        {
            get { return groundElement; }
        }
        public int MaxNumberOfFighters
        {
            get { return maxNumberOfFighters; }
        }
        public float GroundYAxisPosition
        {
            get { return groundYAxisPosition; }
        }
        public TextureCube AlienBaseEnvironmentTextureCube
        {
            get { return alienBaseEnvironmentTextureCube; }
        }
        #endregion

        public MainScene(WarGame game)
            : base(game)
        {
            // creating post render target with 24-bit depth
            PresentationParameters pp = game.Graphics.GraphicsDevice.PresentationParameters;
            postTarget = new RenderTarget2D(game.Graphics.GraphicsDevice, pp.BackBufferWidth, pp.BackBufferHeight, false, game.Graphics.GraphicsDevice.DisplayMode.Format, DepthFormat.Depth24);

            // creating objects used by environment shader
            alienBaseEnvironmentRenTarget = new RenderTargetCube(game.Graphics.GraphicsDevice, 256, false, game.Graphics.GraphicsDevice.DisplayMode.Format, DepthFormat.Depth24);
            alienBaseEnvironmentTextureCube = new TextureCube(game.Graphics.GraphicsDevice, 256, false, game.Graphics.GraphicsDevice.DisplayMode.Format);

            // creating selection rectangle and adding it to the Scene
            SelectionRectangle = new SelectionRectangle(game, this);
            SceneComponents.Add(SelectionRectangle);

            // the game is initially not in the debug mode
            IsInDebugMode = false;

            // adding ground element to the Scene
            groundElement = new GroundElement(game, new Vector3(0, groundYAxisPosition, 0), this);
            SceneComponents.Add(groundElement);
            sceneObjects.Add(groundElement);

            // setting the position of the alien base
            alienBasePosition = new Vector3(170, groundYAxisPosition, 170);

            //adding alien's base to the Scene
            AlienBase = new AlienBuilding(game, alienBasePosition, this);
            SceneComponents.Add(AlienBase);
            sceneObjects.Add(AlienBase);

            // adding alien crafts to the Scene
            for (int i = 0; i < numberOfAliens; i++)
            {
                AlienCraft a;

                // placing new craft in the first random and free place
                do
                {
                    Vector3 randomPosition = new Vector3(AlienBase.Position.X + (float)WarGame.RandomGenerator.NextDouble() * 50.0f - 25.0f,
                                                         GroundYAxisPosition + 1.0f,
                                                         AlienBase.Position.Z + (float)WarGame.RandomGenerator.NextDouble() * 50.0f - 25.0f);

                    a = new AlienCraft(game, this, randomPosition);

                }
                while (a.CheckForCollisions() != null);

                SceneComponents.Add(a);
                sceneObjects.Add(a);
                alienCrafts.Add(a);
            }

            // adding skybox to the Scene
            SceneComponents.Add(new Skybox(game));
        }

        public override void Draw(GameTime gameTime)
        {
            // doing the pre rendering
            PreRender();

            // drawing Scene to post buffer
            base.Draw(gameTime);

            // doing the post rendering
            PostRender();
        }

        /// <summary>
        /// Pre rendering
        /// </summary>
        public void PreRender()
        {
            // setting the render target to post buffer
            (base.Game as WarGame).Graphics.GraphicsDevice.SetRenderTarget(postTarget);
        }

        /// <summary>
        /// Post rendering
        /// </summary>
        public void PostRender()
        {
            // setting the render target to screen
            (base.Game as WarGame).Graphics.GraphicsDevice.SetRenderTarget(null);

            // converting post buffer to texture
            postTexture = postTarget;

            // drawing the post buffer texture using the selected mode
            (base.Game as WarGame).Sprite.Begin(0, BlendState.AlphaBlend, SamplerState.PointClamp, DepthStencilState.Default, null, ((WarGame)base.Game).PostEffect);

            foreach (EffectPass p in ((WarGame)base.Game).PostEffect.CurrentTechnique.Passes)
            {
                p.Apply();
                (base.Game as WarGame).Sprite.Draw(postTexture, Vector2.Zero, Color.White);
            }

            (base.Game as WarGame).Sprite.End();
        }

        /// <summary>
        /// Disactivates all the figters in the Scene
        /// </summary>
        public void DisactivateAllFighters()
        {
            foreach (Fighter f in activeFighters)
                f.Disactivate();

            activeFighters.Clear();
        }

        /// <summary>
        /// Activates all fighters in the Scene
        /// </summary>
        public void ActivateAllFighters()
        {
            activeFighters.Clear();

            foreach (Fighter f in fighters)
            {
                f.Activate();
                activeFighters.Add(f);
            }
        }
    }
}