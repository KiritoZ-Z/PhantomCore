/**
  @file IFSModel.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2006-10-30
 */ 


#include "GLG3D/RenderDevice.h"
#include "GLG3D/IFSModel.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"
#include "G3D/fileutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

// Cached between render calls
VARAreaRef  IFSModel::varArea;
IFSModelRef IFSModel::lastModel;
VAR         IFSModel::lastVertexVAR;
VAR         IFSModel::lastNormalVAR;
VAR         IFSModel::lastTexCoordVAR;


IFSModel::IFSModel() {
}


IFSModel::~IFSModel() {
}


void IFSModel::reset() {
    geometry.clear();
    indexArray.clear();
    faceArray.clear();
    vertexArray.clear();
    edgeArray.clear();
    texArray.clear();
}


IFSModelRef IFSModel::fromFile(const std::string& filename, double scale, const CoordinateFrame& cframe, const bool weld, bool removeDegenerateFaces) {
    return fromFile(filename, Vector3(scale, scale, scale), cframe, weld, removeDegenerateFaces);
}


IFSModelRef IFSModel::fromFile(const std::string& filename, const Vector3& scale, const CoordinateFrame& cframe, const bool weld, bool removeDegenerateFaces) {
    IFSModel* ret = new IFSModel();
    ret->load(filename, scale, cframe, weld, removeDegenerateFaces);
    return ret;
}


static bool close(const Vector3& v0, const Vector2& t0, const Vector3& v1, const Vector2& t1) {
    static const float eps = 0.0000001f;
    return ((v0 - v1).squaredLength() < eps) &&
           ((t0 - t1).squaredLength() < eps);
}


void IFSModel::load(const std::string& filename, const Vector3& scale, const CoordinateFrame& cframe, const bool weld, bool removeDegenerateFaces) {
    reset();

    this->filename = filename;
    load(filename, name, indexArray, geometry.vertexArray, texArray);

    debugAssert(geometry.vertexArray.size() > 0);
    debugAssert(indexArray.size() > 0);

    for (int i = 0; i < geometry.vertexArray.size(); ++i) {
        geometry.vertexArray[i] = cframe.pointToWorldSpace(geometry.vertexArray[i] * scale);
    }

    if (removeDegenerateFaces) {
        // If there are no texture coords, we always match tex coords to this.
        Vector2 dummy(0, 0);

        Array<int> old = indexArray;
        indexArray.fastClear();
        // First remove faces from the index array
        for (int i = 0; i < old.size(); i += 3) {
            int i0 = old[i];
            int i1 = old[i + 1];
            int i2 = old[i + 2];

            const Vector3& v0 = geometry.vertexArray[i0];
            const Vector3& v1 = geometry.vertexArray[i1];
            const Vector3& v2 = geometry.vertexArray[i2];

            const Vector2& t0 = (texArray.size() > 0) ? texArray[i0] : dummy;
            const Vector2& t1 = (texArray.size() > 0) ? texArray[i1] : dummy;
            const Vector2& t2 = (texArray.size() > 0) ? texArray[i2] : dummy;

            if (close(v0, t0, v1, t1) ||
                close(v0, t0, v2, t2) ||
                close(v1, t1, v2, t2)) {
                // This face contains a duplicate vertex; do not carry it forward
            } else {
                indexArray.append(i0, i1, i2);
            }
        }
    }

    MeshAlg::computeAdjacency(geometry.vertexArray, indexArray, faceArray, edgeArray, vertexArray);
    weldedFaceArray = faceArray;
    weldedEdgeArray = edgeArray;
    weldedVertexArray = vertexArray;
	
    if (weld) {
        MeshAlg::weldAdjacency(geometry.vertexArray, faceArray, edgeArray, vertexArray);
    }
	
    MeshAlg::computeNormals(geometry.vertexArray, faceArray, vertexArray, geometry.normalArray, faceNormalArray);
    MeshAlg::computeBounds(geometry.vertexArray, boundingBox, boundingSphere);

    numBoundaryEdges = MeshAlg::countBoundaryEdges(edgeArray);
    numWeldedBoundaryEdges = MeshAlg::countBoundaryEdges(weldedEdgeArray);
}


size_t IFSModel::mainMemorySize() const {

    size_t frameSize   = sizeof(MeshAlg::Geometry) + (sizeof(Vector3) + sizeof(Vector3)) * geometry.vertexArray.size();
    size_t texCoordSize = sizeof(Vector2) * texArray.size();
    size_t indexSize   = indexArray.size() * sizeof(int);
    size_t faceSize    = faceArray.size() * sizeof(MeshAlg::Face);
    size_t valentSize  = vertexArray.size() * sizeof(Array<MeshAlg::Vertex>);
    for (int i = 0; i < vertexArray.size(); ++i) {
        valentSize += vertexArray[i].faceIndex.size() * sizeof(int);
        valentSize += vertexArray[i].edgeIndex.size() * sizeof(int);
    }

    size_t edgeSize    = edgeArray.size() * sizeof(MeshAlg::Edge);

    return sizeof(IFSModel) + frameSize + indexSize + faceSize + valentSize + edgeSize + texCoordSize;
}


