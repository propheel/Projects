#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
#endregion

namespace WarGame
{
    using SBNames = SteeringBehaviors.SteeringBehaviorsNames;

    /// <summary>
    /// This is a class used to controll the game input
    /// </summary>
    public class InputController : Microsoft.Xna.Framework.GameComponent
    {
        #region Variables
        /// <summary>
        /// Status of the F key
        /// </summary>
        bool isFKeyPressed = false;

        /// <summary>
        /// Status of the B key
        /// </summary>
        bool isBKeyPressed = false;

        /// <summary>
        /// Status of the P key
        /// </summary>
        bool isPKeyPressed = false;

        /// <summary>
        /// Status of the Ctrl keys combination
        /// </summary>
        bool isCtrlKeyPressed = false;

        /// <summary>
        /// Status of the A key
        /// </summary>
        bool isAKeyPressed = false;

        /// <summary>
        /// Status of the Up key
        /// </summary>
        bool isUpKeyPressed = false;

        /// <summary>
        /// Status of the Down key
        /// </summary>
        bool isDownKeyPressed = false;

        /// <summary>
        /// Status of the Escape key
        /// </summary>
        bool isEscapePressed = false;

        /// <summary>
        /// Status of the M key
        /// </summary>
        bool isMKeyPressed = false;

        /// <summary>
        /// Status of the left mouse button
        /// </summary>
        bool isLeftMousePressed = false;

        /// <summary>
        /// Minimal distance between the click and release 
        /// position for which it is not a click but a scrolling,
        /// squared for convenience
        /// </summary>
        float leftMouseClickCriticalRadius = 0.5f * 0.5f;

        /// <summary>
        /// Place where the left mouse button was clicked
        /// </summary>
        Vector3 mouseClickPosition = V3Extensions.CreateInvalidVector();

        /// <summary>
        /// Place where the left mouse button was released 
        /// </summary>
        Vector3 mouseReleasePosition = V3Extensions.CreateInvalidVector();

        /// <summary>
        /// Current state of the keyboard input
        /// </summary>
        KeyboardState keyboardState = new KeyboardState();

        /// <summary>
        /// Current state of the mouse input
        /// </summary>
        MouseState mouseState = new MouseState();

        /// <summary>
        /// Minimal and maximal positions in the y direction of the selection bounding box
        /// </summary>
        float selectionBoundingBoxMinimalYValue = 0.0f, selectionBoundingBoxMaximalYValue = 4.0f;
        #endregion

        #region Getters and Setters
        public Vector3 MouseClickPosition
        {
            get { return mouseClickPosition; }
        }
        public Vector3 MouseReleasePosition
        {
            get { return mouseReleasePosition; }
        }
        #endregion

        /// <summary>
        /// Contructor for the input controller
        /// </summary>
        /// <param name="game">Game in which the controller is used</param>
        public InputController(WarGame game)
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
            // TODO: Add your initialization code here

            base.Initialize();
        }

