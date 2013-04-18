#region Using statements
using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
#endregion

namespace WarGameContentPipeline
{
    /// <summary>
    /// This class will be instantiated by the XNA Framework Content Pipeline
    /// to apply custom processing to content data, converting an object of
    /// type TInput to TOutput. The input and output types may be the same if
    /// the processor wishes to alter data without changing its type.
    ///
    /// This should be part of a Content Pipeline Extension Library project.
    ///
    /// TODO: change the ContentProcessor attribute to specify the correct
    /// display name for this processor.
    /// </summary>
    [ContentProcessor(DisplayName = "WarGameContentPipeline.BoundingBoxProcessor")]
    public class BoundingBoxProcessor : ModelProcessor
    {
        // base max and min points
        Vector3 maxPoint = new Vector3(float.MinValue, float.MinValue, float.MinValue);
        Vector3 minPoint = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);

        public override ModelContent Process(NodeContent input, ContentProcessorContext context)
        {
            // bounding box calculation
            CalculateBoundingBox(input);

            // processing model using default processor
            ModelContent content = base.Process(input, context);

            // adding the bound box as a tag of the mesh
            content.Tag = new BoundingBox(minPoint, maxPoint);

            return content;
        }

        public void CalculateBoundingBox(NodeContent node)
        {
            MeshContent mesh = node as MeshContent;

            if (mesh != null)
            {
                // calculating max and min points coordinates
                foreach (Vector3 vertex in mesh.Positions)
                {
                    if (vertex.X < minPoint.X) minPoint.X = vertex.X;
                    if (vertex.Y < minPoint.Y) minPoint.Y = vertex.Y;
                    if (vertex.Z < minPoint.Z) minPoint.Z = vertex.Z;

                    if (vertex.X > maxPoint.X) maxPoint.X = vertex.X;
                    if (vertex.Y > maxPoint.Y) maxPoint.Y = vertex.Y;
                    if (vertex.Z > maxPoint.Z) maxPoint.Z = vertex.Z;
                }
            }

            else
            {
                // calling the function recursively for all the children nodes
                foreach (NodeContent childNode in node.Children)
                {
                    this.CalculateBoundingBox(childNode);
                }
            }
        }
    }
}