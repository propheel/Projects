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

namespace ShadersGame
{
    /// <summary>
    /// This is the main type for your game
    /// </summary>
    public class ShadersGame : Microsoft.Xna.Framework.Game
    {
        #region Fields
        /// <summary>
        /// Global graphics manager.
        /// </summary>
        GraphicsDeviceManager graphics;

        /// <summary>
        /// Basic effect used to draw the cubes.
        /// </summary>
        BasicEffect basicEffect;

        /// <summary>
        /// Custom effects used in the game.
        /// </summary>
        Effect postEffect, environmentEffect;
               
        /// <summary>
        /// Sprite batch used by the post effect.
        /// </summary>
        SpriteBatch postSpriteBatch;

        /// <summary>
        /// Render target used by the post effect.
        /// </summary>
        RenderTarget2D postRenderTarget;

        /// <summary>
        /// Render texture used by the post effect.
        /// </summary>
        Texture2D postRenderTexture;

        /// <summary>
        /// Collection of post effect techniques.
        /// </summary>
        EffectTechnique[] postTechniques = new EffectTechnique[4];

        /// <summary>
        /// Index of technique used by the post effect.
        /// </summary>
        int currentTechnique = 0;

        /// <summary>
        /// Render target used by the environment effect.
        /// </summary>
        RenderTargetCube environmentRenderTarget;

        /// <summary>
        /// Render texture used by the environment effect.
        /// </summary>
        TextureCube environmentRenderTexture;
        
        /// <summary>
        /// Models used in the game.
        /// </summary>
        Model cube, sphere;

        /// <summary>
        /// Texture used to draw the walls.
        /// </summary>
        Texture2D bricks;

        /// <summary>
        /// Global projection and view matrices.
        /// </summary>
        Matrix projection, view;

        /// <summary>
        /// Angle and radius of the camera in the polar coordinate system.
        /// </summary>
        float camAng = 0, camRad = 5;

        /// <summary>
        /// Position of the camera in the carthesian coordinate system.
        /// </summary>
        Vector3 camPos;

        /// <summary>
        /// Angle and radius of the A-cube in the polar coordinate system.
        /// </summary>
        float AcubeAng = 0.0f, AcubeRad = 4.0f;

        /// <summary>
        /// Position of the A-cube in the carthesian coordinate system.
        /// </summary>
        Vector3 AcubePos;

        /// <summary>
        /// Angle and radius of the B-cube in the polar coordinate system.
        /// </summary>
        float BcubeAng = MathHelper.ToRadians(30.0f), BcubeRad = 8.0f;

        /// <summary>
        /// Position of the B-cube in the carthesian coordinate system.
        /// </summary>
        Vector3 BcubePos;

        /// <summary>
        /// Position of the sphere in the carthesian coordinate system.
        /// </summary>
        Vector3 spherePos;

        /// <summary>
        /// Local world matrix of the sphere.
        /// </summary>
        Matrix sphereWorld;

        /// <summary>
        /// Size of the textured cube.
        /// </summary>
        float cubeSize = 20.0f;

        /// <summary>
        /// M key status flag.
        /// </summary>
        bool isMKeyPressed = false;
        #endregion

        #region Getters and Setters
        /// <summary>
        /// Getter and setter of camRad with a simple validator.
        /// </summary>
        float CamRad
        {
            get { return camRad; }
            set
            {
                if (value < 10 && value > 4)
                    camRad = value;
            }
        }

        /// <summary>
        /// Getter and setter of the camAng with a simple validator.
        /// </summary>
        float CamAng
        {
            get { return camAng; }
            set
            {
                camAng = value;

                if (camAng < 0) camAng = MathHelper.Pi * 2;
                else if (camAng > MathHelper.Pi * 2) camAng = 0;
            }

        }

        /// <summary>
        /// Getter and setter of the AcubeAng with a simple validator.
        /// </summary>
        float ACubeAng
        {
            get { return AcubeAng; }
            set
            {
                AcubeAng = value;

                if(AcubeAng < 0) AcubeAng = MathHelper.Pi * 2;
                else if (AcubeAng > MathHelper.Pi * 2) AcubeAng = 0;
            }
        }

        /// <summary>
        /// Getter and setter of the BcubeAng with a simple validator.
        /// </summary>
        float BCubeRot
        {
            get { return BcubeAng; }
            set
            {
                BcubeAng = value;

                if (BcubeAng < 0) BcubeAng = MathHelper.Pi * 2;
                else if (AcubeAng > MathHelper.Pi * 2) BcubeAng = 0;
            }
        }

