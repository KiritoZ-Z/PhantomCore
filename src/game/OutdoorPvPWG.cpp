#include "OutdoorPvPWG.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Chat.h"
#include "MapManager.h"
#include "ScriptedCreature.h"

OutdoorPvPWG::OutdoorPvPWG()
{
    m_TypeId = OUTDOOR_PVP_WG;
    m_LastResurrectTime = 0;
}

void OutdoorPvPWG::LoadTeamPair(TeamPairMap &pairMap, const TeamPair *pair)
{
    while((*pair)[0])
    {
        pairMap[(*pair)[TEAM_ALLIANCE]]    = (*pair)[TEAM_HORDE];
        pairMap[(*pair)[TEAM_HORDE]]       = (*pair)[TEAM_ALLIANCE];
        ++pair;
    }
}

void OutdoorPvPWG::ResetCreatureEntry(Creature *cr, uint32 entry)
{
    if (!cr)
        return;

    cr->SetOriginalEntry(entry);
    cr->Respawn(true);
    cr->SetVisibility(VISIBILITY_ON);
}

std::string OutdoorPvPWG::GetLocaleString(WG_STRING_LOCALE_INDEX idx, LocaleConstant loc)
{
    if (idx >= MAX_WG_STRINGS || loc >= MAX_LOCALE)
    {
        sLog.outError("WINTERGRASP: Bad call of OutdoorPvPWG::GetLocaleString()!");
        return "[STRING NOT FOUND]";
    }

    if (WG_STRING_LOCALE[idx][loc] == "")
        return WG_STRING_LOCALE[idx][LOCALE_enUS];

    return WG_STRING_LOCALE[idx][loc];
}

// To be enabled soon.
/*
Position OutdoorPvPWG::SetQuestgiverPos(WG_QUESTGIVER_MOVEPOS_INDEX idx, TeamId team)
{
    Position pos;

    pos.m_orientation = 0.0f;
    pos.m_positionX = 0.0f;
    pos.m_positionY = 0.0f;
    pos.m_positionZ = 0.0f;

    if (idx >= MAX_WG_QUESTGIVER)
    {
        sLog.outError("WINTERGRASP: Bad call of OutdoorPvPWG::SetQuestgiverPos()!");
        return pos;
    }

    if (getDefenderTeamId() == team)
        pos.Relocate(WG_QUESTGIVER_DEFENDER_MOVEPOS_MAP[idx][getDefenderTeamId()][0], WG_QUESTGIVER_DEFENDER_MOVEPOS_MAP[idx][getDefenderTeamId()][1],
        WG_QUESTGIVER_DEFENDER_MOVEPOS_MAP[idx][getDefenderTeamId()][2], WG_QUESTGIVER_DEFENDER_MOVEPOS_MAP[idx][getDefenderTeamId()][3]);
    else
        pos.Relocate(WG_QUESTGIVER_ATTACKER_MOVEPOS_MAP[idx][getDefenderTeamId()][0], WG_QUESTGIVER_ATTACKER_MOVEPOS_MAP[idx][getDefenderTeamId()][1],
        WG_QUESTGIVER_ATTACKER_MOVEPOS_MAP[idx][getDefenderTeamId()][2], WG_QUESTGIVER_ATTACKER_MOVEPOS_MAP[idx][getDefenderTeamId()][3]);

    return pos;
}
*/

// To be enabled soon.
/*
void OutdoorPvPWG::LoadCreatureMap(uint64 guid, WG_QUESTGIVER_MOVEPOS_INDEX idx, TeamId team)
{
    Position pos = SetQuestgiverPos(idx, team);

    if (getDefenderTeamId() == team)
        m_creatures[guid].def = true;

    m_creatures[guid].lguid = uint32(GUID_LOPART(guid));
    m_creatures[guid].cr = NULL;
    m_creatures[guid].pos = pos;
    m_creatures[guid].idx = idx;
    m_creatures[guid].team = team;
    m_creatures[guid].isQg = true;

    objmgr.MoveCreData(m_creatures[guid].lguid, 571, pos);
}
*/

void OutdoorPvPWG::SaveData()
{
    sWorld.setWorldState(WS_WINTERGRASP_CONTROLING_TEAMID, uint64(m_defender));
    sWorld.setWorldState(WS_WINTERGRASP_CLOCK_ALLY, m_clock[TEAM_ALLIANCE]);
    sWorld.setWorldState(WS_WINTERGRASP_CLOCK_HORDE, m_clock[TEAM_HORDE]);
    sWorld.setWorldState(WS_WINTERGRASP_ISWAR, uint64(m_wartime));
    sWorld.setWorldState(WS_WINTERGRASP_TIMER, m_timer);

    // TODO: Until the team/state is at startup correct set (not implemented yet) we must set 0 here!
    //sWorld.setWorldState(WS_WINTERGRASP_SHOP_CNT_ALLY, uint64(m_workshopCount[TEAM_ALLIANCE]));
    //sWorld.setWorldState(WS_WINTERGRASP_SHOP_CNT_HORDE, uint64(m_workshopCount[TEAM_HORDE]));
    //sWorld.setWorldState(WS_WINTERGRASP_TOWER_DEST_ALLY, uint64(m_towerDestroyedCount[TEAM_ALLIANCE]));
    //sWorld.setWorldState(WS_WINTERGRASP_TOWER_DEST_HORDE, uint64(m_towerDestroyedCount[TEAM_HORDE]));
    //sWorld.setWorldState(WS_WINTERGRASP_VEHICLE_CNT_ALLY, uint64(m_VehicleCnt[TEAM_ALLIANCE]));
    //sWorld.setWorldState(WS_WINTERGRASP_VEHICLE_CNT_HORDE, uint64(m_VehicleCnt[TEAM_HORDE]));

    sWorld.setWorldState(WS_WINTERGRASP_SHOP_CNT_ALLY, uint64(0));
    sWorld.setWorldState(WS_WINTERGRASP_SHOP_CNT_HORDE, uint64(0));
    sWorld.setWorldState(WS_WINTERGRASP_TOWER_DEST_ALLY, uint64(0));
    sWorld.setWorldState(WS_WINTERGRASP_TOWER_DEST_HORDE, uint64(0));
    sWorld.setWorldState(WS_WINTERGRASP_VEHICLE_CNT_ALLY, uint64(0));
    sWorld.setWorldState(WS_WINTERGRASP_VEHICLE_CNT_HORDE, uint64(0));

    m_saveinterval = WG_MIN_SAVE;
}

