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
using WarGame.Controllers;
using WarGame.GameObjects;
#endregion

namespace WarGame
{
    using SBNames = SteeringBehaviors.SteeringBehaviorsNames;

    /// <summary>
    /// This is the main type for your game.
    /// </summary>
    public class WarGame : Microsoft.Xna.Framework.Game
    {
        #region Drawing order
        /// <summary>
        /// Enum value used to determine game components' order of drawing.
        /// </summary>
        public enum DrawingOrder { GameObject, Sprite }
        #endregion

        #region Fields
        /// <summary>
        /// Global generator of the random numbers
        /// </summary>
        public static Random RandomGenerator = new Random();

        /// <summary>
        /// Global graphics device manager
        /// </summary>
        public GraphicsDeviceManager Graphics { get; private set; }

        /// <summary>
        /// Global sprite batch
        /// </summary>
        public SpriteBatch Sprite { get; private set; }

        /// <summary>
        /// Global basic effect
        /// </summary>
        public BasicEffect Effect { get; set; }

        /// <summary>
        /// Global post processing effect
        /// </summary>
        public Effect PostEffect { get; set; }

        /// <summary>
        /// Global input controller
        /// </summary>
        public InputController Input { get; private set; }

        /// <summary>
        /// Global camera controller
        /// </summary>
        public CameraController Camera { get; private set; }

        /// <summary>
        /// Sound
        /// </summary>
        public SoundController Sound { get; private set; }

        /// <summary>
        /// Models used in the game
        /// </summary>
        public Model spaceShipModel, groundElementModel, alienCraftModel, castleModel, skyboxModel, alienBaseModel;

        /// <summary>
        /// Textures used in the game
        /// </summary>
        public Texture2D moonTexture, menuBackgroundTexture;

        /// <summary>
        /// Fonts used in the game
        /// </summary>
        public SpriteFont menuFont;

        /// <summary>
        /// Main Scene in the game.
        /// </summary>
        public MainScene Scene { get; private set; }

        /// <summary>
        /// Menu of the game.
        /// </summary>
        public MainMenu Menu { get; private set; }

        /// <summary>
        /// The Scene which is currently shown.
        /// </summary>
        public Scene CurrentScene { get; set; }

        /// <summary>
        /// Index of current post rendering technique.
        /// </summary>
        int currentTechnique;

        #endregion

        #region Getters and Setters

        /// <summary>
        /// Post effect setter with a simple validation.
        /// </summary>
        public int CurrentTechnique
        {
            get { return currentTechnique; }

            set
            {
                currentTechnique = value;

                if (currentTechnique == 4)
                    currentTechnique = 0;

                PostEffect.CurrentTechnique = PostEffect.Techniques[currentTechnique];
            }
        }

        #endregion

