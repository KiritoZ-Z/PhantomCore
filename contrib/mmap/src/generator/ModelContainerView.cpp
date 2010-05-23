#include "G3D/Array.h"
#include "G3D/Box.h"
#include "G3D/Array.h"
#include "ModelContainerView.h"
#include "Config/Config.h"
#include "GridMap.h"
#include <fstream>
#include "../pathfinding/Recast/Recast.h"
#include "../pathfinding/Detour/DetourNavMesh.h"
#include "../pathfinding/Detour/DetourNavMeshBuilder.h"

using namespace std;
using namespace G3D;

namespace VMAP
{
  extern std::string gMapDataDir;
  extern std::string gVMapDataDir;
  extern std::string gMMapDataDir;
  extern std::string g3dDataDir;
  extern bool gMakeObjFile;

  //==========================================

  ModelContainerView::ModelContainerView (int mapId, int x, int y) :
  iVMapManager ()
  {
    iMap = mapId;
    ix = x;
    iy = y;
    iGlobArray.clear();
    iIndexArray.clear();
    globalTriangleArray.clear();
    offset = 0;
  }
  //===================================================

  ModelContainerView::~ModelContainerView (void)
  {
  }

  //===================================================

  void
  ModelContainerView::cleanup ()
  {
  }

  //==========================================

  void
  fillSubModelArary (const ModelContainer* pModelContainer, const TreeNode *root, Array<SubModel>& array, Vector3& pLo, Vector3& pHi)
  {
    Vector3 lo = Vector3 (inf (), inf (), inf ());
    Vector3 hi = Vector3 (-inf (), -inf (), -inf ());
    //printf("[VMAP] %i Submodels\n", root->getNValues ());
    for (int i = 0; i < root->getNValues (); i++)
      {
        SubModel sm = pModelContainer->getSubModel (root->getStartPosition () + i);
        lo = lo.min (sm.getAABoxBounds ().low ());
        hi = hi.max (sm.getAABoxBounds ().high ());
        array.append (sm);
      }

    if (root->getChild ((TreeNode *) & pModelContainer->getTreeNode (0), 0))
      {
        fillSubModelArary (pModelContainer, root->getChild ((TreeNode *) & pModelContainer->getTreeNode (0), 0), array, lo, hi);
      }
    if (root->getChild ((TreeNode *) & pModelContainer->getTreeNode (0), 1))
      {
        fillSubModelArary (pModelContainer, root->getChild ((TreeNode *) & pModelContainer->getTreeNode (0), 1), array, lo, hi);
      }

    float dist1 = (hi - lo).magnitude ();
    AABox b;
    root->getBounds (b);
    float dist2 = (b.high () - b.low ()).magnitude ();
    if (dist1 > dist2)
      {
        // error
        //printf("error in boundingbox size vs. hi-lo size.");
        // this error seems to happen alot! maybe need to do something against it?
        int xxx = 0;
      }
  }

  //==========================================

  void
  ModelContainerView::addModelContainer (const std::string& pName, const ModelContainer* pModelContainer )
  {
    // VARArea::UsageHint::WRITE_EVERY_FEW_FRAMES
    //printf("[VMAP] Adding ModelContainer: %s\n", pName.c_str());
    Array<SubModel> SMArray;
    Vector3 lo, hi;
    fillSubModelArary (pModelContainer, &pModelContainer->getTreeNode (0), SMArray, lo, hi);


    for (int i = 0; i < SMArray.size (); ++i)
      {
        SubModel sm = SMArray[i];
        Array<Vector3> vArray;
        fillVertexArray (sm, vArray);
        //break;
      }
  }


  //===================================================

  void
  ModelContainerView::fillRenderArray (const SubModel& pSm, Array<TriangleBox> &pArray, const TreeNode* pTreeNode)
  {
    //  printf("fillRenderArray for %i\n",VMAP::hashCode(pSm));
    for (int i = 0; i < pTreeNode->getNValues (); i++)
      {
        pArray.append (pSm.getTriangles ()[i + pTreeNode->getStartPosition ()]);
      }

    if (pTreeNode->getChild (pSm.getTreeNodes (), 0) != 0)
      {
        fillRenderArray (pSm, pArray, pTreeNode->getChild (pSm.getTreeNodes (), 0));
      }

    if (pTreeNode->getChild (pSm.getTreeNodes (), 1) != 0)
      {
        fillRenderArray (pSm, pArray, pTreeNode->getChild (pSm.getTreeNodes (), 1));
      }
  }

  
  //===================================================

