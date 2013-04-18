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
    using SBNames = SteeringBehaviors.SteeringBehaviorsNames;

    /// <summary>
    /// Base class for all the objects in the game
    /// </summary>
    public abstract class GameObject : Microsoft.Xna.Framework.DrawableGameComponent
    {
        #region Fields
        /// <summary>
        /// Scene in which the game object appears.
        /// </summary>
        protected MainScene scene;
        
        /// <summary>
        /// Health points from 0 - dead to 1 - healthy
        /// </summary>
        public float HealthPoints { get; set; }

        /// <summary>
        /// Mass of the game object.
        /// </summary>
        protected float mass;

        /// <summary>
        /// Maximal speed value of the object
        /// </summary>
        protected float maxSpeed;

        /// <summary>
        /// Maximal force which can act at the object
        /// </summary>
        protected float maxForce;

        /// <summary>
        /// Position of the object in 3d space
        /// </summary>
        public Vector3 Position { get; set; }

        /// <summary>
        /// Velocity of the object in 3d space
        /// </summary>
        public Vector3 Velocity { get; set; }

        /// <summary>
        /// Model used to draw the object
        /// </summary>
        protected Model gameObjectModel;

        /// <summary>
        /// Is the model used by the object textured
        /// </summary>
        protected bool isModelTextured = false;

        /// <summary>
        /// Texture used by the model
        /// </summary>
        protected Texture2D modelTexture;

        /// <summary>
        /// Current steering behavior used by the object
        /// </summary>
        protected SBNames currentSB;

        /// <summary>
        /// Scale used to draw the object
        /// </summary>
        protected Vector3 scale;

        /// <summary>
        /// Last non-zero velocity of the object
        /// </summary>
        protected Vector3 lastNonZeroVelocity;

        /// <summary>
        /// Force currently acting at the object
        /// </summary>
        protected Vector3 force;

        /// <summary>
        /// Target seeked by the object
        /// </summary>
        public Vector3 Target { get; set; }

        /// <summary>
        /// World matrix of the object
        /// </summary>
        protected Matrix currentWorld;

        /// <summary>
        /// Distance between object and the target at which the object stops, 
        /// squared for convenience
        /// </summary>
        float criticalRadiusSquared = 0.5f * 0.5f;

        /// <summary>
        /// Bounding box of the object
        /// </summary>
        protected Bbox boundingBox;

        /// <summary>
        /// Value used in the wander steering behavior
        /// </summary>
        public float WanderChange { get; set; }

        /// <summary>
        /// Bar over the object representing its health condition
        /// </summary>
        protected HealthPointsBar bar;

        /// <summary>
        /// Rectangle under the game object shown when its active
        /// </summary>
        BaseRectangle baseRectangle;
        #endregion

        #region Getters and Setters
        public float MaxSpeed
        {
            get { return maxSpeed; }
        }
        public SBNames CurrentSB
        {
            set { currentSB = value; }
        }
        public Model GameObjectModel
        {
            get { return gameObjectModel; }
        }
        public Matrix CurrentWorld
        {
            get { return currentWorld; }
        }
        public BoundingBox BoundingBox
        {
            get { return boundingBox.RealBox; }
        }
        public Vector3 LastNonZeroVelocity
        {
            get { return lastNonZeroVelocity; }
        }
        #endregion

        /// <summary>
        /// Constructor of the game object
        /// </summary>
        /// <param name="game">World in which the game object is set</param>
        public GameObject(WarGame game, MainScene _scene, Vector3 _position)
            : base(game)
        {
            Position = _position;
            scene = _scene;
            HealthPoints = 1.0f;
            Velocity = Vector3.Zero;
            Target = Vector3.Zero;
            lastNonZeroVelocity = Vector3.Right;
            WanderChange = 0.1f;

            // creating and activating health points bar
            bar = new HealthPointsBar(this, game, scene);
            scene.SceneComponents.Add(bar);

            // creating and activating base rectangle
            baseRectangle = new BaseRectangle(this, game, scene);
            scene.SceneComponents.Add(baseRectangle);

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
            // Reaction of the object depends on the current steering behavior
            switch (currentSB)
            {
                case SBNames.None:
                    {
                        force = Vector3.Zero;
                        Velocity = Vector3.Zero;
                    }
                    break;
            }

            // Saving velocity and position
            Vector3 positionBeforeMove = new Vector3(Position.X, Position.Y, Position.Z);
            Vector3 velocityBeforeMove = new Vector3(Velocity.X, Velocity.Y, Velocity.Z);

            // Moving the object depending on the force
            Velocity = V3Extensions.Truncate((Velocity + (force / mass)), maxSpeed);
            Position += Velocity;

            // If the object is too close to the target then stop
            if (V3Extensions.CheckIfVectorIsValid(Target) && Vector3.DistanceSquared(Position, Target) < criticalRadiusSquared)
            {
                Velocity = Vector3.Zero;
                currentSB = SBNames.None;
            }

            // If there is velocity draw the object according to it
            if (Velocity.LengthSquared() != 0)
                currentWorld = Matrix.CreateScale(scale) * Matrix.CreateWorld(Position, Velocity, Vector3.Up);
            
            // Otherwise draw the object according to last non-zero velocity
            else
                currentWorld = Matrix.CreateScale(scale) * Matrix.CreateWorld(Position, lastNonZeroVelocity, Vector3.Up);

            #region Collision Detection
            boundingBox.Update(gameTime);
            List<GameObject> collisionObjects = CheckForCollisions();

            // Collision detection and reaction if there is a collision
            float groundElementWidthBy2 = ((WarGame)base.Game).Scene.GroundElement.WidthOfGroundElement / 2;
            float groundelementHeightBy2 = ((WarGame)base.Game).Scene.GroundElement.HeightOfGroundElement / 2;

            if (collisionObjects != null || (Position.X <= -groundElementWidthBy2 || Position.X >= groundElementWidthBy2 || Position.Z >= groundelementHeightBy2 || Position.Z <= -groundelementHeightBy2))
            {
                if (collisionObjects != null)
                {
                    Vector3 velo = Vector3.Zero;
                    foreach (GameObject gobject in collisionObjects)
                    {
                        velo += V3Extensions.V3fromV2(Vector2.Normalize(V3Extensions.V2fromV3(Position) - V3Extensions.V2fromV3(gobject.Position)));
                    }
                    velo.Normalize();

                    velo = V3Extensions.Truncate(velo, MaxSpeed);

                    //Position = positionBeforeMove;
                    Velocity = velo;
                }
                else
                    Velocity = -2 *velocityBeforeMove;
                //Velocity *= -1;
                Position += Velocity;
                boundingBox.Update(gameTime);
            }
            #endregion

            // If there is a non-zero velocity it becomes new lastNonZeroVelocity
            if (Velocity.LengthSquared() > 0)
                lastNonZeroVelocity = Velocity;

            base.Update(gameTime);
        }

        /// <summary>
        /// Simple collision detection mechanism
        /// </summary>
        /// <returns>true - there is a collision, false - no collision</returns>
        public List<GameObject> CheckForCollisions()
        {
            List<GameObject>collisionList = null;
            if (!(this is GroundElement))
            {
                foreach (GameObject g in scene.SceneObjects)
                {
                    if (!(g is GroundElement))
                    {
                        if (g != this &&
                            boundingBox.RealBox.Intersects(g.BoundingBox))
                        {
                            if (collisionList == null)
                                collisionList = new List<GameObject>();
                                collisionList.Add(g);
                            //return g;
                        }
                    }
                }
            }
            return collisionList;
            //return null;
        }

        public void DrawImage(Matrix viewMAtrix)
        {
            foreach (ModelMesh mesh in gameObjectModel.Meshes)
            {
                foreach (Effect e in mesh.Effects)
                {
                    if (e is BasicEffect)
                    {
                        (e as BasicEffect).EnableDefaultLighting();
                        (e as BasicEffect).World = currentWorld;
                        (e as BasicEffect).View = viewMAtrix;
                        (e as BasicEffect).Projection = ((WarGame)base.Game).Camera.Projection;
                        (e as BasicEffect).TextureEnabled = isModelTextured;

                        // texturize if needed
                        if (isModelTextured)
                            (e as BasicEffect).Texture = modelTexture;
                    }
                }

                mesh.Draw();

            }
        }


        /// <summary>
        /// Enables drawing of the game object
        /// </summary>
        /// <param name="gameTime">Snapshot of time</param>
        public override void Draw(GameTime gameTime)
        {
            foreach (ModelMesh mesh in gameObjectModel.Meshes)
            {
                foreach (Effect e in mesh.Effects)
                {
                    (e as BasicEffect).EnableDefaultLighting();
                    (e as BasicEffect).World = currentWorld;
                    (e as BasicEffect).View = ((WarGame)base.Game).Camera.View;
                    (e as BasicEffect).Projection = ((WarGame)base.Game).Camera.Projection;
                    (e as BasicEffect).TextureEnabled = isModelTextured;

                    // texturize if needed
                    if (isModelTextured)
                        (e as BasicEffect).Texture = modelTexture;
                }

                mesh.Draw();
            }

            base.Draw(gameTime);
        }

        /// <summary>
        /// Activates the game object
        /// </summary>
        public void Activate()
        {
            bar.Enabled = bar.Visible = 
                baseRectangle.Visible = baseRectangle.Enabled = true;
        }

        /// <summary>
        /// Disactivates game object
        /// </summary>
        public void Disactivate()
        {
            bar.Enabled = bar.Visible =
                baseRectangle.Visible = baseRectangle.Enabled = false;
        }

        /// <summary>
        /// Toggles the debug mode
        /// </summary>
        public void DebugModeToggle()
        {
            boundingBox.Visible = !boundingBox.Visible;
        }

        /// <summary>
        /// Game object destructor
        /// </summary>
        public void Destroy()
        {
            bar.Destroy();
            baseRectangle.Destroy();
            boundingBox.Destroy();

            scene.SceneComponents.Remove(this);
            scene.SceneObjects.Remove(this);
            Visible = Enabled = false;
            
            Dispose();
        }
    }
}
