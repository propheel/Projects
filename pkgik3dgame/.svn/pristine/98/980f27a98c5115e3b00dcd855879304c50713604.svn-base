#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
#endregion

namespace WarGame
{
    using SBNames = SteeringBehaviors.SteeringBehaviorsNames;

    /// <summary>
    /// Class representing alien craft
    /// </summary>
    public class AlienCraft : GameObject
    {
        /// <summary>
        /// How long is the health bar visible for
        /// </summary>
        TimeSpan healthBarActivityTime = new TimeSpan();

        /// <summary>
        /// How long can the health bar visible for
        /// </summary>
        TimeSpan healthBarMaxActivityTime = TimeSpan.FromSeconds(2);

        /// <summary>
        /// Constructor of alien craft
        /// </summary>
        /// <param name="game">Game in which the alien craft is set</param>
        public AlienCraft(WarGame game, MainScene _scene, Vector3 _position)
            : base(game, _scene, _position)
        {
            gameObjectModel = game.alienCraftModel;
            scale = new Vector3(0.5f, 0.5f, 0.5f);
            maxSpeed = 0.05f;
            maxForce = 10 * maxSpeed;
            mass = 20.0f;
            currentSB = SBNames.WanderReadyToAttack;
            currentWorld = Matrix.CreateScale(scale) * Matrix.CreateWorld(Position, lastNonZeroVelocity, Vector3.Up);

            boundingBox = new Bbox(this, game, scene);
            scene.SceneComponents.Add(boundingBox);
            boundingBox.RecalculateBoundingBox();
        }

        /// <summary>
        /// Enables alien craft to update itself
        /// </summary>
        /// <param name="gameTime">Snapshot of time.</param>
        public override void Update(GameTime gameTime)
        {
            switch (currentSB)
            {
                case SBNames.Wander:
                    {
                        force = V3Extensions.Truncate(SteeringBehaviors.Wander(this, scene.AlienCrafts), maxForce);
                    }
                    break;

                case SBNames.WanderReadyToAttack:
                    {
                        force = V3Extensions.Truncate(SteeringBehaviors.WanderReadyToAttack(this, scene.AlienCrafts, scene.Fighters), maxForce);
                    }
                    break;
            }

            if (bar.Visible == true)
                healthBarActivityTime += gameTime.ElapsedGameTime;

            if (healthBarActivityTime >= healthBarMaxActivityTime)
            {
                bar.Hide();
                healthBarActivityTime = new TimeSpan();
            }
            
            base.Update(gameTime);
        }

        /// <summary>
        /// Reaction of the alien craft if it gets shot
        /// </summary>
        public void GetHurt()
        {
            HealthPoints -= 0.1f;
            bar.Show();

            // what if the alien craft dies
            if (HealthPoints <= 0)
            {
                foreach(Fighter f in scene.Fighters)
                    if (f.TargetToAttack == this)
                    {
                        f.CurrentSB = SBNames.SeekWithSeparation;
                        f.Target = V3Extensions.V3fromV2(new Vector2(Position.X, Position.Z));
                    }

                Destroy();
            }
        }
    }
}
