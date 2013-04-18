#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
#endregion

namespace WarGame
{
    /// <summary>
    /// This is a class used to control the camera in the game
    /// </summary>
    public class CameraController : Microsoft.Xna.Framework.GameComponent
    {
        #region Variables
        /// <summary>
        /// Current angle and radius used by the camera polar system
        /// </summary>
        float cameraAngle = 0, cameraRadius = 40.0f;

        /// <summary>
        /// Values by witch change angle and radius of the camera in each step
        /// </summary>
        float deltaAngle = 1, deltaRadius = 0.67f;

        /// <summary>
        /// Global matrices
        /// </summary>
        Matrix view, projection, world;

        /// <summary>
        /// Closer clipping plane of the projection frustrum
        /// </summary>
        float closerClippingPlane = 0.1f;

        /// <summary>
        /// Further clipping plane of the projection frustrum
        /// </summary>
        float furtherClippingPlane = 1001.0f;

        /// <summary>
        /// Position of the camera
        /// </summary>
        Vector3 cameraPosition;

        /// <summary>
        /// Point which the camera is directed to
        /// </summary>
        Vector3 cameraLookAt = Vector3.Zero;
        #endregion

        #region Getters and Setters
        public Matrix View
        {
            get { return view; }
        }
        public Matrix Projection
        {
            get { return projection; }
        }
        public Matrix World
        {
            get { return world; }
        }
        public Vector3 Position
        {
            get { return cameraPosition; }
        }
        #endregion

        public CameraController(WarGame game)
            : base(game)
        {
            // TODO: Construct any child components here
        }

        /// <summary>
        /// Allows the game component to perform any initialization it needs to before starting
        /// to run.  This is where it can query for any required services and load content.
        /// </summary>
        public override void Initialize()
        {
            // setting the initial position of the camera
            cameraPosition = new Vector3(cameraRadius * (float)Math.Cos(cameraAngle), cameraRadius / 1.5f, cameraRadius * (float)Math.Sin(cameraAngle));

            // global matrices initialization
            projection = Matrix.CreatePerspectiveFieldOfView(MathHelper.PiOver4, ((WarGame)base.Game).GraphicsDevice.Viewport.AspectRatio, closerClippingPlane, furtherClippingPlane);
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
            world = Matrix.Identity;

            base.Initialize();
        }

        /// <summary>
        /// Allows the game component to update itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        public override void Update(GameTime gameTime)
        {
            #region Updating global Effect
            ((WarGame)base.Game).Effect.Projection = Projection;
            ((WarGame)base.Game).Effect.View = View;
            ((WarGame)base.Game).Effect.World = World;
            ((WarGame)base.Game).Effect.VertexColorEnabled = true;
            #endregion

            base.Update(gameTime);
        }

