/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ScriptData
SDName: instance_trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Trial of the Crusader
EndScriptData */

#include "ScriptedPch.h"
#include "trial_of_the_crusader.h"

struct instance_trial_of_the_crusader : public ScriptedInstance
{
    instance_trial_of_the_crusader(Map* pMap) : ScriptedInstance(pMap) { 
    Difficulty = ((InstanceMap*)pMap)->GetDifficulty();
    Initialize(); 
    }

    uint32 m_auiEncounter[MAX_ENCOUNTERS+1];
    uint32 m_auiEventTimer;
    uint32 m_auiEventNPCId;
    uint32 m_auiNorthrendBeasts;
    uint8 Difficulty;
    std::string m_strInstData;
    bool needsave;

    uint32 m_uiDataDamageFjola;
    uint32 m_uiDataDamageEydis;
    uint32 m_uiFjolaCasting;
    uint32 m_uiEydisCasting;

    uint32 m_auiCrusadersCount;

    uint64 m_uiBarrentGUID;
    uint64 m_uiTirionGUID;
    uint64 m_uiFizzlebangGUID;
    uint64 m_uiGarroshGUID;
    uint64 m_uiRinnGUID;
    uint64 m_uiLich0GUID;
    uint64 m_uiLich1GUID;

    uint64 m_uiGormokGUID;
    uint64 m_uiAcidmawGUID;
    uint64 m_uiDreadscaleGUID;
    uint64 m_uiIcehowlGUID;
    uint64 m_uiJaraxxusGUID;
    uint64 m_uiDarkbaneGUID;
    uint64 m_uiLightbaneGUID;
    uint64 m_uiAnubarakGUID;

    uint64 m_uiCrusader11Guid;
    uint64 m_uiCrusader12Guid;
    uint64 m_uiCrusader13Guid;
    uint64 m_uiCrusader14Guid;
    uint64 m_uiCrusader15Guid;
    uint64 m_uiCrusader16Guid;
    uint64 m_uiCrusader17Guid;
    uint64 m_uiCrusader18Guid;
    uint64 m_uiCrusader19Guid;
    uint64 m_uiCrusader1aGuid;

    uint64 m_uiCrusader21Guid;
    uint64 m_uiCrusader22Guid;
    uint64 m_uiCrusader23Guid;
    uint64 m_uiCrusader24Guid;
    uint64 m_uiCrusader25Guid;
    uint64 m_uiCrusader26Guid;
    uint64 m_uiCrusader27Guid;
    uint64 m_uiCrusader28Guid;
    uint64 m_uiCrusader29Guid;
    uint64 m_uiCrusader2aGuid;

    uint64 m_uiCrusader01Guid;
    uint64 m_uiCrusader02Guid;

    uint64 m_uiCrusadersCacheGUID;
    uint64 m_uiFloorGUID;

    uint64 m_uiTC10h25GUID;
    uint64 m_uiTC10h45GUID;
    uint64 m_uiTC10h50GUID;
    uint64 m_uiTC10h99GUID;

    uint64 m_uiTC25h25GUID;
    uint64 m_uiTC25h45GUID;
    uint64 m_uiTC25h50GUID;
    uint64 m_uiTC25h99GUID;

    uint64 m_uiTributeChest1GUID;
    uint64 m_uiTributeChest2GUID;
    uint64 m_uiTributeChest3GUID;
    uint64 m_uiTributeChest4GUID;

    uint64 m_uiMainGateDoorGUID;

    uint64 m_uiWestPortcullisGUID;
    uint64 m_uiNorthPortcullisGUID;
    uint64 m_uiSouthPortcullisGUID;

