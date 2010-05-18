#ifndef _MODELCONTAINERVIEW_H
#define _MODELCONTAINERVIEW_H

#include "VMapManager.h"
#include <G3D/G3DAll.h>
#include "ModelContainer.h"


namespace VMAP {
    //==========================================


    class ModelContainerView {
    private:

        VARAreaRef iVARAreaRef;

        Table<std::string, Array<Vector3> > iTriVarTable;
        Table<std::string, Array<int> > iTriIndexTable;
        int offset;
        // Here we gather all triangles.
        Array<int> iIndexArray;
        // Here we gather all Vectors to triangles!
        Array<float> iGlobArray;
        Array<Triangle> globalTriangleArray;
        VMapManager iVMapManager;

        int iMap;
        int ix;
        int iy;
    private:
        void addGrid(int iMap, int x, int y);

    public:

        ModelContainerView(int mapId, int x, int y);
        ~ModelContainerView(void);

        void addModelContainer(const std::string& pName, const ModelContainer* pModelContainer);
        void removeModelContainer(const std::string& pName, const ModelContainer* pModelContainer);
        void cleanup();
        void fillRenderArray(const SubModel& pSm, Array<TriangleBox> &pArray, const TreeNode* pTreeNode);
        void fillVertexArray(const SubModel& pSm, Array<Vector3>& vArray);
        void debugLoadNavMesh();
        void debugGenerateObjFile();
        bool generateMoveMapForTile(int pMapId, int x, int y);
        void parseVMap(int pMapId, int x, int y);
        void generateHeightMap(int pMapId, int x, int y);
        void parseVMap(MapTree* mt, std::string dirFileName);
        bool loadAndParseVMap(char *pName, int pMapId);
    };

    //==========================================
}

#endif
