#include "GridMap.h"

namespace VMAP
{
    GridMap::GridMap()
    {
        m_flags = 0;
        // Area data
        m_gridArea = 0;
        m_area_map = NULL;
        // Height level data
        m_gridHeight = INVALID_HEIGHT;
        m_gridGetHeight = &GridMap::getHeightFromFlat;
        m_V9 = NULL;
        m_V8 = NULL;
        // Liquid data
        m_liquidType    = 0;
        m_liquid_offX   = 0;
        m_liquid_offY   = 0;
        m_liquid_width  = 0;
        m_liquid_height = 0;
        m_liquidLevel = INVALID_HEIGHT;
        m_liquid_type = NULL;
        m_liquid_map  = NULL;
    }

    GridMap::~GridMap()
    {
        unloadData();
    }

    bool GridMap::loadData(const char *filename)
    {
        // Unload old data if exist
        unloadData();

        map_fileheader header;
        // Not return error if file not found
        FILE *in = fopen(filename, "rb");
        if (!in)
            return false;
        fread(&header, sizeof(header),1,in);
        if (header.mapMagic     == *((uint32_t const*)(MAP_MAGIC)) &&
            header.versionMagic == *((uint32_t const*)(MAP_VERSION_MAGIC)))
        {
            // loadup area data
            if (header.areaMapOffset && !loadAreaData(in, header.areaMapOffset, header.areaMapSize))
            {
                fclose(in);
                return false;
            }
            // loadup height data
            if (header.heightMapOffset && !loadHeightData(in, header.heightMapOffset, header.heightMapSize))
            {
                fclose(in);
                return false;
            }
            // loadup liquid data
            if (header.liquidMapOffset && !loadLiquidData(in, header.liquidMapOffset, header.liquidMapSize))
            {
                fclose(in);
                return false;
            }
            fclose(in);
            return true;
        }
        fclose(in);
        return false;
    }

    void GridMap::unloadData()
    {
        if (m_area_map) delete[] m_area_map;
        if (m_V9) delete[] m_V9;
        if (m_V8) delete[] m_V8;
        if (m_liquid_type) delete[] m_liquid_type;
        if (m_liquid_map) delete[] m_liquid_map;
        m_area_map = NULL;
        m_V9 = NULL;
        m_V8 = NULL;
        m_liquid_type = NULL;
        m_liquid_map  = NULL;
        m_gridGetHeight = &GridMap::getHeightFromFlat;
    }

    bool GridMap::loadAreaData(FILE *in, uint32_t offset, uint32_t /*size*/)
    {
        map_areaHeader header;
        fseek(in, offset, SEEK_SET);
        fread(&header, sizeof(header), 1, in);
        if (header.fourcc != *((uint32_t const*)(MAP_AREA_MAGIC)))
            return false;

        m_gridArea = header.gridArea;
        if (!(header.flags & MAP_AREA_NO_AREA))
        {
            m_area_map = new uint16_t [16*16];
            fread(m_area_map, sizeof(uint16_t), 16*16, in);
        }
        return true;
    }

    bool  GridMap::loadHeightData(FILE *in, uint32_t offset, uint32_t /*size*/)
    {
        map_heightHeader header;
        fseek(in, offset, SEEK_SET);
        fread(&header, sizeof(header), 1, in);
        if (header.fourcc != *((uint32_t const*)(MAP_HEIGHT_MAGIC)))
            return false;

        m_gridHeight = header.gridHeight;
        if (!(header.flags & MAP_HEIGHT_NO_HEIGHT))
        {
            if ((header.flags & MAP_HEIGHT_AS_INT16))
            {
                m_uint16_V9 = new uint16_t [129*129];
                m_uint16_V8 = new uint16_t [128*128];
                fread(m_uint16_V9, sizeof(uint16_t), 129*129, in);
                fread(m_uint16_V8, sizeof(uint16_t), 128*128, in);
                m_gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 65535;
                m_gridGetHeight = &GridMap::getHeightFromUint16;
            }
            else if ((header.flags & MAP_HEIGHT_AS_INT8))
            {
                m_uint8_V9 = new uint8_t [129*129];
                m_uint8_V8 = new uint8_t [128*128];
                fread(m_uint8_V9, sizeof(uint8_t), 129*129, in);
                fread(m_uint8_V8, sizeof(uint8_t), 128*128, in);
                m_gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 255;
                m_gridGetHeight = &GridMap::getHeightFromUint8;
            }
            else
            {
                m_V9 = new float [129*129];
                m_V8 = new float [128*128];
                fread(m_V9, sizeof(float), 129*129, in);
                fread(m_V8, sizeof(float), 128*128, in);
                m_gridGetHeight = &GridMap::getHeightFromFloat;
            }
        }
        else
            m_gridGetHeight = &GridMap::getHeightFromFlat;
        return true;
    }

