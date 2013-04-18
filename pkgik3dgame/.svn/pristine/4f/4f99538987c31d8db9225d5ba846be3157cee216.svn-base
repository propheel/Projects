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
    /// This is a class representing the main Menu in the war game
    /// </summary>
    public class MainMenu : MenuScene
    {
        /// <summary>
        /// Menu to switch resolution
        /// </summary>
        ResolutionMenu resMenu;

        #region Getters and Setters

        public ResolutionMenu ResMenu
        {
            get { return resMenu; }
        }

        #endregion


        /// <summary>
        /// Constructor of the main Menu.
        /// </summary>
        /// <param name="game">Game where the main Menu is used</param>
        public MainMenu(WarGame game)
            : base(game)
        {
            backgroundImage = game.menuBackgroundTexture;

            int x_position;
            string label;
            int y_position;
            MenuItem item;
            MenuItem.ItemActions action;

            label = "New Game";
            x_position = game.Window.ClientBounds.Width / 2 - (int)game.menuFont.MeasureString(label).X / 2;
            y_position = 50;
            action = MenuItem.ItemActions.NewGame;
            item = new MenuItem(game, this, new Vector2(x_position, y_position), label, action);
            item.Activate();
            activeItemNumber = 0;
            SceneComponents.Add(item);

            label = "Settings";
            x_position = game.Window.ClientBounds.Width / 2 - (int)game.menuFont.MeasureString(label).X / 2;
            y_position = 100;
            action = MenuItem.ItemActions.GoToResolutionMenu;
            item = new MenuItem(game, this, new Vector2(x_position, y_position), label, action);
            SceneComponents.Add(item);

            label = "Exit";
            x_position = game.Window.ClientBounds.Width / 2 - (int)game.menuFont.MeasureString(label).X / 2;
            y_position = 150;
            action = MenuItem.ItemActions.Exit;
            item = new MenuItem(game, this, new Vector2(x_position, y_position), label, action);
            SceneComponents.Add(item);

            game.Components.Add(this);
            resMenu = new ResolutionMenu(game);
            resMenu.baseScene = this;
        }

    }
}
