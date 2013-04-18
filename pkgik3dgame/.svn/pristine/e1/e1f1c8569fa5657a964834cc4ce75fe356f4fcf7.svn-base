#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
#endregion

namespace WarGame
{
    using SBNames = SteeringBehaviors.SteeringBehaviorsNames;

    /// <summary>
    /// Class representing the fighter
    /// </summary>
    public class Fighter : GameObject
    {
        #region Fields
        /// <summary>
        /// Alien craft attacked by the figter
        /// </summary>
        AlienCraft targetToAttack;

        /// <summary>
        /// Value used during the attacks
        /// </summary>
        public bool IsEscaping = false;

        /// <summary>
        /// Bullet shot by the fighter
        /// </summary>
        Bullet bullet;
        #endregion

        #region Getters and Setters

        public AlienCraft TargetToAttack
        {
            get { return targetToAttack; }
            set { targetToAttack = value; }
        }
        public Bullet Bullet
        {
            set { bullet = value; }
        }

        #endregion

        /// <summary>
        /// Constructor of the fighter
        /// </summary>
        /// <param name="game">Game in which the fighter is set</param>
        /// <param name="_position">Initial position of the fighter</param>
        public Fighter(WarGame game, Vector3 _position, MainScene _scene)
            : base(game, _scene, _position)
        {
            gameObjectModel = game.spaceShipModel;
            scale = new Vector3(0.5f, 0.5f, 0.5f);

            maxSpeed = 0.5f;
            maxForce = 10 * maxSpeed;
            mass = 20.0f;
            currentSB = SBNames.None;
            currentWorld = Matrix.CreateScale(scale) * Matrix.CreateWorld(Position, lastNonZeroVelocity, Vector3.Up);

            boundingBox = new Bbox(this, game, scene);
            scene.SceneComponents.Add(boundingBox);
            boundingBox.RecalculateBoundingBox();

            // show the bounding box if the game is in debug mode
            if (scene.IsInDebugMode)
                DebugModeToggle();
        }

        /// <summary>
        /// Enables self-updating of the fighter
        /// </summary>
        /// <param name="gameTime">Snapshot of time</param>
        public override void Update(GameTime gameTime)
        {            
            switch(currentSB)
            {
                case SBNames.Attack:
                    {
                        force = V3Extensions.Truncate(SteeringBehaviors.Attack(this, targetToAttack, scene.Fighters), maxForce);
                        if (CanShoot()) Shoot();
                    }
                    break;


                case SBNames.SeekWithSeparation:
                    {
                        force = V3Extensions.Truncate(SteeringBehaviors.SeekWithSeparation(this, Target, scene.Fighters), maxForce);
                    }
                    break;
        }
            base.Update(gameTime);
        }

        /// <summary>
        /// Checks if the fighter can shoot
        /// </summary>
        /// <returns>true - can shoot, false - cannot</returns>
        public bool CanShoot()
        {
            if (bullet == null)
                return true;

            return false;
        }

        /// <summary>
        /// Shoots the bullet
        /// </summary>
        public void Shoot()
        {
            ((WarGame)base.Game).Sound.ShootSoundInstance.Play();
            bullet = new Bullet(this, base.Game, scene);
        }
    }
}
