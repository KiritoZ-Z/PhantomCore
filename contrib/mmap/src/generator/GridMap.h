/* 
 * File:   GridMap.h
 * Author: gotisch
 *
 * Created on March 21, 2010, 7:35 AM
 */


#ifndef _MSC_VER
#include <stdint.h>
#endif

#ifdef _MSC_VER
#include "stdint.h"
#endif

#include <stdio.h>
#include <string>

#ifndef _GRIDMAP_H
#define	_GRIDMAP_H

namespace VMAP
{
    //******************************************
    // Map file format defines
    //******************************************
    struct map_fileheader
    {
        uint32_t mapMagic;
        uint32_t versionMagic;
        uint32_t buildMagic;
        uint32_t areaMapOffset;
        uint32_t areaMapSize;
        uint32_t heightMapOffset;
        uint32_t heightMapSize;
        uint32_t liquidMapOffset;
        uint32_t liquidMapSize;
    };

    #define MAP_AREA_NO_AREA      0x0001

    struct map_areaHeader
    {
        uint32_t fourcc;
        uint16_t flags;
        uint16_t gridArea;
    };

    #define MAP_HEIGHT_NO_HEIGHT  0x0001
    #define MAP_HEIGHT_AS_INT16   0x0002
    #define MAP_HEIGHT_AS_INT8    0x0004

    struct map_heightHeader
    {
        uint32_t fourcc;
        uint32_t flags;
        float  gridHeight;
        float  gridMaxHeight;
    };

    #define MAP_LIQUID_NO_TYPE    0x0001
    #define MAP_LIQUID_NO_HEIGHT  0x0002

    struct map_liquidHeader
    {
        uint32_t fourcc;
        uint16_t flags;
        uint16_t liquidType;
        uint8_t  offsetX;
        uint8_t  offsetY;
        uint8_t  width;
        uint8_t  height;
        float  liquidLevel;
    };

    enum ZLiquidStatus
    {
        LIQUID_MAP_NO_WATER     = 0x00000000,
        LIQUID_MAP_ABOVE_WATER  = 0x00000001,
        LIQUID_MAP_WATER_WALK   = 0x00000002,
        LIQUID_MAP_IN_WATER     = 0x00000004,
        LIQUID_MAP_UNDER_WATER  = 0x00000008
    };

    #define MAP_LIQUID_TYPE_NO_WATER    0x00
    #define MAP_LIQUID_TYPE_WATER       0x01
    #define MAP_LIQUID_TYPE_OCEAN       0x02
    #define MAP_LIQUID_TYPE_MAGMA       0x04
    #define MAP_LIQUID_TYPE_SLIME       0x08

    #define MAP_ALL_LIQUIDS   (MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN | MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME)

    #define MAP_LIQUID_TYPE_DARK_WATER  0x10
    #define MAP_LIQUID_TYPE_WMO_WATER   0x20

    struct LiquidData
    {
        uint32_t type;
        float  level;
        float  depth_level;
    };

    #define INVALID_HEIGHT       -100000.0f                     // for check, must be equal to VMAP_INVALID_HEIGHT, real value for unknown height is VMAP_INVALID_HEIGHT_VALUE
    #ifndef NULL
    #define NULL    0
    #endif
    #define SEEK_SET        0
    #define MAX_NUMBER_OF_GRIDS      64
    #define SIZE_OF_GRIDS            533.33333f
    #define CENTER_GRID_ID           (MAX_NUMBER_OF_GRIDS/2)
    #define CENTER_GRID_OFFSET      (SIZE_OF_GRIDS/2)
    #define MIN_GRID_DELAY          (MINUTE*IN_MILISECONDS)
    #define MIN_MAP_UPDATE_DELAY    50
    #define MAX_NUMBER_OF_CELLS     8
    #define SIZE_OF_GRID_CELL       (SIZE_OF_GRIDS/MAX_NUMBER_OF_CELLS)
    #define CENTER_GRID_CELL_ID     (MAX_NUMBER_OF_CELLS*MAX_NUMBER_OF_GRIDS/2)
    #define CENTER_GRID_CELL_OFFSET (SIZE_OF_GRID_CELL/2)
    #define TOTAL_NUMBER_OF_CELLS_PER_MAP    (MAX_NUMBER_OF_GRIDS*MAX_NUMBER_OF_CELLS)
    #define MAP_RESOLUTION 128
    #define MAP_SIZE                (SIZE_OF_GRIDS*MAX_NUMBER_OF_GRIDS)
    #define MAP_HALFSIZE            (MAP_SIZE/2)

    static char const* MAP_MAGIC         = "MAPS";
    static char const* MAP_VERSION_MAGIC = "v1.1";
    static char const* MAP_AREA_MAGIC    = "AREA";
    static char const* MAP_HEIGHT_MAGIC  = "MHGT";
    static char const* MAP_LIQUID_MAGIC  = "MLIQ";

    class GridMap
    {
        uint32_t  m_flags;
        // Area data
        uint16_t  m_gridArea;
        uint16_t *m_area_map;
        // Height level data
        float   m_gridHeight;
        float   m_gridIntHeightMultiplier;
        union{
            float  *m_V9;
            uint16_t *m_uint16_V9;
            uint8_t  *m_uint8_V9;
        };
        union{
            float  *m_V8;
            uint16_t *m_uint16_V8;
            uint8_t  *m_uint8_V8;
        };
        // Liquid data
        uint16_t  m_liquidType;
        uint8_t   m_liquid_offX;
        uint8_t   m_liquid_offY;
        uint8_t   m_liquid_width;
        uint8_t   m_liquid_height;
        float   m_liquidLevel;
        uint8_t  *m_liquid_type;
        float  *m_liquid_map;

        bool  loadAreaData(FILE *in, uint32_t offset, uint32_t size);
        bool  loadHeightData(FILE *in, uint32_t offset, uint32_t size);
        bool  loadLiquidData(FILE *in, uint32_t offset, uint32_t size);

        // Get height functions and pointers
        typedef float (GridMap::*pGetHeightPtr) (float x, float y) const;
        pGetHeightPtr m_gridGetHeight;
        float  getHeightFromFloat(float x, float y) const;
        float  getHeightFromUint16(float x, float y) const;
        float  getHeightFromUint8(float x, float y) const;
        float  getHeightFromFlat(float x, float y) const;

    public:
        GridMap();
        ~GridMap();
        bool  loadData(const char *filename);
        void  unloadData();

        uint16_t getArea(float x, float y);
        inline float getHeight(float x, float y) {return (this->*m_gridGetHeight)(x, y);}
        float  getLiquidLevel(float x, float y);
        uint8_t  getTerrainType(float x, float y);
        ZLiquidStatus getLiquidStatus(float x, float y, float z, uint8_t ReqLiquidType, LiquidData *data = 0);
    };
}


#endif	/* _GRIDMAP_H */