        /// <summary>
        /// Constructor of the game
        /// </summary>
        public WarGame()
        {
            // creating graphics and content managers
            Graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";

            // creating input controller and adding it to the game
            Input = new InputController(this);
            Components.Add(Input);

            // creating camera controller and adding it to the game 
            Camera = new CameraController(this);
            Components.Add(Camera);

            // creating sound controller and adding it to the game
            Sound = new SoundController(this);
            Components.Add(Sound);
        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            Effect = new BasicEffect(GraphicsDevice);

            // setting the window options
            Window.AllowUserResizing = false;
            Window.Title = "Moon Wars";
            IsMouseVisible = true;
            Graphics.ApplyChanges();

            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            Sprite = new SpriteBatch(GraphicsDevice);

            #region Loading models
            spaceShipModel = Content.Load<Model>("Models/spaceship");
            groundElementModel = Content.Load<Model>("Models/cube");
            alienCraftModel = Content.Load<Model>("Models/StarCruiser");
            castleModel = Content.Load<Model>("Models/castle");
            skyboxModel = Content.Load<Model>("Models/skybox/skybox");
            alienBaseModel = Content.Load<Model>("Models/alienBase");
            #endregion

            #region Loading textures
            moonTexture = Content.Load<Texture2D>("Textures/moon");
            menuBackgroundTexture = Content.Load<Texture2D>("Textures/FullMoon");
            #endregion

            #region Loading fonts
            menuFont = Content.Load<SpriteFont>("Fonts/WarGameMenuFont");
            #endregion

            #region Loading shaders
            PostEffect = Content.Load<Effect>("Shaders/PostProcessing");
            #endregion

            // applying the moon texture to the ground element
            VertexPositionNormalTexture[] vertex = new VertexPositionNormalTexture[groundElementModel.Meshes[0].MeshParts[0].NumVertices];

            groundElementModel.Meshes[0].MeshParts[0].VertexBuffer.GetData<VertexPositionNormalTexture>(vertex);

            vertex[0].TextureCoordinate = new Vector2(0, 0);
            vertex[1].TextureCoordinate = new Vector2(0, 1);
            vertex[2].TextureCoordinate = new Vector2(1, 0);
            vertex[3].TextureCoordinate = new Vector2(1, 1);

            groundElementModel.Meshes[0].MeshParts[0].VertexBuffer.SetData<VertexPositionNormalTexture>(vertex);

            // creating the main Menu and setting it as a current Scene
            Menu = new MainMenu(this);
            CurrentScene = Menu;
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            // checking if the game is over
            if (CurrentScene is MainScene)
            {
                if (EndGameCondition())
                {
                    CurrentScene.ExecuteEscape();
                    CurrentScene.baseScene = null;
                }
            }

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            GraphicsDevice.Clear(Color.CornflowerBlue);

            base.Draw(gameTime);
        }

        public void NewGame()
        {
            // if there is an old camera controller than destroy it
            if (Camera != null)
            {
                Components.Remove(Camera);
                Camera.Dispose();
            }

            // creating new camera controller and adding it to the game 
            Camera = new CameraController(this);
            Components.Add(Camera);

            // if there is an old main Scene destroy it
            if (Scene != null)
            {
                Components.Remove(Scene);
                Scene.Dispose();
            }

            // creating new main Scene and adding it to the game
            Scene = new MainScene(this);
            Components.Add(Scene);

            // setting the base scenes for the Menu and main Scene
            Menu.baseScene = Scene;
            Scene.baseScene = Menu;

            // displaying the main Scene and hiding the Menu
            Scene.Show();
            CurrentScene = Scene;
            Menu.Hide();

            // playing the main music theme
            if (Sound.ThemeSoundInstance.State == SoundState.Paused)
                Sound.ThemeSoundInstance.Stop();
            Sound.ThemeSoundInstance.Play();

            // setting initial rendering technique
            currentTechnique = 0;
            PostEffect.CurrentTechnique = PostEffect.Techniques[currentTechnique];
        }

        /// <summary>
        /// Sets the game resolution with the given parameters
        /// </summary>
        /// <param name="width">Width of the screen</param>
        /// <param name="height">Height of the screen</param>
        public void SetResolution(int width, int height)
        {
            Graphics.PreferredBackBufferWidth = width;
            Graphics.PreferredBackBufferHeight = height;
            Graphics.ApplyChanges();

            // updating the Menu items
            UpdateMenuItemsPositions();
        }

        /// <summary>
        /// Updates Menu items' positions
        /// </summary>
        public void UpdateMenuItemsPositions()
        {
            foreach (GameComponent c in Components)
            {
                if (c is MenuScene)
                {
                    foreach (MenuItem i in (c as MenuScene).SceneComponents)
                    {
                        i.Position = new Vector2(Window.ClientBounds.Width / 2 - (int)menuFont.MeasureString(i.Label).X / 2, i.Position.Y);
                    }
                }
            }
        }

        /// <summary>
        /// Check the end game condition
        /// </summary>
        /// <returns>true - end of the game, false - the game is still not finished</returns>
        public bool EndGameCondition()
        {
            foreach (AlienCraft c in ((MainScene)CurrentScene).AlienCrafts)
                if (c.Visible)
                    return false;

            foreach (Fighter f in ((MainScene)CurrentScene).ActiveFighters)
            {
                if (f.BoundingBox.Intersects(((MainScene)CurrentScene).AlienBase.BoundingBox))
                    return true;
            }

            return false;
        }
    }
}