bool OutdoorPvPWG::SetupOutdoorPvP()
{
    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
    {
        sWorld.setWorldState(WS_WINTERGRASP_CONTROLING_TEAMID, uint64(TEAM_NEUTRAL));
        return false;
    }

    m_defender = (TeamId)sWorld.getWorldState(WS_WINTERGRASP_CONTROLING_TEAMID);

    m_clock[TEAM_ALLIANCE] = sWorld.getWorldState(WS_WINTERGRASP_CLOCK_ALLY);
    m_clock[TEAM_HORDE] = sWorld.getWorldState(WS_WINTERGRASP_CLOCK_HORDE);

    m_wartime = (bool)sWorld.getWorldState(WS_WINTERGRASP_ISWAR);
    m_timer = sWorld.getWorldState(WS_WINTERGRASP_TIMER);

    // TODO: Until the team/state is at startup correct set (not implemented yet) we must set 0 here!
    //m_workshopCount[TEAM_ALLIANCE] = uint32(sWorld.getWorldState(WS_WINTERGRASP_SHOP_CNT_ALLY));
    //m_workshopCount[TEAM_HORDE] = uint32(sWorld.getWorldState(WS_WINTERGRASP_SHOP_CNT_HORDE));
    //m_towerDestroyedCount[TEAM_ALLIANCE] = uint32(sWorld.getWorldState(WS_WINTERGRASP_TOWER_DEST_ALLY));
    //m_towerDestroyedCount[TEAM_HORDE] = uint32(sWorld.getWorldState(WS_WINTERGRASP_TOWER_DEST_HORDE));
    //m_VehicleCnt[TEAM_ALLIANCE] = uint32(sWorld.getWorldState(WS_WINTERGRASP_VEHICLE_CNT_ALLY));
    //m_VehicleCnt[TEAM_HORDE] = uint32(sWorld.getWorldState(WS_WINTERGRASP_VEHICLE_CNT_HORDE));

    m_workshopCount[TEAM_ALLIANCE] = 0;
    m_workshopCount[TEAM_HORDE] = 0;
    m_towerDestroyedCount[TEAM_ALLIANCE] = 0;
    m_towerDestroyedCount[TEAM_HORDE] = 0;
    m_VehicleCnt[TEAM_ALLIANCE] = 0;
    m_VehicleCnt[TEAM_HORDE] = 0;

    m_towerDamagedCount[TEAM_ALLIANCE] = 0;
    m_towerDamagedCount[TEAM_HORDE] = 0;
    m_changeDefender = false;
    m_tenacityStack = 0;

    m_gate = NULL;
    m_gate_collision1 = NULL;
    m_gate_collision2 = NULL;

    std::list<uint32> engGuids;
    std::list<uint32> spiritGuids;

    // Store Eng, spirit guide guids and questgiver for later use
    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT `guid`,`id` FROM `creature` WHERE `map`=571 AND `id` IN (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
        WG_CREATURE_ENGINEER_A,         WG_CREATURE_ENGINEER_H,
        WG_CREATURE_SPIRIT_GUIDE_A,     WG_CREATURE_SPIRIT_GUIDE_H,
        WG_CREATURE_ENHANCEMENT_A,      WG_CREATURE_ENHANCEMENT_H,
        WG_CREATURE_QUESTGIVER_1_A,     WG_CREATURE_QUESTGIVER_1_H,
        WG_CREATURE_QUESTGIVER_2_A,     WG_CREATURE_QUESTGIVER_2_H,
        WG_CREATURE_QUESTGIVER_3_A,     WG_CREATURE_QUESTGIVER_3_H,
        WG_CREATURE_QUESTGIVER_4_A,     WG_CREATURE_QUESTGIVER_4_H,
        WG_CREATURE_QUESTGIVER_5_A,     WG_CREATURE_QUESTGIVER_5_H,
        WG_CREATURE_QUESTGIVER_6_A,     WG_CREATURE_QUESTGIVER_6_H);

    if (!result)
        sLog.outError("WINTERGRASP: Can't find siege workshop master or spirit guides in creature!");
    else
    {
        do
        {
            Position posHorde, posAlli;
            Field *fields = result->Fetch();
            switch(fields[1].GetUInt32())
            {
                case CRE_ENG_A:
                case CRE_ENG_H:
                    engGuids.push_back(fields[0].GetUInt32());
                    break;
                case CRE_SPI_A:
                case CRE_SPI_H:
                    spiritGuids.push_back(fields[0].GetUInt32());
                    break;
                case 31051:
                    posHorde.Relocate(5081.7, 2173.73, 365.878, 0.855211);
                    posAlli.Relocate(5296.56, 2789.87, 409.275, 0.733038);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31101:
                    posHorde.Relocate(5296.56, 2789.87, 409.275, 0.733038);
                    posAlli.Relocate(5016.57, 3677.53, 362.982, 5.7525);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31052:
                    posHorde.Relocate(5100.07, 2168.89, 365.779, 1.97222);
                    posAlli.Relocate(5298.43, 2738.76, 409.316, 3.97174);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31102:
                    posHorde.Relocate(5298.43, 2738.76, 409.316, 3.97174);
                    posAlli.Relocate(5030.44, 3659.82, 363.194, 1.83336);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31109:
                    posHorde.Relocate(5080.4, 2199, 359.489, 2.96706);
                    posAlli.Relocate(5234.97, 2883.4, 409.275, 4.29351);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31107:
                    posHorde.Relocate(5234.97, 2883.4, 409.275, 4.29351);
                    posAlli.Relocate(5008.64, 3659.91, 361.07, 4.0796);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31153:
                    posHorde.Relocate(5088.49, 2188.18, 365.647, 5.25344);
                    posAlli.Relocate(5366.13, 2833.4, 409.323, 3.14159);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31151:
                    posHorde.Relocate(5366.13, 2833.4, 409.323, 3.14159);
                    posAlli.Relocate(5032.33, 3680.7, 363.018, 3.43167);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31108:
                    posHorde.Relocate(5095.67, 2193.28, 365.924, 4.93928);
                    posAlli.Relocate(5295.56, 2926.67, 409.275, 0.872665);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31106:
                    posHorde.Relocate(5295.56, 2926.67, 409.275, 0.872665);
                    posAlli.Relocate(5032.66, 3674.28, 363.053, 2.9447);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31054:
                    posHorde.Relocate(5088.61, 2167.66, 365.689, 0.680678);
                    posAlli.Relocate(5371.4, 3026.51, 409.206, 3.25003);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31053:
                    posHorde.Relocate(5371.4, 3026.51, 409.206, 3.25003);
                    posAlli.Relocate(5032.44, 3668.66, 363.11, 2.87402);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31036:
                    posHorde.Relocate(5078.28, 2183.7, 365.029, 1.46608);
                    posAlli.Relocate(5359.13, 2837.99, 409.364, 4.69893);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                case 31091:
                    posHorde.Relocate(5359.13, 2837.99, 409.364, 4.69893);
                    posAlli.Relocate(5022.43, 3659.91, 361.61, 1.35426);
                    LoadQuestGiverMap(fields[0].GetUInt32(), posHorde, posAlli);
                    break;
                default:
                    break;
            }
        }
        while (result->NextRow());
    }

    // Select POI
    AreaPOIList areaPOIs;
    float minX = 9999, minY = 9999, maxX = -9999, maxY = -9999;

    for (uint32 i = 0; i < sAreaPOIStore.GetNumRows(); ++i)
    {
        const AreaPOIEntry * poiInfo = sAreaPOIStore.LookupEntry(i);
        if (poiInfo && poiInfo->zoneId == NORTHREND_WINTERGRASP)
        {
            areaPOIs.push_back(poiInfo);
            if (minX > poiInfo->x) minX = poiInfo->x;
            if (minY > poiInfo->y) minY = poiInfo->y;
            if (maxX < poiInfo->x) maxX = poiInfo->x;
            if (maxY < poiInfo->y) maxY = poiInfo->y;
        }
    }
    minX -= 20; minY -= 20; maxX += 20; maxY += 20;

    // Coords: 4290.330078, 1790.359985 - 5558.379883, 4048.889893
    result = WorldDatabase.PQuery("SELECT guid FROM gameobject,gameobject_template WHERE gameobject.map=571 AND gameobject.position_x>%f AND gameobject.position_y>%f"
        " AND gameobject.position_x<%f AND gameobject.position_y<%f AND gameobject_template.type=33 AND gameobject.id=gameobject_template.entry",minX, minY, maxX, maxY);

    if (!result)
    {
        sLog.outError("WINTERGRASP: Can't find any GO within Wintergrasp!");
        return false;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 guid = fields[0].GetUInt32();
        GameObjectData const * goData = objmgr.GetGOData(guid);

        if (!goData) // this should not happen
            continue;

        float x = goData->posX, y = goData->posY;
        float minDist = 100;

        AreaPOIList::iterator poi = areaPOIs.end();
        for (AreaPOIList::iterator itr = areaPOIs.begin(); itr != areaPOIs.end(); ++itr)
        {
            if (!(*itr)->icon[1]) // note: may for other use
                continue;

            float dist = (abs((*itr)->x - x) + abs((*itr)->y - y));
            if (minDist > dist)
            {
                minDist = dist;
                poi = itr;
            }
        }

        if (poi == areaPOIs.end())
            continue;

        // add building to the list
        TeamId teamId = x > POS_X_CENTER ? getDefenderTeamId() : getAttackerTeamId();
        m_buildingStates[guid] = new BuildingState((*poi)->worldState, teamId, getDefenderTeamId() != TEAM_ALLIANCE);

        if ((*poi)->id == 2246)
            m_gate = m_buildingStates[guid];

        areaPOIs.erase(poi);

        // add capture point
        uint32 capturePointEntry = 0;

        switch(goData->id)
        {
            case WG_GO_WORKSHOP_NW:
            case WG_GO_WORKSHOP_W:
            case WG_GO_WORKSHOP_SW:
                capturePointEntry = WG_GO_WORKSHOP_BANNER_NO;
                break;
            case WG_GO_WORKSHOP_NE:
            case WG_GO_WORKSHOP_E:
            case WG_GO_WORKSHOP_SE:
                capturePointEntry = WG_GO_WORKSHOP_BANNER_NW;
                break;
        }

        if (capturePointEntry)
        {
            uint32 engGuid = 0;
            uint32 spiritGuid = 0;
            float minDist = 100; // Find closest Eng to Workshop

            for (std::list<uint32>::iterator itr = engGuids.begin(); itr != engGuids.end(); ++itr)
            {
                const CreatureData *creData = objmgr.GetCreatureData(*itr);

                if (!creData)
                    continue;

                float dist = (abs(creData->posX - x) + abs(creData->posY - y));
                if (minDist > dist)
                {
                    minDist = dist;
                    engGuid = *itr;
                }
            }

            if (!engGuid)
            {
                sLog.outError("WINTERGRASP: Can't find nearby siege workshop master!");
                continue;
            }
            else
                engGuids.remove(engGuid);

            // Find closest Spirit Guide to Workshop
            minDist = 255;

            for (std::list<uint32>::iterator itr = spiritGuids.begin(); itr != spiritGuids.end(); ++itr)
            {
                const CreatureData *creData = objmgr.GetCreatureData(*itr);

                if (!creData)
                    continue;

                float dist = (abs(creData->posX - x) + abs(creData->posY - y));

                if (minDist > dist)
                {
                    minDist = dist;
                    spiritGuid = *itr;
                }
            }

            // Inside fortress won't be capturable
            OPvPCapturePointWG *workshop = new OPvPCapturePointWG(this, m_buildingStates[guid]);
            if (goData->posX < POS_X_CENTER && !workshop->SetCapturePointData(capturePointEntry, goData->mapid, goData->posX + 40 * cos(goData->orientation + M_PI / 2), goData->posY + 40 * sin(goData->orientation + M_PI / 2), goData->posZ))
            {
                delete workshop;
                sLog.outError("WINTERGRASP: Can't add capture point!");
                continue;
            }

            const CreatureData *creData = objmgr.GetCreatureData(engGuid);

            if (!creData)
                continue;

            workshop->m_engEntry = const_cast<uint32*>(&creData->id);
            const_cast<CreatureData*>(creData)->displayid = 0;
            workshop->m_engGuid = engGuid;

            // Back spirit is linked to one of the inside workshops, 1 workshop wont have spirit
            if (spiritGuid)
            {
                spiritGuids.remove(spiritGuid);

                const CreatureData *spiritData = objmgr.GetCreatureData(spiritGuid);

                if (!spiritData)
                    continue;

                workshop->m_spiEntry = const_cast<uint32*>(&spiritData->id);
                const_cast<CreatureData*>(spiritData)->displayid = 0;
                workshop->m_spiGuid = spiritGuid;
            }
            else
                workshop->m_spiGuid = 0;

            workshop->m_workshopGuid = guid;
            AddCapturePoint(workshop);
            m_buildingStates[guid]->type = BUILDING_WORKSHOP;
            workshop->SetTeamByBuildingState();
        }
    }
    while (result->NextRow());

    engGuids.clear();
    spiritGuids.clear();

    if (!m_gate)
    {
        sLog.outError("WINTERGRASP: Can't find wintergrasp fortress gate!");
        return false;
    }

    // Load Graveyard
    GraveYardMap::const_iterator graveLow  = objmgr.mGraveYardMap.lower_bound(NORTHREND_WINTERGRASP);
    GraveYardMap::const_iterator graveUp   = objmgr.mGraveYardMap.upper_bound(NORTHREND_WINTERGRASP);

    for (AreaPOIList::iterator itr = areaPOIs.begin(); itr != areaPOIs.end();)
    {
        if ((*itr)->icon[1] == 8)
        {
            // find or create grave yard
            const WorldSafeLocsEntry *loc = objmgr.GetClosestGraveYard((*itr)->x, (*itr)->y, (*itr)->z, (*itr)->mapId, 0);

            if (!loc)
            {
                ++itr;
                continue;
            }

            GraveYardMap::const_iterator graveItr;

            for (graveItr = graveLow; graveItr != graveUp; ++graveItr)
                if (graveItr->second.safeLocId == loc->ID)
                    break;

            if (graveItr == graveUp)
            {
                GraveYardData graveData;
                graveData.safeLocId = loc->ID;
                graveData.team = 0;
                graveItr = objmgr.mGraveYardMap.insert(std::make_pair(NORTHREND_WINTERGRASP, graveData));
            }

            for (BuildingStateMap::iterator stateItr = m_buildingStates.begin(); stateItr != m_buildingStates.end(); ++stateItr)
            {
                if (stateItr->second->worldState == (*itr)->worldState)
                {
                    stateItr->second->graveTeam = const_cast<uint32*>(&graveItr->second.team);
                    break;
                }
            }
            areaPOIs.erase(itr++);
        }
        else
            ++itr;
    }

    // Titan Relic
    objmgr.AddGOData(WG_GO_TITAN_RELIC, 571, 5440.0f, 2840.8f, 420.43f + 10.0f, 0);

    LoadTeamPair(m_goDisplayPair, OutdoorPvPWGGODisplayPair);
    LoadTeamPair(m_creEntryPair, OutdoorPvPWGCreEntryPair);

    if (!m_timer)
        m_timer = sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_START_TIME) * MINUTE * IN_MILISECONDS;

    m_saveinterval = WG_MIN_SAVE;

    RegisterZone(NORTHREND_WINTERGRASP);

    return true;
}

