#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
#endregion

namespace WarGame
{
    public abstract class MenuScene : Scene
    {
        /// <summary>
        /// Index of item which is currently active.
        /// </summary>
        protected int activeItemNumber;

        /// <summary>
        /// Image which is shown as a background.
        /// </summary>
        protected Texture2D backgroundImage;

        public MenuScene(WarGame game)
            : base(game)
        {
            
        }

        public override void Draw(GameTime gameTime)
        {
            if (backgroundImage != null)
            {
                ((WarGame)base.Game).Sprite.Begin();
                ((WarGame)base.Game).Sprite.Draw(backgroundImage, new Vector2(0, 0), Color.White);
                ((WarGame)base.Game).Sprite.End();

                ((WarGame)base.Game).Graphics.GraphicsDevice.BlendState = BlendState.Opaque;
                ((WarGame)base.Game).Graphics.GraphicsDevice.DepthStencilState = DepthStencilState.Default;
            }

            base.Draw(gameTime);
        }

        public void SwitchDown()
        {
            (SceneComponents[activeItemNumber] as MenuItem).Disactivate();

            activeItemNumber++;
            activeItemNumber %= SceneComponents.Count;

            (SceneComponents[activeItemNumber] as MenuItem).Activate();
        }

        public void SwitchUp()
        {
            (SceneComponents[activeItemNumber] as MenuItem).Disactivate();

            activeItemNumber--;
            if (activeItemNumber < 0) activeItemNumber = SceneComponents.Count - 1;

            (SceneComponents[activeItemNumber] as MenuItem).Activate();
        }

        public void ExecuteAction()
        {
            switch ((SceneComponents[activeItemNumber] as MenuItem).Action)
            {
                case MenuItem.ItemActions.None:
                    break;

                case MenuItem.ItemActions.NewGame:
                    (base.Game as WarGame).NewGame();
                    break;

                case MenuItem.ItemActions.GoToResolutionMenu:
                    (this as MainMenu).Hide();
                    (this as MainMenu).ResMenu.Show();
                    (base.Game as WarGame).CurrentScene = (this as MainMenu).ResMenu;
                    break;

                case MenuItem.ItemActions.SetResolution:
                    (base.Game as WarGame).SetResolution((int)((this as ResolutionMenu).CrrentResolution.X), (int)((this as ResolutionMenu).CrrentResolution.Y));
                    break;

                case MenuItem.ItemActions.Exit:
                    base.Game.Exit();
                    break;
            }
        }
    }
}