PosedModel::Ref IFSModel::pose(const CoordinateFrame& cframe, bool perVertexNormals) {
    return new PosedIFSModel(this, cframe, perVertexNormals, GMaterial(), false);
}

PosedModel::Ref IFSModel::pose(const CoordinateFrame& cframe, const GMaterial& mat, bool perVertexNormals) {
    return new PosedIFSModel(this, cframe, perVertexNormals, mat, true);
}

//////////////////////////////////////////////////////////////////////////
void IFSModel::save(
    const std::string&          filename,
    const std::string&          name,
    const Array<int>&           index,
    const Array<Vector3>&       vertex,
	const Array<Vector2>&		texCoord) {

	if ("ifs" == filenameExt(filename)) {
		
		float32 ifs_version = (texCoord.size() == 0)? 1.0f : 1.1f;

		BinaryOutput b(filename, G3D_LITTLE_ENDIAN);

		b.writeString32("IFS");
		b.writeFloat32(ifs_version);
		b.writeString32(name);

		b.writeString32("VERTICES");

		b.writeUInt32(vertex.size());

		for (uint32 v = 0; v < (uint32)vertex.size(); ++v) {
			vertex[v].serialize(b);
		}

		b.writeString32("TRIANGLES");

		b.writeUInt32(index.size() / 3);
		for (uint32 i = 0; i < (uint32)index.size(); ++i) {
			b.writeUInt32(index[i]);
		}
		
		if (ifs_version == 1.1f) {
			b.writeString32("TEXTURECOORD");
			alwaysAssertM(texCoord.size() == vertex.size(), "Number of texCoords must match the number of vertices") ;
			b.writeUInt32(texCoord.size());
			for(uint32 t = 0; t < (uint32)texCoord.size(); ++t) {
				texCoord[t].serialize(b);
			}
		}

		b.commit(false);
	} else if ("ply2" == filenameExt(filename)) { 
		alwaysAssertM(texCoord.size() == 0,  format("texCoord.size() != 0, PLY2 files do not support saving texCoords."));
		TextOutput to(filename);

		const int nF = index.size() / 3;
		to.printf("%d\n%d\n",vertex.size(), nF);

		for(uint32 i = 0; i < (uint32)vertex.size(); ++i) {
			to.printf("%f ",vertex[i].x);
			to.printf("%f ",vertex[i].y);
			to.printf("%f\n",vertex[i].z);
		}

		for(uint32 i = 0; i < (uint32)nF; ++i) {
			to.printf("3 %d  %d  %d\n", index[3*i], index[3*i + 1], index[3*i + 2]);
		}

		to.commit(false);
	} else {
		alwaysAssertM(false,  format("unsupported filename type %s", filenameExt(filename).c_str()));
	}
}


