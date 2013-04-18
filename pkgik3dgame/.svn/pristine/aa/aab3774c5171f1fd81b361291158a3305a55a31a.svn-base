#region Using Statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
#endregion

namespace WarGame.GameObjects
{
    using SBNames = SteeringBehaviors.SteeringBehaviorsNames;

    /// <summary>
    /// This is a game component that implements IUpdateable.
    /// </summary>
    public class AlienBuilding : GameObject
    {
        public AlienBuilding(WarGame game, Vector3 _position, MainScene _scene)
            : base(game, _scene, _position)
        {
            gameObjectModel = game.alienBaseModel;
            scale = new Vector3(0.5f, 0.5f, 0.5f);
            maxForce = 0;
            maxSpeed = 0;
            mass = 1000;
            currentSB = SBNames.None;
            currentWorld = Matrix.CreateScale(scale) * Matrix.CreateWorld(Position, lastNonZeroVelocity, Vector3.Up);


            boundingBox = new Bbox(this, game, scene);
            scene.SceneComponents.Add(boundingBox);

            boundingBox.RecalculateBoundingBox();
        }
    }
}