void OutdoorPvPWG::ProcessEvent(GameObject *obj, uint32 eventId)
{
    if (!obj)
        return;

    if (obj->GetEntry() == WG_GO_TITAN_RELIC)
    {
        if (isWarTime() && m_gate && obj->GetGOInfo()->goober.eventId == eventId && m_gate->damageState == DAMAGE_DESTROYED)
        {
            m_changeDefender = true;
            m_timer = 0;
        }
    }
    else if (obj->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        BuildingStateMap::const_iterator itr = m_buildingStates.find(obj->GetDBTableGUIDLow());

        if (itr == m_buildingStates.end())
            return;

        std::string msgStr;

        switch(eventId)
        {
            case WG_GO_TOWER_FLAMEWATCH_DAMAGED:
            case WG_GO_TOWER_FLAMEWATCH_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_TOWER_FLAMEWATCH, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_TOWER_SHADOWSIGHT_DAMAGED:
            case WG_GO_TOWER_SHADOWSIGHT_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_TOWER_SHADOWSIGHT, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_TOWER_WINTERS_EDGE_DAMAGED:
            case WG_GO_TOWER_WINTERS_EDGE_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_TOWER_WINTERS_EDGE, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_WORKSHOP_E_DAMAGED:
            case WG_GO_WORKSHOP_E_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_WORKSHOP_E, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_WORKSHOP_W_DAMAGED:
            case WG_GO_WORKSHOP_W_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_WORKSHOP_W, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_WORKSHOP_NW_DAMAGED:
            case WG_GO_WORKSHOP_NW_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_WORKSHOP_NW, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_WORKSHOP_NE_DAMAGED:
            case WG_GO_WORKSHOP_NE_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_WORKSHOP_NE, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_WORKSHOP_SW_DAMAGED:
            case WG_GO_WORKSHOP_SW_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_WORKSHOP_SW, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_WORKSHOP_SE_DAMAGED:
            case WG_GO_WORKSHOP_SE_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_WORKSHOP_SE, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_KEEP_TOWER_NW_DAMAGED:
            case WG_GO_KEEP_TOWER_NW_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_KEEP_TOWER_NW, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_KEEP_TOWER_NE_DAMAGED:
            case WG_GO_KEEP_TOWER_NE_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_KEEP_TOWER_NE, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_KEEP_TOWER_SW_DAMAGED:
            case WG_GO_KEEP_TOWER_SW_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_KEEP_TOWER_SW, sWorld.GetDefaultDbcLocale());
                break;
            case WG_GO_KEEP_TOWER_SE_DAMAGED:
            case WG_GO_KEEP_TOWER_SE_DESTROYED:
                msgStr = GetLocaleString(WG_STRING_KEEP_TOWER_SE, sWorld.GetDefaultDbcLocale());
                break;
            default:
                msgStr = GetLocaleString(WG_STRING_DEFAULT, sWorld.GetDefaultDbcLocale());
        }

        BuildingState *state = itr->second;

        if (!state || !state->building)
            return;

        if (eventId == obj->GetGOInfo()->building.damagedEvent)
        {
            state->damageState = DAMAGE_DAMAGED;

            switch(state->type)
            {
                case BUILDING_WORKSHOP:
                    msgStr = fmtstring(objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_WORKSHOP_DAMAGED), msgStr.c_str(), objmgr.GetTrinityStringForDBCLocale(getDefenderTeamId() == TEAM_ALLIANCE ? LANG_BG_AB_HORDE : LANG_BG_AB_ALLY));
                    sWorld.SendZoneText(NORTHREND_WINTERGRASP, msgStr.c_str());
                    break;
                case BUILDING_WALL:
                    sWorld.SendZoneText(NORTHREND_WINTERGRASP, objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_FORTRESS_UNDER_ATTACK));
                    break;
                case BUILDING_TOWER:
                    ++m_towerDamagedCount[state->GetTeamId()];
                    msgStr = fmtstring(objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_TOWER_DAMAGED), msgStr.c_str());
                    sWorld.SendZoneText(NORTHREND_WINTERGRASP, msgStr.c_str());
                    break;
            }
        }
        else if (eventId == obj->GetGOInfo()->building.destroyedEvent)
        {
            state->damageState = DAMAGE_DESTROYED;

            // If the gate was destroyed, the invisible collisions must be opened!
            if (m_gate && m_gate->building && state->building->GetEntry() == m_gate->building->GetEntry())
            {
                if (!m_gate_collision1)
                    sLog.outError("WINTERGRASP: Can't find GO with entry 194162 'Doodad_WG_Keep_Door01_collision01'!");
                else
                    m_gate_collision1->SetGoState(GO_STATE_ACTIVE);

                if (!m_gate_collision2)
                    sLog.outError("WINTERGRASP: Can't find GO with entry 194323 'Wintergrasp Keep Collision Wall'!");
                else
                    m_gate_collision2->SetGoState(GO_STATE_ACTIVE);
            }

            switch(state->type)
            {
                case BUILDING_WORKSHOP:
                    ModifyWorkshopCount(state->GetTeamId(), false);
                    msgStr = fmtstring(objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_WORKSHOP_DESTROYED), msgStr.c_str(), objmgr.GetTrinityStringForDBCLocale(getDefenderTeamId() == TEAM_ALLIANCE ? LANG_BG_AB_HORDE : LANG_BG_AB_ALLY));
                    sWorld.SendZoneText(NORTHREND_WINTERGRASP, msgStr.c_str());
                    break;
                case BUILDING_WALL:
                    sWorld.SendZoneText(NORTHREND_WINTERGRASP, objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_FORTRESS_UNDER_ATTACK));
                    break;
                case BUILDING_TOWER:
                    --m_towerDamagedCount[state->GetTeamId()];
                    ++m_towerDestroyedCount[state->GetTeamId()];

                    if (state->GetTeamId() == getAttackerTeamId())
                    {
                        TeamCastSpell(getAttackerTeamId(), -SPELL_TOWER_CONTROL);
                        TeamCastSpell(getDefenderTeamId(), -SPELL_TOWER_CONTROL);
                        uint32 attStack = 3 - m_towerDestroyedCount[getAttackerTeamId()];

                        if (m_towerDestroyedCount[getAttackerTeamId()])
                        {
                            for (PlayerSet::iterator itr = m_players[getDefenderTeamId()].begin(); itr != m_players[getDefenderTeamId()].end(); ++itr)
                                if ((*itr)->getLevel() > 69)
                                    (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), m_towerDestroyedCount[getAttackerTeamId()]);
                        }

                        if (attStack)
                        {
                            for (PlayerSet::iterator itr = m_players[getAttackerTeamId()].begin(); itr != m_players[getAttackerTeamId()].end(); ++itr)
                                if ((*itr)->getLevel() > 69)
                                    (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), attStack);
                        }
                        else
                        {
                            if (m_timer < 600000)
                                m_timer = 0;
                            else
                                m_timer = m_timer - 600000; // - 10 mins
                        }
                    }
                    msgStr = fmtstring(objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_TOWER_DESTROYED), msgStr.c_str());
                    sWorld.SendZoneText(NORTHREND_WINTERGRASP, msgStr.c_str());
                    break;
            }
            BroadcastStateChange(state);
        }
        SaveData();
    }
}