void IFSModel::load(
    const std::string&      filename,
    std::string&            name,
    Array<int>&             index, 
    Array<Vector3>&         vertex,
    Array<Vector2>&			texCoord) {


    if (filenameExt(filename) == "ifs" ) {
        BinaryInput bi(filename, G3D_LITTLE_ENDIAN);
        
        if (bi.getLength() == 0) {
            throw std::string("Failed to open " + filename);
        }
        
        std::string header = bi.readString32();
        if (header != "IFS") {
            throw std::string("File is not an IFS file");
        }
        float32 ifsversion  = bi.readFloat32();
        if (ifsversion != 1.0f && ifsversion != 1.1f) {
            throw std::string("Bad IFS version, expecting 1.0 or 1.1");
        }
        
        name = bi.readString32();
        
        texCoord.resize(0);
        while (bi.hasMore()) {
            std::string str = bi.readString32();
            
            if (str == "VERTICES") {
                debugAssertM(vertex.size() == 0, "Multiple vertex fields!");
                uint32 num = bi.readUInt32();
                
                if ((num <= 0) || (num > 10000000)) {
                    throw std::string("Bad number of vertices");
                }
                
                vertex.resize(num);
                
                for (uint32 i = 0; i < num; ++i) {
                    vertex[i].deserialize(bi);
                }
                
            } else if (str == "TRIANGLES") {
                debugAssertM(index.size() == 0,
                             "Multiple triangle fields!");
                uint32 num = bi.readUInt32();
                
                if ((num <= 0) || (num > 100000000)) {
                    throw std::string("Bad number of triangles");
                }
                
                index.resize(num * 3);
                for (uint32 i = 0; i < (uint32)index.size(); ++i) {
                    index[i] = bi.readUInt32();
                }
            } else if (str == "TEXTURECOORD") {
                debugAssertM(ifsversion == 1.1f,
                             "IFS Version should be 1.1");
                debugAssertM(texCoord.size() == 0,
                             "Multiple texcoord fields!");
                uint32 num = bi.readUInt32();
                texCoord.resize(num);
                debugAssertM(texCoord.size() == vertex.size(),
                             " Must have same number of texcoords as vertices");
                for(uint32 t = 0; t < num; ++t) {
                    texCoord[t].deserialize(bi);
                }
            }
        }
    } else if ("ply2" == filenameExt(filename)) {	
        
        TextInput ti(filename);
        
        const int nV = iFloor(ti.readNumber());
        const int nF = iFloor(ti.readNumber());
        
        vertex.resize(nV);
        index.resize(3*nF);
        texCoord.resize(0);
        name = filenameBaseExt(filename);
        
        double x,y,z;
        
        
        for(int i = 0; i < nV; ++i) {
            x = ti.readNumber();
            y = ti.readNumber();
            z = ti.readNumber();
            vertex[i] = Vector3(x ,y ,z);
        }
        
        
        for(int i = 0; i < nF; ++i) {
            const int three = iFloor(ti.readNumber());
            alwaysAssertM(three == 3, "ill formed PLY2 file");
            index[3*i	 ] = iFloor(ti.readNumber());
            index[3*i + 1] = iFloor(ti.readNumber());
            index[3*i + 2] = iFloor(ti.readNumber());
        }
        
    } else {
        alwaysAssertM(false,  format("unsupported filename type %s", filenameExt(filename).c_str()));
    }
}


void GMaterial::configure(class RenderDevice* rd) const {
    rd->setColor(color);
    for (int t = 0; t < texture.size(); ++t) {
        rd->setTexture(t, texture[t]);
    }
    rd->setShininess(shininess);
    rd->setSpecularCoefficient(specularCoefficient);
}

//////////////////////////////////////////////////////////////////////////
IFSModel::PosedIFSModel::PosedIFSModel(
    IFSModelRef                 _model,
    const CoordinateFrame&      _cframe,
    bool                        _pvn,
    const GMaterial&            _mat,
    bool                        _useMat) :
     model(_model), 
     cframe(_cframe),
     perVertexNormals(_pvn),
     useMaterial(_useMat),
     material(_mat) {
}


