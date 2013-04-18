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
#endregion

namespace WarGame
{
    /// <summary>
    /// Base class for all the scenes.
    /// </summary>
    public abstract class Scene : Microsoft.Xna.Framework.DrawableGameComponent
    {
        #region Fields
        /// <summary>
        /// List of game components appearing in the Scene.
        /// </summary>
        public List<GameComponent> SceneComponents { get; set; }

        /// <summary>
        /// Scene where to after after the escape is pressed
        /// </summary>
        public Scene baseScene { get; set; }
        #endregion

        /// <summary>
        /// Constructor of the Scene
        /// </summary>
        /// <param name="game">Game in which the Scene is used</param>
        public Scene(WarGame game)
            : base(game)
        {
            SceneComponents = new List<GameComponent>();
            Show();
        }

        /// <summary>
        /// Allows the game component to perform any initialization it needs to before starting
        /// to run.  This is where it can query for any required services and load content.
        /// </summary>
        public override void Initialize()
        {
            // TODO: Add your initialization code here

            base.Initialize();
        }

        /// <summary>
        /// Allows the game component to update itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        public override void Update(GameTime gameTime)
        {
            List<GameComponent> SceneComponentsCopy = new List<GameComponent>(SceneComponents);

            foreach (GameComponent g in SceneComponentsCopy)
                if (g.Enabled) g.Update(gameTime);

            base.Update(gameTime);
        }

        /// <summary>
        /// Allows the Scene to draw itself.
        /// </summary>
        /// <param name="gameTime">Snapshot of time.</param>
        public override void Draw(GameTime gameTime)
        {
            List<GameComponent> SceneComponentsCopy = new List<GameComponent>(SceneComponents);
           
            // drawing 3D stuff
            foreach (GameComponent g in SceneComponentsCopy)
            {
                if (g is DrawableGameComponent && 
                    (g as DrawableGameComponent).DrawOrder == (int)WarGame.DrawingOrder.GameObject)
                {
                    if ((g as DrawableGameComponent).Visible)
                        (g as DrawableGameComponent).Draw(gameTime);
                }
            }

            // drawing sprites
            foreach (GameComponent g in SceneComponentsCopy)
            {
                if (g is DrawableGameComponent &&
                    (g as DrawableGameComponent).DrawOrder == (int)WarGame.DrawingOrder.Sprite)
                {
                    if ((g as DrawableGameComponent).Visible)
                        (g as DrawableGameComponent).Draw(gameTime);
                }
            }

            base.Draw(gameTime);
        }

        /// <summary>
        /// Shows the Scene
        /// </summary>
        public virtual void Show()
        {
            Enabled = Visible = true;
        }

        /// <summary>
        /// Hides the Scene
        /// </summary>
        public virtual void Hide()
        {
            Enabled = Visible = false;
        }

        /// <summary>
        /// Toggles the Scene pause
        /// </summary>
        public virtual void TogglePause()
        {
            Enabled = !Enabled;
        }

        /// <summary>
        /// Reaction for pressing the escape key by the user
        /// </summary>
        public void ExecuteEscape()
        {
            if (baseScene != null)
            {
                Hide();
                baseScene.Show();
                ((WarGame)base.Game).CurrentScene = baseScene;
            }
        }
    }
}
