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

struct instance_trial_of_the_crussader : public ScriptedInstance
{
    instance_trial_of_the_crussader(Map* pMap) : ScriptedInstance(pMap), m_uiFloorGO(NULL), CacheFactionChampions(NULL)
        {
                Initialize();
        }

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;
    Difficulty m_difficulty;


    uint64 m_uiGormokImpalerGUID;
        uint64 m_uiAcidmawGUID;
        uint64 m_uiIcehowlGUID;
        uint64 m_uiDreadscaleGUID;
        uint64 m_uiLordJaraxxusGUID;
        uint64 m_uiFjolaGUID;
        uint64 m_uiEydisGUID;
        uint64 m_uiAnubarakGUID;
        uint64 m_uiAnonserGUID;
        uint64 m_uiFloorGUID;
        uint64 m_uiTirionGUID;
        uint64 m_uiVarianWrynnGUID;
        uint64 m_uiFizzlebangGUID;
        uint64 m_uiGarroshGUID;

        GameObject* m_uiFloorGO;
        GameObject* m_uiGateGO;
        GameObject* CacheFactionChampions;

    void Initialize()
    {
                m_uiGormokImpalerGUID                   = 0;
                m_uiAcidmawGUID                                 = 0;
                m_uiIcehowlGUID                                 = 0;
                m_uiDreadscaleGUID                              = 0;
                m_uiLordJaraxxusGUID                    = 0;
                m_uiFjolaGUID                                   = 0;
                m_uiEydisGUID                                   = 0;
                m_uiAnubarakGUID                                = 0;
                m_uiAnonserGUID                                 = 0;
                m_uiFloorGUID                                   = 0;
                m_uiTirionGUID                                  = 0;
                m_uiVarianWrynnGUID                             = 0;
                m_uiFizzlebangGUID                              = 0;
                m_uiGarroshGUID                                 = 0;

        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

                SetData(DATA_UPDATE_STATE_UI_COUNT, 50);
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool bAdd)
    {
        Map::PlayerList const &players = instance->GetPlayers();
        uint32 TeamInInstance = 0;
        if (!players.isEmpty())
        {
            if (Player* pPlayer = players.begin()->getSource())
                TeamInInstance = pPlayer->GetTeam();
        }

        switch(pCreature->GetEntry())
        {
            // Northrend Beasts
            case NPC_GORMOK_IMPALER:
                                m_uiGormokImpalerGUID = pCreature->GetGUID();
            break;

            case NPC_ACIDMAW:
                                m_uiAcidmawGUID = pCreature->GetGUID();
            break;

            case NPC_ICEHOWL:
                                m_uiIcehowlGUID = pCreature->GetGUID();
            break;

            case NPC_DREADSCALE:
                                m_uiDreadscaleGUID = pCreature->GetGUID();
            break;
                        // ----
            case NPC_LORD_JARAXXUS:
                                m_uiLordJaraxxusGUID = pCreature->GetGUID();
            break;

                        // Faction Champions
                        // ---- Champion of the Alliance
                        // -----
                        // ---

            case NPC_FJOLA:
                                m_uiFjolaGUID = pCreature->GetGUID();
            break;

            case NPC_EYDIS:
                                m_uiEydisGUID = pCreature->GetGUID();
            break;

            case NPC_ANUBARAK:
                                m_uiAnubarakGUID = pCreature->GetGUID();
            break;

                        case NPC_ANONSER:
                                m_uiAnonserGUID = pCreature->GetGUID();
                        break;

                        case NPC_TIRION:
                                m_uiTirionGUID = pCreature->GetGUID();
                        break;

                        case NPC_VARIAN_WRYNN:
                                m_uiVarianWrynnGUID = pCreature->GetGUID();
                        break;

                        case NPC_FIZZLEBANG:
                                m_uiFizzlebangGUID = pCreature->GetGUID();
                        break;

                        case NPC_GARROSH:
                                m_uiGarroshGUID = pCreature->GetGUID();
                        break;
        }
    }

        bool OnRaidWipe()
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

    void OnGameObjectCreate(GameObject* pGo, bool add)
    {
                switch(pGo->GetEntry())
        {
                        case GO_FLOOR:
                                m_uiFloorGO = add ? pGo : NULL;
                                m_uiFloorGUID = pGo->GetGUIDLow();
                        break;

                        case CHAMPIONS_CACHE_NORMAL_10: CacheFactionChampions = add ? pGo : NULL; break;
                        case CHAMPIONS_CACHE_NORMAL_25: CacheFactionChampions = add ? pGo : NULL; break;
                        case CHAMPIONS_CACHE_HEROIC_10: CacheFactionChampions = add ? pGo : NULL; break;
                        case CHAMPIONS_CACHE_HEROIC_25: CacheFactionChampions = add ? pGo : NULL; break;

                        case GO_GATE:  m_uiGateGO  = add ? pGo : NULL; break;
                }
    }

