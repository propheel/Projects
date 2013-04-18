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


namespace WarGame
{
    /// <summary>
    /// This is a class representing the health points bar
    /// </summary>
    public class HealthPointsBar : Microsoft.Xna.Framework.DrawableGameComponent
    {
        /// <summary>
        /// Scene in which the health points bar appears
        /// </summary>
        MainScene scene;

        /// <summary>
        /// Game object which owns the health points bar
        /// </summary>
        GameObject owner;

        /// <summary>
        /// Position of the health points bar
        /// </summary>
        Vector2 position;

        /// <summary>
        /// Texture representing the health points bar
        /// </summary>
        Texture2D texture;

        /// <summary>
        /// Dimensions of the health points bar
        /// </summary>
        int baseWidth = 40, baseHeight = 10;

        /// <summary>
        /// Color of the health points bar depend on the health status of the owner
        /// </summary>
        Color healthyColor = Color.Green, mediumColor = Color.Orange, dangerColor = Color.Red;

        /// <summary>
        /// Constructor of the health points bar
        /// </summary>
        /// <param name="_owner">Owner of the health points bar</param>
        /// <param name="_game">Game which the health points bar is set in</param>
        public HealthPointsBar(GameObject _owner, WarGame _game, MainScene _scene)
            : base(_game)
        {
            owner = _owner;
            scene = _scene;

            Hide();

            DrawOrder = (int)WarGame.DrawingOrder.Sprite;
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
            if (owner.HealthPoints <= 0.05f)
                return;

            // calculating projected position
            Vector3 projectedPosition =
                ((WarGame)base.Game).Graphics.GraphicsDevice.Viewport.Project(
                owner.Position, ((WarGame)base.Game).Camera.Projection,
                ((WarGame)base.Game).Camera.View,
                ((WarGame)base.Game).Camera.World);

            position = new Vector2(projectedPosition.X, projectedPosition.Y) - 
                new Vector2(20,20);

            // generating the bar texture
            texture = new Texture2D(((WarGame)base.Game).Graphics.GraphicsDevice, (int)((int)(baseWidth * owner.HealthPoints)), baseHeight);

            Color[] colorData = new Color[baseHeight * (int)(baseWidth * owner.HealthPoints)];

            // selecting the colour
            Color currentColor;

            if ((owner.HealthPoints >= 0.7f) && (owner.HealthPoints <= 1f))
                currentColor = healthyColor;

            else if ((owner.HealthPoints >= 0.3) && (owner.HealthPoints < 0.7f))
                currentColor = mediumColor;

            else
                currentColor = dangerColor;

            for (int i = 0; i < baseHeight * (int)(baseWidth * owner.HealthPoints); i++)
                colorData[i] = currentColor;

            // setting the texture
            texture.SetData<Color>(colorData);

            base.Update(gameTime);
        }

        /// <summary>
        /// Enables health points bar self-drawing
        /// </summary>
        /// <param name="gameTime">Snapshot of time</param>
        public override void Draw(GameTime gameTime)
        {
            if (owner.HealthPoints <= 0.05f)
                return;

            if (texture != null)
            {
                ((WarGame)base.Game).Sprite.Begin();
                ((WarGame)base.Game).Sprite.Draw(texture,
                    position, Color.White);
                ((WarGame)base.Game).Sprite.End();

                ((WarGame)base.Game).Graphics.GraphicsDevice.BlendState = BlendState.Opaque;
                ((WarGame)base.Game).Graphics.GraphicsDevice.DepthStencilState = DepthStencilState.Default;
            }
            base.Draw(gameTime);
        }

        /// <summary>
        /// Shows health points bar
        /// </summary>
        public void Show()
        {
            Visible = Enabled = true;
        }

        /// <summary>
        /// Hides health points bar
        /// </summary>
        public void Hide()
        {
            Visible = Enabled = false;
        }

        /// <summary>
        /// Health points bar destructor
        /// </summary>
        public void Destroy()
        {
            scene.SceneComponents.Remove(this);
            Visible = Enabled = false;
            Dispose();
        }
    }
}
