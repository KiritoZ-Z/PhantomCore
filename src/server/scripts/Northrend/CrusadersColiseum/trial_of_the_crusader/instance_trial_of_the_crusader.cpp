#include "ScriptPCH.h"
#include "def.h"

#define ENCOUNTERS 6

const Position SpawnPosition = {563.539551, 213.565628, 395.098206, 4.66094};

struct  instance_trial_of_the_crusader : public ScriptedInstance
{

    instance_trial_of_the_crusader(Map* pMap) : ScriptedInstance(pMap), m_uiChampionGest(NULL), m_uiArgentGest(NULL), Floor(NULL) {Initialize();}
    
    uint32 Encounters[ENCOUNTERS];

    uint64 m_uiGormok;
    uint64 m_uiDreadscale;
    uint64 m_uiAsidmaw;
    uint64 m_uiIcehowl;
    uint64 m_uiJaraxxus;
    uint64 m_uiFjola;
    uint64 m_uiEydis;
    uint64 m_uiLord;
    uint64 m_uiKrik;
    uint64 m_uiAnubarak;
    uint64 uiAnnouncerGUID;
    uint64 m_uiAnnouncer;
    uint32 uiLightKing;
    uint32 uiChampion;
    uint64 m_uiMainDoor;
    uint64 m_uiEnterDoor;
    uint8 uiMainDoor;
    uint8 uiEnterDoor;
    uint64 uiFloorGUID;
    uint64 uiMainDoorGUID;
    uint8 uiDataStart1;
    uint8 uiDataStart2;
    uint8 uiDataStart3;

    GameObject* Floor, *m_uiChampionGest, *m_uiArgentGest;
    std::string str_data;
   