void IFSModel::PosedIFSModel::sendGeometry(RenderDevice* renderDevice) const {
    const size_t varSize = 2 * 1024 * 1024;
    if (IFSModel::varArea.isNull()) {
        // Initialize VAR
        IFSModel::varArea = VARArea::create(varSize);
    }

    if (perVertexNormals) {
        size_t modelSize = sizeof(Vector3) * 2 * model->geometry.vertexArray.size() + 
							  sizeof(Vector2) * model->texArray.size();

        if (! IFSModel::varArea.isNull() && 
		    (varArea->totalSize() >= modelSize)) {
            // Can use VAR

            if (varArea->freeSize() < modelSize + 128) {
                // Not enough free space left in the common area;
                // reset it to allocate new arrays (this might
                // stall the pipeline).  Otherwise, just allocate
                // on top of what was already there.
                varArea->reset();

                // Resetting invalidates the old VAR arrays.
                // Just knock out the lastModel to prevent a match.

                lastModel = NULL;
            }

            VAR vertex;
            VAR normal;
			VAR tex;

            if (model != lastModel) {
                // Upload new data (cache miss)
                lastModel       = model;
                lastVertexVAR   = VAR(model->geometry.vertexArray, IFSModel::varArea);
                lastNormalVAR   = VAR(model->geometry.normalArray, IFSModel::varArea);
                lastTexCoordVAR = VAR(model->texArray, IFSModel::varArea);
            }

            vertex = lastVertexVAR;
            normal = lastNormalVAR;
            tex    = lastTexCoordVAR;

            renderDevice->beginIndexedPrimitives();
                if (model->texArray.size() > 0) {
                    renderDevice->setTexCoordArray(0, tex);
                }
                renderDevice->setNormalArray(normal);
                renderDevice->setVertexArray(vertex);
                renderDevice->sendIndices(RenderDevice::TRIANGLES, model->indexArray);
            renderDevice->endIndexedPrimitives();

        } else {
            // No VAR
            const int* indexArray = model->indexArray.getCArray();
            const Vector3* vertexArray = model->geometry.vertexArray.getCArray();
            const Vector3* normalArray = model->geometry.normalArray.getCArray();
			const Vector2* texCoordArray = model->texArray.getCArray();
            const int n = model->indexArray.size();

            renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                if (model->texArray.size() > 0) {
                    for (int i = 0; i < n; ++i) {
                        const int v = indexArray[i];            
                        renderDevice->setTexCoord(0, texCoordArray[v]);
                        renderDevice->setNormal(normalArray[v]);
                        renderDevice->sendVertex(vertexArray[v]);
                    }
                    
                } else {
                    for (int i = 0; i < n; ++i) {
                        const int v = indexArray[i];            
                        renderDevice->setNormal(normalArray[v]);
                        renderDevice->sendVertex(vertexArray[v]);
                    }
                }
            renderDevice->endPrimitive();
        }

    } else {

        // Face Normals (slow)

        const Vector3* vertexArray = model->geometry.vertexArray.getCArray();
        const Vector3* faceNormalArray = model->faceNormalArray.getCArray();           
        const MeshAlg::Face* faceArray = model->faceArray.getCArray();
		const Vector2* texCoordArray = model->texArray.getCArray();
        const int n = model->faceArray.size();

        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
            if (model->texArray.size() > 0) {
                for (int f = 0; f < n; ++f) {
                    renderDevice->setNormal(faceNormalArray[f]);
                    for (int j = 0; j < 3; ++j) {                    
                        renderDevice->setTexCoord(0, texCoordArray[faceArray[f].vertexIndex[j]]);
                        renderDevice->sendVertex(vertexArray[faceArray[f].vertexIndex[j]]);
                    }
                }
            } else {
                for (int f = 0; f < n; ++f) {
                    renderDevice->setNormal(faceNormalArray[f]);
                    for (int j = 0; j < 3; ++j) {                    
                        renderDevice->sendVertex(vertexArray[faceArray[f].vertexIndex[j]]);
                    }
                }
            }
        renderDevice->endPrimitive();
    }
}

void IFSModel::PosedIFSModel::render(RenderDevice* renderDevice) const {

//    renderDevice->pushState();

        if (useMaterial) {
            material.configure(renderDevice);
        }

        if (perVertexNormals) {
            renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        }

        renderDevice->setObjectToWorldMatrix(coordinateFrame());
    
        sendGeometry(renderDevice);
  //  renderDevice->popState();
}


std::string IFSModel::PosedIFSModel::name() const {
    return model->name;
}


void IFSModel::PosedIFSModel::getCoordinateFrame(CoordinateFrame& c) const {
    c = cframe;
}


const MeshAlg::Geometry& IFSModel::PosedIFSModel::objectSpaceGeometry() const {
    return model->geometry;
}


const Array<int>& IFSModel::PosedIFSModel::triangleIndices() const {
    return model->indexArray;
}


const Array<MeshAlg::Face>& IFSModel::PosedIFSModel::faces() const {
    return model->faceArray;
}


const Array<MeshAlg::Edge>& IFSModel::PosedIFSModel::edges() const {
    return model->edgeArray;
}


const Array<MeshAlg::Vertex>& IFSModel::PosedIFSModel::vertices() const {
    return model->vertexArray;
}


const Array<MeshAlg::Face>& IFSModel::PosedIFSModel::weldedFaces() const {
    return model->weldedFaceArray;
}


const Array<MeshAlg::Edge>& IFSModel::PosedIFSModel::weldedEdges() const {
    return model->weldedEdgeArray;
}


const Array<MeshAlg::Vertex>& IFSModel::PosedIFSModel::weldedVertices() const {
    return model->weldedVertexArray;
}

bool IFSModel::PosedIFSModel::hasTexCoords() const {
    return (model->texArray.size() > 0);
}

const Array<Vector2>&  IFSModel::PosedIFSModel::texCoords() const {
    return model->texArray;
}

void IFSModel::PosedIFSModel::getObjectSpaceBoundingSphere(Sphere& s) const {
    s = model->boundingSphere;
}


void IFSModel::PosedIFSModel::getObjectSpaceBoundingBox(Box& b) const {
    b = model->boundingBox;
}


int IFSModel::PosedIFSModel::numBoundaryEdges() const {
    return model->numBoundaryEdges;
}


int IFSModel::PosedIFSModel::numWeldedBoundaryEdges() const {
    return model->numWeldedBoundaryEdges;
}


const Array<Vector3>& IFSModel::PosedIFSModel::objectSpaceFaceNormals(bool normalize) const {
    // Normals are always unit length
    (void)normalize;
    return model->faceNormalArray;
}

} // namespace