    void Initialize()
    {
        for (uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
            m_auiEncounter[i] = NOT_STARTED;

        m_auiEncounter[0] = 0;
        m_auiEncounter[7] = 50;
        m_auiEncounter[8] = 0;

        m_uiTributeChest1GUID = 0;
        m_uiTributeChest2GUID = 0;
        m_uiTributeChest3GUID = 0;
        m_uiTributeChest4GUID = 0;
        m_uiDataDamageFjola = 0;
        m_uiDataDamageEydis = 0;
        m_uiLich0GUID = 0;
        m_uiLich1GUID = 0;

        m_auiNorthrendBeasts = NOT_STARTED;

        m_auiEventTimer = 1000;

        switch (Difficulty)
        {
        case RAID_DIFFICULTY_10MAN_NORMAL :
        case RAID_DIFFICULTY_10MAN_HEROIC :
            m_auiCrusadersCount = 6;
            break;
        case RAID_DIFFICULTY_25MAN_NORMAL :
        case RAID_DIFFICULTY_25MAN_HEROIC :
            m_auiCrusadersCount = 10;
            break;
        }

        needsave = false;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 1; i < MAX_ENCOUNTERS-2 ; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnPlayerEnter(Player *m_player)
    {
        if (Difficulty == RAID_DIFFICULTY_10MAN_HEROIC || Difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW,1);
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_COUNT, GetData(TYPE_COUNTER));
        }
    }

    bool IsRaidWiped()
    {
       Map::PlayerList const &players = instance->GetPlayers();

       for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
              {
              if(Player* pPlayer = i->getSource())
                    {
                    if(pPlayer->isAlive())
                    return false;
                    }
               }
        return true;
     }

    void OpenDoor(uint64 guid)
    {
        if(!guid) return;
        GameObject* pGo = instance->GetGameObject(guid);
        if(pGo) pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
    }

    void CloseDoor(uint64 guid)
    {
        if(!guid) return;
        GameObject* pGo = instance->GetGameObject(guid);
        if(pGo) pGo->SetGoState(GO_STATE_READY);
    }