        /// <summary>
        /// Rotates the camera left
        /// </summary>
        public void RotateLeft()
        {
            cameraAngle += MathHelper.ToRadians(deltaAngle);

            cameraPosition.X = (float)(cameraLookAt.X + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Cos(cameraAngle));
            cameraPosition.Z = (float)(cameraLookAt.Z + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Sin(cameraAngle));

            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraAngle -= MathHelper.ToRadians(deltaAngle);

                cameraPosition.X = (float)(cameraLookAt.X + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Cos(cameraAngle));
                cameraPosition.Z = (float)(cameraLookAt.Z + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Sin(cameraAngle));
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Rotates the camera right
        /// </summary>
        public void RotateRight()
        {
            cameraAngle -= MathHelper.ToRadians(deltaAngle);

            cameraPosition.X = (float)(cameraLookAt.X + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Cos(cameraAngle));
            cameraPosition.Z = (float)(cameraLookAt.Z + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Sin(cameraAngle));

            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraAngle += MathHelper.ToRadians(deltaAngle);

                cameraPosition.X = (float)(cameraLookAt.X + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Cos(cameraAngle));
                cameraPosition.Z = (float)(cameraLookAt.Z + Math.Sqrt(Math.Pow(cameraLookAt.X - cameraPosition.X, 2) + Math.Pow(cameraLookAt.Z - cameraPosition.Z, 2)) * Math.Sin(cameraAngle));
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Zooms the camera up
        /// </summary>
        public void ZoomUp()
        {
            if (cameraPosition.Y > cameraRadius / 1.5f)
            {
                cameraPosition.X -= (float)Math.Cos(cameraAngle);
                cameraPosition.Z -= (float)Math.Sin(cameraAngle);
                cameraPosition.Y -= deltaRadius;
            }
            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraPosition.X += (float)Math.Cos(cameraAngle);
                cameraPosition.Z += (float)Math.Sin(cameraAngle);
                cameraPosition.Y += deltaRadius;
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Zooms the camera down
        /// </summary>
        public void ZoomDown()
        {
            if (cameraPosition.Y < 50)
            {
                cameraPosition.X += (float)Math.Cos(cameraAngle);
                cameraPosition.Z += (float)Math.Sin(cameraAngle);
                cameraPosition.Y += deltaRadius;
            }
            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraPosition.X -= (float)Math.Cos(cameraAngle);
                cameraPosition.Z -= (float)Math.Sin(cameraAngle);
                cameraPosition.Y -= deltaRadius;
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Moves the camera forward
        /// </summary>
        public void MoveForward()
        {
            cameraPosition.X -= (float)Math.Cos(cameraAngle);
            cameraPosition.Z -= (float)Math.Sin(cameraAngle);
            cameraLookAt.X -= (float)Math.Cos(cameraAngle);
            cameraLookAt.Z -= (float)Math.Sin(cameraAngle);
            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraPosition.X += (float)Math.Cos(cameraAngle);
                cameraPosition.Z += (float)Math.Sin(cameraAngle);
                cameraLookAt.X += (float)Math.Cos(cameraAngle);
                cameraLookAt.Z += (float)Math.Sin(cameraAngle);
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Moves the camera backwards
        /// </summary>
        public void MoveBackwards()
        {
            cameraPosition.X += (float)Math.Cos(cameraAngle);
            cameraPosition.Z += (float)Math.Sin(cameraAngle);
            cameraLookAt.X += (float)Math.Cos(cameraAngle);
            cameraLookAt.Z += (float)Math.Sin(cameraAngle);
            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraPosition.X -= (float)Math.Cos(cameraAngle);
                cameraPosition.Z -= (float)Math.Sin(cameraAngle);
                cameraLookAt.X -= (float)Math.Cos(cameraAngle);
                cameraLookAt.Z -= (float)Math.Sin(cameraAngle);
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Moves the camera left
        /// </summary>
        public void MoveLeft()
        {
            cameraPosition.Z += (float)Math.Cos(-cameraAngle);
            cameraPosition.X += (float)Math.Sin(-cameraAngle);
            cameraLookAt.Z += (float)Math.Cos(-cameraAngle);
            cameraLookAt.X += (float)Math.Sin(-cameraAngle);
            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraPosition.Z -= (float)Math.Cos(-cameraAngle);
                cameraPosition.X -= (float)Math.Sin(-cameraAngle);
                cameraLookAt.Z -= (float)Math.Cos(-cameraAngle);
                cameraLookAt.X -= (float)Math.Sin(-cameraAngle);
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }

        /// <summary>
        /// Moves the camera right
        /// </summary>
        public void MoveRight()
        {
            cameraPosition.Z -= (float)Math.Cos(-cameraAngle);
            cameraPosition.X -= (float)Math.Sin(-cameraAngle);
            cameraLookAt.Z -= (float)Math.Cos(-cameraAngle);
            cameraLookAt.X -= (float)Math.Sin(-cameraAngle);
            if (cameraPosition.X <= -214 || cameraPosition.X >= 214 || cameraPosition.Z >= 214 || cameraPosition.Z <= -214)
            {
                cameraPosition.Z += (float)Math.Cos(-cameraAngle);
                cameraPosition.X += (float)Math.Sin(-cameraAngle);
                cameraLookAt.Z += (float)Math.Cos(-cameraAngle);
                cameraLookAt.X += (float)Math.Sin(-cameraAngle);
            }
            view = Matrix.CreateLookAt(cameraPosition, cameraLookAt, Vector3.Up);
        }
    }
}