        /// <summary>
        /// Getter and setter of the currentTechnique with a simple validator.
        /// </summary>
        int CurrentTechnique
        {
            get { return currentTechnique; }
            set
            {
                currentTechnique = value;

                if (currentTechnique == 4) currentTechnique = 0;
            }
        }
        #endregion

        /// <summary>
        /// Creates an instance of the ShadersGame
        /// </summary>
        public ShadersGame()
        {
            graphics = new GraphicsDeviceManager(this);
            Content.RootDirectory = "Content";
            IsMouseVisible = true;
        }

        /// <summary>
        /// Allows the game to perform any initialization it needs to before starting to run.
        /// This is where it can query for any required services and load any non-graphic
        /// related content.  Calling base.Initialize will enumerate through any components
        /// and initialize them as well.
        /// </summary>
        protected override void Initialize()
        {
            basicEffect = new BasicEffect(graphics.GraphicsDevice);
           
            // Setting the projection matrix
            projection = Matrix.CreatePerspectiveFieldOfView(MathHelper.PiOver2, graphics.GraphicsDevice.Viewport.AspectRatio, 0.1f, 100.0f);
            
            // Setting the sphere position and world matrix
            spherePos = Vector3.Zero;
            sphereWorld = Matrix.CreateScale(3f) * Matrix.CreateTranslation(spherePos);

            // Setting the post render target
            PresentationParameters pp = graphics.GraphicsDevice.PresentationParameters;
            postRenderTarget = new RenderTarget2D(graphics.GraphicsDevice, pp.BackBufferWidth, pp.BackBufferHeight, false, graphics.GraphicsDevice.DisplayMode.Format, DepthFormat.Depth24);

            // Setting the environment render target and texture
            environmentRenderTarget = new RenderTargetCube(graphics.GraphicsDevice, 256, true, graphics.GraphicsDevice.DisplayMode.Format, DepthFormat.Depth24);
            environmentRenderTexture = new TextureCube(graphics.GraphicsDevice, 256, true, graphics.GraphicsDevice.DisplayMode.Format);

            base.Initialize();
        }

        /// <summary>
        /// LoadContent will be called once per game and is the place to load
        /// all of your content.
        /// </summary>
        protected override void LoadContent()
        {
            // Create a new SpriteBatch, which can be used to draw textures.
            postSpriteBatch = new SpriteBatch(GraphicsDevice);

            // XNA Framework Reach profile requires TextureAddressMode to be Clamp when using texture sizes that are not powers of two.
            graphics.GraphicsDevice.SamplerStates[0] = SamplerState.PointClamp;

            // Loading the post effect and creating the array of it's techniques
            postEffect = Content.Load<Effect>("Shaders/post");
            for (int i = 0; i < 4; i++)
                postTechniques[i] = postEffect.Techniques[i];
            postEffect.CurrentTechnique = postTechniques[CurrentTechnique];

            // Loading the environment effect
            environmentEffect = Content.Load<Effect>("Shaders/environment");
            environmentEffect.CurrentTechnique = environmentEffect.Techniques["Reflection"];

            // Loading the models
            cube = Content.Load<Model>("Models/cube");
            sphere = Content.Load<Model>("Models/sphere");

            // Attachning the environment effect to all meshes of the sphere model
            foreach (ModelMesh m in sphere.Meshes)
                foreach (ModelMeshPart p in m.MeshParts)
                    p.Effect = environmentEffect;

            // Loading the bricks texture
            bricks = Content.Load<Texture2D>("Textures/bricks");
        }

        /// <summary>
        /// UnloadContent will be called once per game and is the place to unload
        /// all content.
        /// </summary>
        protected override void UnloadContent()
        {
            // TODO: Unload any non ContentManager content here
        }

