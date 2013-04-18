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
    /// This is a class representing human main base
    /// </summary>
    public class Building : GameObject
    {
        /// <summary>
        /// Constructor of the building
        /// </summary>
        /// <param name="game">Game where the building is set</param>
        /// <param name="_position">Position of the building</param>
        public Building(WarGame game, Vector3 _position, MainScene _scene)
            : base(game, _scene, _position)
        {
            gameObjectModel = game.castleModel;
            scale = new Vector3(0.001f, 0.001f, 0.001f);
            maxSpeed = 0f;
            maxForce = 0f;
            mass = 1000f;
            currentSB = SBNames.None;
            currentWorld = Matrix.CreateScale(scale) * Matrix.CreateWorld(Position, lastNonZeroVelocity, Vector3.Up);

            HealthPoints = 0.05f;
            bar.Show();

            boundingBox = new Bbox(this, game, scene);
            scene.SceneComponents.Add(boundingBox);
            boundingBox.RecalculateBoundingBox();

            // show the bounding box if the game is in debug mode
            if (scene.IsInDebugMode)
                DebugModeToggle();
        }

        /// <summary>
        /// Enables self-updating of the building
        /// </summary>
        /// <param name="gameTime">Snapshot of time.</param>
        public override void Update(GameTime gameTime)
        {
            if (scene.Fighters.Count <= scene.MaxNumberOfFighters)
            {
                HealthPoints += 0.01f;

                if (HealthPoints >= 1)
                {
                    Fighter f = null;
                    do
                    {
                        if (f != null)
                        {
                            f.Destroy();
                        }
                        Vector3 randomPosition  = new Vector3(Position.X + (float)WarGame.RandomGenerator.NextDouble() * 40.0f - 20.0f,
                                                              scene.GroundYAxisPosition + 1.0f,
                                                              Position.Z + (float)WarGame.RandomGenerator.NextDouble() * 40.0f - 20.0f);

                        f = new Fighter((WarGame)base.Game, randomPosition, scene);

                    } while (f.CheckForCollisions() != null);

                    HealthPoints = 0.0f;
                    ((WarGame)base.Game).Sound.BoooiiinnngSoundInstance.Play();
                    scene.SceneComponents.Add(f);
                    scene.SceneObjects.Add(f);
                    scene.Fighters.Add(f);
                }
            }
            base.Update(gameTime);
        }
    }
}