void OutdoorPvPWG::ModifyWorkshopCount(TeamId team, bool add)
{
    if (team == TEAM_NEUTRAL)
        return;

    if (add)
        ++m_workshopCount[team];
    else if (m_workshopCount[team])
        --m_workshopCount[team];
    else
        sLog.outError("WINTERGRASP: OutdoorPvPWG::ModifyWorkshopCount: negative workshop count!");

    SendUpdateWorldState(MaxVehNumWorldState[team], m_workshopCount[team] * MAX_VEHICLE_PER_WORKSHOP);
    SaveData();
}

OutdoorPvPWGCreType OutdoorPvPWG::GetCreatureType(uint32 entry) const
{
    // VEHICLES, GUARDS and TURRETS gives kill credit
    // OTHER Not in wartime
    // TURRET Only during wartime
    // SPECIAL like "OTHER" but no despawn conditions
    // Entries like Case A: Case: B have their own despawn function
    switch(entry)
    {
        // Catapult
        case 27881:
        // Demolisher
        case 28094:
        // Alliance Siege Engine
        case 28312:
        // Horde Siege Engine
        case 32627:
        // Siege turret
        case 28319:
        // Siege turret
        case 32629:
            return CREATURE_SIEGE_VEHICLE;

        // Wintergrasp Tower cannon
        case 28366:
            return CREATURE_TURRET;

        // Alliance Engineer
        case CRE_ENG_A:
        // Horde Engineer
        case CRE_ENG_H:
            return CREATURE_ENGINEER;

        // Champions
        case 30739:
        case 30740:
        // Guards
        case 32307:
        case 32308:
            return CREATURE_GUARD;

        // Dwarven Spirit Guide
        case CRE_SPI_A:
        // Taunka Spirit Guide
        case CRE_SPI_H:
            return CREATURE_SPIRIT_GUIDE;

        // Spirit Healers
        case 6491:
            return CREATURE_SPIRIT_HEALER;

        // Hoodoo Master & Sorceress
        case 31101:
        case 31051:
        // Vieron Blazefeather & Bowyer
        case 31102:
        case 31052:
        // Lieutenant & Senior Demolitionist
        case 31107:
        case 31109:
        // Tactical Officer
        case 31151:
        case 31153:
        // Siegesmith & Siege Master
        case 31106:
        case 31108:
        // Primalist & Anchorite
        case 31053:
        case 31054:
        // Commander
        case 31091:
        case 31036:
            return CREATURE_QUESTGIVER;

        // Warbringer && Brigadier General
        case 32615:
        case 32626:
        // Quartermasters
        case 32296:
        case 32294:
        case 39173:
        case 39172:
        // Flight Masters
        case 30870:
        case 30869:
            return CREATURE_SPECIAL;

        // To be enabled soon.
        /*
        case WG_CREATURE_INVISIBLE_STALKER:
            return WG_CREATURE_TRIGGER;
        */

        // Revenants, Elementals, etc
        default:
            return CREATURE_OTHER;
    }
}

void OutdoorPvPWG::OnCreatureCreate(Creature *creature, bool add)
{
    uint64 guid = creature->GetGUID();
    TeamId team = TEAM_NEUTRAL;

    if (creature->getFaction() == WintergraspFaction[TEAM_ALLIANCE])
        team = TEAM_ALLIANCE;
    else if (creature->getFaction() == WintergraspFaction[TEAM_HORDE])
        team = TEAM_HORDE;

    switch(GetCreatureType(creature->GetEntry()))
    {
        case WG_CREATURE_TRIGGER:
            break;

        case CREATURE_SIEGE_VEHICLE:
            if (!creature->isSummon())
                break;

            if (creature->GetEntry() == WG_CREATURE_SIEGE_TURRET_A || creature->GetEntry() == WG_CREATURE_SIEGE_TURRET_H)
            {
                // For some reason the horde siege turrets have wrong faction (1732)!
                if (creature->GetOwner() && creature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
                    if (creature->getFaction() != WintergraspFaction[((Player*)creature->GetOwner())->GetTeamId()])
                    {
                        creature->setFaction(WintergraspFaction[((Player*)creature->GetOwner())->GetTeamId()]);
                        m_turrets[((Player*)creature->GetOwner())->GetTeamId()].insert(creature);
                    }
            }

            if (add)
            {
                if (team == TEAM_NEUTRAL)
                    break;

                if (uint32 engLowguid = GUID_LOPART(creature->ToTempSummon()->GetSummonerGUID()))
                {
                    if (OPvPCapturePointWG *workshop = GetWorkshopByEngGuid(engLowguid))
                    {
                        if (CanBuildVehicle(workshop))
                            m_vehicles[team].insert(creature);
                        else
                        {
                            creature->SetRespawnTime(DAY);
                            creature->ForcedDespawn();
                            break;
                        }
                    }
                }

                if (m_tenacityStack > 0 && team == TEAM_ALLIANCE)
                    creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, m_tenacityStack);
                else if (m_tenacityStack < 0 && team == TEAM_HORDE)
                    creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, -m_tenacityStack);
            }
            else // the faction may be changed in uncharm
            {
                // TODO: now you have to wait until the corpse of vehicle disappear to build a new one
                if (m_vehicles[TEAM_ALLIANCE].erase(creature))
                    team = TEAM_ALLIANCE;
                else if (m_vehicles[TEAM_HORDE].erase(creature))
                    team = TEAM_HORDE;
                else
                    break;
            }
            SendUpdateWorldState(VehNumWorldState[team], m_vehicles[team].size());
            break;

        case CREATURE_QUESTGIVER:
            if (add)
                m_questgivers[creature->GetDBTableGUIDLow()] = creature;
            else
                m_questgivers.erase(creature->GetDBTableGUIDLow());
            break;
        case CREATURE_ENGINEER:
            for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
            {
                if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                    if (workshop->m_engGuid == creature->GetDBTableGUIDLow())
                    {
                        workshop->m_engineer = add ? creature : NULL;
                        break;
                    }
            }
            break;
        case CREATURE_SPIRIT_GUIDE:
            for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
            {
                if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                    if (workshop->m_spiGuid == creature->GetDBTableGUIDLow())
                    {
                        workshop->m_spiritguide = add ? creature : NULL;
                        break;
                    }
            }
            creature->CastSpell(creature, SPELL_SPIRITUAL_IMMUNITY, true);
        case CREATURE_SPIRIT_HEALER:
        case CREATURE_TURRET:
        case CREATURE_OTHER:
            if (add)
                UpdateCreatureInfo(creature);
        default:
            if (add)
                m_creatures.insert(creature);
            else
                m_creatures.erase(creature);
            break;
    }
}

void OutdoorPvPWG::OnGameObjectCreate(GameObject *go, bool add)
{
    OutdoorPvP::OnGameObjectCreate(go, add);

    switch(go->GetEntry())
    {
        case WG_GO_KEEP_DOOR01_COLLISION:
            m_gate_collision1 = const_cast<GameObject*>(go);
            break;
        case WG_GO_KEEP_COLLISION_WALL:
            m_gate_collision2 = const_cast<GameObject*>(go);
            break;
    }

    if (UpdateGameObjectInfo(go))
    {
        if (add)
            m_gobjects.insert(go);
        else
            m_gobjects.erase(go);
    }
    //do we need to store building?
    else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        BuildingStateMap::const_iterator itr = m_buildingStates.find(go->GetDBTableGUIDLow());
        if (itr != m_buildingStates.end())
        {
            itr->second->building = add ? go : NULL;

            if (go->GetGOInfo()->displayId == WG_GO_DISPLAY_KEEP_TOWER || go->GetGOInfo()->displayId == WG_GO_DISPLAY_TOWER)
                itr->second->type = BUILDING_TOWER;

            if (!add || itr->second->damageState == DAMAGE_INTACT && !itr->second->health)
                itr->second->health = go->GetGOValue()->building.health;
            else
            {
                go->GetGOValue()->building.health = itr->second->health;

                if (itr->second->damageState == DAMAGE_DAMAGED)
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
                else if (itr->second->damageState == DAMAGE_DESTROYED)
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
            }
        }
    }
}

