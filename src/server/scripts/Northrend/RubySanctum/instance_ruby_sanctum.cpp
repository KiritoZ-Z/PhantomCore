/* Copyright (C) 2010 /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "ScriptPCH.h"
#include "ruby_sanctum.h"

#define MAX_ENCOUNTER 4

struct instance_ruby_sanctum : public ScriptedInstance
{
        instance_ruby_sanctum(Map *pMap) : ScriptedInstance(pMap) {Initialize();};

        uint8 m_auiEncounter[MAX_ENCOUNTER];
        std::string str_data;

        uint32 m_uiXerestrasza;
        uint32 m_uiBossCounter;

        uint64 m_uiBaltharusGUID;
        uint64 m_uiZarithrianGUID;
        uint64 m_uiRagefireGUID;
        uint64 m_uiHalionGUID;
        uint64 m_uiXerestraszaGUID;

        void Initialize()
        {
                memset(m_auiEncounter, 0, sizeof(m_auiEncounter));
                m_uiXerestrasza = NOT_STARTED;
                m_uiBossCounter = 0;

                m_uiBaltharusGUID = 0;
                m_uiZarithrianGUID = 0;
                m_uiRagefireGUID = 0;
                m_uiHalionGUID = 0;
                m_uiXerestraszaGUID = 0;
        }

        void OnCreatureCreate(Creature *pCreature, bool )
        {
                switch(pCreature->GetEntry())
                {
                case NPC_BALTHARUS:
                        m_uiBaltharusGUID = pCreature->GetGUID();
                        break;
                case NPC_ZARITHRIAN:
                        m_uiZarithrianGUID = pCreature->GetGUID();
                        break;
                case NPC_RAGEFIRE:
                        m_uiRagefireGUID = pCreature->GetGUID();
                        break;
                case NPC_HALION:
                        m_uiHalionGUID = pCreature->GetGUID();
                        break;
                case NPC_XERESTRASZA:
                        m_uiXerestraszaGUID = pCreature->GetGUID();
                        break;
                }
        }

        void SetData(uint32 uiType, uint32 uiData)
        {
                switch(uiType)
                {
                case DATA_BALTHARUS_EVENT:
                        m_auiEncounter[0] = uiData;
                        if(uiData == DONE)
                                m_uiBossCounter++;
                        if(m_uiBossCounter == 3)
                        {
                                Creature *pCreature = instance->GetCreature(GetData64(DATA_HALION));
                                pCreature->SetReactState(REACT_AGGRESSIVE);
                                pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pCreature->SetVisibility(VISIBILITY_ON);
                        }               
                        break;
                case DATA_ZARITHRIAN_EVENT:
                        m_auiEncounter[1] = uiData;
                        if(uiData == DONE)
                                m_uiBossCounter++;
                        if(m_uiBossCounter == 3)
                        {
                                Creature *pCreature = instance->GetCreature(GetData64(DATA_HALION));
                                pCreature->SetReactState(REACT_AGGRESSIVE);
                                pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pCreature->SetVisibility(VISIBILITY_ON);
                        }
                        break;
                case DATA_RAGEFIRE_EVENT:
                        m_auiEncounter[2] = uiData;
                        if(uiData == DONE)
                                m_uiBossCounter++;
                        if(m_uiBossCounter == 3)
                        {
                                Creature *pCreature = instance->GetCreature(GetData64(DATA_HALION));
                                pCreature->SetReactState(REACT_AGGRESSIVE);
                                pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                pCreature->SetVisibility(VISIBILITY_ON);
                        }
                        break;
                case DATA_HALION_EVENT:
                        m_auiEncounter[3] = uiData;
                        if(uiData == DONE)
                        {
                                switch(instance->GetDifficulty())
                                {
                                case RAID_DIFFICULTY_10MAN_NORMAL:
                                        DoCompleteAchievement(4817);
                                case RAID_DIFFICULTY_10MAN_HEROIC:
                                        DoCompleteAchievement(4818);
                                case RAID_DIFFICULTY_25MAN_NORMAL:
                                        DoCompleteAchievement(4815);
                                case RAID_DIFFICULTY_25MAN_HEROIC:
                                        DoCompleteAchievement(4816);
                                }
                        }
                case DATA_XERESTRASZA_EVENT:
                        m_uiXerestrasza = uiData;
                }
        }

        uint32 GetData(uint32 uiType)
        {
                if(uiType == DATA_BALTHARUS_EVENT)
                        return m_auiEncounter[0];
                if(uiType == DATA_ZARITHRIAN_EVENT)
                        return m_auiEncounter[1];
                if(uiType == DATA_RAGEFIRE_EVENT)
                        return m_auiEncounter[2];
                if(uiType == DATA_HALION_EVENT)
                        return m_auiEncounter[3];
                if(uiType == DATA_XERESTRASZA_EVENT)
                        return m_uiXerestrasza;
        }

        uint64 GetData64(uint32 uiType)
        {
                if(uiType == DATA_BALTHARUS)
                        return m_uiBaltharusGUID;
                if(uiType == DATA_ZARITHRIAN)
                        return m_uiZarithrianGUID;
                if(uiType == DATA_RAGEFIRE)
                        return m_uiRagefireGUID;
                if(uiType == DATA_HALION)
                        return m_uiHalionGUID;
                if(uiType == DATA_XERESTRASZA)
                        return m_uiXerestraszaGUID;
        }

        std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
                // "C"hamber of Aspects, "R"uby Sanctum
        saveStream << "C R " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_uiXerestrasza;

        str_data = saveStream.str();

        OUT_SAVE_INST_DATA_COMPLETE;
        return str_data;
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        char dataHead1, dataHead2;
        uint16 data0, data1, data2, data3, data4;

        std::istringstream loadStream(in);
        loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4;

        if (dataHead1 == 'C' && dataHead2 == 'R')
        {
            m_auiEncounter[0] = data0;
            m_auiEncounter[1] = data1;
            m_auiEncounter[2] = data2;
            m_auiEncounter[3] = data3;
                        m_uiXerestrasza = data4;

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED;

        } else OUT_LOAD_INST_DATA_FAIL;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_ruby_sanctum(Map *pMap)
{
        return new instance_ruby_sanctum(pMap);
}

void AddSC_instance_ruby_sanctum()
{
        Script* newscript;
        newscript = new Script;
        newscript->Name = "instance_ruby_sanctum";
        newscript->GetInstanceData = &GetInstanceData_instance_ruby_sanctum;
        newscript->RegisterSelf();
}