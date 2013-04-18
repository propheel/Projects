#region Using statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
#endregion

namespace WarGame
{
    /// <summary>
    /// This is a class representing a bullet shot by the fighter
    /// </summary>
    public class Bullet : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// Scene in which the bullet appears.
        /// </summary>
        MainScene scene;

        /// <summary>
        /// Owner of the bullet
        /// </summary>
        Fighter owner;

        /// <summary>
        /// How long is the bullet visible for and how long it can be visible for
        /// </summary>
        TimeSpan lifeTime, maxLifeTime = TimeSpan.FromSeconds(1.0);

        /// <summary>
        /// Current position of the bullet
        /// </summary>
        Vector3 position;

        /// <summary>
        /// Second ending of the bullet
        /// </summary>
        Vector3 secondPoint;

        /// <summary>
        /// Direction in which the bullet flies
        /// </summary>
        Vector3 headVector;

        /// <summary>
        /// Length of the vector
        /// </summary>
        float length = 3.0f;

        /// <summary>
        /// Current velocity of the bullet
        /// </summary>
        Vector3 velocity;

        /// <summary>
        /// Color of the bullet
        /// </summary>
        Color color;

        /// <summary>
        /// Array of indices used to draw the bullet
        /// </summary>
        short[] index = new short[] { 0, 1 };

        /// <summary>
        /// List of vertices used to draw the bullet
        /// </summary>
        List<VertexPositionColor> vertex = new List<VertexPositionColor>();

        #region Getters and Setters

        public Vector3 Position
        {
            get { return position; }
        }
        public Vector3 SecondPoint
        {
            get { return secondPoint; }
        }

        #endregion

        /// <summary>
        /// Constructor of the bullet
        /// </summary>
        /// <param name="_owner">Fighter which shoots the bullet</param>
        /// <param name="_game">Game in which the bullet exists</param>
        public Bullet(Fighter _owner, Game _game, MainScene _scene)
            : base(_game)
        {
            scene = _scene;
            owner = _owner;
            position = _owner.Position;
            headVector = Vector3.Normalize(_owner.TargetToAttack.Position - position);
            velocity = headVector * owner.MaxSpeed * 2;
            secondPoint = position + headVector * length;
            color = new Color(new Vector3((float)WarGame.RandomGenerator.NextDouble(),
                                          (float)WarGame.RandomGenerator.NextDouble(),
                                          (float)WarGame.RandomGenerator.NextDouble()));
            DrawOrder = (int)WarGame.DrawingOrder.GameObject;
            scene.SceneComponents.Add(this);
            lifeTime = new TimeSpan();
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
            // updating the life time
            lifeTime += gameTime.ElapsedGameTime;

            // if the bullet exists for too long remove it
            if (lifeTime >= maxLifeTime)
            {
                Destroy();
                return;
            }

            position += velocity;
            secondPoint += velocity;

            vertex.Clear();
            vertex.Add(new VertexPositionColor(position, color));
            vertex.Add(new VertexPositionColor(secondPoint, color));

            // checking for collisions with the alien crafts
            Vector3 first = ((WarGame)base.Game).Graphics.GraphicsDevice.Viewport.Unproject(position,
                ((WarGame)base.Game).Camera.Projection, ((WarGame)base.Game).Camera.View, ((WarGame)base.Game).Camera.World);
            Vector3 second = ((WarGame)base.Game).Graphics.GraphicsDevice.Viewport.Unproject(secondPoint,
                ((WarGame)base.Game).Camera.Projection, ((WarGame)base.Game).Camera.View, ((WarGame)base.Game).Camera.World);

            Vector3 direction = Vector3.Normalize(second - first);
            Ray shootingRay = new Ray(position, direction);

            float? minDistance = float.MaxValue;
            AlienCraft closestEnemy = null;

            foreach (AlienCraft a in scene.AlienCrafts)
            {
                float? d = shootingRay.Intersects(a.BoundingBox);

                if (d != null && (float)d < length && d < minDistance)
                {
                    minDistance = d;
                    closestEnemy = a;
                }
            }

            // if the alien craft gets hit
            if (closestEnemy != null)
            {
                Destroy();
                closestEnemy.GetHurt();
            }
            base.Update(gameTime);
        }

        /// <summary>
        /// Allows the bullet to draw itself
        /// </summary>
        /// <param name="gameTime">Snapshot of time</param>
        public override void Draw(GameTime gameTime)
        {
            if (vertex.Count > 0)
            {
                foreach (EffectPass p in ((WarGame)base.Game).Effect.CurrentTechnique.Passes)
                {
                    p.Apply();
                    ((WarGame)base.Game).GraphicsDevice.DrawUserIndexedPrimitives<VertexPositionColor>(PrimitiveType.LineStrip, vertex.ToArray(), 0, vertex.Count, index, 0, index.Length - 1);
                }
            }

            base.Draw(gameTime);
        }

        /// <summary>
        /// Bullet destructor
        /// </summary>
        void Destroy()
        {
            owner.Bullet = null;
            scene.SceneComponents.Remove(this);
            this.Visible = this.Enabled = false;
            this.Dispose();
        }
    }
}