void OutdoorPvPWG::UpdateAllWorldObject()
{
    // update cre and go factions
    for (GameObjectSet::iterator itr = m_gobjects.begin(); itr != m_gobjects.end(); ++itr)
        UpdateGameObjectInfo(*itr);

    for (CreatureSet::iterator itr = m_creatures.begin(); itr != m_creatures.end(); ++itr)
        UpdateCreatureInfo(*itr);

    for (QuestGiverMap::iterator itr = m_questgivers.begin(); itr != m_questgivers.end(); ++itr)
        UpdateQuestGiverPosition((*itr).first, (*itr).second);

    // TODO: Must only be done @ battle start!
    RebuildAllBuildings();

    // TODO: Must only be done @ battle start!
    for (OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
            workshop->SetTeamByBuildingState();

}

void OutdoorPvPWG::RebuildAllBuildings()
{
    m_workshopCount[TEAM_ALLIANCE] = 0;
    m_workshopCount[TEAM_HORDE] = 0;

    for (BuildingStateMap::const_iterator itr = m_buildingStates.begin(); itr != m_buildingStates.end(); ++itr)
    {
        if (itr->second->building)
        {
            UpdateGameObjectInfo(itr->second->building);
            itr->second->building->Rebuild();
            itr->second->health = itr->second->building->GetGOValue()->building.health;
        }
        else
            itr->second->health = 0;

        if (itr->second->type == BUILDING_WORKSHOP)
            ModifyWorkshopCount(itr->second->GetTeamId(), true);

        itr->second->damageState = DAMAGE_INTACT;
        itr->second->SetTeam(getDefenderTeamId() == TEAM_ALLIANCE ? OTHER_TEAM(itr->second->defaultTeam) : itr->second->defaultTeam);
    }
    m_towerDamagedCount[TEAM_ALLIANCE] = 0;
    m_towerDamagedCount[TEAM_HORDE] = 0;
    m_towerDestroyedCount[TEAM_ALLIANCE] = 0;
    m_towerDestroyedCount[TEAM_HORDE] = 0;
    m_VehicleCnt[TEAM_ALLIANCE] = 0;
    m_VehicleCnt[TEAM_HORDE] = 0;
}

void OutdoorPvPWG::SendInitWorldStatesTo(Player *player) const
{
    WorldPacket data(SMSG_INIT_WORLD_STATES, (4+4+4+2+(m_buildingStates.size()*8)));

    data << uint32(571);
    data << uint32(NORTHREND_WINTERGRASP);
    data << uint32(0);
    data << uint16(4+2+4+m_buildingStates.size());

    data << uint32(3803) << uint32(getDefenderTeamId() == TEAM_ALLIANCE ? 1 : 0);
    data << uint32(3802) << uint32(getDefenderTeamId() != TEAM_ALLIANCE ? 1 : 0);
    data << uint32(3801) << uint32(isWarTime() ? 0 : 1);
    data << uint32(3710) << uint32(isWarTime() ? 1 : 0);

    for (uint32 i = 0; i < 2; ++i)
        data << ClockWorldState[i] << m_clock[i];

    data << uint32(3490) << uint32(m_vehicles[TEAM_HORDE].size());
    data << uint32(3491) << m_workshopCount[TEAM_HORDE] * MAX_VEHICLE_PER_WORKSHOP;
    data << uint32(3680) << uint32(m_vehicles[TEAM_ALLIANCE].size());
    data << uint32(3681) << m_workshopCount[TEAM_ALLIANCE] * MAX_VEHICLE_PER_WORKSHOP;

    for (BuildingStateMap::const_iterator itr = m_buildingStates.begin(); itr != m_buildingStates.end(); ++itr)
        itr->second->FillData(data);

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        BroadcastPacket(data);
}

void OutdoorPvPWG::BroadcastStateChange(BuildingState *state) const
{
    if (m_sendUpdate)
        for (uint32 team = 0; team < 2; ++team)
            for (PlayerSet::const_iterator p_itr = m_players[team].begin(); p_itr != m_players[team].end(); ++p_itr)
                state->SendUpdate(*p_itr);
}

// Called at Start and Battle End
bool OutdoorPvPWG::UpdateCreatureInfo(Creature *creature)
{
    if (!creature)
        return false;

    switch(GetCreatureType(creature->GetEntry()))
    {
        case CREATURE_TURRET:
            if (isWarTime())
            {
                if (!creature->isAlive())
                    creature->Respawn(true);

                creature->setFaction(WintergraspFaction[getDefenderTeamId()]);
                creature->SetVisibility(VISIBILITY_ON);
            }
            else
            {
                if (creature->IsVehicle() && creature->GetVehicleKit())
                    creature->GetVehicleKit()->RemoveAllPassengers();

                creature->SetVisibility(VISIBILITY_OFF);
                creature->setFaction(35);
            }
            return false;
        case CREATURE_OTHER:
            if (isWarTime())
            {
                creature->SetVisibility(VISIBILITY_OFF);
                creature->setFaction(35);
            }
            else
            {
                creature->RestoreFaction();
                creature->SetVisibility(VISIBILITY_ON);
            }
            return false;
        case CREATURE_SPIRIT_GUIDE:
            if (isWarTime())
                creature->SetVisibility(VISIBILITY_ON);
            else
                creature->SetVisibility(VISIBILITY_OFF);
            return false;
        case CREATURE_SPIRIT_HEALER:
            creature->SetVisibility(isWarTime() ? VISIBILITY_OFF : VISIBILITY_ON);
            return false;
        case CREATURE_ENGINEER:
            return false;
        case CREATURE_SIEGE_VEHICLE:
            creature->ForcedDespawn();
            return false;
        case CREATURE_GUARD:
        case CREATURE_SPECIAL:
            {
                TeamPairMap::const_iterator itr = m_creEntryPair.find(creature->GetCreatureData()->id);
                if (itr != m_creEntryPair.end())
                    ResetCreatureEntry(creature, getDefenderTeamId() == TEAM_ALLIANCE ? itr->second : itr->first);

                return false;
            }
        default:
            return false;
    }
}

bool OutdoorPvPWG::UpdateQuestGiverPosition(uint32 guid, Creature *creature)
{
    Position pos = m_qgPosMap[std::pair<uint32, bool>(guid, getDefenderTeamId() == TEAM_HORDE)];

    if (creature && creature->IsInWorld())
    {
        // if not questgiver or position is the same, do nothing
        if (creature->GetPositionX() == pos.GetPositionX() && creature->GetPositionY() == pos.GetPositionY() && creature->GetPositionZ() == pos.GetPositionZ())
            return false;

        creature->SetHomePosition(pos);
        creature->DestroyForNearbyPlayers();

        if (!creature->GetMap()->IsLoaded(pos.GetPositionX(), pos.GetPositionY()))
            creature->GetMap()->LoadGrid(pos.GetPositionX(), pos.GetPositionY());

        creature->GetMap()->CreatureRelocation(creature, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
        creature->Respawn(true);
        creature->CombatStop(true);
        creature->getThreatManager().getThreatList().clear();
    }
    else
        objmgr.MoveCreData(guid, 571, pos);

    return true;
}

// Return false = Need to rebuild at battle End/Start
//        true  = no need to rebuild (ie: Banners or teleporters)
bool OutdoorPvPWG::UpdateGameObjectInfo(GameObject *go) const
{
    uint32 attFaction = 35;
    uint32 defFaction = 35;

    if (isWarTime())
    {
        attFaction = WintergraspFaction[getAttackerTeamId()];
        defFaction = WintergraspFaction[getDefenderTeamId()];
    }

    switch(go->GetGOInfo()->displayId)
    {
        case 8556:
            // 194162: Doodad_WG_Keep_Door01_collision01 - "Invisible Wall"
            // 194323: Wintergrasp Keep Collision Wall - "Invisible Wall"
            if (isWarTime())
                go->SetGoState(GO_STATE_READY);
            return true;
        case 8244: // Defender's Portal - Vehicle Teleporter
            go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[getDefenderTeamId()]);
            return true;
        case 7967: // Titan relic
            go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[getAttackerTeamId()]);
            return true;
        case 8165: // Wintergrasp Keep Door
        case 7877: // Wintergrasp Fortress Wall
        case 7878: // Wintergrasp Keep Tower
        case 7906: // Wintergrasp Fortress Gate
        case 7909: // Wintergrasp Wall
            go->SetUInt32Value(GAMEOBJECT_FACTION, defFaction);
            return false;
        case 7900: // Flamewatch Tower - Shadowsight Tower - Winter's Edge Tower
            go->SetUInt32Value(GAMEOBJECT_FACTION, attFaction);
            return false;
        case 8208: // Goblin Workshop
            if (OPvPCapturePointWG *workshop = GetWorkshopByGOGuid(go->GetGUID()))
                go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[workshop->m_buildingState->GetTeamId()]);
            return false;
    }

    // Note: this is only for test, still need db support
    TeamPairMap::const_iterator itr = m_goDisplayPair.find(go->GetGOInfo()->displayId);
    if (itr != m_goDisplayPair.end())
    {
        go->SetUInt32Value(GAMEOBJECT_DISPLAYID, getDefenderTeamId() == TEAM_ALLIANCE ?
            itr->second : itr->first);
        return true;
    }
    return false;
}

void OutdoorPvPWG::HandleEssenceOfWintergrasp(Player *plr, uint32 zoneId)
{
    if (!plr || !zoneId)
        return;

    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED) || isWarTime() || plr->GetTeamId() != getDefenderTeamId())
    {
        plr->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_WINNER);
        plr->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_WORLD);
        return;
    }

    switch(zoneId)
    {
        // Outdoor
        case NORTHREND_WINTERGRASP:
            plr->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_WORLD);
            plr->CastSpell(plr, SPELL_ESSENCE_OF_WINTERGRASP_WINNER, true);
            break;
        case NORTHREND_BOREAN_TUNDRA:
        case NORTHREND_CRYSTALSONG_FOREST:
        case NORTHREND_DALARAN:
        case NORTHREND_DRAGONBLIGHT:
        case NORTHREND_GRIZZLY_HILLS:
        case NORTHREND_HOWLING_FJORD:
        case NORTHREND_HROTHGARS_LANDING:
        case NORTHREND_ICECROWN:
        case NORTHREND_SHOLAZAR_BASIN:
        case NORTHREND_STORM_PEAKS:
        case NORTHREND_ZULDRAK:

        // Dungeons
        case NORTHREND_AHNKAHET:
        case NORTHREND_AZJOL_NERUB:
        case NORTHREND_CULLING_OF_STRATHOLME:
        case NORTHREND_TRIAL_OF_THE_CHAMPION:
        case NORTHREND_DRAKTHARON_KEEP:
        case NORTHREND_GUNDRAK:
        case NORTHREND_NEXUS:
        case NORTHREND_OCULUS:
        case NORTHREND_VIOLET_HOLD:
        case NORTHREND_HALLS_OF_LIGHTNING:
        case NORTHREND_HALLS_OF_STONE:
        case NORTHREND_UTGARDE_KEEP:
        case NORTHREND_UTGARDE_PINNACLE:
        case NORTHREND_FORGE_OF_SOULS:
        case NORTHREND_PIT_OF_SARON:
        case NORTHREND_HALLS_OF_REFLECTION:

        // Raids
        case NORTHREND_TRIAL_OF_THE_CRUSADER:
        case NORTHREND_NAXXRAMAS:
        case NORTHREND_OBSIDIAN_SANCTUM:
        case NORTHREND_ULDUAR:
        case NORTHREND_ICECROWN_CITADEL:
            plr->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_WINNER);
            plr->CastSpell(plr, SPELL_ESSENCE_OF_WINTERGRASP_WORLD, true);
            break;
        default:
            plr->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_WINNER);
            plr->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_WORLD);
            break;
    }
}

