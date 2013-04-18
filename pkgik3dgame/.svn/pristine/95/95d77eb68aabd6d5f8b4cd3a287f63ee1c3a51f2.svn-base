#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
#endregion

namespace WarGame
{
    public class ResolutionMenu : MenuScene
    {
        #region Fields
        /// <summary>
        /// List of available resolutions.
        /// </summary>
        List<Vector2> availableResoultions = new List<Vector2>();

        /// <summary>
        /// Number of resolutions to present.
        /// </summary>
        int numberOfResolutions = 6;
        #endregion

        #region Getters and Setters
        public Vector2  CrrentResolution
        {
            get { return availableResoultions[activeItemNumber]; }
        }
        #endregion

        public ResolutionMenu(WarGame game)
            : base(game)
        {
            backgroundImage = game.menuBackgroundTexture;

            int x_position;
            string label;
            int y_position = 0;
            MenuItem item;
            MenuItem.ItemActions action;

            foreach (DisplayMode d in GraphicsAdapter.DefaultAdapter.SupportedDisplayModes)
                availableResoultions.Add(new Vector2(d.Width, d.Height));

            List<Vector2> highestResolutions = new List<Vector2>();

            do
            {
                Vector2 maxValue = new Vector2(float.MinValue, float.MinValue);

                foreach (Vector2 v in availableResoultions)
                {
                    if (v.X + v.Y > maxValue.X + maxValue.Y)
                        maxValue = v;
                }

                availableResoultions.Remove(maxValue);

                if (!highestResolutions.Contains(maxValue))
                    highestResolutions.Add(maxValue);

            } while (highestResolutions.Count < numberOfResolutions);

            availableResoultions = highestResolutions;

            foreach (Vector2 v in availableResoultions)
            {
                label = v.X.ToString() + " X " + v.Y.ToString();
                x_position = x_position = game.Window.ClientBounds.Width / 2 - (int)game.menuFont.MeasureString(label).X / 2;
                y_position += 50;
                action = MenuItem.ItemActions.SetResolution;
                item = new MenuItem(game, this, new Vector2(x_position, y_position), label, action);
                SceneComponents.Add(item);
            }

            activeItemNumber = 0;
            (SceneComponents[activeItemNumber] as MenuItem).Activate();
            game.Components.Add(this);
            game.SetResolution((int)availableResoultions[activeItemNumber].X, (int)availableResoultions[activeItemNumber].Y);

            Hide();
        }
    }
}
