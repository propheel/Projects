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
    /// This is a class representing a base rectangle
    /// </summary>
    public class BaseRectangle : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// Game object which owns this base rectangle
        /// </summary>
        GameObject owner;

        /// <summary>
        /// Color of the base rectangle
        /// </summary>
        Color color = Color.Red;

        /// <summary>
        /// List of vertices used to draw the base rectangle
        /// </summary>
        List<VertexPositionColor> vertex = new List<VertexPositionColor>();

        /// <summary>
        /// Scene in which the base rectangle appears
        /// </summary>
        MainScene scene;

        /// <summary>
        /// Array of indices used to draw the base rectangle
        /// </summary>
        short[] index = { 0, 1, 2, 3, 0 };

        /// <summary>
        /// Constructor of base rectangle
        /// </summary>
        /// <param name="_owner">Game object which owns the base rectangle</param>
        /// <param name="_game">Game which the base rectangle is set in</param>
        public BaseRectangle(GameObject _owner, WarGame _game, MainScene _scene)
            : base(_game)
        {
            owner = _owner;
            scene = _scene;
            DrawOrder = (int)WarGame.DrawingOrder.GameObject;
            Hide();
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
            
            BoundingBox box = owner.BoundingBox;
            Vector3[] corners = {   new Vector3(box.Min.X, scene.GroundYAxisPosition+0.2f, box.Min.Z),
                                    new Vector3(box.Min.X, scene.GroundYAxisPosition+0.2f, box.Max.Z),
                                    new Vector3(box.Max.X, scene.GroundYAxisPosition+0.2f, box.Max.Z),
                                    new Vector3(box.Max.X, scene.GroundYAxisPosition+0.2f, box.Min.Z)};

            vertex = new List<VertexPositionColor>();

            for (int i = 0; i < corners.Length; i++)
                vertex.Add(new VertexPositionColor(corners[i], color));

            base.Update(gameTime);
        }

        /// <summary>
        /// Enables self-drawing of the base rectangle
        /// </summary>
        /// <param name="gameTime">Snapshot of time</param>
        public override void Draw(GameTime gameTime)
        {
            foreach (EffectPass p in ((WarGame)base.Game).Effect.CurrentTechnique.Passes)
            {
                p.Apply();
                ((WarGame)base.Game).GraphicsDevice.DrawUserIndexedPrimitives<VertexPositionColor>(PrimitiveType.LineStrip, vertex.ToArray(), 0, vertex.Count, index, 0, index.Length - 1);
            }
            
            base.Draw(gameTime);
        }

        /// <summary>
        /// Shows the base rectangle
        /// </summary>
        public void Show()
        {
            Enabled = Visible = true;
        }

        /// <summary>
        /// Hides the base rectangle
        /// </summary>
        public void Hide()
        {
            Enabled = Visible = false;
        }

        /// <summary>
        /// Base rectangle destructor
        /// </summary>
        public void Destroy()
        {
            owner.Game.Components.Remove(this);
            Hide();
            this.Dispose();
        }
    }
}