void OutdoorPvPWG::HandlePlayerEnterZone(Player *plr, uint32 zone)
{
    HandleEssenceOfWintergrasp(plr, zone);

    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return;

    if (isWarTime())
    {
        if (plr->getLevel() > 69)
        {
            if (!plr->HasAura(SPELL_RECRUIT) && !plr->HasAura(SPELL_CORPORAL) && !plr->HasAura(SPELL_LIEUTENANT))
                plr->CastSpell(plr, SPELL_RECRUIT, true);

            if (plr->GetTeamId() == getAttackerTeamId())
            {
                if (m_towerDestroyedCount[getAttackerTeamId()] < 3)
                    plr->SetAuraStack(SPELL_TOWER_CONTROL, plr, 3 - m_towerDestroyedCount[getAttackerTeamId()]);
            }
            else
            {
                if (m_towerDestroyedCount[getAttackerTeamId()])
                    plr->SetAuraStack(SPELL_TOWER_CONTROL, plr, m_towerDestroyedCount[getAttackerTeamId()]);
            }
        }
    }

    SendInitWorldStatesTo(plr);
    OutdoorPvP::HandlePlayerEnterZone(plr, zone);
    UpdateTenacityStack();
}

// Reapply Auras if needed
void OutdoorPvPWG::HandlePlayerResurrects(Player *plr, uint32 zone)
{
    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return;

    if (isWarTime())
    {
        if (plr->getLevel() > 69)
        {
            // Tenacity
            if (plr->GetTeamId() == TEAM_ALLIANCE && m_tenacityStack > 0 || plr->GetTeamId() == TEAM_HORDE && m_tenacityStack < 0)
            {
                if (plr->HasAura(SPELL_TENACITY))
                    plr->RemoveAurasDueToSpell(SPELL_TENACITY);

                int32 newStack = m_tenacityStack < 0 ? -m_tenacityStack : m_tenacityStack;

                if (newStack > 20)
                    newStack = 20;

                plr->SetAuraStack(SPELL_TENACITY, plr, newStack);
            }
            // Tower Control
            if (plr->GetTeamId() == getAttackerTeamId())
            {
                if (m_towerDestroyedCount[getAttackerTeamId()] < 3)
                    plr->SetAuraStack(SPELL_TOWER_CONTROL, plr, 3 - m_towerDestroyedCount[getAttackerTeamId()]);
            }
            else
            {
                if (m_towerDestroyedCount[getAttackerTeamId()])
                    plr->SetAuraStack(SPELL_TOWER_CONTROL, plr, m_towerDestroyedCount[getAttackerTeamId()]);
            }
        }
    }
    OutdoorPvP::HandlePlayerResurrects(plr, zone);
}

void OutdoorPvPWG::HandlePlayerLeaveZone(Player * plr, uint32 zone)
{
    HandleEssenceOfWintergrasp(plr, zone);

    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return;

    if (!plr->GetSession()->PlayerLogout())
    {
        if (plr->GetVehicle()) // dismiss in change zone case
            plr->GetVehicle()->Dismiss();

        plr->RemoveAurasDueToSpell(SPELL_RECRUIT);
        plr->RemoveAurasDueToSpell(SPELL_CORPORAL);
        plr->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
        plr->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        plr->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
    }
    plr->RemoveAurasDueToSpell(SPELL_TENACITY);
    OutdoorPvP::HandlePlayerLeaveZone(plr, zone);
    UpdateTenacityStack();
}

void OutdoorPvPWG::PromotePlayer(Player *killer) const
{
    if (!killer)
        return;

    Aura * aur;

    if (aur = killer->GetAura(SPELL_RECRUIT))
    {
        if (aur->GetStackAmount() >= 5)
        {
            killer->RemoveAura(SPELL_RECRUIT);
            killer->CastSpell(killer, SPELL_CORPORAL, true);
            ChatHandler(killer).PSendSysMessage(LANG_BG_WG_RANK1);
        }
        else
            killer->CastSpell(killer, SPELL_RECRUIT, true);
    }
    else if (aur = killer->GetAura(SPELL_CORPORAL))
    {
        if (aur->GetStackAmount() >= 5)
        {
            killer->RemoveAura(SPELL_CORPORAL);
            killer->CastSpell(killer, SPELL_LIEUTENANT, true);
            ChatHandler(killer).PSendSysMessage(LANG_BG_WG_RANK2);
        }
        else
            killer->CastSpell(killer, SPELL_CORPORAL, true);
    }
}

void OutdoorPvPWG::HandleKill(Player *killer, Unit *victim)
{
    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED) || !isWarTime())
        return;

    bool ok = false;

    if (victim->GetTypeId() == TYPEID_PLAYER)
    {
        if (victim->getLevel() >= 70)
            ok = true;

        killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL, victim);
    }
    else
    {
        switch(GetCreatureType(victim->GetEntry()))
        {
            case CREATURE_SIEGE_VEHICLE:
                killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL_V, victim);
                ok = true;
                break;
            case CREATURE_GUARD:
                killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL, victim);
                ok = true;
                break;
            case CREATURE_TURRET:
                ok = true;
                break;
        }
    }

    if (ok)
    {
        if (Group *pGroup = killer->GetGroup())
        {
            for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                if (itr->getSource()->IsAtGroupRewardDistance(killer) && itr->getSource()->getLevel() > 69)
                    PromotePlayer(itr->getSource());
        }
        else if (killer->getLevel() > 69)
            PromotePlayer(killer);
    }
}

// Recalculates Tenacity and applies it to Players / Vehicles
void OutdoorPvPWG::UpdateTenacityStack()
{
    if (!isWarTime())
        return;

    TeamId team = TEAM_NEUTRAL;
    uint32 allianceNum = 0;
    uint32 hordeNum = 0;
    int32 newStack = 0;

    for (PlayerSet::iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
        if ((*itr)->getLevel() > 69)
            ++allianceNum;

    for (PlayerSet::iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
        if ((*itr)->getLevel() > 69)
            ++hordeNum;

    if (allianceNum && hordeNum)
    {
        if (allianceNum < hordeNum)
            newStack = int32((float(hordeNum) / float(allianceNum) - 1)*4); // positive, should cast on alliance
        else if (allianceNum > hordeNum)
            newStack = int32((1 - float(allianceNum) / float(hordeNum))*4); // negative, should cast on horde
    }

    if (newStack == m_tenacityStack)
        return;

    if (m_tenacityStack > 0 && newStack <= 0) // old buff was on alliance
        team = TEAM_ALLIANCE;
    else if (m_tenacityStack < 0 && newStack >= 0) // old buff was on horde
        team = TEAM_HORDE;

    m_tenacityStack = newStack;

    // Remove old buff
    if (team != TEAM_NEUTRAL)
    {
        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if ((*itr)->getLevel() > 69)
                (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);

        for (CreatureSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);
    }

    // Apply new buff
    if (newStack)
    {
        team = newStack > 0 ? TEAM_ALLIANCE : TEAM_HORDE;
        if (newStack < 0)
            newStack = -newStack;
        if (newStack > 20)
            newStack = 20;

        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if ((*itr)->getLevel() > 69)
                (*itr)->SetAuraStack(SPELL_TENACITY, (*itr), newStack);

        for (CreatureSet::const_iterator itr = m_vehicles[team].begin(); itr != m_vehicles[team].end(); ++itr)
            (*itr)->SetAuraStack(SPELL_TENACITY_VEHICLE, (*itr), newStack);
    }
}

void OutdoorPvPWG::UpdateClockDigit(uint32 &timer, uint32 digit, uint32 mod)
{
    uint32 value = timer%mod;

    if (m_clock[digit] != value)
    {
        m_clock[digit] = value;
        SendUpdateWorldState(ClockWorldState[digit], (timer + time(NULL)));
        sWorld.SetWintergrapsTimer(timer + time(NULL), digit);
    }
}

void OutdoorPvPWG::UpdateClock()
{
    uint32 timer = m_timer / 1000;

    if (!isWarTime())
        UpdateClockDigit(timer, 1, 10);
    else
        UpdateClockDigit(timer, 0, 10);
}

bool OutdoorPvPWG::Update(uint32 diff)
{
    if (!sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_ENABLED))
        return false;

    if (m_saveinterval <= diff)
        SaveData();
    else
        m_saveinterval -= diff;

    if (m_timer > diff)
    {
        m_timer -= diff;

        if (isWarTime())
        {
            OutdoorPvP::Update(diff); // update capture points

            /*********************************************************/
            /***        BATTLEGROUND RESSURECTION SYSTEM           ***/
            /*********************************************************/

            //this should be handled by spell system
            m_LastResurrectTime += diff;

            if (m_LastResurrectTime >= RESURRECTION_INTERVAL)
            {
                if (GetReviveQueueSize())
                {
                    for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
                    {
                        Creature *sh = NULL;
                        for (std::vector<uint64>::const_iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
                        {
                            Player *plr = objmgr.GetPlayer(*itr2);
                            if (!plr)
                                continue;

                            if (!sh && plr->IsInWorld())
                            {
                                sh = plr->GetMap()->GetCreature(itr->first);
                                // only for visual effect
                                if (sh)
                                    // Spirit Heal, effect 117
                                    sh->CastSpell(sh, SPELL_SPIRIT_HEAL, true);
                            }

                            // Resurrection visual
                            plr->CastSpell(plr, SPELL_RESURRECTION_VISUAL, true);
                            m_ResurrectQueue.push_back(*itr2);
                        }
                        (itr->second).clear();
                    }
                    m_ReviveQueue.clear();
                    m_LastResurrectTime = 0;
                }
                else
                    // queue is clear and time passed, just update last resurrection time
                    m_LastResurrectTime = 0;
            }
            else if (m_LastResurrectTime > 500)    // Resurrect players only half a second later, to see spirit heal effect on NPC
            {
                for (std::vector<uint64>::const_iterator itr = m_ResurrectQueue.begin(); itr != m_ResurrectQueue.end(); ++itr)
                {
                    Player *plr = objmgr.GetPlayer(*itr);
                    if (!plr)
                        continue;

                    plr->ResurrectPlayer(1.0f);
                    plr->CastSpell(plr, 6962, true);
                    plr->CastSpell(plr, SPELL_SPIRIT_HEAL_MANA, true);

                    ObjectAccessor::Instance().ConvertCorpseForPlayer(*itr);
                }
                m_ResurrectQueue.clear();
            }
        }
        UpdateClock();
    }
    else
    {
        m_sendUpdate = false;
        int32 entry = LANG_BG_WG_DEFENDED;

        if (m_changeDefender)
        {
            m_changeDefender = false;
            m_defender = getAttackerTeamId();
            entry = LANG_BG_WG_CAPTURED;
        }

        if (isWarTime())
        {
            if (m_timer != 1) // 1 = forceStopBattle
                sWorld.SendZoneText(NORTHREND_WINTERGRASP, fmtstring(objmgr.GetTrinityStringForDBCLocale(entry), objmgr.GetTrinityStringForDBCLocale(getDefenderTeamId() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE)));

            EndBattle();
        }
        else
        {
            if (m_timer != 1) // 1 = forceStartBattle
                sWorld.SendZoneText(NORTHREND_WINTERGRASP, objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_BATTLE_STARTS));

            StartBattle();
        }

        UpdateAllWorldObject();
        UpdateClock();

        SendInitWorldStatesTo();
        m_sendUpdate = true;

        SaveData();
    }
    return false;
}

void OutdoorPvPWG::forceStartBattle()
{   // Uptime will do all the work
    m_wartime = false;

    if (m_timer != 1)
    {
        m_timer = 1;
        sWorld.SendZoneText(NORTHREND_WINTERGRASP, objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_BATTLE_FORCE_START));
    }
}

