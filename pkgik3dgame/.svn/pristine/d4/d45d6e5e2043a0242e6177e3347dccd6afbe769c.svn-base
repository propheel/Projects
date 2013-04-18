#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
#endregion

namespace WarGame
{
    /// <summary>
    /// This is a game skybox
    /// </summary>
    public class Skybox : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// Model used to draw the skybox
        /// </summary>
        Model skybox;

        /// <summary>
        /// View matrix of the skybox
        /// </summary>
        Matrix view;

        /// <summary>
        /// Constructor of the skybox
        /// </summary>
        /// <param name="game"></param>
        public Skybox(WarGame game)
            : base(game)
        {
            skybox = game.skyboxModel;
            DrawOrder = (int)WarGame.DrawingOrder.GameObject;
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
            view = ((WarGame)base.Game).Camera.View;
            view.M41 = 0;
            view.M42 = 0;
            view.M43 = 0;

            base.Update(gameTime);
        }

        /// <summary>
        /// Enables self-drawing of the skybox.
        /// </summary>
        /// <param name="gameTime">Snapshot of time.</param>
        public override void Draw(GameTime gameTime)
        {
            foreach (ModelMesh mesh in skybox.Meshes)
            {
                foreach (BasicEffect e in mesh.Effects)
                {
                    e.EnableDefaultLighting();
                    e.World = Matrix.CreateScale(700f);
                    e.View = view;
                    e.Projection = ((WarGame)base.Game).Camera.Projection;
                    e.TextureEnabled = true;
                }
                mesh.Draw();
            }

            base.Draw(gameTime);
        }
    }
}
