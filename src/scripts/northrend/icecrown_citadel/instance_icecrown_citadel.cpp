/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Phantom Project <http://phantom-project.org/>
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

#define MAX_ENCOUNTER     12

struct instance_icecrown_citadel : public ScriptedInstance
{
    instance_icecrown_citadel(Map* pMap) : ScriptedInstance(pMap)
	{
		Initialize();
	};

    uint64 uiLordMarrowgar;
    uint64 uiLadyDeathwhisper;
	uint64 uiDeathbringerSaurfang;
	uint64 uiFestergut;
	uint64 uiRotface;
	uint64 uiProfessorPutricide;
	uint64 uiPrinceValanar;
	uint64 uiPrinceKeleseth;	
	uint64 uiPrinceTaldaram;
	uint64 uiBloodQueenLanathel;
	uint64 uiValithriaDreamwalker;
	uint64 uiSindragosa;
	uint64 uiLichKing;

    uint64 uiMarrowgarDoor1;
	uint64 uiMarrowgarDoor2;
	uint64 uiMarrowgarDoor3;
	uint64 uiFrozenThrone;
	uint64 m_uiSaurfangCacheGUID;
    uint64 uiLadyDeathwisperTransporter;
    uint64 uiOratoryDoor;
    uint64 uiSaurfangDoor;
    uint64 uiFirstTp;
    uint64 uiMarrowgarTp;
    uint64 uiFlightWarTp;
    uint64 uiSaurfangTp;
    uint64 uiCitadelTp;
	uint64 uiLichTp;

    uint32 auiEncounter[MAX_ENCOUNTER];