void OutdoorPvPWG::forceStopBattle()
{   // Uptime will do all the work.
    if (!isWarTime())
        m_wartime = true;

    if (m_timer != 1)
    {
        m_timer = 1;
        sWorld.SendZoneText(NORTHREND_WINTERGRASP, objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_BATTLE_FORCE_STOP));
    }
}

void OutdoorPvPWG::forceChangeTeam()
{
    m_changeDefender = true;
    m_timer = 1;
    sWorld.SendZoneText(NORTHREND_WINTERGRASP, fmtstring(objmgr.GetTrinityStringForDBCLocale(LANG_BG_WG_SWITCH_FACTION), objmgr.GetTrinityStringForDBCLocale(getAttackerTeamId() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE)));

    if (isWarTime())
        forceStartBattle();
    else
        forceStopBattle();
}

// Can be forced by gm's while in battle so have to reset in case it was wartime
void OutdoorPvPWG::StartBattle()
{
    m_wartime = true;
    m_timer = sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_BATTLE_TIME) * MINUTE * IN_MILISECONDS;

    // destroyed all vehicles
    for (uint32 team = 0; team < 2; ++team)
    {
        while(!m_vehicles[team].empty())
        {
            Creature *veh = *m_vehicles[team].begin();
            m_vehicles[team].erase(m_vehicles[team].begin());
            veh->ForcedDespawn();
        }
        while(!m_turrets[team].empty())
        {
            Creature *veh = *m_turrets[team].begin();
            m_turrets[team].erase(m_turrets[team].begin());
            veh->ForcedDespawn();
        }
    }

    // Remove All Wintergrasp auras. Add Recruit rank and Tower Control
    for (PlayerSet::iterator itr = m_players[getAttackerTeamId()].begin(); itr != m_players[getAttackerTeamId()].end(); ++itr)
    {
        HandleEssenceOfWintergrasp((*itr), NORTHREND_WINTERGRASP);

        (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
        (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
        (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
        (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);

        if ((*itr)->getLevel() > 69)
        {
            (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), 3);
            (*itr)->CastSpell(*itr, SPELL_RECRUIT, true);
        }
    }

    // Remove All Wintergrasp auras. Add Recruit rank
    for (PlayerSet::iterator itr = m_players[getDefenderTeamId()].begin(); itr != m_players[getDefenderTeamId()].end(); ++itr)
    {
        HandleEssenceOfWintergrasp((*itr), NORTHREND_WINTERGRASP);

        (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
        (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
        (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
        (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);

        if ((*itr)->getLevel() > 69)
            (*itr)->CastSpell(*itr, SPELL_RECRUIT, true);
    }
    UpdateTenacityStack();

    SaveData();
}

void OutdoorPvPWG::EndBattle()
{
    m_wartime = false;

    for (uint32 team = 0; team < 2; ++team)
    {
        // destroyed all vehicles
        while(!m_vehicles[team].empty())
        {
            Creature *veh = *m_vehicles[team].begin();
            m_vehicles[team].erase(m_vehicles[team].begin());
            veh->ForcedDespawn();
        }
        while(!m_turrets[team].empty())
        {
            Creature *veh = *m_turrets[team].begin();
            m_turrets[team].erase(m_turrets[team].begin());
            veh->ForcedDespawn();
        }

        if (m_players[team].empty())
            continue;

        for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
        {
            // When WG ends the zone is cleaned including corpses, revive all players if dead
            if ((*itr)->isDead())
            {
                (*itr)->ResurrectPlayer(1.0f);
                ObjectAccessor::Instance().ConvertCorpseForPlayer((*itr)->GetGUID());
            }

            (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);
            (*itr)->CombatStop(true);
            (*itr)->getHostileRefManager().deleteReferences();

            HandleEssenceOfWintergrasp((*itr), NORTHREND_WINTERGRASP);
        }

        if (m_timer == 1) // Battle End was forced so no reward.
        {
            for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            {
                (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
                (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
                (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
                (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
                (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
            }
            continue;
        }

        // calculate rewards
        uint32 intactNum = 0;
        uint32 damagedNum = 0;

        for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
            if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
                if (workshop->m_buildingState->GetTeamId() == team)
                    if (workshop->m_buildingState->damageState == DAMAGE_DAMAGED)
                        ++damagedNum;
                    else if (workshop->m_buildingState->damageState == DAMAGE_INTACT)
                        ++intactNum;

        uint32 spellRewardId = team == getDefenderTeamId() ? SPELL_VICTORY_REWARD : SPELL_DEFEAT_REWARD;
        uint32 baseHonor = 0;
        uint32 marks = 0;
        uint32 playersWithRankNum = 0;
        uint32 honor = 0;

        if (sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_CUSTOM_HONOR))
        {
            // Calculate Level 70+ with Corporal or Lieutenant rank
            for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
                if ((*itr)->getLevel() > 69 && ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL)))
                    ++playersWithRankNum;

            baseHonor = team == getDefenderTeamId() ? sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_WIN_BATTLE) : sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_LOSE_BATTLE);
            baseHonor += (sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_DAMAGED_TOWER) * m_towerDamagedCount[OTHER_TEAM(team)]);
            baseHonor += (sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_DESTROYED_TOWER) * m_towerDestroyedCount[OTHER_TEAM(team)]);
            baseHonor += (sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_INTACT_BUILDING) * intactNum);
            baseHonor += (sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_DAMAGED_BUILDING) * damagedNum);

            if (playersWithRankNum)
                baseHonor /= playersWithRankNum;
        }

        for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
        {

            if ((*itr)->getLevel() < 70)
                continue; // No rewards for level <70

            // give rewards
            if (sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_CUSTOM_HONOR))
            {
                if (team == getDefenderTeamId())
                {
                    if ((*itr)->HasAura(SPELL_LIEUTENANT))
                    {
                        marks = 3;
                        honor = baseHonor;
                    }
                    else if ((*itr)->HasAura(SPELL_CORPORAL))
                    {
                        marks = 2;
                        honor = baseHonor;
                    }
                    else
                    {
                        marks = 1;
                        honor = 0;
                    }
                }
                else
                {
                    if ((*itr)->HasAura(SPELL_LIEUTENANT))
                    {
                        marks = 1;
                        honor = baseHonor;
                    }
                    else if ((*itr)->HasAura(SPELL_CORPORAL))
                    {
                        marks = 1;
                        honor = baseHonor;
                    }
                    else
                    {
                        marks = 0;
                        honor = 0;
                    }
                }
                (*itr)->RewardHonor(NULL, 1, honor);
                RewardMarkOfHonor(*itr, marks);
                (*itr)->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, spellRewardId);
            }
            else
            {
                if ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL))
                {
                    // TODO - Honor from SpellReward should be shared by team players
                    // TODO - Marks should be given depending on Rank but 3 are given
                    // each time so Won't give any to recruits
                    (*itr)->CastSpell(*itr, spellRewardId, true);

                    for (uint32 i = 0; i < intactNum; ++i)
                        (*itr)->CastSpell(*itr, SPELL_INTACT_BUILDING, true);

                    for (uint32 i = 0; i < damagedNum; ++i)
                        (*itr)->CastSpell(*itr, SPELL_DAMAGED_BUILDING, true);

                    for (uint32 i = 0; i < m_towerDamagedCount[OTHER_TEAM(team)]; ++i)
                        (*itr)->CastSpell(*itr, SPELL_DAMAGED_TOWER, true);

                    for (uint32 i = 0; i < m_towerDestroyedCount[OTHER_TEAM(team)]; ++i)
                        (*itr)->CastSpell(*itr, SPELL_DESTROYED_TOWER, true);
                }
            }

            if (team == getDefenderTeamId())
            {
                if ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL))
                {
                    (*itr)->AreaExploredOrEventHappens(A_VICTORY_IN_WG);
                    (*itr)->AreaExploredOrEventHappens(H_VICTORY_IN_WG);
                }
            }
            (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
            (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
            (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
            (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
            (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
        }
    }

    m_timer = sWorld.getConfig(CONFIG_OUTDOORPVP_WINTERGRASP_INTERVAL) * MINUTE * IN_MILISECONDS;

    // Teleport all attackers (except accs with sec. >= SEC_ANWAERTER) to Dalaran
    for (PlayerSet::iterator itr = m_players[getAttackerTeamId()].begin(); itr != m_players[getAttackerTeamId()].end(); ++itr)
        if ((*itr)->GetSession()->GetSecurity() < SEC_MODERATOR)
            (*itr)->CastSpell(*itr, SPELL_TELEPORT_DALARAN, true);

    SaveData();
}

bool OutdoorPvPWG::CanBuildVehicle(OPvPCapturePointWG *workshop) const
{
    TeamId team = workshop->m_buildingState->GetTeamId();
    if (team == TEAM_NEUTRAL)
        return false;

    return isWarTime() && workshop->m_buildingState->damageState != DAMAGE_DESTROYED && m_vehicles[team].size() < m_workshopCount[team] * MAX_VEHICLE_PER_WORKSHOP;
}

uint32 OutdoorPvPWG::GetData(uint32 id)
{
    // if can build more vehicles
    if (OPvPCapturePointWG *workshop = GetWorkshopByEngGuid(id))
        return CanBuildVehicle(workshop) ? 1 : 0;

    return 0;
}

void OutdoorPvPWG::RewardMarkOfHonor(Player *plr, uint32 count)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->HasAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    if (count == 0)
        return;

    ItemPosCountVec dest;
    uint32 no_space_count = 0;
    uint8 msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, WG_MARK_OF_HONOR, count, &no_space_count);

    if (msg == EQUIP_ERR_ITEM_NOT_FOUND)
    {
        sLog.outErrorDb("Wintergrasp reward item (Entry %u) not exist in `item_template`.", WG_MARK_OF_HONOR);
        return;
    }

    if (msg != EQUIP_ERR_OK) // convert to possible store amount
        count -= no_space_count;

    if (count != 0 && !dest.empty()) // can add some
        if (Item* item = plr->StoreNewItem(dest, WG_MARK_OF_HONOR, true, 0))
            plr->SendNewItem(item, count, true, false);
}

