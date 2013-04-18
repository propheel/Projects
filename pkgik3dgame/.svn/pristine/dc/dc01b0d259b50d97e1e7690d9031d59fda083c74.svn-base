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
    /// This is a representation of the ground in the game
    /// </summary>
    public class GroundElement : GameObject
    {
        /// <summary>
        /// Dimensions of the ground element.
        /// </summary>
        int widthOfGroundElement = 500, heightOfGroundElement = 500;

        #region Getters and Setters
        public int WidthOfGroundElement
        {
            get { return widthOfGroundElement; }
        }
        public int HeightOfGroundElement
        {
            get { return heightOfGroundElement; }
        }
        #endregion

        /// <summary>
        /// Constructor of the ground element
        /// </summary>
        /// <param name="game">Game in which the ground element is set</param>
        /// <param name="_position">Position of the ground element</param>
        public GroundElement(WarGame game, Vector3 _position, MainScene _scene)
            : base(game, _scene, _position)
        {
            gameObjectModel = game.groundElementModel;
            scale = new Vector3(widthOfGroundElement, 0.1f, heightOfGroundElement);
            maxSpeed = 0f;
            maxForce = 10 * maxSpeed;
            mass = 200.0f;
            currentSB = SBNames.None;
            isModelTextured = true;
            modelTexture = game.moonTexture;
            scene = _scene;
            boundingBox = new Bbox(this, game, scene);
            scene.SceneComponents.Add(boundingBox);
        }
    }
}