    bool  GridMap::loadLiquidData(FILE *in, uint32_t offset, uint32_t /*size*/)
    {
        map_liquidHeader header;
        fseek(in, offset, SEEK_SET);
        fread(&header, sizeof(header), 1, in);
        if (header.fourcc != *((uint32_t const*)(MAP_LIQUID_MAGIC)))
            return false;

        m_liquidType   = header.liquidType;
        m_liquid_offX  = header.offsetX;
        m_liquid_offY  = header.offsetY;
        m_liquid_width = header.width;
        m_liquid_height= header.height;
        m_liquidLevel  = header.liquidLevel;

        if (!(header.flags & MAP_LIQUID_NO_TYPE))
        {
            m_liquid_type = new uint8_t [16*16];
            fread(m_liquid_type, sizeof(uint8_t), 16*16, in);
        }
        if (!(header.flags & MAP_LIQUID_NO_HEIGHT))
        {
            m_liquid_map = new float [m_liquid_width*m_liquid_height];
            fread(m_liquid_map, sizeof(float), m_liquid_width*m_liquid_height, in);
        }
        return true;
    }

    uint16_t GridMap::getArea(float x, float y)
    {
        if (!m_area_map)
            return m_gridArea;

        x = 16 * (32 - x/SIZE_OF_GRIDS);
        y = 16 * (32 - y/SIZE_OF_GRIDS);
        int lx = (int)x & 15;
        int ly = (int)y & 15;
        return m_area_map[lx*16 + ly];
    }

    float  GridMap::getHeightFromFlat(float /*x*/, float /*y*/) const
    {
        return m_gridHeight;
    }

    float  GridMap::getHeightFromFloat(float x, float y) const
    {
        if (!m_V8 || !m_V9)
            return m_gridHeight;

        x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

        int x_int = (int)x;
        int y_int = (int)y;
        x -= x_int;
        y -= y_int;
        x_int&=(MAP_RESOLUTION - 1);
        y_int&=(MAP_RESOLUTION - 1);

        // Height stored as: h5 - its v8 grid, h1-h4 - its v9 grid
        // +--------------> X
        // | h1-------h2     Coordinates is:
        // | | \  1  / |     h1 0,0
        // | |  \   /  |     h2 0,1
        // | | 2  h5 3 |     h3 1,0
        // | |  /   \  |     h4 1,1
        // | | /  4  \ |     h5 1/2,1/2
        // | h3-------h4
        // V Y
        // For find height need
        // 1 - detect triangle
        // 2 - solve linear equation from triangle points
        // Calculate coefficients for solve h = a*x + b*y + c

        float a,b,c;
        // Select triangle:
        if (x+y < 1)
        {
            if (x > y)
            {
                // 1 triangle (h1, h2, h5 points)
                float h1 = m_V9[(x_int  )*129 + y_int];
                float h2 = m_V9[(x_int+1)*129 + y_int];
                float h5 = 2 * m_V8[x_int*128 + y_int];
                a = h2-h1;
                b = h5-h1-h2;
                c = h1;
            }
            else
            {
                // 2 triangle (h1, h3, h5 points)
                float h1 = m_V9[x_int*129 + y_int  ];
                float h3 = m_V9[x_int*129 + y_int+1];
                float h5 = 2 * m_V8[x_int*128 + y_int];
                a = h5 - h1 - h3;
                b = h3 - h1;
                c = h1;
            }
        }
        else
        {
            if (x > y)
            {
                // 3 triangle (h2, h4, h5 points)
                float h2 = m_V9[(x_int+1)*129 + y_int  ];
                float h4 = m_V9[(x_int+1)*129 + y_int+1];
                float h5 = 2 * m_V8[x_int*128 + y_int];
                a = h2 + h4 - h5;
                b = h4 - h2;
                c = h5 - h4;
            }
            else
            {
                // 4 triangle (h3, h4, h5 points)
                float h3 = m_V9[(x_int  )*129 + y_int+1];
                float h4 = m_V9[(x_int+1)*129 + y_int+1];
                float h5 = 2 * m_V8[x_int*128 + y_int];
                a = h4 - h3;
                b = h3 + h4 - h5;
                c = h5 - h4;
            }
        }
        // Calculate height
        return a * x + b * y + c;
    }

