/*
* Copyright (C) 2009 - 2010 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptPCH.h"
#include "icecrown_citadel.h"

#define MAX_ENCOUNTER     12

struct instance_icecrown_citadel : public ScriptedInstance
{
    instance_icecrown_citadel(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint64 uiLordMarrowgar;
    uint64 uiLadyDeathwhisper;
    uint64 uiGunship;
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

    uint64 uiIceWall1;
    uint64 uiIceWall2;
    uint64 uiMarrowgarEntrance;
    uint64 uiFrozenThrone;
    uint64 m_uiSaurfangCacheGUID;
    uint64 uiLadyDeathwisperTransporter;
    uint64 uiOratoryDoor;
    uint64 uiSaurfangDoor;
    uint64 uiOrangeMonsterDoor;
    uint64 uiGreenMonsterDoor;
    uint64 uiProfCollisionDoor;
    uint64 uiOrangePipe;
    uint64 uiGreenPipe;
    uint64 uiOozeValve;
    uint64 uiGasValve;
    uint64 uiProfDoorOrange;
    uint64 uiProfDoorGreen;
    uint64 uiRotfaceEntrance;
    uint64 uiFestergurtEntrance;
    uint64 uiProffesorDoor;
    uint64 uiBloodwingDoor;
    uint64 uiCrimsonHallDoor1;
    uint64 uiCrimsonHallDoor2;
    uint64 uiCrimsonHallDoor3;
    uint64 uiBloodQueenTransporter;
    uint64 uiFrostwingDoor;
    uint64 uiDragonDoor1;
    uint64 uiDragonDoor2;
    uint64 uiDragonDoor3;
    uint64 uiRoostDoor1;
    uint64 uiRoostDoor2;
    uint64 uiRoostDoor3;
    uint64 uiRoostDoor4;
    uint64 m_uiDreamwalkerCacheGUID; 
    uint64 uiSindragosaDoor1;
    uint64 uiSindragosaDoor2;
    uint64 uiFirstTp;
    uint64 uiMarrowgarTp;
    uint64 uiFlightWarTp;
    uint64 uiSaurfangTp;
    uint64 uiCitadelTp;
    uint64 uiSindragossaTp;
    uint64 uiLichTp;

    uint32 auiEncounter[MAX_ENCOUNTER];

    void Initialize()
    {
        memset(&auiEncounter, 0, sizeof(auiEncounter));

        uiLordMarrowgar         = 0;
        uiLadyDeathwhisper      = 0;
        uiGunship               = 0;
        uiDeathbringerSaurfang  = 0;
        uiFestergut             = 0;
        uiRotface               = 0;
        uiProfessorPutricide    = 0;
        uiPrinceValanar         = 0;
        uiPrinceKeleseth        = 0;    
        uiPrinceTaldaram        = 0;
        uiBloodQueenLanathel    = 0;
        uiValithriaDreamwalker  = 0;
        uiSindragosa            = 0;
        uiLichKing              = 0;
        m_uiSaurfangCacheGUID   = 0;

        uiIceWall1              = 0;
        uiIceWall2              = 0;
        uiMarrowgarEntrance     = 0;
        uiLadyDeathwisperTransporter = 0;
        uiOratoryDoor           = 0;
        uiSaurfangDoor          = 0;
        uiOrangeMonsterDoor     = 0;
        uiGreenMonsterDoor      = 0;
        uiProfCollisionDoor     = 0;
        uiOrangePipe            = 0;
        uiGreenPipe             = 0;
        uiOozeValve             = 0;
        uiGasValve              = 0;
        uiProfDoorOrange        = 0;
        uiProfDoorGreen         = 0;
        uiRotfaceEntrance       = 0;
        uiFestergurtEntrance    = 0;
        uiProffesorDoor         = 0;
        uiBloodwingDoor         = 0;
        uiCrimsonHallDoor1      = 0;
        uiCrimsonHallDoor2      = 0;
        uiCrimsonHallDoor3      = 0;
        uiBloodQueenTransporter = 0;
        uiFrostwingDoor         = 0;
        uiDragonDoor1           = 0;
        uiDragonDoor2           = 0;
        uiDragonDoor3           = 0;
        uiRoostDoor1            = 0;
        uiRoostDoor2            = 0;
        uiRoostDoor3            = 0;
        uiRoostDoor4            = 0;
        uiSindragosaDoor1       = 0;
        uiSindragosaDoor2       = 0;
        uiFirstTp               = 0;
        uiMarrowgarTp           = 0;
        uiFlightWarTp           = 0;
        uiSaurfangTp            = 0;
        uiCitadelTp             = 0;
        uiSindragossaTp         = 0;
        uiLichTp                = 0;
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
        case 36855: uiLadyDeathwhisper = pCreature->GetGUID(); break;
        case 30343: uiGunship = pCreature->GetGUID(); break;
        case 37813: uiDeathbringerSaurfang = pCreature->GetGUID(); break;
        case 36626: uiFestergut = pCreature->GetGUID(); break;
        case 36627: uiRotface = pCreature->GetGUID(); break;
        case 36678: uiProfessorPutricide = pCreature->GetGUID(); break;
        case 37970: uiPrinceValanar = pCreature->GetGUID(); break;
        case 37972: uiPrinceKeleseth = pCreature->GetGUID(); break;
        case 37973: uiPrinceTaldaram = pCreature->GetGUID(); break;
        case 37955: uiBloodQueenLanathel = pCreature->GetGUID(); break;
        case 36789: uiValithriaDreamwalker = pCreature->GetGUID(); break;
        case 36853: uiSindragosa = pCreature->GetGUID(); break;
        case 36597: uiLichKing = pCreature->GetGUID(); break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool add)
    {
        switch (pGo->GetEntry())
        {
        case 201910:
            uiIceWall1 = pGo->GetGUID();
            if (auiEncounter[0] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201911:
            uiIceWall2 = pGo->GetGUID();
            if (auiEncounter[0] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201857:
            uiMarrowgarEntrance = pGo->GetGUID();
            if (auiEncounter[0] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201563:
            uiOratoryDoor = pGo->GetGUID();
            if (auiEncounter[1] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 202220:
            uiLadyDeathwisperTransporter = pGo->GetGUID();
            break;
        case 201825:
            uiSaurfangDoor = pGo->GetGUID();
            if (auiEncounter[3] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201919:
            uiBloodwingDoor = pGo->GetGUID();
            if (auiEncounter[3] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201920:
            uiFrostwingDoor = pGo->GetGUID();
            if (auiEncounter[3] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201376:
            uiCrimsonHallDoor1 = pGo->GetGUID();
            if (auiEncounter[7] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201377:
            uiCrimsonHallDoor2 = pGo->GetGUID();
            if (auiEncounter[7] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201378:
            uiCrimsonHallDoor3 = pGo->GetGUID();
            if (auiEncounter[7] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201755:
            uiBloodQueenTransporter = pGo->GetGUID();
            if (auiEncounter[8] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201375:
            uiDragonDoor1 = pGo->GetGUID();
            if (auiEncounter[8] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201374:
            uiDragonDoor2 = pGo->GetGUID();
            if (auiEncounter[9] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201379:
            uiDragonDoor3 = pGo->GetGUID();
            if (auiEncounter[9] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201380:
            uiRoostDoor1 = pGo->GetGUID();
            if (auiEncounter[9] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201381:
            uiRoostDoor2 = pGo->GetGUID();
            if (auiEncounter[9] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201382:
            uiRoostDoor3 = pGo->GetGUID();
            if (auiEncounter[9] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201383:
            uiRoostDoor4 = pGo->GetGUID();
            if (auiEncounter[9] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201373:
            uiSindragosaDoor1 = pGo->GetGUID();
            if (auiEncounter[10] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201369:
            uiSindragosaDoor2 = pGo->GetGUID();
            if (auiEncounter[10] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 202239:
            m_uiSaurfangCacheGUID = pGo->GetGUID();
            break;
        case 202240:
            m_uiSaurfangCacheGUID = pGo->GetGUID();
            break;
        case 202238:
            m_uiSaurfangCacheGUID = pGo->GetGUID();
            break;
        case 202241:
            m_uiSaurfangCacheGUID = pGo->GetGUID();
            break;
        case 201959:
            m_uiDreamwalkerCacheGUID = pGo->GetGUID();
            break;
        case 202339:
            m_uiDreamwalkerCacheGUID = pGo->GetGUID();
            break;
        case 202338:
            m_uiDreamwalkerCacheGUID = pGo->GetGUID();
            break;
        case 202340:
            m_uiDreamwalkerCacheGUID = pGo->GetGUID();
            break;
        case 202242:
            uiFirstTp = pGo->GetGUID();
            if (auiEncounter[0] == DONE) //NEED TEST
                HandleGameObject(NULL,true,pGo);
            break;
        case 202243:
            uiMarrowgarTp = pGo->GetGUID();
            if (auiEncounter[0] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 202244:
            uiFlightWarTp = pGo->GetGUID(); 
            if (auiEncounter[2] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);			
            break;
        case 202245:
            uiSaurfangTp = pGo->GetGUID();
            if (auiEncounter[4] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 202235:
            uiCitadelTp = pGo->GetGUID();
            if (auiEncounter[4] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);			
            break;
        case 202246:
            uiSindragossaTp = pGo->GetGUID();
            if(auiEncounter[10] == NOT_STARTED)
                HandleGameObject(NULL,true,pGo);
            break;
        case 202223:
            uiLichTp = pGo->GetGUID();
            if(auiEncounter[10] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201612:
            uiProfCollisionDoor = pGo->GetGUID();
            if (auiEncounter[4] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201618:
            uiGreenPipe = pGo->GetGUID();
            if (auiEncounter[5] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201615:
            uiOozeValve = pGo->GetGUID();
            if (auiEncounter[5] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201614:
            uiProfDoorGreen = pGo->GetGUID();
            if (auiEncounter[5] == NOT_STARTED)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201617:
            uiOrangePipe = pGo->GetGUID();
            if (auiEncounter[4] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201616:
            uiGasValve = pGo->GetGUID();
            if (auiEncounter[4] == NOT_STARTED)
                HandleGameObject(NULL,false,pGo);
            break;
        case 201613:
            uiProfDoorOrange = pGo->GetGUID();
            if (auiEncounter[4] == NOT_STARTED)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201370:
            uiGreenMonsterDoor = pGo->GetGUID();
            if (auiEncounter[5] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201371:
            uiOrangeMonsterDoor = pGo->GetGUID();
            if (auiEncounter[4] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        case 201372:
            uiProffesorDoor = pGo->GetGUID();
            if (auiEncounter[6] == DONE)
                HandleGameObject(NULL,true,pGo);
            break;
        }
    }

    uint64 GetData64(uint32 identifier)
    {
        switch(identifier)
        {
        case DATA_MARROWGAR:                return uiLordMarrowgar;
        case DATA_DEATHWHISPER:             return uiLadyDeathwhisper;
        case DATA_GUNSHIP_BATTLE:           return uiGunship;
        case DATA_SAURFANG:                 return uiDeathbringerSaurfang;
        case DATA_FESTERGURT:               return uiFestergut;
        case DATA_ROTFACE:                  return uiRotface;
        case DATA_PROFESSOR_PUTRICIDE:      return uiProfessorPutricide;
        case DATA_PRINCE_VALANAR_ICC:       return uiPrinceValanar;
        case DATA_PRINCE_KELESETH_ICC:      return uiPrinceKeleseth;
        case DATA_PRINCE_TALDARAM_ICC:      return uiPrinceTaldaram;
        case DATA_BLOOD_QUEEN_LANATHEL:     return uiBloodQueenLanathel;
        case DATA_VALITHRIA_DREAMWALKER:    return uiValithriaDreamwalker;
        case DATA_SINDRAGOSA:               return uiSindragosa;
        case DATA_LICH_KING:                return uiLichKing;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
        case DATA_MARROWGAR_EVENT:
            switch(data)
            {
            case DONE:
                HandleGameObject(uiIceWall1,true);
                HandleGameObject(uiIceWall2,true);
                HandleGameObject(uiMarrowgarEntrance,true);
                break;
            case NOT_STARTED:
                HandleGameObject(uiIceWall1,false);
                HandleGameObject(uiIceWall2,false);
                HandleGameObject(uiMarrowgarEntrance,true);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiMarrowgarEntrance,false);
                break;
            }
            auiEncounter[0] = data;
            break;

        case DATA_DEATHWHISPER_EVENT:
            switch(data)
            {
            case DONE:
                HandleGameObject(uiOratoryDoor,true);
                if (GameObject* pGO = instance->GetGameObject(uiLadyDeathwisperTransporter))
                    {
                        pGO->SetUInt32Value(GAMEOBJECT_LEVEL, 0);
                        pGO->SetGoState(GO_STATE_READY);
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

/*      case DATA_GUNSHIP_BATTLE_EVENT:
            switch(data)
            {
            case DONE:
            break;
            case NOT_STARTED:
            break;
            }
            auiEncounter[2] = data;
            break;*/

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
                HandleGameObject(uiBloodwingDoor,true);
                HandleGameObject(uiFrostwingDoor,true);
                break;
            case NOT_STARTED:
                HandleGameObject(uiSaurfangDoor,false);
                HandleGameObject(uiBloodwingDoor,false);
                HandleGameObject(uiFrostwingDoor,false);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiSaurfangDoor,false);
                HandleGameObject(uiBloodwingDoor,false);
                HandleGameObject(uiFrostwingDoor,false);
                break;
            }
            auiEncounter[3] = data;
            break;

        case DATA_FESTERGURT_EVENT:
            switch(data)
            {
            case DONE:
                if (auiEncounter[5] == DONE)
                {
                    HandleGameObject(uiProfCollisionDoor,true);
                }
                HandleGameObject(uiOrangeMonsterDoor,true);
                HandleGameObject(uiOrangePipe,true);
                HandleGameObject(uiGasValve,true);
                HandleGameObject(uiProfDoorOrange,false);
                break;
            case NOT_STARTED:
                HandleGameObject(uiOrangeMonsterDoor,true);
                HandleGameObject(uiOrangePipe,false);
                HandleGameObject(uiGasValve,false);
                HandleGameObject(uiProfDoorOrange,true);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiOrangeMonsterDoor,false);
                HandleGameObject(uiOrangePipe,false);
                HandleGameObject(uiGasValve,false);
                HandleGameObject(uiProfDoorOrange,true);
                break;
            }
            auiEncounter[4] = data;
            break;

        case DATA_ROTFACE_EVENT:
            switch(data)
            {
            case DONE:
                if (auiEncounter[4] == DONE)
                {
                    HandleGameObject(uiProfCollisionDoor,true);
                }
                HandleGameObject(uiGreenMonsterDoor,true);
                HandleGameObject(uiGreenPipe,true);
                HandleGameObject(uiOozeValve,true);
                HandleGameObject(uiProfDoorGreen,false);
                break;
            case NOT_STARTED:
                HandleGameObject(uiGreenMonsterDoor,true);
                HandleGameObject(uiGreenPipe,false);
                HandleGameObject(uiOozeValve,false);
                HandleGameObject(uiProfDoorGreen,true);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiGreenMonsterDoor,false);
                HandleGameObject(uiGreenPipe,false);
                HandleGameObject(uiOozeValve,false);
                HandleGameObject(uiProfDoorGreen,true);
                break;
            }
            auiEncounter[5] = data;
            break;

        case DATA_PROFESSOR_PUTRICIDE_EVENT:
            switch(data)
            {
            case DONE:
                HandleGameObject(uiProffesorDoor,true);
                break;
            case NOT_STARTED:
                HandleGameObject(uiProffesorDoor,true);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiProffesorDoor,false);
                break;
            }
            auiEncounter[6] = data;
            break;

        case DATA_BLOOD_PRINCE_COUNCIL_EVENT:
            switch(data)
            {
            case DONE:
                HandleGameObject(uiCrimsonHallDoor1,true);
                HandleGameObject(uiCrimsonHallDoor2,true);
                HandleGameObject(uiCrimsonHallDoor3,true);
                break;
            case NOT_STARTED:
                HandleGameObject(uiCrimsonHallDoor1,true);
                HandleGameObject(uiCrimsonHallDoor2,false);
                HandleGameObject(uiCrimsonHallDoor3,false);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiCrimsonHallDoor1,false);
                break;
            }
            auiEncounter[7] = data;
            break;

        case DATA_BLOOD_QUEEN_LANATHEL_EVENT:
            switch(data)
            {
            case DONE:
                if (GameObject* pGO = instance->GetGameObject(uiBloodQueenTransporter))
                    {
                        pGO->SetUInt32Value(GAMEOBJECT_LEVEL, 0);
                        pGO->SetGoState(GO_STATE_READY);
                    }
                break;
            case NOT_STARTED:
                break;
            case IN_PROGRESS:
                break;
            }
            auiEncounter[8] = data;
            break;

        case DATA_VALITHRIA_DREAMWALKER_EVENT:
            switch(data)
            {
            case DONE:
                if (GameObject* pChest = instance->GetGameObject(m_uiDreamwalkerCacheGUID))
                    if (pChest && !pChest->isSpawned()) 
                    {
                        pChest->SetRespawnTime(pChest->GetRespawnDelay());
                    }
                HandleGameObject(uiDragonDoor1,true);
                HandleGameObject(uiDragonDoor2,true);
                HandleGameObject(uiDragonDoor3,true);
                HandleGameObject(uiRoostDoor1,false);
                HandleGameObject(uiRoostDoor2,false);
                HandleGameObject(uiRoostDoor3,false);
                HandleGameObject(uiRoostDoor4,false);
                break;
            case NOT_STARTED:
                HandleGameObject(uiDragonDoor1,true);
                HandleGameObject(uiDragonDoor2,false);
                HandleGameObject(uiDragonDoor3,false);
                HandleGameObject(uiRoostDoor1,false);
                HandleGameObject(uiRoostDoor2,false);
                HandleGameObject(uiRoostDoor3,false);
                HandleGameObject(uiRoostDoor4,false);
                break;
            case IN_PROGRESS:
                if (Difficulty() == RAID_DIFFICULTY_10MAN_NORMAL ||
                    Difficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
                {
                    HandleGameObject(uiDragonDoor1,false);
                    HandleGameObject(uiRoostDoor3,true);
                    HandleGameObject(uiRoostDoor2,true);
                    HandleGameObject(uiRoostDoor1,false);
                    HandleGameObject(uiRoostDoor4,false);
                    break;
                }
                if (Difficulty() == RAID_DIFFICULTY_25MAN_NORMAL || //It must work!!!
                    Difficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                    HandleGameObject(uiDragonDoor1,false);
                    HandleGameObject(uiRoostDoor1,true);
                    HandleGameObject(uiRoostDoor2,true);
                    HandleGameObject(uiRoostDoor3,true);
                    HandleGameObject(uiRoostDoor4,true);
                    break;
				}
            }
            auiEncounter[9] = data;
            break;

        case DATA_SINDRAGOSA_EVENT:
            switch(data)
            {
            case DONE:
                HandleGameObject(uiSindragosaDoor1,true);
                HandleGameObject(uiSindragosaDoor2,true);
                break;
            case NOT_STARTED:
                HandleGameObject(uiSindragosaDoor1,true);
                HandleGameObject(uiSindragosaDoor2,true);
                break;
            case IN_PROGRESS:
                HandleGameObject(uiSindragosaDoor1,false);
                HandleGameObject(uiSindragosaDoor2,false);
                break;
            }
            auiEncounter[10] = data;
            break;

        case DATA_LICH_KING_EVENT:
            switch(data)
            {
            case DONE:
                break;
            case NOT_STARTED:
                break;
            case IN_PROGRESS:
                break;
            }
            auiEncounter[10] = data;
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
        case DATA_MARROWGAR_EVENT:               return auiEncounter[0];
        case DATA_DEATHWHISPER_EVENT:            return auiEncounter[1];
        case DATA_GUNSHIP_BATTLE_EVENT:          return auiEncounter[2];
        case DATA_SAURFANG_EVENT:                return auiEncounter[3];
        case DATA_FESTERGURT_EVENT:              return auiEncounter[4];
        case DATA_ROTFACE_EVENT:                 return auiEncounter[5];
        case DATA_PROFESSOR_PUTRICIDE_EVENT:     return auiEncounter[6];
        case DATA_BLOOD_PRINCE_COUNCIL_EVENT:    return auiEncounter[7];
        case DATA_BLOOD_QUEEN_LANATHEL_EVENT:    return auiEncounter[8];
        case DATA_VALITHRIA_DREAMWALKER_EVENT:   return auiEncounter[9];
        case DATA_SINDRAGOSA_EVENT:              return auiEncounter[10];
        case DATA_LICH_KING_EVENT:               return auiEncounter[11];
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
    Script *newscript;

    newscript = new Script;
    newscript->Name = "instance_icecrown_citadel";
    newscript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
    newscript->RegisterSelf();
}
