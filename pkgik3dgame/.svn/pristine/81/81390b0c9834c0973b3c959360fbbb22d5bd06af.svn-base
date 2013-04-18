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
    /// This is a class representing the selection rectangle
    /// </summary>
    public class SelectionRectangle : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// List of vertices used to draw the rectangle
        /// </summary>
        List<VertexPositionColor> vertex = new List<VertexPositionColor>();

        /// <summary>
        /// Array of indices used to draw the rectangle
        /// </summary>
        short[] index = { 0, 1, 2, 3, 0 };

        /// <summary>
        /// Color of the rectangle
        /// </summary>
        Color color = Color.Red;

        /// <summary>
        /// Scene in which the selection rectangle appears.
        /// </summary>
        MainScene scene;

        /// <summary>
        /// Constructor of the selection rectangle
        /// </summary>
        /// <param name="game">Game in which the selection rectangle is set</param>
        public SelectionRectangle(WarGame game, MainScene _scene)
            : base(game)
        {
            scene = _scene;
            Hide();

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
            vertex.Clear();

            Vector3[] corners = { new Vector3(((WarGame)base.Game).Input.MouseClickPosition.X, scene.GroundYAxisPosition + 0.2f, ((WarGame)base.Game).Input.MouseClickPosition.Z),
                                  new Vector3(((WarGame)base.Game).Input.MouseClickPosition.X, scene.GroundYAxisPosition + 0.2f, ((WarGame)base.Game).Input.MouseReleasePosition.Z),
                                  new Vector3(((WarGame)base.Game).Input.MouseReleasePosition.X, scene.GroundYAxisPosition + 0.2f, ((WarGame)base.Game).Input.MouseReleasePosition.Z),
                                  new Vector3(((WarGame)base.Game).Input.MouseReleasePosition.X, scene.GroundYAxisPosition + 0.2f, ((WarGame)base.Game).Input.MouseClickPosition.Z)};

            for (int i = 0; i < corners.Length; i++)
                vertex.Add(new VertexPositionColor(corners[i], color));

            base.Update(gameTime);
        }

        /// <summary>
        /// Enables selection rectangle self-drawing
        /// </summary>
        /// <param name="gameTime"></param>
        public override void Draw(GameTime gameTime)
        {
            if (vertex.Count > 0)
            {
                foreach (EffectPass p in ((WarGame)base.Game).Effect.CurrentTechnique.Passes)
                {
                    p.Apply();
                    ((WarGame)base.Game).Graphics.GraphicsDevice.DrawUserIndexedPrimitives<VertexPositionColor>(PrimitiveType.LineStrip, vertex.ToArray(), 0, vertex.Count, index, 0, index.Length - 1);
                }
            }

            base.Draw(gameTime);
        }

        /// <summary>
        /// Shows selection rectangle
        /// </summary>
        public void Show()
        {
            Enabled = Visible = true;
        }

        // Hides selection rectangle
        public void Hide()
        {
            Enabled = Visible = false;
        }
    }
}
