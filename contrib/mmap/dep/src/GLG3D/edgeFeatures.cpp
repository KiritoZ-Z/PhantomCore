/**
 @file edgeFeatures.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-20
 @edited  2004-04-25
 */

#include "GLG3D/edgeFeatures.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/VAR.h"

namespace G3D {

void drawFeatureEdges(RenderDevice* renderDevice, const PosedModel::Ref& model, float creaseAngle) {

    float dotThreshold = max(cosf(creaseAngle), 0.0f);

    bool drawCreases = (creaseAngle <= pi() / 2);

    const Vector3 wsEye = renderDevice->getCameraToWorldMatrix().translation;

    const Array<MeshAlg::Edge>&     edgeArray   = model->weldedEdges();
    const Array<Vector3>&           faceNormal  = model->objectSpaceFaceNormals(drawCreases);
    const Array<MeshAlg::Face>&     faceArray   = model->weldedFaces();
    const Array<Vector3>&           vertexArray = model->objectSpaceGeometry().vertexArray;

    // Work in the object space of the model so we don't
    // have to transform the geometry. 
    const CoordinateFrame           cframe      = model->coordinateFrame();
    const Vector3                   eye         = cframe.pointToObjectSpace(wsEye);

    // Compute backfaces
    static Array<bool> backface;
    backface.resize(faceNormal.size(), DONT_SHRINK_UNDERLYING_ARRAY);
    for (int f = faceNormal.size() - 1; f >= 0; --f) {
        // View vector
        const Vector3 V = (eye - vertexArray[faceArray[f].vertexIndex[0]]);
        backface[f] = faceNormal[f].dot(V) < 0;
    }

    // Find contour edges
    static Array<Vector3> cpuVertexArray;
    cpuVertexArray.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
    for (int e = edgeArray.size() - 1; e >= 0; --e) {
        const MeshAlg::Edge& edge = edgeArray[e];

        const int f0 = edge.faceIndex[0];
        const int f1 = edge.faceIndex[1];

        if (
            // Boundaries:
            (f0 == MeshAlg::Face::NONE) ||
            (f1 == MeshAlg::Face::NONE) ||

            // Contours:
            (backface[f0] ^ backface[f1]) ||

            // Front-face creases:
            (drawCreases &&
             (faceNormal[f0].dot(faceNormal[f1]) <= dotThreshold) && 
             ! (backface[f0] && backface[f1]))) {

            cpuVertexArray.append(
                vertexArray[edge.vertexIndex[0]], 
                vertexArray[edge.vertexIndex[1]]);
        }
    }

    VARAreaRef varArea = VARArea::create(cpuVertexArray.size() * sizeof(Vector3));
    VAR gpuVertexArray(cpuVertexArray, varArea);

    renderDevice->pushState();
        renderDevice->setObjectToWorldMatrix(cframe);
        renderDevice->beginIndexedPrimitives();
            renderDevice->setVertexArray(gpuVertexArray);
            renderDevice->sendSequentialIndices(RenderDevice::LINES, cpuVertexArray.size());
        renderDevice->endIndexedPrimitives();
    renderDevice->popState();
}

}