void OutdoorPvPWG::LoadQuestGiverMap(uint32 guid, Position posHorde, Position posAlli)
{
    m_qgPosMap[std::pair<uint32, bool>(guid, true)] = posHorde,
    m_qgPosMap[std::pair<uint32, bool>(guid, false)] = posAlli,
    m_questgivers[guid] = NULL;

    if (getDefenderTeamId() == TEAM_ALLIANCE)
        objmgr.MoveCreData(guid, 571, posAlli);
}

OPvPCapturePointWG *OutdoorPvPWG::GetWorkshop(uint32 lowguid) const
{
    if (OPvPCapturePoint *cp = GetCapturePoint(lowguid))
        return dynamic_cast<OPvPCapturePointWG*>(cp);

    return NULL;
}

OPvPCapturePointWG *OutdoorPvPWG::GetWorkshopByEngGuid(uint32 lowguid) const
{
    for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
            if (workshop->m_engGuid == lowguid)
                return workshop;

    return NULL;
}

OPvPCapturePointWG *OutdoorPvPWG::GetWorkshopByGOGuid(uint32 lowguid) const
{
    for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        if (OPvPCapturePointWG *workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
            if (workshop->m_workshopGuid == lowguid)
                return workshop;

    return NULL;
}

/*########################################################
 * Copy of Battleground system to make Spirit Guides Work
 *#######################################################*/
void OutdoorPvPWG::SendAreaSpiritHealerQueryOpcode(Player *pl, const uint64& guid)
{
    WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
    uint32 time_ = 30000 - GetLastResurrectTime();      // resurrect every 30 seconds

    if (time_ == uint32(-1))
        time_ = 0;

    data << guid << time_;
    pl->GetSession()->SendPacket(&data);
}

void OutdoorPvPWG::AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid)
{
    m_ReviveQueue[npc_guid].push_back(player_guid);

    Player *plr = objmgr.GetPlayer(player_guid);
    if (!plr)
        return;

    plr->CastSpell(plr, SPELL_WAITING_FOR_RESURRECT, true);
}

void OutdoorPvPWG::RemovePlayerFromResurrectQueue(uint64 player_guid)
{
    for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
    {
        for (std::vector<uint64>::iterator itr2 =(itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
        {
            if (*itr2 == player_guid)
            {
                (itr->second).erase(itr2);

                Player *plr = objmgr.GetPlayer(player_guid);
                if (!plr)
                    return;

                plr->RemoveAurasDueToSpell(SPELL_WAITING_FOR_RESURRECT);

                return;
            }
        }
    }
}

void OutdoorPvPWG::RelocateDeadPlayers(Creature *cr)
{
    if (!cr || GetCreatureType(cr->GetEntry()) != CREATURE_SPIRIT_GUIDE)
        return;

    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<uint64> ghost_list = m_ReviveQueue[cr->GetGUID()];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const *ClosestGrave = NULL;
        for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* plr = objmgr.GetPlayer(*itr);
            if (!plr)
                continue;

            if (!ClosestGrave)
                ClosestGrave = objmgr.GetClosestGraveYard(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetMapId(), plr->GetTeam());

            if (ClosestGrave)
                plr->TeleportTo(plr->GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, plr->GetOrientation());
        }
    }
}

/*######
##OPvPCapturePointWG
######*/

OPvPCapturePointWG::OPvPCapturePointWG(OutdoorPvPWG *opvp, BuildingState *state) : OPvPCapturePoint(opvp),
m_buildingState(state), m_wintergrasp(opvp), m_engineer(NULL), m_engGuid(0), m_spiritguide(NULL), m_spiGuid(0) {}

void OPvPCapturePointWG::SetTeamByBuildingState()
{
    if (m_buildingState->GetTeamId() == TEAM_ALLIANCE)
    {
        m_value = m_maxValue;
        m_State = OBJECTIVESTATE_ALLIANCE;
    }
    else if (m_buildingState->GetTeamId() == TEAM_HORDE)
    {
        m_value = -m_maxValue;
        m_State = OBJECTIVESTATE_HORDE;
    }
    else
    {
        m_value = 0;
        m_State = OBJECTIVESTATE_NEUTRAL;
    }

    if (m_team != m_buildingState->GetTeamId())
    {
        TeamId oldTeam = m_team;
        m_team = m_buildingState->GetTeamId();
        ChangeTeam(oldTeam);
    }

    SendChangePhase();
}

void OPvPCapturePointWG::ChangeTeam(TeamId oldTeam)
{
    uint32 entry = 0;
    uint32 guide_entry = 0;

    if (oldTeam != TEAM_NEUTRAL)
        m_wintergrasp->ModifyWorkshopCount(oldTeam, false);

    if (m_team != TEAM_NEUTRAL)
    {
        entry = m_team == TEAM_ALLIANCE ? CRE_ENG_A : CRE_ENG_H;
        guide_entry = m_team == TEAM_ALLIANCE ? CRE_SPI_A : CRE_SPI_H;
        m_wintergrasp->ModifyWorkshopCount(m_team, true);
    }

    if (m_capturePoint)
        GameObject::SetGoArtKit(CapturePointArtKit[m_team], m_capturePoint, m_capturePointGUID);

    m_buildingState->SetTeam(m_team);
    // TODO: this may be sent twice
    m_wintergrasp->BroadcastStateChange(m_buildingState);

    if (m_buildingState->building)
        m_buildingState->building->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[m_team]);

    if (entry)
    {
        if (m_engGuid)
        {
            *m_engEntry = entry;
            m_wintergrasp->ResetCreatureEntry(m_engineer, entry);
        }
        if (m_spiGuid)
        {
            *m_spiEntry = guide_entry;
            m_wintergrasp->ResetCreatureEntry(m_spiritguide, guide_entry);
            m_wintergrasp->RelocateDeadPlayers(m_spiritguide);
        }
    }
    else if (m_engineer)
        m_engineer->SetVisibility(VISIBILITY_OFF);

    sLog.outDebug("Wintergrasp workshop now belongs to %u.", (uint32)m_buildingState->GetTeamId());
}
