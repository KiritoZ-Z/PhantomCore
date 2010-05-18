
#include "ModelContainerView.h"

#include "Config/Config.h"

namespace VMAP
{
  std::string gMapDataDir;
  std::string gVMapDataDir;
  std::string gMMapDataDir;
  std::string g3dDataDir;
  bool gMakeObjFile;
}

#define SIZE_OF_GRIDS 533.33333f
int
main (int argc, char** argv)
{
  if (argc < 3)
    {
      printf("This will generate MoveMaps for the given grid coordinates.\n");
      printf ("Need 3 arguments: MapId, Map X coord, Map y coord\n");
      return 1;
    }

  Config conf;
  conf.SetSource("mmap.conf");
  
  std::string gDataDir = conf.GetStringDefault("mmap.datadir",".");
  VMAP::gMapDataDir = gDataDir + "/maps";
  VMAP::gMMapDataDir = gDataDir + "/mmaps";
  VMAP::gVMapDataDir = gDataDir + "/vmaps";
  VMAP::gMakeObjFile = conf.GetBoolDefault("mmap.makeobjfile", false);
  VMAP::g3dDataDir = conf.GetStringDefault("mmap.g3ddatadir",".");
  // The correct grid for a set of coordinates is:
  //float x = atof (argv[2]);
  //float y = atof (argv[3]);
  int map = atoi (argv[1]);
  int gx = atoi(argv[2]); //(int) (32 - x / SIZE_OF_GRIDS);
  int gy = atoi(argv[3]); //(int) (32 - y / SIZE_OF_GRIDS);
  printf("Generating Map for Grid [%i][%i]\n", gx,gy);
  VMAP::ModelContainerView modelContainerView (map, gx, gy);
  if (modelContainerView.generateMoveMapForTile(map,gx,gy))
    return 0;
  return 1;
}


#include "Config/Config.cpp"