    void Initialize()
    {
    m_uiGormok    = 0;
    m_uiDreadscale = 0;
    m_uiIcehowl    = 0;
    m_uiJaraxxus = 0;
    m_uiFjola    = 0;
    m_uiEydis    = 0;
    m_uiAnubarak = 0;
    uiMainDoor    = 0;
    uiLightKing = 0;
    Floor    = 0;
    m_uiChampionGest = 0;
    m_uiArgentGest = 0;
    uiAnnouncerGUID = 0;
    m_uiAnnouncer = 0;
    uiFloorGUID = 0;
    m_uiMainDoor = 0;
    m_uiEnterDoor = 0;
    uiMainDoor = 0;
    uiEnterDoor = 0;
    uiDataStart1 = 0;
    uiDataStart2 = 0;
    uiDataStart3 = 0;
    
    memset(&Encounters, 0, sizeof(Encounters));

    }
    
    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if(Encounters[i] == IN_PROGRESS) return true;

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
    case NPC_ANNOUNCER:
    m_uiAnnouncer = pCreature->GetGUID(); break;
    case BOSS_GORMOK:
    m_uiGormok = pCreature->GetGUID(); break;
    case BOSS_DREADSCALE:
    m_uiDreadscale = pCreature->GetGUID(); break;
    case BOSS_ICEHOWL:
    m_uiIcehowl = pCreature->GetGUID(); break;
    case BOSS_LORD_JARAXXUS:
    m_uiJaraxxus = pCreature->GetGUID(); break;
    case BOSS_FJOLA:
    m_uiFjola = pCreature->GetGUID(); break;
    case BOSS_EYDIS:
    m_uiEydis = pCreature->GetGUID(); break;
    case BOSS_ANUBARAK:
    m_uiAnubarak = pCreature->GetGUID(); break;
    case NPC_TRALL:
    uiAnnouncerGUID = pCreature->GetGUID();
    if (TeamInInstance == ALLIANCE)
    pCreature->UpdateEntry(NPC_LORD,ALLIANCE);
    break;
    case NPC_LORD:
    uiAnnouncerGUID = pCreature->GetGUID();
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_TRALL,HORDE);
    break;
    case NPC_KRIK:
    m_uiKrik = pCreature->GetGUID(); break;
    case NPC_DK_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_DK_A,ALLIANCE); break;
    case NPC_MAGE_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_MAGE_A,ALLIANCE); break;
    case NPC_DRUID_BALANS_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_DRUID_BALANS_A,ALLIANCE); break;
    case NPC_DRUID_RESTO_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_DRUID_RESTO_A,ALLIANCE); break;
    case NPC_HUNTER_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_HUNTER_A,ALLIANCE); break;
    case NPC_PRIST_DIST_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_PRIST_DIST_A,ALLIANCE); break;
    case NPC_PRIST_SHAD_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_PRIST_SHAD_A,ALLIANCE); break;
    case NPC_PAL_HOLY_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_PAL_HOLY_A,ALLIANCE); break;
    case NPC_PAL_RETRO_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_PAL_RETRO_A,ALLIANCE); break;
    case NPC_ROGUE_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_ROGUE_A,ALLIANCE); break;
    case NPC_SHAM_ENH_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_SHAM_ENH_A,ALLIANCE); break;
    case NPC_SHAM_RESTO_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_SHAM_RESTO_A,ALLIANCE); break;
    case NPC_WARLOCK_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_WARLOCK_A,ALLIANCE); break;
    case NPC_WARRIOR_H:
    if (TeamInInstance == HORDE)
    pCreature->UpdateEntry(NPC_WARRIOR_A,ALLIANCE); break;
        }
    }
    
    void OnGameObjectCreate(GameObject* pGo, bool add)
    {
        switch(pGo->GetEntry())
        {
    case GO_MAIN_DOOR: m_uiMainDoor = pGo->GetGUID(); break;
    case GO_ENTER_DOOR: m_uiEnterDoor = pGo->GetGUID(); break;
    case GO_FLOOR: Floor = add ? pGo : NULL; break;
    case GO_HOLY_CHEST:
    case GO_HOLY_CHEST_H:
        // champions
            case CHAMPIONS_CACHE_NORMAL_10: m_uiChampionGest = add ? pGo : NULL; break;
            case CHAMPIONS_CACHE_NORMAL_25: m_uiChampionGest = add ? pGo : NULL; break;
            case CHAMPIONS_CACHE_HEROIC_10: m_uiChampionGest = add ? pGo : NULL; break;
            case CHAMPIONS_CACHE_HEROIC_25: m_uiChampionGest = add ? pGo : NULL; break;
                // finish
            case ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_10_25_44: m_uiArgentGest = add ? pGo : NULL; break;
            case ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_10_50: m_uiArgentGest = add ? pGo : NULL; break;
            case ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_25_25_44: m_uiArgentGest = add ? pGo : NULL; break;
            case ARGENT_CRUSADE_TRIBUTE_CHEST_HEROIC_25_50: m_uiArgentGest = add ? pGo : NULL; break;
        }
    }
    
    void SetData64(uint32 uiType, uint64 uiData)
    {
    switch (uiType)
    {
    case DATA_MAIN_DOOR:
             uiMainDoor = uiData;
         if (uiMainDoor == 2)
         {
         if (GameObject* pMainDoor = instance->GetGameObject(m_uiMainDoor))
         pMainDoor->SetGoState(GO_STATE_ACTIVE);
         }
         if (uiMainDoor == 1)
         {
         if (GameObject* pMainDoor = instance->GetGameObject(m_uiMainDoor))
         pMainDoor->SetGoState(GO_STATE_READY);
         }
    break;
    case DATA_ENTER_DOOR:
             uiEnterDoor = uiData;
         if (uiEnterDoor == 2)
         {
         if (GameObject* pEnterDoor = instance->GetGameObject(m_uiEnterDoor))
         pEnterDoor->SetGoState(GO_STATE_ACTIVE);
         }
         if (uiEnterDoor == 1)
         {
         if (GameObject* pEnterDoor = instance->GetGameObject(m_uiEnterDoor))
         pEnterDoor->SetGoState(GO_STATE_READY);
         }
    break;

    }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {

    case DATA_BOSS_GORMOK: Encounters[0] = uiData; 
        if (uiData == DONE)
          {
        if (Creature* pAnnounser =  instance->GetCreature(m_uiAnnouncer))
        pAnnounser->AI()->SetData(DATA_START1,0);
          }
        break;
        
    case DATA_BOSS_DREADSCALE: m_uiDreadscale = uiData; 
        if (uiData == DONE)
          {
        if (Creature* pAnnounser =  instance->GetCreature(m_uiAnnouncer))
        pAnnounser->AI()->SetData(DATA_START2,0);
          }
        break;
        
    case DATA_BOSS_ICEHOWL: Encounters[1] = uiData;
        if (uiData == DONE)
          {
        if (Creature* pAnnounser =  instance->GetCreature(m_uiAnnouncer))
        pAnnounser->AI()->SetData(DATA_START3,0);
          }
        break;
        
    case DATA_BOSS_LORD_JARAXXUS: Encounters[2] = uiData; 
        if (uiData == DONE)
          {
        if (Creature* pAnnounser =  instance->GetCreature(m_uiAnnouncer))
        pAnnounser->AI()->SetData(DATA_PHASE2_1,0);
          }
        break;
        
    case TYPE_CHAMPIONS: Encounters[3] = uiData;
        if(uiData == DONE)
        {
            if (m_uiChampionGest)
                m_uiChampionGest->SetRespawnTime(m_uiChampionGest->GetRespawnDelay());

            if (Creature* pAnnouncer =  instance->GetCreature(m_uiAnnouncer))
            {
                pAnnouncer->AI()->SetData(DATA_PHASE4_1,0);
            }
        }
        break;
        
    case TYPE_FJOLA: Encounters[4] = uiData; 
        if (uiData == DONE)
          {
          if (Creature* pAnnounser =  instance->GetCreature(m_uiAnnouncer))
          pAnnounser->AI()->SetData(DATA_PHASE4_1,0);
          }
        break;
        
    case DATA_LIGHT_KING:
        if (uiData == DONE && Floor)
            Floor->TakenDamage(1000000);
        break; 
          
    case DATA_BOSS_ANUBARAK: Encounters[5] = uiData;
        if(uiData == DONE)
        {
            if (m_uiArgentGest)
                m_uiArgentGest->SetRespawnTime(m_uiArgentGest->GetRespawnDelay());
            
            if (Creature* pAnnouncer =  instance->GetCreature(m_uiAnnouncer))
            {
                    pAnnouncer->AI()->SetData(DATA_PHASE5_1,0);
            }
        }
        break;
        }
        if(uiData == DONE)
            SaveToDB();
    }
     uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
        case DATA_BOSS_GORMOK: return Encounters[0];
    case DATA_BOSS_DREADSCALE: return m_uiDreadscale;
    case DATA_BOSS_ICEHOWL: return Encounters[1];
    case DATA_BOSS_LORD_JARAXXUS: return Encounters[2];
    case TYPE_CHAMPIONS : return Encounters[3];
    case TYPE_FJOLA: return Encounters[4];
    case DATA_LIGHT_KING: return uiLightKing;
    case DATA_BOSS_ANUBARAK: return Encounters[5];
    
        }
        return 0;
    }
    
    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
    case DATA_ANOUNSER: return uiAnnouncerGUID;
    case DATA_FLOOR: return uiFloorGUID;
    case DATA_FJOLA: return m_uiFjola;
    case DATA_EYDIS: return m_uiEydis;
    }
    return 0;
    }
    
    
    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
    
        std::ostringstream stream;
    
        stream << Encounters[0] << " " << Encounters[1] << " " << Encounters[2] << " " << Encounters[3] << " " << Encounters[4]  << " " << Encounters[5];
    
    str_data = stream.str();

        OUT_SAVE_INST_DATA_COMPLETE;
        return str_data;
    }
    
    const char* Save()
    {
        return str_data.c_str();
    }
 
    void Load(const char* In)
    {
        if (!In)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }
 
        OUT_LOAD_INST_DATA(In);
 
        std::istringstream loadStream(In);
        loadStream >> Encounters[0] >> Encounters[1] >> Encounters[2] >> Encounters[3] >> Encounters[4] >>Encounters[5];
 
        for (uint8 i = 0; i < ENCOUNTERS; ++i)
                if (Encounters[i] == IN_PROGRESS)
                    Encounters[i] = NOT_STARTED;
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crusader(Map* map)
{
    return new instance_trial_of_the_crusader(map);
}

void AddSC_instance_trial_of_the_crusader()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crusader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crusader;
    newscript->RegisterSelf();
}
