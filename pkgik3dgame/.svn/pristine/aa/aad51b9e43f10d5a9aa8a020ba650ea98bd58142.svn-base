#region Using statements
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
#endregion

namespace WarGame
{
    /// <summary>
    /// This is a class representing steering behaviors
    /// </summary>
    public static class SteeringBehaviors
    {
        #region Steering behaviors names
        /// <summary>
        /// Names for SB
        /// </summary>
        public enum SteeringBehaviorsNames
        {
            None,
            Seek,
            Flee,
            SeekWithSeparation,
            Wander,
            Attack,
            WanderReadyToAttack
        }
        #endregion

        /// <summary>
        /// Seek steering behavior
        /// </summary>
        /// <param name="caller">Game object which calls the seek SB</param>
        /// <param name="target">Target seeked by the caller</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 Seek(GameObject caller, Vector3 target)
        {
            return V3Extensions.V3fromV2(Vector2.Normalize(V3Extensions.V2fromV3(target) - V3Extensions.V2fromV3(caller.Position)) * caller.MaxSpeed - V3Extensions.V2fromV3(caller.Velocity));
        }

        /// <summary>
        /// Flee steering behavior
        /// </summary>
        /// <param name="caller">Game object which calls the flee SB</param>
        /// <param name="target">Target from which the caller flies</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 Flee(GameObject caller, Vector3 target)
        {
            return V3Extensions.V3fromV2(Vector2.Normalize(V3Extensions.V2fromV3(caller.Position) - V3Extensions.V2fromV3(target)) * caller.MaxSpeed - V3Extensions.V2fromV3(caller.Velocity));
        }

        /// <summary>
        /// Separation stering behavior
        /// </summary>
        /// <param name="caller">Game object which calls the wander SB</param>
        /// <param name="objects">List of fighters that we want to separate from</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 Separation(GameObject caller, List<Fighter> objects)
        {
            Vector3 separationForce = Vector3.Zero;

            if (objects != null)
            {
                foreach (GameObject o in objects)
                {
                    if (caller != o)
                    {
                        Vector3 distanceVector = V3Extensions.V3fromV2(V3Extensions.V2fromV3(caller.Position) - V3Extensions.V2fromV3(o.Position));
                        separationForce += Vector3.Normalize(distanceVector) / distanceVector.Length();
                    }
                }
            }

            return separationForce * caller.MaxSpeed;
        }

        /// <summary>
        /// Separation stering behavior
        /// </summary>
        /// <param name="caller">Game object which calls the wander SB</param>
        /// <param name="objects">List of alien crafts that we want to separate from</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 Separation(GameObject caller, List<AlienCraft> objects)
        {
            Vector3 separationForce = Vector3.Zero;

            if (objects != null)
            {
                foreach (GameObject o in objects)
                {
                    if (caller != o && Vector3.Distance(caller.Position, o.Position) < 1)
                    {
                        Vector3 distanceVector = V3Extensions.V3fromV2(V3Extensions.V2fromV3(caller.Position) - V3Extensions.V2fromV3(o.Position));
                        separationForce += Vector3.Normalize(distanceVector); // distanceVector.Length();
                    }
                }
            }

            return separationForce * caller.MaxSpeed;
        }

        /// <summary>
        /// Seek steering behavior combined with the separation
        /// </summary>
        /// <param name="caller">Game object which calls this SB</param>
        /// <param name="target">Target seeked by the caller</param>
        /// <param name="objects">List of fighters to separate from</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 SeekWithSeparation(Fighter caller, Vector3 target, List<Fighter> objects)
        {
            if ((caller.Position - target).Length() > 5)
                return Seek(caller, target) + 2 * Separation(caller, objects);
            else if (Separation(caller, objects).Length() < 0.1f)
                return Vector3.Zero;
            else
                return Seek(caller, target) + 6 * Separation(caller, objects);
        }