  void
  ModelContainerView::addGrid ( int iMap, int x, int y )
  {
    generateMoveMapForTile (iMap, x, y);
    consolePrintf("Loaded %d-%d\n",x,y);
  }

  //===================================================
  #define FULL 64.0*SIZE_OF_GRIDS
  #define MID FULL/2
  void
  ModelContainerView::fillVertexArray (const SubModel& pSm, Array<Vector3>& vArray)
  {
    //printf("fillVertexArray for %i-%i\n",VMAP::hashCode(SubModel),5);
    //printf("fillVertexArray for %i\n",VMAP::hashCode(pSm));
    Array<TriangleBox> tbarray;
    fillRenderArray (pSm, tbarray, &pSm.getTreeNode (0));
    int len = tbarray.size ();
    Vector3 toconvert[3];
    for (int i = 0; i < len; ++i)
      {
        // TODO: Get correct Conversion of position values.
        // The system between vmaps and MaNGOS differs in that:
        /* from iinternal(xyz) -> MaNGOS(xyz)
        float pos[3];
        pos[0] = z;
        pos[1] = x;
        pos[2] = y;
        double full = 64.0*533.33333333;
        double mid = full/2.0;
        pos[0] = -((mid+pos[0])-full); -> -mid -pos[0] + full -> mid - z
        pos[1] = -((mid+pos[1])-full); -> -mid -pos[0] + full -> mid - x
        */
        TriangleBox old = tbarray[i];
        toconvert[0] = old.vertex(0).getVector3 () + pSm.getBasePosition ();
        toconvert[1] = old.vertex(1).getVector3 () + pSm.getBasePosition ();
        toconvert[2] = old.vertex(2).getVector3 () + pSm.getBasePosition ();
        for (int j = 0; j < 3; j++) {
            toconvert[j] = toconvert[j].zxy();
            toconvert[j].x = MID - toconvert[j].x;
            toconvert[j].y = MID - toconvert[j].y;
        }
        //TODO: playing around with correct range of triangles for detour
        Triangle t = Triangle(toconvert[0],toconvert[1],toconvert[2]);
        Triangle t1 = Triangle(toconvert[2],toconvert[1],toconvert[0]);
        globalTriangleArray.append(t,t1);
        /*
        for (int j = 2; j>= 0; j--) {
            Vector3 vertex = tbarray[i].vertex(j).getVector3() + pSm.getBasePosition();
            // x = [expr {-(($::mid+$x)-$::full)}]
            //printf("x[%.2f] y[%.2f] z[%.2f]",vertex.x,vertex.y,vertex.z);
            // v 152.333 60.0304 -9561.67
            vertex = Vector3(-((64*533.333333333/2 + vertex.z) - 64*533.333333333),
                            -((64*533.333333333/2 + vertex.x) - 64*533.333333333),
                            vertex.y);
            vArray.append(vertex);
        }
        */
      }
  }

  //====================================================================
  /*
   * Loads the map for the given mapid, x and y value.
   *
   *
   *
   */
  void
  ModelContainerView::parseVMap (int pMapId, int x, int y)
  {
    MapTree* mt = iVMapManager.getInstanceMapTree (pMapId);
    std::string dirFileName = iVMapManager.getDirFileName (pMapId, x, y);
    
    if (!mt->hasDirFile (dirFileName))
      dirFileName = iVMapManager.getDirFileName (pMapId);
    // This will add all models in map.
    parseVMap (mt, dirFileName);
  }