     void OnCreatureCreate(Creature* pCreature, bool /*add*/)
     {
        switch(pCreature->GetEntry())
        {
         case NPC_BARRENT:  m_uiBarrentGUID = pCreature->GetGUID(); break;
         case NPC_TIRION:      m_uiTirionGUID = pCreature->GetGUID(); break;
         case NPC_FIZZLEBANG:  m_uiFizzlebangGUID = pCreature->GetGUID(); break;
         case NPC_GARROSH:     m_uiGarroshGUID = pCreature->GetGUID(); break;
         case NPC_RINN:        m_uiRinnGUID = pCreature->GetGUID(); break;
         case NPC_LICH_KING_0: m_uiLich0GUID = pCreature->GetGUID(); break;
         case NPC_LICH_KING_1: m_uiLich1GUID = pCreature->GetGUID(); break;

         case NPC_GORMOK: m_uiGormokGUID = pCreature->GetGUID(); break;
         case NPC_ACIDMAW: m_uiAcidmawGUID = pCreature->GetGUID(); break;
         case NPC_DREADSCALE: m_uiDreadscaleGUID = pCreature->GetGUID(); break;
         case NPC_ICEHOWL: m_uiIcehowlGUID = pCreature->GetGUID(); break;
         case NPC_JARAXXUS: m_uiJaraxxusGUID = pCreature->GetGUID(); break;
         case NPC_DARKBANE: m_uiDarkbaneGUID = pCreature->GetGUID(); break;
         case NPC_LIGHTBANE: m_uiLightbaneGUID = pCreature->GetGUID(); break;
         case NPC_ANUBARAK: m_uiAnubarakGUID = pCreature->GetGUID(); break;

         case NPC_CRUSADER_1_1: m_uiCrusader11Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_2: m_uiCrusader12Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_3: m_uiCrusader13Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_4: m_uiCrusader14Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_5: m_uiCrusader15Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_6: m_uiCrusader16Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_7: m_uiCrusader17Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_8: m_uiCrusader18Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_9: m_uiCrusader19Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_1_10: m_uiCrusader1aGuid = pCreature->GetGUID(); break;

         case NPC_CRUSADER_2_1: m_uiCrusader21Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_2: m_uiCrusader22Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_3: m_uiCrusader23Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_4: m_uiCrusader24Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_5: m_uiCrusader25Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_6: m_uiCrusader26Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_7: m_uiCrusader27Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_8: m_uiCrusader28Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_9: m_uiCrusader29Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_2_10: m_uiCrusader2aGuid = pCreature->GetGUID(); break;

         case NPC_CRUSADER_0_1: m_uiCrusader01Guid = pCreature->GetGUID(); break;
         case NPC_CRUSADER_0_2: m_uiCrusader02Guid = pCreature->GetGUID(); break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*bAdd*/)
    {
        switch(pGo->GetEntry())
        {
        case GO_CRUSADERS_CACHE_10:
                                  if(Difficulty == RAID_DIFFICULTY_10MAN_NORMAL)
                                  m_uiCrusadersCacheGUID = pGo->GetGUID(); 
                                  break;
        case GO_CRUSADERS_CACHE_25:
                                  if(Difficulty == RAID_DIFFICULTY_25MAN_NORMAL)
                                  m_uiCrusadersCacheGUID = pGo->GetGUID(); 
                                  break;
        case GO_CRUSADERS_CACHE_10_H:
                                  if(Difficulty == RAID_DIFFICULTY_10MAN_HEROIC)
                                  m_uiCrusadersCacheGUID = pGo->GetGUID(); 
                                  break;
        case GO_CRUSADERS_CACHE_25_H:
                                  if(Difficulty == RAID_DIFFICULTY_25MAN_HEROIC)
                                  m_uiCrusadersCacheGUID = pGo->GetGUID(); 
                                  break;
        case GO_ARGENT_COLISEUM_FLOOR: 
                                  m_uiFloorGUID = pGo->GetGUID(); 
                                  break;
        case GO_MAIN_GATE_DOOR:   m_uiMainGateDoorGUID = pGo->GetGUID(); break;

        case GO_SOUTH_PORTCULLIS: m_uiSouthPortcullisGUID = pGo->GetGUID(); break;
        case GO_WEST_PORTCULLIS:  m_uiWestPortcullisGUID = pGo->GetGUID(); break;
        case GO_NORTH_PORTCULLIS: m_uiNorthPortcullisGUID = pGo->GetGUID(); break;


        case GO_TRIBUTE_CHEST_10H_25: m_uiTC10h25GUID = pGo->GetGUID(); break;
        case GO_TRIBUTE_CHEST_10H_45: m_uiTC10h45GUID = pGo->GetGUID(); break;
        case GO_TRIBUTE_CHEST_10H_50: m_uiTC10h50GUID = pGo->GetGUID(); break;
        case GO_TRIBUTE_CHEST_10H_99: m_uiTC10h99GUID = pGo->GetGUID(); break;

        case GO_TRIBUTE_CHEST_25H_25: m_uiTC25h25GUID = pGo->GetGUID(); break;
        case GO_TRIBUTE_CHEST_25H_45: m_uiTC25h45GUID = pGo->GetGUID(); break;
        case GO_TRIBUTE_CHEST_25H_50: m_uiTC25h50GUID = pGo->GetGUID(); break;
        case GO_TRIBUTE_CHEST_25H_99: m_uiTC25h99GUID = pGo->GetGUID(); break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
        case TYPE_STAGE:     m_auiEncounter[0] = uiData; break;
        case TYPE_BEASTS:    m_auiEncounter[1] = uiData; break;
        case TYPE_JARAXXUS:  m_auiEncounter[2] = uiData; break;
        case TYPE_CRUSADERS: if (uiData == FAIL && (m_auiEncounter[3] == FAIL || m_auiEncounter[3] == NOT_STARTED))
                             m_auiEncounter[3] = NOT_STARTED;
                             else  m_auiEncounter[3] = uiData;
                             if (uiData == DONE) {
                                   if (GameObject* pChest = instance->GetGameObject(m_uiCrusadersCacheGUID))
                                       if (pChest && !pChest->isSpawned())
                                             pChest->SetRespawnTime(7*DAY);
                                   };
                             break;
        case TYPE_CRUSADERS_COUNT:  if (uiData == 0) --m_auiCrusadersCount;
                                         else m_auiCrusadersCount = uiData;
                                    break;
        case TYPE_VALKIRIES: if (m_auiEncounter[4] == SPECIAL && uiData == SPECIAL) uiData = DONE;
                             m_auiEncounter[4] = uiData; break;
        case TYPE_LICH_KING: m_auiEncounter[5] = uiData; break;
        case TYPE_ANUBARAK:  m_auiEncounter[6] = uiData; 
                            if (uiData == DONE) {
                            if(Difficulty == RAID_DIFFICULTY_10MAN_HEROIC){
                                if ( m_auiEncounter[7] >= 25) m_uiTributeChest1GUID = m_uiTC10h25GUID;
                                if ( m_auiEncounter[7] >= 45) m_uiTributeChest2GUID = m_uiTC10h45GUID;
                                if ( m_auiEncounter[7] >= 49) m_uiTributeChest3GUID = m_uiTC10h50GUID;
                                m_uiTributeChest4GUID = m_uiTC10h99GUID;
                            }
                            if(Difficulty == RAID_DIFFICULTY_25MAN_HEROIC){
                                if ( m_auiEncounter[7] >= 25) m_uiTributeChest1GUID = m_uiTC25h25GUID;
                                if ( m_auiEncounter[7] >= 45) m_uiTributeChest2GUID = m_uiTC25h45GUID;
                                if ( m_auiEncounter[7] >= 49) m_uiTributeChest3GUID = m_uiTC25h50GUID;
                                m_uiTributeChest4GUID = m_uiTC25h99GUID;
                            }
                            // Attention! It is (may be) not off-like, but  spawning all Tribute Chests is real
                            // reward for clearing TOC instance
                            if (m_uiTributeChest1GUID)
                              if (GameObject* pChest1 = instance->GetGameObject(m_uiTributeChest1GUID))
                                if (pChest1 && !pChest1->isSpawned()) pChest1->SetRespawnTime(7*DAY);
                            if (m_uiTributeChest2GUID)
                              if (GameObject* pChest2 = instance->GetGameObject(m_uiTributeChest2GUID))
                                if (pChest2 && !pChest2->isSpawned()) pChest2->SetRespawnTime(7*DAY);
                            if (m_uiTributeChest3GUID)
                              if (GameObject* pChest3 = instance->GetGameObject(m_uiTributeChest3GUID))
                                if (pChest3 && !pChest3->isSpawned()) pChest3->SetRespawnTime(7*DAY);
                            if (m_uiTributeChest4GUID)
                              if (GameObject* pChest4 = instance->GetGameObject(m_uiTributeChest4GUID))
                                if (pChest4 && !pChest4->isSpawned()) pChest4->SetRespawnTime(7*DAY);
                            };
        break;
        case TYPE_COUNTER:   m_auiEncounter[7] = uiData; uiData = DONE; break;
        case TYPE_EVENT:     m_auiEncounter[8] = uiData; uiData = NOT_STARTED; break;
        case TYPE_EVENT_TIMER:      m_auiEventTimer = uiData; uiData = NOT_STARTED; break;
        case TYPE_NORTHREND_BEASTS:
            m_auiNorthrendBeasts = uiData;
            if (uiData == ICEHOWL_DONE)
            {
                m_auiEncounter[1] = DONE;
                needsave = true;
            }
            break;
        case DATA_HEALTH_FJOLA:     m_uiDataDamageFjola = uiData; uiData = NOT_STARTED; break;
        case DATA_HEALTH_EYDIS:     m_uiDataDamageEydis = uiData; uiData = NOT_STARTED; break;
        case DATA_CASTING_FJOLA:    m_uiFjolaCasting = uiData; uiData = NOT_STARTED; break;
        case DATA_CASTING_EYDIS:    m_uiEydisCasting = uiData; uiData = NOT_STARTED; break;
        }
        if (IsEncounterInProgress())
        {
            CloseDoor(GetData64(GO_WEST_PORTCULLIS));
            CloseDoor(GetData64(GO_NORTH_PORTCULLIS));
            //CloseDoor(GetData64(GO_SOUTH_PORTCULLIS));
        } else {
            OpenDoor(GetData64(GO_WEST_PORTCULLIS));
            OpenDoor(GetData64(GO_NORTH_PORTCULLIS));
            //OpenDoor(GetData64(GO_SOUTH_PORTCULLIS));
        }

        if (uiData == FAIL && uiType != TYPE_STAGE
                           && uiType != TYPE_EVENT
                           && uiType != TYPE_COUNTER
                           && uiType != TYPE_EVENT_TIMER)
        { if (IsRaidWiped()) { --m_auiEncounter[7];
                               needsave = true;
                             }
                               uiData = NOT_STARTED;
        }

        if ((uiData == DONE && uiType != TYPE_STAGE
                           && uiType != TYPE_EVENT
                           && uiType != TYPE_EVENT_TIMER)
                           || needsave == true)
        {
            needsave = false;
            SaveToDB();
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case NPC_BARRENT:  return m_uiBarrentGUID;
            case NPC_TIRION:   return m_uiTirionGUID;
            case NPC_FIZZLEBANG: return m_uiFizzlebangGUID;
            case NPC_GARROSH:  return m_uiGarroshGUID;
            case NPC_RINN:     return m_uiRinnGUID;
            case NPC_LICH_KING_0: return m_uiLich0GUID;
            case NPC_LICH_KING_1: return m_uiLich1GUID;

            case NPC_GORMOK: return m_uiGormokGUID;
            case NPC_ACIDMAW: return m_uiAcidmawGUID;
            case NPC_DREADSCALE: return m_uiDreadscaleGUID;
            case NPC_ICEHOWL: return m_uiIcehowlGUID;
            case NPC_JARAXXUS: return  m_uiJaraxxusGUID;
            case NPC_DARKBANE: return m_uiDarkbaneGUID;
            case NPC_LIGHTBANE: return m_uiLightbaneGUID;
            case NPC_ANUBARAK: return m_uiAnubarakGUID;

            case NPC_CRUSADER_1_1: return m_uiCrusader11Guid;
            case NPC_CRUSADER_1_2: return m_uiCrusader12Guid;
            case NPC_CRUSADER_1_3: return m_uiCrusader13Guid;
            case NPC_CRUSADER_1_4: return m_uiCrusader14Guid;
            case NPC_CRUSADER_1_5: return m_uiCrusader15Guid;
            case NPC_CRUSADER_1_6: return m_uiCrusader16Guid;
            case NPC_CRUSADER_1_7: return m_uiCrusader17Guid;
            case NPC_CRUSADER_1_8: return m_uiCrusader18Guid;
            case NPC_CRUSADER_1_9: return m_uiCrusader19Guid;
            case NPC_CRUSADER_1_10: return m_uiCrusader1aGuid;

            case NPC_CRUSADER_2_1: return m_uiCrusader21Guid;
            case NPC_CRUSADER_2_2: return m_uiCrusader22Guid;
            case NPC_CRUSADER_2_3: return m_uiCrusader23Guid;
            case NPC_CRUSADER_2_4: return m_uiCrusader24Guid;
            case NPC_CRUSADER_2_5: return m_uiCrusader25Guid;
            case NPC_CRUSADER_2_6: return m_uiCrusader26Guid;
            case NPC_CRUSADER_2_7: return m_uiCrusader27Guid;
            case NPC_CRUSADER_2_8: return m_uiCrusader28Guid;
            case NPC_CRUSADER_2_9: return m_uiCrusader29Guid;
            case NPC_CRUSADER_2_10: return m_uiCrusader2aGuid;

            case NPC_CRUSADER_0_1: return m_uiCrusader01Guid;
            case NPC_CRUSADER_0_2: return m_uiCrusader02Guid;
            case GO_ARGENT_COLISEUM_FLOOR: return m_uiFloorGUID;
            case GO_MAIN_GATE_DOOR:        return m_uiMainGateDoorGUID;

            case GO_SOUTH_PORTCULLIS:       return m_uiSouthPortcullisGUID;
            case GO_WEST_PORTCULLIS:        return m_uiWestPortcullisGUID;
            case GO_NORTH_PORTCULLIS:       return m_uiNorthPortcullisGUID;

        }
        return 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_STAGE:     return m_auiEncounter[0];
            case TYPE_BEASTS:    return m_auiEncounter[1];
            case TYPE_JARAXXUS:  return m_auiEncounter[2];
            case TYPE_CRUSADERS: return m_auiEncounter[3];
            case TYPE_VALKIRIES: return m_auiEncounter[4];
            case TYPE_LICH_KING: return m_auiEncounter[5];
            case TYPE_ANUBARAK:  return m_auiEncounter[6];
            case TYPE_COUNTER:   return m_auiEncounter[7];
            case TYPE_EVENT:     return m_auiEncounter[8];
            case TYPE_DIFFICULTY:   return Difficulty;
            case TYPE_NORTHREND_BEASTS:    return m_auiNorthrendBeasts;
            case TYPE_EVENT_TIMER:  return m_auiEventTimer;
            case TYPE_CRUSADERS_COUNT:  return m_auiCrusadersCount;
            case TYPE_EVENT_NPC: switch (m_auiEncounter[8]) 
                                 {
                                 case 110:
                                 case 140:
                                 case 150:
                                 case 200:
                                 case 205:
                                 case 210:
                                 case 300:
                                 case 305:
                                 case 310:
                                 case 400:
                                 case 666:
                                 case 1010:
                                 case 1180:
                                 case 2000:
                                 case 2030:
                                 case 3000:
                                 case 3001:
                                 case 3060:
                                 case 3061:
                                 case 3090:
                                 case 3091:
                                 case 3100:
                                 case 3110:
                                 case 4000:
                                 case 4010:
                                 case 4015:
                                 case 4040:
                                 case 4050:
                                 case 5000:
                                 case 5005:
                                 case 5020:
                                 case 6000:
                                 case 6005:
                                 case 6010:
                                 m_auiEventNPCId = NPC_TIRION;
                                 break;

                                 case 5010:
                                 case 5030:
                                 case 5040:
                                 case 5050:
                                 case 5060:
                                 case 5070:
                                 case 5080:
                                 m_auiEventNPCId = NPC_LICH_KING_1;
                                 break;

                                 case 130:
                                 case 132:
                                 case 2020:
                                 case 3080:
                                 case 3051:
                                 case 3071:
                                 case 4020:
                                 m_auiEventNPCId = NPC_RINN;
                                 break;

                                 case 120:
                                 case 122:
                                 case 2010:
                                 case 3050:
                                 case 3070:
                                 case 3081:
                                 case 4030:
                                 m_auiEventNPCId = NPC_GARROSH;
                                 break;

                                 case 1110:
                                 case 1120:
                                 case 1130:
                                 case 1135:
                                 case 1140:
                                 case 1150:
                                 case 1160:
                                 case 1170:
                                 m_auiEventNPCId = NPC_FIZZLEBANG;
                                 break;

                                 default:
                                 m_auiEventNPCId = NPC_TIRION;
                                 break;

                                 };
                                 return m_auiEventNPCId;

        case DATA_HEALTH_FJOLA: return m_uiDataDamageFjola;
        case DATA_HEALTH_EYDIS: return m_uiDataDamageEydis;
        case DATA_CASTING_FJOLA: return m_uiFjolaCasting;
        case DATA_CASTING_EYDIS: return m_uiEydisCasting;
        }
        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        for(uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
            saveStream << m_auiEncounter[i] << " ";

        m_strInstData = saveStream.str();

        OUT_SAVE_INST_DATA_COMPLETE;
        return m_strInstData;
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);

        for(uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
        {
            loadStream >> m_auiEncounter[i];

            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }
        m_auiEncounter[TYPE_EVENT] = 0;
        m_auiEncounter[TYPE_STAGE] = 0;

        OUT_LOAD_INST_DATA_COMPLETE;

    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crusader(Map* pMap)
{
    return new instance_trial_of_the_crusader(pMap);
}

void AddSC_instance_trial_of_the_crusader()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crusader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crusader;
    newscript->RegisterSelf();
}
