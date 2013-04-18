#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
#endregion

namespace WarGame.Controllers
{
    /// <summary>
    /// This is a class used to control the game sound.
    /// </summary>
    public class SoundController : Microsoft.Xna.Framework.GameComponent
    {
        #region Variables
        /// <summary>
        /// Shooting sound
        /// </summary>
        SoundEffect shootSound;
        SoundEffectInstance shootSoundInstance;
        
        /// <summary>
        /// Main music theme
        /// </summary>
        SoundEffect themeSound;
        SoundEffectInstance themeSoundInstance;

        /// <summary>
        /// Click sound
        /// </summary>
        SoundEffect clickSound;
        SoundEffectInstance clickSoundInstance;

        /// <summary>
        /// Popping sound
        /// </summary>
        SoundEffect popSound;
        SoundEffectInstance popSoundInstance;

        /// <summary>
        /// Booing sound
        /// </summary>
        SoundEffect boooiiinnngSound;
        SoundEffectInstance boooiiinnngSoundInstance;
        #endregion

        #region Getters and Setters
        public SoundEffectInstance ShootSoundInstance
        {
            get
            {
                return shootSoundInstance;
            }
        }
        public SoundEffectInstance ThemeSoundInstance
        {
            get
            {
                return themeSoundInstance;
            }
        }
        public SoundEffectInstance ClickSoundInstance
        {
            get
            {
                return clickSoundInstance;
            }
        }
        public SoundEffectInstance PopSoundInstance
        {
            get
            {
                return popSoundInstance;
            }
        }
        public SoundEffectInstance BoooiiinnngSoundInstance
        {
            get
            {
                return boooiiinnngSoundInstance;
            }
        }
        #endregion

        public SoundController(Game game)
            : base(game)
        {
            shootSound = game.Content.Load<SoundEffect>("Sounds\\shoot");
            shootSoundInstance = shootSound.CreateInstance();

            themeSound = game.Content.Load<SoundEffect>("Sounds\\starwars");
            themeSoundInstance = themeSound.CreateInstance();

            clickSound = game.Content.Load<SoundEffect>("Sounds\\click");
            clickSoundInstance = clickSound.CreateInstance();

            popSound = game.Content.Load<SoundEffect>("Sounds\\pop");
            popSoundInstance = popSound.CreateInstance();

            boooiiinnngSound = game.Content.Load<SoundEffect>("Sounds\\boooiiinnng");
            boooiiinnngSoundInstance = boooiiinnngSound.CreateInstance();

            themeSoundInstance.IsLooped = true;
        }

        /// <summary>
        /// Allows the game component to update itself.
        /// </summary>
        /// <param name="gameTime">Provides a snapshot of timing values.</param>
        public override void Update(GameTime gameTime)
        {
            // TODO: Add your update code here

            bool isMenuEnabled = ((WarGame)base.Game).CurrentScene is MainMenu || ((WarGame)base.Game).CurrentScene is ResolutionMenu;
            themeSoundInstance.Stop();
            /*
            if (themeSoundInstance.State == SoundState.Stopped && !isMenuEnabled)
                themeSoundInstance.Play();
            else if (themeSoundInstance.State == SoundState.Paused && !isMenuEnabled)
                themeSoundInstance.Resume();
            else if (themeSoundInstance.State == SoundState.Playing && isMenuEnabled)
                themeSoundInstance.Pause();
            */
            base.Update(gameTime);
        }
    }
}