   void Initialize()
   {
        memset(&auiEncounter, 0, sizeof(auiEncounter));

        uiLordMarrowgar			= 0;
        uiLadyDeathwhisper		= 0;
		uiDeathbringerSaurfang	= 0;
		uiFestergut				= 0;
		uiRotface				= 0;
		uiProfessorPutricide	= 0;
		uiPrinceValanar			= 0;
		uiPrinceKeleseth		= 0;	
		uiPrinceTaldaram		= 0;
		uiBloodQueenLanathel	= 0;
		uiValithriaDreamwalker	= 0;
		uiSindragosa			= 0;
		uiLichKing				= 0;
		m_uiSaurfangCacheGUID   = 0;

        uiMarrowgarDoor1	= 0;
        uiMarrowgarDoor2	= 0;
		uiMarrowgarDoor3	= 0;
		uiLadyDeathwisperTransporter	=	0;
        uiOratoryDoor       = 0;
		uiSaurfangDoor      = 0;
//      Portals
		uiFirstTp           = 0;
        uiMarrowgarTp       = 0;
        uiFlightWarTp       = 0;
        uiSaurfangTp        = 0;
        uiCitadelTp         = 0;
		uiLichTp            = 0;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (auiEncounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool add)
    {
        switch(pCreature->GetEntry())
        {
            case 36612: uiLordMarrowgar = pCreature->GetGUID(); break;
            case 36855:	uiLadyDeathwhisper = pCreature->GetGUID(); break;
			case 37813:	uiDeathbringerSaurfang = pCreature->GetGUID(); break;
			case 36626: uiFestergut = pCreature->GetGUID(); break;
			case 36627: uiRotface = pCreature->GetGUID(); break;
			case 36678: uiProfessorPutricide = pCreature->GetGUID(); break;
			case 37970: uiPrinceValanar = pCreature->GetGUID(); break;
			case 37972: uiPrinceKeleseth = pCreature->GetGUID(); break;
			case 37973: uiPrinceTaldaram = pCreature->GetGUID(); break;
			case 37955: uiBloodQueenLanathel = pCreature->GetGUID(); break;
			case 36789: uiValithriaDreamwalker = pCreature->GetGUID(); break;
			case 36853: uiSindragosa = pCreature->GetGUID(); break;
			case 36597:	uiLichKing = pCreature->GetGUID(); break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool add)
    {
        switch (pGo->GetEntry())
        {
            case 201910:
                uiMarrowgarDoor1 = pGo->GetGUID();
				if (auiEncounter[0] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
                break;
			case 201911:
                uiMarrowgarDoor2 = pGo->GetGUID();
                if (auiEncounter[0] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
                break;
			case 201857:
				uiMarrowgarDoor3 = pGo->GetGUID();
                if (auiEncounter[0] == DONE)
                    HandleGameObject(NULL,true,pGo);
                break;
			case 202242:
                uiFirstTp = pGo->GetGUID();
				if (auiEncounter[0] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
				break;
			case 202243:
                uiMarrowgarTp = pGo->GetGUID();
				if (auiEncounter[0] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
				break;
			case 201563:
                uiOratoryDoor = pGo->GetGUID();
				if (auiEncounter[1] == DONE)
                    HandleGameObject(NULL,true,pGo);
				break;
			case 202220:
				uiLadyDeathwisperTransporter = pGo->GetGUID();
				if (auiEncounter[1] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
                break;
			case 201825:
				uiSaurfangDoor = pGo->GetGUID();
				if (auiEncounter[3] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
				break;
			case 202239: // 10 Normal
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
            case 202240: // 25 Normal
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
            case 202238: // 10 Heroic
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
            case 202241: // 25 Heroic
				m_uiSaurfangCacheGUID = pGo->GetGUID();
				break;
			case 202244:
                uiFlightWarTp = pGo->GetGUID(); 
				break;
			case 202245:
                uiSaurfangTp = pGo->GetGUID();
				if (auiEncounter[1] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
				break;
			case 202235:
				uiCitadelTp = pGo->GetGUID();
				if (auiEncounter[3] == NOT_STARTED)
                    HandleGameObject(NULL,false,pGo);
				break;
			case 202223:
				uiLichTp = pGo->GetGUID();
				break;
			 }
		}

    uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
            case DATA_MARROWGAR:				return uiLordMarrowgar;
            case DATA_DEATHWHISPER:				return uiLadyDeathwhisper;
			case DATA_SAURFANG:					return uiDeathbringerSaurfang;
			case DATA_FESTERGURT:				return uiFestergut;
			case DATA_ROTFACE:					return uiRotface;
			case DATA_PROFESSOR_PUTRICIDE:		return uiProfessorPutricide;
			case DATA_PRINCE_VALANAR_ICC:		return uiPrinceValanar;
			case DATA_PRINCE_KELESETH_ICC:		return uiPrinceKeleseth;
			case DATA_PRINCE_TALDARAM_ICC:		return uiPrinceTaldaram;
			case DATA_BLOOD_QUEEN_LANATHEL:		return uiBloodQueenLanathel;
			case DATA_VALITHRIA_DREAMWALKER:	return uiValithriaDreamwalker;
			case DATA_SINDRAGOSA:				return uiSindragosa;
			case DATA_LICH_KING:				return uiLichKing;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
		{
//////////////////////////////////////
			case DATA_MARROWGAR_EVENT:
			switch(data)
            {
			case DONE:
				HandleGameObject(uiMarrowgarDoor1,true);
				HandleGameObject(uiMarrowgarDoor2,true); // true -> Close
				HandleGameObject(uiMarrowgarDoor3,true); // false -> Open
                HandleGameObject(uiFirstTp,true);
                HandleGameObject(uiMarrowgarTp,true);
			    if (GameObject* pFirstTp = instance->GetGameObject(uiFirstTp))
			        if (pFirstTp && pFirstTp->isSpawned()) 
			        {
			            pFirstTp->RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
			        }
			    if (GameObject* pMarrowgarTp = instance->GetGameObject(uiMarrowgarTp))
			        if (pMarrowgarTp && pMarrowgarTp->isSpawned()) 
			        {
			            pMarrowgarTp->RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
			        }
				break;
            case NOT_STARTED:
                HandleGameObject(uiMarrowgarDoor1,false);
				HandleGameObject(uiMarrowgarDoor2,false);
				HandleGameObject(uiMarrowgarDoor3,true);
                HandleGameObject(uiFirstTp,false);
                HandleGameObject(uiMarrowgarTp,false);
				break;
			case IN_PROGRESS:
				HandleGameObject(uiMarrowgarDoor3,false);
				break;
			}
			auiEncounter[0] = data;
			break;
///////////////////////////////////////////////
			case DATA_DEATHWHISPER_EVENT:					//uiLadyDeathwisperTransporter
			switch(data)
            {
			case DONE:
                HandleGameObject(uiSaurfangTp,true);
				HandleGameObject(uiLadyDeathwisperTransporter,true);
                HandleGameObject(uiFlightWarTp,true);
                HandleGameObject(uiOratoryDoor,true);
			    if (GameObject* pSaurfangTp = instance->GetGameObject(uiSaurfangTp))
			        if (pSaurfangTp && pSaurfangTp->isSpawned()) 
			        {
			            pSaurfangTp->RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
			        }
			    if (GameObject* pFlightWarTp = instance->GetGameObject(uiFlightWarTp))
			        if (pFlightWarTp && pFlightWarTp->isSpawned()) 
			        {
			            pFlightWarTp->RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
			        }
				break;
			case IN_PROGRESS:
				HandleGameObject(uiOratoryDoor,false);
				break;
            case NOT_STARTED:
                HandleGameObject(uiOratoryDoor,true);
				break;
			}
			auiEncounter[1] = data;
			break;
////////////////////////////////////////////////////
/*			case DATA_GUNSHIP_BATTLE_EVENT:
			switch(data)
            {
			case DONE:
                HandleGameObject(uiOratoryDoor,true);
				break;
            case NOT_STARTED:
                HandleGameObject(uiOratoryDoor,false);
				break;
			}
			auiEncounter[2] = data; break;*/
////////////////////////////////////////////////////
			case DATA_SAURFANG_EVENT:
            switch(data)
			{
			case DONE:
			    if (GameObject* pChest = instance->GetGameObject(m_uiSaurfangCacheGUID))
			        if (pChest && !pChest->isSpawned()) 
			        {
			            pChest->SetRespawnTime(pChest->GetRespawnDelay());
			        }
			    HandleGameObject(uiSaurfangDoor,true);
			    HandleGameObject(uiCitadelTp, true);
			    if (GameObject* pCitadelTp = instance->GetGameObject(uiCitadelTp))
			        if (pCitadelTp && pCitadelTp->isSpawned()) 
			        {
			            pCitadelTp->RemoveFlag(GAMEOBJECT_FLAGS,GO_FLAG_INTERACT_COND);
			        }
            case NOT_STARTED:
                HandleGameObject(uiSaurfangDoor,true);
				break;
			case IN_PROGRESS:
                HandleGameObject(uiSaurfangDoor,false);
				break;
			}
				auiEncounter[3] = data;
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
            case DATA_MARROWGAR_EVENT:				return auiEncounter[0];
            case DATA_DEATHWHISPER_EVENT:			return auiEncounter[1];
			case DATA_GUNSHIP_BATTLE_EVENT:			return auiEncounter[2];
			case DATA_SAURFANG_EVENT:				return auiEncounter[3];
			case DATA_FESTERGURT_EVENT:				return auiEncounter[4];
			case DATA_ROTFACE_EVENT:				return auiEncounter[5];
			case DATA_PROFESSOR_PUTRICIDE_EVENT:	return auiEncounter[6];
			case DATA_BLOOD_PRINCE_COUNCIL_EVENT:	return auiEncounter[7];
			case DATA_BLOOD_QUEEN_LANATHEL_EVENT:	return auiEncounter[8];
			case DATA_VALITHRIA_DREAMWALKER_EVENT:	return auiEncounter[9];
			case DATA_SINDRAGOSA_EVENT:				return auiEncounter[10];
			case DATA_LICH_KING_EVENT:				return auiEncounter[11];
        }
        return 0;
    }

   std::string GetSaveData()
   {
		OUT_SAVE_INST_DATA;

		std::ostringstream saveStream;
		saveStream << "I C" << auiEncounter[0] << " " << auiEncounter[1] << " " << auiEncounter[2] << " " << auiEncounter[3] 
		<< " " << auiEncounter[4] << " " << auiEncounter[5] << " " << auiEncounter[6] << " " << auiEncounter[7] << " " << auiEncounter[8]
		<< " " << auiEncounter[9] << " " << auiEncounter[10] << " " << auiEncounter[11];

		OUT_SAVE_INST_DATA_COMPLETE;
		return saveStream.str();
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
        uint16 data0,data1,data2,data3,data4,data5,data6,data7,data8,data9,data10,data11;

        std::istringstream loadStream(in);
        loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4 >> data5 >> data6
		>> data7 >> data8 >> data9 >> data10 >> data11;

        if (dataHead1 == 'I' && dataHead2 == 'C')
        {
            auiEncounter[0] = data0;
            auiEncounter[1] = data1;
			auiEncounter[2] = data2;
			auiEncounter[3] = data3;
			auiEncounter[4] = data4;
			auiEncounter[5] = data5;
			auiEncounter[6] = data6;
			auiEncounter[7] = data7;
			auiEncounter[8] = data8;
			auiEncounter[9] = data9;
			auiEncounter[10] = data10;
			auiEncounter[11] = data11;

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (auiEncounter[i] == IN_PROGRESS)
                    auiEncounter[i] = NOT_STARTED;

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
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_icecrown_citadel";
    pNewScript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
    pNewScript->RegisterSelf();
}