    float  GridMap::getHeightFromUint8(float x, float y) const
    {
        if (!m_uint8_V8 || !m_uint8_V9)
            return m_gridHeight;

        x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

        int x_int = (int)x;
        int y_int = (int)y;
        x -= x_int;
        y -= y_int;
        x_int&=(MAP_RESOLUTION - 1);
        y_int&=(MAP_RESOLUTION - 1);

        int32_t a, b, c;
        uint8_t *V9_h1_ptr = &m_uint8_V9[x_int*128 + x_int + y_int];
        if (x+y < 1)
        {
            if (x > y)
            {
                // 1 triangle (h1, h2, h5 points)
                int32_t h1 = V9_h1_ptr[  0];
                int32_t h2 = V9_h1_ptr[129];
                int32_t h5 = 2 * m_uint8_V8[x_int*128 + y_int];
                a = h2-h1;
                b = h5-h1-h2;
                c = h1;
            }
            else
            {
                // 2 triangle (h1, h3, h5 points)
                int32_t h1 = V9_h1_ptr[0];
                int32_t h3 = V9_h1_ptr[1];
                int32_t h5 = 2 * m_uint8_V8[x_int*128 + y_int];
                a = h5 - h1 - h3;
                b = h3 - h1;
                c = h1;
            }
        }
        else
        {
            if (x > y)
            {
                // 3 triangle (h2, h4, h5 points)
                int32_t h2 = V9_h1_ptr[129];
                int32_t h4 = V9_h1_ptr[130];
                int32_t h5 = 2 * m_uint8_V8[x_int*128 + y_int];
                a = h2 + h4 - h5;
                b = h4 - h2;
                c = h5 - h4;
            }
            else
            {
                // 4 triangle (h3, h4, h5 points)
                int32_t h3 = V9_h1_ptr[  1];
                int32_t h4 = V9_h1_ptr[130];
                int32_t h5 = 2 * m_uint8_V8[x_int*128 + y_int];
                a = h4 - h3;
                b = h3 + h4 - h5;
                c = h5 - h4;
            }
        }
        // Calculate height
        return (float)((a * x) + (b * y) + c)*m_gridIntHeightMultiplier + m_gridHeight;
    }

    float  GridMap::getHeightFromUint16(float x, float y) const
    {
        if (!m_uint16_V8 || !m_uint16_V9)
            return m_gridHeight;

        x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

        int x_int = (int)x;
        int y_int = (int)y;
        x -= x_int;
        y -= y_int;
        x_int&=(MAP_RESOLUTION - 1);
        y_int&=(MAP_RESOLUTION - 1);

        int32_t a, b, c;
        uint16_t *V9_h1_ptr = &m_uint16_V9[x_int*128 + x_int + y_int];
        if (x+y < 1)
        {
            if (x > y)
            {
                // 1 triangle (h1, h2, h5 points)
                int32_t h1 = V9_h1_ptr[  0];
                int32_t h2 = V9_h1_ptr[129];
                int32_t h5 = 2 * m_uint16_V8[x_int*128 + y_int];
                a = h2-h1;
                b = h5-h1-h2;
                c = h1;
            }
            else
            {
                // 2 triangle (h1, h3, h5 points)
                int32_t h1 = V9_h1_ptr[0];
                int32_t h3 = V9_h1_ptr[1];
                int32_t h5 = 2 * m_uint16_V8[x_int*128 + y_int];
                a = h5 - h1 - h3;
                b = h3 - h1;
                c = h1;
            }
        }
        else
        {
            if (x > y)
            {
                // 3 triangle (h2, h4, h5 points)
                int32_t h2 = V9_h1_ptr[129];
                int32_t h4 = V9_h1_ptr[130];
                int32_t h5 = 2 * m_uint16_V8[x_int*128 + y_int];
                a = h2 + h4 - h5;
                b = h4 - h2;
                c = h5 - h4;
            }
            else
            {
                // 4 triangle (h3, h4, h5 points)
                int32_t h3 = V9_h1_ptr[  1];
                int32_t h4 = V9_h1_ptr[130];
                int32_t h5 = 2 * m_uint16_V8[x_int*128 + y_int];
                a = h4 - h3;
                b = h3 + h4 - h5;
                c = h5 - h4;
            }
        }
        // Calculate height
        return (float)((a * x) + (b * y) + c)*m_gridIntHeightMultiplier + m_gridHeight;
    }

