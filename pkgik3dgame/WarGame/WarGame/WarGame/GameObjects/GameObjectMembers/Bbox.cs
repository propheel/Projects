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
    /// This is a class representing a game object bounding box
    /// </summary>
    public class Bbox : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// Scene in which the bounding box appears
        /// </summary>
        MainScene scene;

        /// <summary>
        /// Game object which owns the bounding box
        /// </summary>
        GameObject owner;

        /// <summary>
        /// Initial bounding box and bounding box after transformations
        /// </summary>
        BoundingBox baseBox, realBox;

        /// <summary>
        /// Color of the bounding box
        /// </summary>
        Color color = Color.Yellow;

        /// <summary>
        /// Array of indices used to draw the bounding box
        /// </summary>
        short[] index = new short[] { 3, 2, 0, 1, 4, 5, 6, 2, 7, 3, 4 };

        /// <summary>
        /// Values used to draw the bounding box
        /// </summary>
        RasterizerState WIREFRAME_RASTERIZER_STATE = new RasterizerState() { CullMode = CullMode.None, FillMode = FillMode.WireFrame };
        RasterizerState SOLID_RASTERIZER_STATE = new RasterizerState() { CullMode = CullMode.None, FillMode = FillMode.Solid };

        #region Getters and Setters

        public BoundingBox RealBox
        {
            get { return realBox; }
        }
        public BoundingBox BaseBox
        {
            get { return baseBox; }
        }

        #endregion

        /// <summary>
        /// Constructor of the bounding box
        /// </summary>
        /// <param name="_owner">Game object which owns the bounding box</param>
        /// <param name="_game">Game which the bounding box i set in</param>
        public Bbox(GameObject _owner, WarGame _game, MainScene _scene)
            : base(_game)
        {
            scene = _scene;
            owner = _owner;
            baseBox = (BoundingBox)_owner.GameObjectModel.Tag;
            Visible = false;
            DrawOrder = (int)WarGame.DrawingOrder.GameObject;
        }

        /// <summary>
        /// Allows the game component to update itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        public override void Update(GameTime gameTime)
        {
            RecalculateBoundingBox();

            base.Update(gameTime);
        }

        /// <summary>
        /// Enables the bounding box self-drawing
        /// </summary>
        /// <param name="gameTime">Snapshot of time</param>
        public override void Draw(GameTime gameTime)
        {
            List<VertexPositionColor> vertices = new List<VertexPositionColor>();
            Vector3[] vectors = realBox.GetCorners();

            for (int i = 0; i < 8; i++)
                vertices.Add(new VertexPositionColor(vectors[i], color));

            ((WarGame)base.Game).GraphicsDevice.RasterizerState = WIREFRAME_RASTERIZER_STATE;

            foreach (EffectPass p in ((WarGame)base.Game).Effect.CurrentTechnique.Passes)
            {
                p.Apply();
                ((WarGame)base.Game).GraphicsDevice.DrawUserIndexedPrimitives<VertexPositionColor>(PrimitiveType.TriangleStrip, vertices.ToArray(), 0, vertices.Count, index, 0, index.Length - 2);
            }

            ((WarGame)base.Game).GraphicsDevice.RasterizerState = SOLID_RASTERIZER_STATE;
            
            base.Draw(gameTime);
        }

        /// <summary>
        /// Bounding box destructor
        /// </summary>
        public void Destroy()
        {
            scene.SceneComponents.Remove(this);
            Visible = Enabled = false;
            Dispose();
        }

        public void RecalculateBoundingBox()
        {
            List<Vector3> corners = new List<Vector3>();

            foreach (Vector3 corner in (baseBox.GetCorners()))
            {
                corners.Add(Vector3.Transform(corner, owner.CurrentWorld));
            }

            realBox = BoundingBox.CreateFromPoints(corners);
        }
    }
}