        /// <summary>
        /// Allows the game to run logic such as updating the world,
        /// checking for collisions, gathering input, and playing audio.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Update(GameTime gameTime)
        {
            #region Moving the cubes
            // Moving the A-cube
            AcubeAng += MathHelper.Pi / 180;
            AcubePos = new Vector3(AcubeRad * (float)Math.Cos(AcubeAng), 0, AcubeRad * (float)Math.Sin(AcubeAng));

            // Moving the B-cube
            BcubeAng -= MathHelper.Pi / 180;
            BcubePos = new Vector3(0, BcubeRad * (float)Math.Cos(BcubeAng), BcubeRad * (float)Math.Sin(BcubeAng));
            #endregion

            #region Input controlling
            KeyboardState kState = Keyboard.GetState();

            if (kState.IsKeyDown(Keys.Escape))
                this.Exit();

            if (kState.IsKeyDown(Keys.Left))
                CamAng -= MathHelper.Pi / 180;

            if (kState.IsKeyDown(Keys.Right))
                CamAng += MathHelper.Pi / 180;

            if (kState.IsKeyDown(Keys.Up))
                CamRad += 0.1f;

            if (kState.IsKeyDown(Keys.Down))
                CamRad -= 0.1f;

            if (kState.IsKeyDown(Keys.M) && !isMKeyPressed)
            {
                isMKeyPressed = true;
                postEffect.CurrentTechnique = postTechniques[CurrentTechnique++];
            }

            if (kState.IsKeyUp(Keys.M))
                isMKeyPressed = false;
            #endregion

            // Updating the camPos and view matrix
            camPos = new Vector3(CamRad * (float)Math.Cos(CamAng), 0, CamRad * (float)Math.Sin(CamAng));
            view = Matrix.CreateLookAt(camPos, Vector3.Zero, Vector3.Up);

            base.Update(gameTime);
        }

        /// <summary>
        /// This is called when the game should draw itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        protected override void Draw(GameTime gameTime)
        {
            #region Environemnt probing
            // View matrix used in the probing process.
            Matrix probingView = Matrix.Identity;
            // Projection matrix used in the probing matrix.
            Matrix probingProjection = Matrix.CreatePerspectiveFieldOfView(MathHelper.PiOver2, 1.0f, 0.1f, 100.0f);

            // Render our cube map, once for each cube face( 6 times ).
            for (int i = 0; i < 6; i++)
            {
                // render the scene to all cubemap faces
                CubeMapFace cubeMapFace = (CubeMapFace)i;

                switch (cubeMapFace)
                {
                    case CubeMapFace.NegativeX:
                        {
                            probingView = Matrix.CreateLookAt(Vector3.Zero, Vector3.Right, Vector3.Down);
                            break;
                        }
                    case CubeMapFace.NegativeY:
                        {
                            probingView = Matrix.CreateLookAt(Vector3.Zero, Vector3.Up, Vector3.Forward);
                            break;
                        }
                    case CubeMapFace.NegativeZ:
                        {
                            probingView = Matrix.CreateLookAt(Vector3.Zero, Vector3.Backward, Vector3.Down);
                            break;
                        }
                    case CubeMapFace.PositiveX:
                        {
                            probingView = Matrix.CreateLookAt(Vector3.Zero, Vector3.Left, Vector3.Down);
                            break;
                        }
                    case CubeMapFace.PositiveY:
                        {
                            probingView = Matrix.CreateLookAt(Vector3.Zero, Vector3.Down, Vector3.Backward);
                            break;
                        }
                    case CubeMapFace.PositiveZ:
                        {
                            probingView = Matrix.CreateLookAt(Vector3.Zero, Vector3.Forward, Vector3.Down);
                            break;
                        }
                }

                // Set the cubemap render target, using the selected face
                graphics.GraphicsDevice.SetRenderTarget(environmentRenderTarget, cubeMapFace);

                this.GraphicsDevice.Clear(Color.White);

                // Drawing the reflection of the textured cube
                DrawTexturedCube(probingView, probingProjection);

                // Drawing the reflection of the A and B-cubes
                DrawCube(AcubePos, probingView, probingProjection);
                DrawCube(BcubePos, probingView, probingProjection);
            }

            // Converting the render target into the render texture
            environmentRenderTexture = environmentRenderTarget;
            #endregion

            // Pre-rendering
            PreRender();

            // Rendering
            GraphicsDevice.Clear(Color.CornflowerBlue);

            // Drawing the textured cube
            DrawTexturedCube(view, projection);

            // Drawing the A and B-cubes
            DrawCube(AcubePos, view, projection);
            DrawCube(BcubePos, view, projection);

            // Drawing the sphere with the reflection of the environment on it.
            foreach (ModelMesh m in sphere.Meshes)
            {
                foreach (Effect e in m.Effects)
                {
                    e.Parameters["matWorldViewProj"].SetValue(sphereWorld * view * projection);
                    e.Parameters["matInverseWorld"].SetValue(Matrix.Invert(sphereWorld));
                    e.Parameters["vLightDirection"].SetValue(new Vector4(camPos - spherePos, 0));
                    e.Parameters["vecEye"].SetValue(new Vector4(-camPos, 0));
                    e.Parameters["vDiffuseColor"].SetValue(Color.LightGray.ToVector4());
                    e.Parameters["vSpecularColor"].SetValue(Color.White.ToVector4());
                    e.Parameters["vAmbientColor"].SetValue(Color.Gray.ToVector4());
                    e.Parameters["ReflectionCubeMap"].SetValue(environmentRenderTexture);
                }

                m.Draw();
            }

            // Post rendering
            PostRender();

            base.Draw(gameTime);
        }