  //====================================================================
  /*
   * Tries to load vmap and tilemap for a gridtile and creates a navmesh for it.
   *
   */
  bool
  ModelContainerView::generateMoveMapForTile (int pMapId, int x, int y)
  {
        bool result = iVMapManager.loadMap (gVMapDataDir.c_str (), pMapId, x, y) == VMAP_LOAD_RESULT_OK;
        if (result == VMAP_LOAD_RESULT_OK)
        {
            //VMap loaded. Add data from vmap to global Triangle-Array
            parseVMap (pMapId, x, y);
        }
        // Add data from Height-Map to global Triangle-Array
        generateHeightMap(pMapId,x,y);
        // We will now add all triangles inside the given zone to the vectormap.
        // We could also do additional checks here.
        double x_max = (32-x)*SIZE_OF_GRIDS + 50;
        double y_max = (32-y)*SIZE_OF_GRIDS + 50;
        double x_min = x_max - SIZE_OF_GRIDS - 100;
        double y_min = y_max - SIZE_OF_GRIDS - 100;
        Vector3 low = Vector3(x_min,y_min,-inf());
        Vector3 high = Vector3(x_max,y_max,inf());
        AABox checkBox = AABox(low,high);
        AABox check;
        Triangle t;
        //each triangle has mangos format.
        for (int i = 0; i < globalTriangleArray.size(); i++) {
            t = globalTriangleArray[i];
            t.getBounds(check);
            if (checkBox.contains(check)) {
                // Write it down in detour format.
                iGlobArray.append(t.vertex(0).y,t.vertex(0).z,t.vertex(0).x);
                iGlobArray.append(t.vertex(1).y,t.vertex(1).z,t.vertex(1).x);
                iGlobArray.append(t.vertex(2).y,t.vertex(2).z,t.vertex(2).x);
            }
                
        }
        if (iGlobArray.size() == 0) {
            printf("No models - check your mmap.datadir in your config");
            return true;
        }

        if(gMakeObjFile)
            debugGenerateObjFile(); // create obj file for Recast Demo viewer
        //return true;
        float bmin[3], bmax[3];
        /*
         * The format looks like this
         * Verticle = float[3]
         * Triangle = Verticle[3]
         * So there are
         * array.size() floats
         * that means there are
         * nverts = array.size()/3 Verticles
         * that means there are
         * ntris = nverts/3
         */
         //array/3 verticles
        const int nverts = iGlobArray.size()/3; // because 1 vert is 3 float.
        // -> vert = float[3]
        const float* verts = iGlobArray.getCArray();
        rcCalcBounds(verts,nverts,bmin,bmax);
        // nverts/3 triangles
        // -> Triangle = vert[3] = float[9]
        int* tris = new int[nverts];// because 1 triangle is 3 verts
        for (int i = 0; i< nverts; i++)
            tris[i] = i;
        /* tris[i] = 1,2,3;4,5,6;7,8,9;
         *
         */
        const int ntris = (nverts/3);
	rcConfig m_cfg;
	//
	// Step 1. Initialize build config.
	//

	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
        // Change config settings here!
	m_cfg.cs = 0.3f;
	m_cfg.ch = 0.2f;
	m_cfg.walkableSlopeAngle = 50.0f;
	m_cfg.walkableHeight = 10;
	m_cfg.walkableClimb = 4;
	m_cfg.walkableRadius = 2;
	m_cfg.maxEdgeLen = (int)(12 / 0.3f);
	m_cfg.maxSimplificationError = 1.3f;
	m_cfg.minRegionSize = (int)rcSqr(50);
	m_cfg.mergeRegionSize = (int)rcSqr(20);
	m_cfg.maxVertsPerPoly = (int)6;
	m_cfg.detailSampleDist = 1.8f;
	m_cfg.detailSampleMaxError = 0.2f * 1;
        bool m_keepInterResults = false;
        printf("CellSize        : %.2f\n",m_cfg.cs);
        printf("CellHeight      : %.2f\n",m_cfg.ch);
        printf("WalkableSlope   : %.2f\n",m_cfg.walkableSlopeAngle);
        printf("WalkableHeight  : %i\n",m_cfg.walkableHeight);
        printf("walkableClimb   : %i\n",m_cfg.walkableClimb);
        printf("walkableRadius  : %i\n",m_cfg.walkableRadius);
        printf("maxEdgeLen      : %i\n",m_cfg.maxEdgeLen);
        printf("maxSimplific.Er.: %.2f\n",m_cfg.maxSimplificationError);
        printf("minRegionSize   : %i\n",m_cfg.minRegionSize);
        printf("mergedRegSize   : %i\n",m_cfg.mergeRegionSize);
        printf("maxVertsPerPoly : %i\n",m_cfg.maxVertsPerPoly);
        printf("detailSampledist: %.2f\n",m_cfg.detailSampleDist);
        printf("det.Samp.max.err: %.2f\n",m_cfg.detailSampleMaxError);
	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	vcopy(m_cfg.bmin, bmin);
	vcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heighfield where we rasterize our input data to.
	rcHeightfield* m_solid = new rcHeightfield;
	if (!m_solid)
	{
		printf("buildNavigation: Out of memory 'solid'.\n");
		return false;
	}
	if (!rcCreateHeightfield(*m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		printf("buildNavigation: Could not create solid heightfield.\n");
		return false;
	}

	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	unsigned char* m_triflags = new unsigned char[ntris];
	if (!m_triflags)
	{
		printf("buildNavigation: Out of memory 'triangleFlags' (%d).\n", ntris);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the flags for each of the meshes and rasterize them.
	memset(m_triflags, 0, ntris*sizeof(unsigned char));
	rcMarkWalkableTriangles(m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triflags);
	rcRasterizeTriangles(verts, nverts, tris, m_triflags, ntris, *m_solid, m_cfg.walkableClimb);

    // should delete [] verts?  - probably not, this is just pointer to data in a G3D Array
    // should delete [] tris?

	if (!m_keepInterResults)
	{
		delete [] m_triflags;
		m_triflags = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	rcFilterWalkableLowHeightSpans(m_cfg.walkableHeight, *m_solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	rcCompactHeightfield* m_chf = new rcCompactHeightfield;
	if (!m_chf)
	{
		printf("buildNavigation: Out of memory 'chf'.\n");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_cfg.walkableHeight, m_cfg.walkableClimb, RC_WALKABLE, *m_solid, *m_chf))
	{
		printf( "buildNavigation: Could not build compact data.\n");
		return false;
	}

	if (!m_keepInterResults)
	{
		delete m_solid;
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeArea(RC_WALKABLE_AREA, m_cfg.walkableRadius, *m_chf))
	{
		printf("buildNavigation: Could not erode.\n");
		return false;
	}

	// (Optional) Mark areas.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
	//	rcMarkConvexPolyArea(vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);

	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	if (!rcBuildDistanceField(*m_chf))
	{
		printf("buildNavigation: Could not build distance field.\n");
		return false;
	}

	// Partition the walkable surface into simple regions without holes.
	if (!rcBuildRegions(*m_chf, m_cfg.borderSize, m_cfg.minRegionSize, m_cfg.mergeRegionSize))
	{
		printf("buildNavigation: Could not build regions.\n");
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	rcContourSet* m_cset = new rcContourSet;
	if (!m_cset)
	{
		printf("buildNavigation: Out of memory 'cset'.\n");
		return false;
	}
	if (!rcBuildContours(*m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		printf("buildNavigation: Could not create contours.\n");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	rcPolyMesh* m_pmesh = new rcPolyMesh;
	if (!m_pmesh)
	{
		printf("buildNavigation: Out of memory 'pmesh'.\n");
		return false;
	}
	if (!rcBuildPolyMesh(*m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		printf( "buildNavigation: Could not triangulate contours.\n");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	rcPolyMeshDetail* m_dmesh = new rcPolyMeshDetail;
	if (!m_dmesh)
	{
		printf("buildNavigation: Out of memory 'pmdtl'.\n");
		return false;
	}

	if (!rcBuildPolyMeshDetail(*m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
	{
		printf("buildNavigation: Could not build detail mesh.\n");
	}

	if (!m_keepInterResults)
	{
		delete m_chf;
		m_chf = 0;
		delete m_cset;
		m_cset = 0;
	}

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;
                // Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
                    // for now all generated navmesh is walkable by everyone.
                    // else there will be no pathfinding at all!
                    m_pmesh->flags[i] = RC_WALKABLE_AREA;
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		params.offMeshConVerts = 0;
		params.offMeshConRad = 0;
		params.offMeshConDir = 0;
		params.offMeshConAreas = 0;
		params.offMeshConFlags = 0;
		params.offMeshConCount = 0;
		params.walkableHeight = 2.0f;
		params.walkableRadius = 0.6f;
		params.walkableClimb = 0.9f;
		vcopy(params.bmin, m_pmesh->bmin);
		vcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		printf("vertcount       : %05u\n",params.vertCount);
                printf("polycount       : %05u\n",params.polyCount);
                printf("detailVertsCount: %05u\n",params.detailVertsCount);
                printf("detailTriCount  : %05u\n",params.detailTriCount);
                printf("walkableClimb   : %.2f\n",params.walkableClimb);
                printf("walkableRadius  : %.2f\n",params.walkableRadius);
                printf("walkableHeight  : %.2f\n",params.walkableHeight);
		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			printf("Could not build Detour navmesh.\n");
			return false;
		}
                // navData now contains the MoveMap
                printf("Generated Navigation Mesh! Size: %i bytes/ %i kB / %i MB\n",navDataSize,navDataSize/1024,navDataSize/(1024*1024));
                char tmp[14];
                sprintf(tmp, "%03u%02u%02u.mmap",iMap,ix,iy);
                std::string savefilepath = gMMapDataDir + "/" + tmp;
                ofstream inf( savefilepath.c_str(),ofstream::binary );
                if( inf )
                {
                        inf.write( (char*)( &navData[0] ), navDataSize ) ;
                }
                printf("MoveMap saved under %s\n", savefilepath.c_str());
                delete [] navData;
        }
       // debugLoadNavMesh();
        return (result);
  }

  void ModelContainerView::debugLoadNavMesh()
  {
        char tmp[14];
        sprintf(tmp, "%03u%02u%02u.mmap",iMap,ix,iy);
        std::string savefilepath = gMMapDataDir + "/" + tmp;
        std::ifstream LoadFile (savefilepath.c_str(),std::ifstream::binary);
        printf("Trying to open %s\n", savefilepath.c_str());
        ifstream inf( savefilepath.c_str(), ofstream::binary );
        if( inf )
        {
                printf("File %s opened.\n", savefilepath.c_str());
                inf.seekg(0,std::ifstream::end);
                int navDataSize = inf.tellg();
                inf.seekg(0);
                printf("DataSize is %i\n", navDataSize);
                unsigned char* navData = new unsigned char[navDataSize];
                inf.read((char*) (navData),navDataSize);
                dtNavMesh* m_navMesh = new dtNavMesh;
		if (!m_navMesh)
		{
			delete [] navData;
            delete m_navMesh;
			return;
		}
                printf("Created navMesh\n");
		if (!m_navMesh->init((unsigned char*)navData, navDataSize, true, 2048))
		{
			return;
		}
                printf("Loaded navMesh!\n");
                return;
        }
  }

  void ModelContainerView::debugGenerateObjFile ()
  {
    int numberofpoints = iGlobArray.size();
    // lets create file in two steps
    char tmp[12];
    sprintf(tmp, "%03u%02u%02u.obj",iMap,ix,iy);
    std::string savefilepath = gMMapDataDir + "/" + tmp;
    ofstream SaveFile(savefilepath.c_str());
    printf("Generating Obj-File for %s in %s\n", tmp, savefilepath.c_str());
    printf("Number of Points: %i\n", numberofpoints/3);
    printf("This should be 0: %i\n", (numberofpoints/3) % 3);
    // 1. write down all points
    // The vector3 in the array have the wow coordinates!
    // (x,y,z) to use with Detour usage is:
    // what seems to work is y z x.
    SaveFile << "# Generated by MoveMap-Generator for debugging in RecastDemo.\n";
    for (int count = 0; count < numberofpoints;) {
        SaveFile << "v ";
        SaveFile << iGlobArray[count++];
        SaveFile << " ";
        SaveFile << iGlobArray[count++];
        SaveFile << " ";
        SaveFile << iGlobArray[count++];
        SaveFile << "\n";
    }
    // 2 write down all surfaces wich is just all combinantion of points.
    for (int count = 1; count < numberofpoints/3; ) {
        SaveFile << "f ";
        SaveFile << count++;
        SaveFile << " ";
        SaveFile << count++;
        SaveFile << " ";
        SaveFile << count++;
        SaveFile << "\n";
    }
    SaveFile.close();
  }

  void ModelContainerView::generateHeightMap(int pMapId, int x, int y)
  {
      //TODO: Fix this in general.
      //Here we need to load the GridMap from .map file. then generate vertex map from heigh points.
      GridMap mapArray[64][64]; //TODO: make smaller array and recalculate matching gridmap.
      for (int x1 = x-1; x1 <= x+1; ++x1)
        for (int y1 = y-1; y1 <= y+1; ++y1) {
            mapArray[x1][y1] = GridMap();
            char tmp[12];
            sprintf(tmp, "%03u%02u%02u.map",pMapId,x1,y1);
            std::string gmap =  gMapDataDir + "/" + tmp;
            if (mapArray[x1][y1].loadData(gmap.c_str()))
                printf("Loaded %s\n", gmap.c_str());
        }
    float x_min,y_min,x_max, y_max;
    x_max = (32-x)*SIZE_OF_GRIDS + 50;
    y_max = (32-y)*SIZE_OF_GRIDS + 50;
    x_min = (32-x)*SIZE_OF_GRIDS - 533 - 50;
    y_min = (32-y)*SIZE_OF_GRIDS - 533 - 50;
    for (float x = x_min; x < x_max-2;x += 2)
        for (float y = y_min; y < y_max-2;y += 2) {
            int gx,gy;
            // Here we need to add vertexes. so 3 Vector3 for each triangle.
            // FIXME: This is overly efficient since we visit each vector3 multipletimes during loop.
            gx = (int)(32 - x / SIZE_OF_GRIDS);
            gy = (int)(32 - y / SIZE_OF_GRIDS);
            float heightxy = mapArray[gx][gy].getHeight(x,y);
            gx = (int)(32 - (x+2) / SIZE_OF_GRIDS);
            float heightx1y = mapArray[gx][gy].getHeight(x+2,y);
            gx = (int)(32 - x / SIZE_OF_GRIDS);
            gy = (int)(32 - (y+2) / SIZE_OF_GRIDS);
            float heightxy1 = mapArray[gx][gy].getHeight(x,y+2);
            gx = (int)(32 - (x+2) / SIZE_OF_GRIDS);
            float heightx1y1 = mapArray[gx][gy].getHeight(x+2,y+2);
            Vector3 vector1(x,y,heightxy);
            Vector3 vector2(x+2,y,heightx1y);
            Vector3 vector3(x,y+2,heightxy1);
            Vector3 vector4(x+2,y+2,heightx1y1);
            /*
             * vector1 ------ vector2
             *   |     \     /    |
             *   |      \   /     |
             *   |       \ /      |
             *   |        X       |
             *   |       / \      |
             *   |      /   \     |
             *   |     /     \    |
             * vector3 -------- vector4
             */

            Triangle t1 = Triangle(vector1,vector4,vector3);
            Triangle t2 = Triangle(vector1,vector2,vector4);
            // Check if the center of this Triangle is deep under water. (here: 1 meter)
            Vector3 center = t1.center();
            if (mapArray[(int)(32 - center.x / SIZE_OF_GRIDS)][(int)(32 - center.y / SIZE_OF_GRIDS)].getLiquidLevel(center.x,center.y) < center.z - 1.f)
                globalTriangleArray.append(t1);
            center = t2.center();
            if (mapArray[(int)(32 - center.x / SIZE_OF_GRIDS)][(int)(32 - center.y / SIZE_OF_GRIDS)].getLiquidLevel(center.x,center.y) < center.z - 1.f)
                globalTriangleArray.append(t2);
        }
  }

  void
  ModelContainerView::parseVMap (MapTree* mt, std::string dirFileName)
  {
      printf("[VMAP]Parsing: %s\n", dirFileName.c_str());
      if (mt->hasDirFile (dirFileName))
      {
        FilesInDir filesInDir = mt->getDirFiles (dirFileName);
        if (filesInDir.getRefCount () == 1)
          {
            Array<std::string> fileNames = filesInDir.getFiles ();
            for (int i = 0; i < fileNames.size (); ++i)
              {
                std::string name = fileNames[i];
                ManagedModelContainer* mc = mt->getModelContainer (name);
                addModelContainer (name, mc );
              }
          }
      }
  }

  //=======================================================================

  bool
  ModelContainerView::loadAndParseVMap (char *pName, int pMapId)
  {
    bool result = iVMapManager.loadMap (gVMapDataDir.c_str (), pMapId, -1, -1) == VMAP_LOAD_RESULT_OK;
    if (result)
      {
        MapTree* mt = iVMapManager.getInstanceMapTree (pMapId);
        std::string dirFileName = iVMapManager.getDirFileName (pMapId);
        iTriIndexTable = Table<std::string, Array<int> >(); // reset table
        parseVMap (mt, dirFileName);
      }
    return (result);
  }
}