        /// <summary>
        /// Allows the game component to update itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        public override void Update(GameTime gameTime)
        {
            #region Keyboard control
            keyboardState = Keyboard.GetState();

            #region Left Key

            if (keyboardState.IsKeyDown(Keys.Left))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    ((WarGame)base.Game).Camera.RotateLeft();
                }
            }

            #endregion

            #region Right Key

            if (keyboardState.IsKeyDown(Keys.Right))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    ((WarGame)base.Game).Camera.RotateRight();
                }
            }

            #endregion

            #region Up Key

            if (keyboardState.IsKeyDown(Keys.Up))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                    ((WarGame)base.Game).Camera.ZoomUp();

                else if (((WarGame)base.Game).CurrentScene is MenuScene && !isUpKeyPressed)
                {
                    isUpKeyPressed = true;
                    (((WarGame)base.Game).CurrentScene as MenuScene).SwitchUp();
                    ((WarGame)base.Game).Sound.ClickSoundInstance.Play();
                }
            }

            if (keyboardState.IsKeyUp(Keys.Up))
                isUpKeyPressed = false;

            #endregion

            #region Down Key

            if (keyboardState.IsKeyDown(Keys.Down))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                    ((WarGame)base.Game).Camera.ZoomDown();

                else if (((WarGame)base.Game).CurrentScene is MenuScene && !isDownKeyPressed)
                {
                    isDownKeyPressed = true;
                    (((WarGame)base.Game).CurrentScene as MenuScene).SwitchDown();
                    ((WarGame)base.Game).Sound.ClickSoundInstance.Play();
                }
            }

            if(keyboardState.IsKeyUp(Keys.Down))
                isDownKeyPressed = false;

            #endregion

            #region W Key

            if (keyboardState.IsKeyDown(Keys.W))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    ((WarGame)base.Game).Camera.MoveForward();
                }
            }

            #endregion

            #region S Key

            if (keyboardState.IsKeyDown(Keys.S))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    ((WarGame)base.Game).Camera.MoveBackwards();
                }
            }

            #endregion

            #region A Key and Left Ctrl key

            if (keyboardState.IsKeyDown(Keys.A))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    if (keyboardState.IsKeyDown(Keys.LeftControl))
                    {
                        if ((!isAKeyPressed || !isCtrlKeyPressed))
                        {
                            isAKeyPressed = true;
                            isCtrlKeyPressed = true;

                            ((WarGame)base.Game).Scene.ActivateAllFighters();
                        }
                    }

                    else
                        ((WarGame)base.Game).Camera.MoveLeft();
                }
            }

            if (keyboardState.IsKeyUp(Keys.A))
                isAKeyPressed = false;

            if (keyboardState.IsKeyUp(Keys.LeftControl))
                isCtrlKeyPressed = false;

            #endregion

            #region D Key

            if (keyboardState.IsKeyDown(Keys.D))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    ((WarGame)base.Game).Camera.MoveRight();
                }
            }

            #endregion

            #region M Key
            if (keyboardState.IsKeyDown(Keys.M) && !isMKeyPressed)
            {
                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    isMKeyPressed = true;
                    ((WarGame)base.Game).CurrentTechnique++;
                }
            }

            if (keyboardState.IsKeyUp(Keys.M))
                isMKeyPressed = false;
            #endregion

            #region Enter key

            if (keyboardState.IsKeyDown(Keys.Enter))
            {
                if (((WarGame)base.Game).CurrentScene is MenuScene)
                {
                    (((WarGame)base.Game).CurrentScene as MenuScene).ExecuteAction();
                    ((WarGame)base.Game).Sound.PopSoundInstance.Play();
                }
            }

            #endregion

            #region Escape Key

            if (keyboardState.IsKeyDown(Keys.Escape) && !isEscapePressed)
            {
                isEscapePressed = true;
                ((WarGame)base.Game).CurrentScene.ExecuteEscape();
            }

            if (keyboardState.IsKeyUp(Keys.Escape))
                isEscapePressed = false;

            #endregion

            #region P Key

            if (keyboardState.IsKeyDown(Keys.P) && !isPKeyPressed)
            {
                isPKeyPressed = true;

                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    ((WarGame)base.Game).Scene.TogglePause();
                }
            }

            if (keyboardState.IsKeyUp(Keys.P))
                isPKeyPressed = false;

            #endregion

            #region B Key

            if (keyboardState.IsKeyDown(Keys.B) && !isBKeyPressed)
            {
                isBKeyPressed = true;

                if (((WarGame)base.Game).CurrentScene is MainScene)
                {
                    foreach (GameObject o in ((WarGame)base.Game).Scene.SceneObjects)
                    {
                        o.DebugModeToggle();
                    }

                    ((WarGame)base.Game).Scene.IsInDebugMode = !((WarGame)base.Game).Scene.IsInDebugMode;
                }
            }

            if (keyboardState.IsKeyUp(Keys.B))
                isBKeyPressed = false;

            #endregion

            #region F Key

            if (keyboardState.IsKeyDown(Keys.F) && !isFKeyPressed)
            {
                isFKeyPressed = true;
                ((WarGame)base.Game).Graphics.ToggleFullScreen();
                ((WarGame)base.Game).UpdateMenuItemsPositions();
            }

            if (keyboardState.IsKeyUp(Keys.F))
                isFKeyPressed = false;

            #endregion

            #region C Key

            if (keyboardState.IsKeyDown(Keys.C))
            {
                if (((WarGame)base.Game).CurrentScene is MainScene && ((WarGame)base.Game).Scene.HumanBase == null)
                {
                    Ray directionRay = GetTheCurrentMouseRay();

                    Nullable<float> d = directionRay.Intersects(((WarGame)base.Game).Scene.GroundElement.BoundingBox);

                    if (d != null)
                    {
                        float distance = (float)d;
                        mouseClickPosition = directionRay.Position + directionRay.Direction * distance;

                        Building b = new Building(((WarGame)base.Game), new Vector3(mouseClickPosition.X, ((WarGame)base.Game).Scene.GroundYAxisPosition + 2.0f, mouseClickPosition.Z), ((WarGame)base.Game).Scene);

                        if (b.CheckForCollisions() == null)
                        {
                            ((WarGame)base.Game).Scene.HumanBase = b;
                            ((WarGame)base.Game).Scene.SceneComponents.Add(b);
                            ((WarGame)base.Game).Scene.SceneObjects.Add(b);
                        }

                        else
                            b.Destroy();

                        mouseClickPosition = V3Extensions.CreateInvalidVector();
                        mouseReleasePosition = V3Extensions.CreateInvalidVector();
                    }
                }
            }

            #endregion

            #endregion

            #region Mouse control

            mouseState = Mouse.GetState();

            #region Left mouse button

            #region LMB click

            if (mouseState.LeftButton == ButtonState.Pressed && !isLeftMousePressed && 
                ((WarGame)base.Game).CurrentScene is MainScene)
            {
                isLeftMousePressed = true;

                Ray directionRay = GetTheCurrentMouseRay();

                Nullable<float> d = directionRay.Intersects(((WarGame)base.Game).Scene.GroundElement.BoundingBox);

                if (d != null)
                {
                    float distance = (float)d;
                    mouseClickPosition = directionRay.Position + directionRay.Direction * distance;
                }
            }

            #endregion

            #region LMB scrolling

            if (mouseState.LeftButton == ButtonState.Pressed && isLeftMousePressed && 
                ((WarGame)base.Game).CurrentScene is MainScene)
            {
                Ray directionRay = GetTheCurrentMouseRay();

                Nullable<float> d = directionRay.Intersects(((WarGame)base.Game).Scene.GroundElement.BoundingBox);

                if (d != null)
                {
                    float distance = (float)d;
                    mouseReleasePosition = directionRay.Position + directionRay.Direction * distance;

                    //drawing selection rectangle
                    ((WarGame)base.Game).Scene.SelectionRectangle.Show();
                }
            }

            #endregion

            #region LMB releasing

            if (mouseState.LeftButton == ButtonState.Released && isLeftMousePressed &&
                ((WarGame)base.Game).CurrentScene is MainScene)
            {
                isLeftMousePressed = false;

                // if the click and release points are far enough each other than it is scrolling
                if ((mouseClickPosition - mouseReleasePosition).LengthSquared() > leftMouseClickCriticalRadius)
                {
                    // creating the selection bounding box
                    BoundingBox box = new BoundingBox(new Vector3(
                        Math.Min(mouseClickPosition.X, mouseReleasePosition.X), selectionBoundingBoxMinimalYValue, Math.Min(mouseClickPosition.Z, mouseReleasePosition.Z)),
                                                      new Vector3(
                        Math.Max(mouseClickPosition.X, mouseReleasePosition.X), selectionBoundingBoxMaximalYValue, Math.Max(mouseClickPosition.Z, mouseReleasePosition.Z)));

                    ((WarGame)base.Game).Scene.DisactivateAllFighters();

                    // activating the figters inside the selection bounding box
                    foreach (Fighter f in ((WarGame)base.Game).Scene.Fighters)
                    {
                        if (box.Intersects(f.BoundingBox))
                        {
                            f.Activate();
                            ((WarGame)base.Game).Scene.ActiveFighters.Add(f);
                        }
                    }

                    // restaring the mouse click and release positions
                    mouseClickPosition = V3Extensions.CreateInvalidVector();
                    mouseReleasePosition = V3Extensions.CreateInvalidVector();

                    // hiding selection rectangle
                    ((WarGame)base.Game).Scene.SelectionRectangle.Hide();
                }

                // in any other case it is single-click
                else
                {
                    Ray directionRay = GetTheCurrentMouseRay();

                    Nullable<float> minDistance = float.MaxValue;
                    GameObject closestObject = null;

                    // finding the object which is closest to the user
                    foreach (GameObject g in ((WarGame)base.Game).Scene.SceneObjects)
                    {
                        Nullable<float> d = directionRay.Intersects(g.BoundingBox);

                        if (d != null && d < minDistance)
                        {
                            closestObject = g;
                            minDistance = d;
                        }
                    }

                    // reaction depends on the type of the closes object
                    if (closestObject != null)
                    {
                        switch (closestObject.GetType().Name)
                        {
                            case "Fighter":
                                ((WarGame)base.Game).Scene.DisactivateAllFighters();

                                ((WarGame)base.Game).Scene.ActiveFighters.Add((Fighter)closestObject);
                                ((Fighter)closestObject).Activate();
                                break;

                            case "AlienCraft":
                                foreach (Fighter f in ((WarGame)base.Game).Scene.ActiveFighters)
                                {
                                    f.CurrentSB = SBNames.Attack;
                                    f.TargetToAttack = (AlienCraft)closestObject;
                                    f.Target = V3Extensions.CreateInvalidVector();
                                }
                                break;

                            case "GroundElement":
                                foreach (Fighter f in ((WarGame)base.Game).Scene.ActiveFighters)
                                {
                                    f.CurrentSB = SBNames.SeekWithSeparation;
                                    f.Target = directionRay.Position + directionRay.Direction * (float)minDistance + new Vector3(0, 1, 0);
                                    f.TargetToAttack = null;
                                }
                                break;
                        }
                    }
                }
            }

            #endregion

            #endregion

            #region Right mouse button

            #region RMB click

            if (mouseState.RightButton == ButtonState.Pressed && 
                ((WarGame)base.Game).CurrentScene is MainScene)
            {
                ((WarGame)base.Game).Scene.DisactivateAllFighters();
            }

            #endregion

            #endregion

            bool isMenuEnabled = ((WarGame)base.Game).CurrentScene is MainMenu;

            if (isMenuEnabled == true && ((WarGame)base.Game).IsMouseVisible)
                ((WarGame)base.Game).IsMouseVisible = false;
            else if (isMenuEnabled == false && !((WarGame)base.Game).IsMouseVisible)
                ((WarGame)base.Game).IsMouseVisible = true;

            #endregion

            base.Update(gameTime);
        }

        /// <summary>
        /// Creates the ray directing from the user to the point of the mouse click
        /// </summary>
        /// <returns>Ray from user to the mouse click</returns>
        private Ray GetTheCurrentMouseRay()
        {
            float x = mouseState.X;
            float y = mouseState.Y;

            Vector3 closerPoint = new Vector3(x, y, 0);
            Vector3 furtherPoint = new Vector3(x, y, 1);

            closerPoint = ((WarGame)base.Game).Graphics.GraphicsDevice.Viewport.Unproject(closerPoint, ((WarGame)base.Game).Camera.Projection, ((WarGame)base.Game).Camera.View, Matrix.CreateTranslation(Vector3.Zero));
            furtherPoint = ((WarGame)base.Game).Graphics.GraphicsDevice.Viewport.Unproject(furtherPoint, ((WarGame)base.Game).Camera.Projection, ((WarGame)base.Game).Camera.View, Matrix.CreateTranslation(Vector3.Zero));

            Vector3 directionVector = (furtherPoint - closerPoint);
            directionVector.Normalize();

            return new Ray(closerPoint, directionVector);
        }
    }
}