    void SetData(uint32 Type, uint32 Data)
    {
        switch(Type)
        {
                        case PHASE_1:
                                if(Data == IN_PROGRESS)
                                {
                                        //m_uiGateGO->SetLootState(GO_READY);
                                        //m_uiGateGO->UseDoorOrButton(10000);
                                }
                                m_auiEncounter[0] = Data;
                        break;

                        case PHASE_2:
                                if(Data == IN_PROGRESS)
                                {
                                        //m_uiGateGO->SetLootState(GO_READY);
                                        //m_uiGateGO->UseDoorOrButton(10000);
                                }

                                m_auiEncounter[1] = Data;
                        break;

                        case PHASE_3:
                                if(Data == IN_PROGRESS)
                                {
                                        //m_uiGateGO->SetLootState(GO_READY);
                                        //m_uiGateGO->UseDoorOrButton(10000);
                                }

                                m_auiEncounter[2] = Data;
                        break;

                        case PHASE_4:
                                if(Data == IN_PROGRESS)
                                        SetData(PHASE_SPECHIAL, 15);

                                m_auiEncounter[3] = Data;
                        break;

                        case PHASE_5:
                                if(Data == IN_PROGRESS)
                                {
                                        SetData(PHASE_SPECHIAL, 50);
                                        //m_uiGateGO->SetLootState(GO_READY);
                                        //m_uiGateGO->UseDoorOrButton(10000);
                                }

                                if(Data == DONE)
                                {
                                        //m_uiGateGO->SetLootState(GO_READY);
                                        //m_uiGateGO->UseDoorOrButton(10000);
                                        CacheFactionChampions->SetRespawnTime(CacheFactionChampions->GetRespawnDelay());
                                }
                                m_auiEncounter[4] = Data;
                        break;

                        case PHASE_6:
                                if (Data == IN_PROGRESS)
                                {
                                        SetData(PHASE_SPECHIAL, 57);
                                        //m_uiGateGO->TakenDamage(10000);
                                }

                                m_auiEncounter[5] = Data;
                        break;

                        case PHASE_7:
                                if (Data == IN_PROGRESS)
                                {
                                        SetData(PHASE_SPECHIAL, 37);
                                }
                                m_auiEncounter[6] = Data;
                        break;

                        case DATA_UPDATE_STATE_UI_COUNT:
                                DoUpdateWorldState(UPDATE_STATE_UI_COUNT, Data);
                                m_auiEncounter[7] = Data;
                        break;

                        case PHASE_SPECHIAL:
                                if(Data == 4 /*|| /*Data == 7*/ || Data == 9 || Data == 10|| Data == 11 || Data == 12 || Data == 15 || Data == 16 || Data == 57)
                                {
                                        m_uiGateGO->SetLootState(GO_READY);
                                        m_uiGateGO->UseDoorOrButton(5000);
                                }
                                if(Data == 45)
                                {
                                        m_uiFloorGO->TakenDamage(10000);
                                }
                                m_auiEncounter[8] = Data;
                        break;
                        case SPECHIAL_TIMER:
                                m_auiEncounter[9] = Data;
                        break;

        }

        if (Data == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    void OnPlayerEnter(Player *m_player)
    {
                if (m_player->GetDifficulty(instance->IsRaid()) == RAID_DIFFICULTY_10MAN_HEROIC || m_player->GetDifficulty(instance->IsRaid()) == RAID_DIFFICULTY_25MAN_HEROIC)
        {
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW,1);
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_COUNT, GetData(DATA_UPDATE_STATE_UI_COUNT));
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
                        case NPC_GORMOK_IMPALER:                return m_uiGormokImpalerGUID;   break;
                        case NPC_TIRION:                                return m_uiTirionGUID;                  break;
                        case NPC_VARIAN_WRYNN:                  return m_uiVarianWrynnGUID;     break;
                        case NPC_FIZZLEBANG:                    return m_uiFizzlebangGUID;              break;
                        case NPC_GARROSH:                               return m_uiGarroshGUID;                 break;
                        case NPC_ANONSER:                               return m_uiAnonserGUID;                 break;
                        case GO_GATE:                                   return m_uiFloorGUID;                   break;
                        case DATA_FJOLA:                                return m_uiFjolaGUID;                   break;
                        case DATA_EYDIS:                                return m_uiEydisGUID;                   break;
        }

        return 0;
    }

    std::string GetSaveData()
    {
        return strInstData;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case PHASE_1:
                                return m_auiEncounter[0];
                        break;
                        case PHASE_2:
                                return m_auiEncounter[1];
                        break;
                        case PHASE_3:
                                return m_auiEncounter[2];
                        break;
                        case PHASE_4:
                                return m_auiEncounter[3];
                        break;
                        case PHASE_5:
                                return m_auiEncounter[4];
                        break;
                        case PHASE_6:
                                return m_auiEncounter[5];
                        break;
                        case PHASE_7:
                                return m_auiEncounter[6];
                        break;
                        case DATA_UPDATE_STATE_UI_COUNT:
                                return m_auiEncounter[7];
                        break;

                        case PHASE_SPECHIAL:
                                return m_auiEncounter[8];
                        break;
                        case SPECHIAL_TIMER:
                                return m_auiEncounter[9];
                        break;
        }

        return 0;
    }

    void Load(const char *chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7];

        for (uint8 i = 0; i < 9; ++i)
                {
            if (m_auiEncounter[i] == IN_PROGRESS)
                        {
                                if(i==7 || i==8 || i==9)
                                        break;
                                else
                                        m_auiEncounter[i] = NOT_STARTED;
                        }
                }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crussader(Map* pMap)
{
    return new instance_trial_of_the_crussader(pMap);
}

void AddSC_instance_trial_of_the_crussader()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crussader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crussader;
    newscript->RegisterSelf();
}