        /// <summary>
        /// Seek steering behavior combined with the separation
        /// </summary>
        /// <param name="caller">Game object which calls this SB</param>
        /// <param name="target">Target seeked by the caller</param>
        /// <param name="objects">List of fighters to separate from</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 SeekWithSeparation(AlienCraft caller, Vector3 target, List<AlienCraft> objects)
        {
            if ((caller.Position - target).Length() > 5)
                return Seek(caller, target) + 2 * Separation(caller, objects);
            else if (Separation(caller, objects).Length() < 0.1f)
                return Vector3.Zero;
            else
                return Seek(caller, target) + 6 * Separation(caller, objects);
        }

        /// <summary>
        /// Wander steering behavior
        /// </summary>
        /// <param name="caller">Game object which calls the wander SB</param>
        /// <returns>Force acting at the caller</returns>
        public static Vector3 Wander(AlienCraft caller, List<AlienCraft> friends)
        {
            float wanderR = 2.0f;
            float wanderD = 4.0f;

            caller.WanderChange += (float)WarGame.RandomGenerator.NextDouble() - 0.5f;

            Vector3 circleloc = caller.Velocity;
            if (circleloc.LengthSquared() > 0)
                circleloc.Normalize();
            circleloc *= wanderD;
            circleloc += caller.Position;

            Vector2 circleOffSet = new Vector2(wanderR * (float)Math.Cos(caller.WanderChange), wanderR * (float)Math.Sin(caller.WanderChange));
            Vector3 target = new Vector3(circleloc.X + circleOffSet.X, circleloc.Y, circleloc.Z + circleOffSet.Y);

            caller.Target = target;

            Vector3 seekForce = Seek(caller, caller.Target);

            return seekForce + 2 * Separation(caller, friends);
        }

        /// <summary>
        /// Wander ready to attack steering behaviour
        /// </summary>
        /// <param name="caller">GameObject which calls the ready to attack SB</param>
        /// <param name="objects">List of fighters to attack</param>
        /// <returns>Force acting on the game object</returns>
        public static Vector3 WanderReadyToAttack (AlienCraft caller, List<AlienCraft> friends, List<Fighter> targets)
        {
            int FOV = 50;

            Fighter fighterToSeek = null;
            float minDistance = float.MaxValue;

            foreach (Fighter f in targets)
            {
                float d = (caller.Position - f.Position).Length();

                if (d < FOV)
                    if (d < minDistance)
                    {
                        minDistance = d;
                        fighterToSeek = f;
                    }
            }

            if (fighterToSeek == null)
                return Wander(caller, friends);
            else
                return SeekWithSeparation(caller, fighterToSeek.Position, friends);

        }

        /// <summary>
        /// Atack steering behavior
        /// </summary>
        /// <param name="caller">Game object which calles atack SB</param>
        /// <param name="target">Target of the attack</param>
        /// <returns>Force acting on caller</returns>
        public static Vector3 Attack(Fighter caller, AlienCraft target, List<Fighter> friends)
        {
            float minimalDistanceToTargetSquared = 5.0f * 5.0f;
            float shootingDistanceSquared = 15.0f * 15.0f;
            float maximalDistaneToTargetSquared = 30.0f * 30.0f;

            // if you're not escaping
            if (!caller.IsEscaping)
            {
                Vector3 seekingForce = SeekWithSeparation(caller, target.Position, friends);
                Vector3 separationForce = Vector3.Zero;

                // if you're to close start escaping
                if (Vector3.DistanceSquared(caller.Position, target.Position) < minimalDistanceToTargetSquared)
                {
                    caller.IsEscaping = true;
                }

                // if you're in the shooting range slow down to minimum
                if (Vector3.DistanceSquared(caller.Position, target.Position) < shootingDistanceSquared)
                {
                    caller.Velocity = Vector3.Zero;
                }

                return seekingForce + separationForce;
            }

            // if you're escaping
            else
            {
                // if you're far enough start shooting once again
                if (Vector3.DistanceSquared(caller.Position, target.Position) > maximalDistaneToTargetSquared)
                {
                    caller.IsEscaping = false;
                }

                return Flee(caller, target.Position);
            }
        }
    }
}