    float  GridMap::getLiquidLevel(float x, float y)
    {
        if (!m_liquid_map)
            return m_liquidLevel;

        x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

        int cx_int = ((int)x & (MAP_RESOLUTION-1)) - m_liquid_offY;
        int cy_int = ((int)y & (MAP_RESOLUTION-1)) - m_liquid_offX;

        if (cx_int < 0 || cx_int >=m_liquid_height)
            return INVALID_HEIGHT;
        if (cy_int < 0 || cy_int >=m_liquid_width )
            return INVALID_HEIGHT;

        return m_liquid_map[cx_int*m_liquid_width + cy_int];
    }

    uint8_t  GridMap::getTerrainType(float x, float y)
    {
        if (!m_liquid_type)
            return (uint8_t)m_liquidType;

        x = 16 * (32 - x/SIZE_OF_GRIDS);
        y = 16 * (32 - y/SIZE_OF_GRIDS);
        int lx = (int)x & 15;
        int ly = (int)y & 15;
        return m_liquid_type[lx*16 + ly];
    }

    // Get water state on map
    inline ZLiquidStatus GridMap::getLiquidStatus(float x, float y, float z, uint8_t ReqLiquidType, LiquidData *data)
    {
        // Check water type (if no water return)
        if (!m_liquid_type && !m_liquidType)
            return LIQUID_MAP_NO_WATER;

        // Get cell
        float cx = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
        float cy = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

        int x_int = (int)cx & (MAP_RESOLUTION-1);
        int y_int = (int)cy & (MAP_RESOLUTION-1);

        // Check water type in cell
        uint8_t type = m_liquid_type ? m_liquid_type[(x_int>>3)*16 + (y_int>>3)] : m_liquidType;
        if (type == 0)
            return LIQUID_MAP_NO_WATER;

        // Check req liquid type mask
        if (ReqLiquidType && !(ReqLiquidType&type))
            return LIQUID_MAP_NO_WATER;

        // Check water level:
        // Check water height map
        int lx_int = x_int - m_liquid_offY;
        int ly_int = y_int - m_liquid_offX;
        if (lx_int < 0 || lx_int >=m_liquid_height)
            return LIQUID_MAP_NO_WATER;
        if (ly_int < 0 || ly_int >=m_liquid_width )
            return LIQUID_MAP_NO_WATER;

        // Get water level
        float liquid_level = m_liquid_map ? m_liquid_map[lx_int*m_liquid_width + ly_int] : m_liquidLevel;
        // Get ground level (sub 0.2 for fix some errors)
        float ground_level = getHeight(x, y);

        // Check water level and ground level
        if (liquid_level < ground_level || z < ground_level - 2)
            return LIQUID_MAP_NO_WATER;

        // All ok in water -> store data
        if (data)
        {
            data->type  = type;
            data->level = liquid_level;
            data->depth_level = ground_level;
        }

        // For speed check as int values
        int delta = int((liquid_level - z) * 10);

        // Get position delta
        if (delta > 20)                   // Under water
            return LIQUID_MAP_UNDER_WATER;
        if (delta > 0 )                   // In water
            return LIQUID_MAP_IN_WATER;
        if (delta > -1)                   // Walk on water
            return LIQUID_MAP_WATER_WALK;
                                          // Above water
        return LIQUID_MAP_ABOVE_WATER;
    }
}