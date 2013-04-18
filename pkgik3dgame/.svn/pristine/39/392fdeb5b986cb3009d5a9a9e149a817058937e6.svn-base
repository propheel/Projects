#region Using Statements
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
    // This is a compilation of useful functions concerning Vector3 class
    public static class V3Extensions
    {
        /// <summary>
        /// Truncates the vector in accordance to the maximal length
        /// </summary>
        /// <param name="vectorToTruncate">Vector to truncate</param>
        /// <param name="maxLengthOfVector">Maximal legal length of the vector</param>
        /// <returns>Truncated vector</returns>
        public static Vector3 Truncate(Vector3 vectorToTruncate, float maxLengthOfVector)
        {
            if (vectorToTruncate.Length() > maxLengthOfVector)
                vectorToTruncate = Vector3.Normalize(vectorToTruncate) * maxLengthOfVector;

            return vectorToTruncate;
        }

        /// <summary>
        /// Creates vector with all values set to not a number value.
        /// </summary>
        /// <returns>NaN vector</returns>
        public static Vector3 CreateInvalidVector()
        {
            return new Vector3(float.NaN, float.NaN, float.NaN);
        }

        /// <summary>
        /// Checks if the vector is invalid or not
        /// </summary>
        /// <param name="v">Vector to validate</param>
        /// <returns>true - valid, false - ivalid</returns>
        public static bool CheckIfVectorIsValid(Vector3 v)
        {
            if (v.X != float.NaN && v.Y != float.NaN && v.Z != float.NaN)
                return true;

            return false;
        }

        /// <summary>
        /// Converts Vector2 to Vector3
        /// </summary>
        /// <param name="v">Vector to convert</param>
        /// <returns>Converted vector</returns>
        public static Vector3 V3fromV2(Vector2 v)
        {
            return new Vector3(v.X, 0, v.Y);
        }

        /// <summary>
        /// Converts Vector3 to Vector2
        /// </summary>
        /// <param name="v">Vector to converts</param>
        /// <returns>Converted vector</returns>
        public static Vector2 V2fromV3(Vector3 v)
        {
            return new Vector2(v.X, v.Z);
        }
    }
}