        /// <summary>
        /// Pre-rendering.
        /// </summary>
        public void PreRender()
        {
            graphics.GraphicsDevice.SetRenderTarget(postRenderTarget);
        }

        /// <summary>
        /// Post rendering.
        /// </summary>
        public void PostRender()
        {
            postRenderTexture = postRenderTarget;
            graphics.GraphicsDevice.SetRenderTarget(null);

            postSpriteBatch.Begin(0, BlendState.Opaque, SamplerState.LinearClamp, graphics.GraphicsDevice.DepthStencilState, null, postEffect);

            foreach (EffectPass p in postEffect.CurrentTechnique.Passes)
            {
                p.Apply();
                postSpriteBatch.Draw(postRenderTexture, Vector2.Zero, Color.White);
            }

            postSpriteBatch.End();
        }

        /// <summary>
        /// Drawing the cube.
        /// </summary>
        /// <param name="cubePos">Position of the cube</param>
        /// <param name="viewMatrix">View matrix used to draw the cube</param>
        /// <param name="projectionMatrix">Projection matrix used to draw the cube</param>
        void DrawCube(Vector3 cubePos, Matrix viewMatrix, Matrix projectionMatrix)
        {
            foreach (ModelMesh m in cube.Meshes)
            {
                foreach (BasicEffect e in m.Effects)
                {
                    e.EnableDefaultLighting();
                    e.View = viewMatrix;
                    e.Projection = projectionMatrix;
                    e.World = Matrix.CreateTranslation(cubePos);
                }

                m.Draw();
            }
        }

        /// <summary>
        /// Drawing the textured cube.
        /// </summary>
        /// <param name="viewMatrix">View matrix used to draw the cube</param>
        /// <param name="projectionMatrix">Projection matrix used to draw the cube</param>
        void DrawTexturedCube(Matrix viewMatrix, Matrix projectionMatrix)
        {
            List<VertexPositionNormalTexture> vertices = new List<VertexPositionNormalTexture>();
            short[] indices = new short[] { 6, 7, 5, 4, 3, 7, 2, 6, 0, 5, 1, 3, 0, 2 };

            // back-side
            vertices.Add(new VertexPositionNormalTexture(new Vector3(-cubeSize, -cubeSize, -cubeSize), Vector3.Backward, new Vector2(0, 0)));
            vertices.Add(new VertexPositionNormalTexture(new Vector3(-cubeSize, cubeSize, -cubeSize), Vector3.Backward, new Vector2(0, 1)));
            vertices.Add(new VertexPositionNormalTexture(new Vector3(cubeSize, -cubeSize, -cubeSize), Vector3.Backward, new Vector2(1, 0)));
            vertices.Add(new VertexPositionNormalTexture(new Vector3(cubeSize, cubeSize, -cubeSize), Vector3.Backward, new Vector2(1, 1)));
            
            // front-side
            vertices.Add(new VertexPositionNormalTexture(new Vector3(cubeSize, cubeSize, cubeSize), Vector3.Left, new Vector2(0, 0)));
            vertices.Add(new VertexPositionNormalTexture(new Vector3(-cubeSize, cubeSize, cubeSize), Vector3.Down, new Vector2(1, 0)));
            vertices.Add(new VertexPositionNormalTexture(new Vector3(-cubeSize, -cubeSize, cubeSize), Vector3.Down, new Vector2(1, 1)));
            vertices.Add(new VertexPositionNormalTexture(new Vector3(cubeSize, -cubeSize, cubeSize), Vector3.Down, new Vector2(0, 1)));

            basicEffect.VertexColorEnabled = false;
            basicEffect.TextureEnabled = true;
            basicEffect.Texture = bricks;

            basicEffect.World = Matrix.Identity;
            basicEffect.View = viewMatrix;
            basicEffect.Projection = projectionMatrix;

            foreach (EffectPass p in basicEffect.CurrentTechnique.Passes)
            {
                p.Apply();
                graphics.GraphicsDevice.DrawUserIndexedPrimitives<VertexPositionNormalTexture>(PrimitiveType.TriangleStrip, vertices.ToArray(), 0, vertices.Count, indices, 0, indices.Length - 2);
            }
        }
    }
}
