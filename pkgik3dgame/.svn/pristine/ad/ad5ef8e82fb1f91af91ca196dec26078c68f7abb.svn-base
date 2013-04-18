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
    /// This is a class representing single element in the Menu
    /// </summary>
    public class MenuItem : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// Names of actions which can be taken after the enter is pressed.
        /// </summary>
        public enum ItemActions { None, NewGame, SetResolution, GoToResolutionMenu, Exit }

        #region Fields
        /// <summary>
        /// SMenucene in which the item is used.
        /// </summary>
        MenuScene scene;

        /// <summary>
        /// Position of the item.
        /// </summary>
        public Vector2 Position { get; set; }

        /// <summary>
        /// Label of the item.
        /// </summary>
        string label;

        /// <summary>
        /// Is the item active or not.
        /// </summary>
        bool isActive;

        /// <summary>
        /// Action attached to this item.
        /// </summary>
        ItemActions action;

        /// <summary>
        /// Active color.
        /// </summary>
        Color activeColor = Color.Red;

        /// <summary>
        /// Non-active color.
        /// </summary>
        Color nonActiveColor = Color.Yellow;
        #endregion

        #region Getters and Setters
        public ItemActions Action
        {
            get { return action; }
        }
        public string Label
        {
            get { return label; }
        }
        #endregion

        /// <summary>
        /// Constructor of the Menu item.
        /// </summary>
        /// <param name="game">War game in which the item is used</param>
        /// <param name="_scene">Menu in which the item is used</param>
        /// <param name="_position">Position of the item</param>
        /// <param name="_label">Label of the item</param>
        /// <param name="_action">Action attached to this item</param>
        public MenuItem(WarGame game, MenuScene _scene, Vector2 _position, string _label, ItemActions _action)
            : base(game)
        {
            scene = _scene;
            Position = _position;
            label = _label;
            action = _action;

            DrawOrder = (int)WarGame.DrawingOrder.Sprite;
        }

        /// <summary>
        /// Enables drawing of the item
        /// </summary>
        /// <param name="gameTime">Snapshot of time.</param>
        public override void Draw(GameTime gameTime)
        {
            Color currentColor;

            if (isActive) currentColor = activeColor;
            else currentColor = nonActiveColor;

            ((WarGame)base.Game).Sprite.Begin();
            ((WarGame)base.Game).Sprite.DrawString(((WarGame)base.Game).menuFont, label, Position, currentColor);
            ((WarGame)base.Game).Sprite.End();

            ((WarGame)base.Game).Graphics.GraphicsDevice.BlendState = BlendState.Opaque;
            ((WarGame)base.Game).Graphics.GraphicsDevice.DepthStencilState = DepthStencilState.Default;

            base.Draw(gameTime);
        }

        /// <summary>
        /// Activates the item.
        /// </summary>
        public void Activate()
        {
            isActive = true;
        }

        /// <summary>
        /// Disactivates the item.
        /// </summary>
        public void Disactivate()
        {
            isActive = false;
        }
    }
}
