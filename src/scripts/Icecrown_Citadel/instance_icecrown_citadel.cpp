/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ScriptedPch.h"
#include "instance_icecrown_citadel.h"

#define MAX_ENCOUNTER     4

struct instance_icecrown_citadel : public ScriptedInstance
{
    instance_icecrown_citadel(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

	std::string str_data;

	uint64 m_uiMarrowgar;
    uint64 m_uiDeathwhisper;
    uint64 m_uiGunship;
    uint64 m_uiDeathbringer;

	uint64 uiMarrowgarDoor;
	uint64 uiOratoryDoor;
	uint64 uiExitDoor;
	uint64 uiIceblock1;
	uint64 uiIceblock2;

    uint32 m_auiEncounter[MAX_ENCOUNTER];

   void Initialize()
   {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

	    uiMarrowgarDoor = 0;
        uiOratoryDoor = 0;
        uiExitDoor = 0;
		uiIceblock1 = 0;
		uiIceblock2 = 0;

        m_uiMarrowgar = 0;
        m_uiDeathwhisper = 0;
        m_uiGunship = 0;
        m_uiDeathbringer = 0;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool add)
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
			case 36612: 
				m_uiMarrowgar = pCreature->GetGUID(); 
            break;
			case 36855: 
				m_uiDeathwhisper = pCreature->GetGUID(); 
            break;
			case 30343: 
				m_uiGunship = pCreature->GetGUID(); 
            break;
			case 37813: 
				m_uiDeathbringer = pCreature->GetGUID(); 
			break;
            case NPC_SAURFANG:
                if (TeamInInstance == ALLIANCE)
                   pCreature->UpdateEntry(NPC_MURADIN, ALLIANCE);
			break;
			case NPC_KORKRONGUARD:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_ALLIANCEGUARD, ALLIANCE);
			break;
			case NPC_VENDOR_SHAMAN_H:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_VENDOR_SHAMAN_A, ALLIANCE);
			break;
			case NPC_VENDOR_MAGE_H:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_VENDOR_MAGE_A, ALLIANCE);
			break;
			case NPC_VENDOR_DRUID_H:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_VENDOR_DRUID_A, ALLIANCE);
			break;
			case NPC_VENDOR_ROGUE_H:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_VENDOR_ROGUE_A, ALLIANCE);
			break;
			case NPC_VENDOR_WARLOCK_H:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_VENDOR_WARLOCK_A, ALLIANCE);
			break;
			case NPC_VENDOR_HUNTER_H:
				if (TeamInInstance == ALLIANCE)
					pCreature->UpdateEntry(NPC_VENDOR_HUNTER_A, ALLIANCE);
			break;

        }
    }

	void OnGameObjectCreate(GameObject* pGo, bool add)
    {
        switch(pGo->GetEntry())
        {
            case GO_ORATORY_ENTRANCE:
                uiOratoryDoor = pGo->GetGUID();
                break;
            case GO_MARROWGAR_DOOR:
                uiMarrowgarDoor = pGo->GetGUID();
            if (m_auiEncounter[0] == IN_PROGRESS) HandleGameObject(NULL,false,pGo);
            if (m_auiEncounter[0] == DONE || m_auiEncounter[0] == FAIL || m_auiEncounter[0] == NOT_STARTED) HandleGameObject(NULL,true,pGo);
                break;
            case GO_ICECROWN_EXIT:
                uiExitDoor = pGo->GetGUID();
                break;
            case GO_ICEBLOCK_1:
                uiIceblock1 = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE) HandleGameObject(NULL,true,pGo);
                break;
            case GO_ICEBLOCK_2:
                uiIceblock2 = pGo->GetGUID();
                if (m_auiEncounter[0] == DONE) HandleGameObject(NULL,true,pGo);
                break;
        }
        if (GameObject* pOratoryDoor = instance->GetGameObject(uiOratoryDoor))
                    pOratoryDoor->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* pMarrowgarDoor = instance->GetGameObject(uiMarrowgarDoor))
                    pMarrowgarDoor->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* pExitDoor = instance->GetGameObject(uiExitDoor))
                    pExitDoor->SetGoState(GO_STATE_ACTIVE);
	}

    void SetData(uint32 type, uint32 data)
    {

        switch(type)
        {
            case DATA_MARROWGAR_EVENT:
                m_auiEncounter[0] = data;
                if (data == DONE)
                    HandleGameObject(uiIceblock1,true);
                if (data == DONE)
                    HandleGameObject(uiIceblock2,true);
                if (data == IN_PROGRESS)
                    HandleGameObject(uiMarrowgarDoor,false);
                if (data == DONE || data == FAIL || data == NOT_STARTED)
                    HandleGameObject(uiMarrowgarDoor,true);
                break;
            case DATA_DEATHWHISPER_EVENT:
                m_auiEncounter[0] = data;
                break;
            case DATA_GUNSHIP_EVENT:
                m_auiEncounter[0] = data;
                break;
            case DATA_SAURFANG_EVENT:
                m_auiEncounter[0] = data;
                break;
        }

        if (data == DONE)
        {
            SaveToDB();
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_MARROWGAR_EVENT:			 return m_auiEncounter[0];
            case DATA_DEATHWHISPER_EVENT:        return m_auiEncounter[1];
            case DATA_GUNSHIP_EVENT:             return m_auiEncounter[2];
            case DATA_SAURFANG_EVENT:            return m_auiEncounter[3];
        }

        return 0;
    }

    uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
            case NPC_MARROWGAR:                    return m_uiMarrowgar;
            case NPC_DEATHWHISPER:                 return m_uiDeathwhisper;
            case NPC_GUNSHIP_A:                    return m_uiGunship;
            case NPC_DEATHBRINGER_SAURFANG:        return m_uiDeathbringer;
        }

        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << "I C " << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
            << m_auiEncounter[2] << " " << m_auiEncounter[3];

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
        uint16 data0, data1, data2, data3;

        std::istringstream loadStream(in);
        loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

        if (dataHead1 == 'I' && dataHead2 == 'C')
        {
            m_auiEncounter[0] = data0;
            m_auiEncounter[1] = data1;
            m_auiEncounter[2] = data2;
            m_auiEncounter[3] = data3;

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    m_auiEncounter[i] = NOT_STARTED;

        } else OUT_LOAD_INST_DATA_FAIL;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_icecrown_citadel(Map* pMap)
{
   return new instance_icecrown_citadel(pMap);
}

void AddSC_instance_icecrown_citadel()
{
   Script *newscript;
   newscript = new Script;
   newscript->Name = "instance_icecrown_citadel";
   newscript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
   newscript->RegisterSelf();
}