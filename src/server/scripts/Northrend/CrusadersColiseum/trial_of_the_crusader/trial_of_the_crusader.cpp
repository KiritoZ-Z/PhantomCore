/*этот скрипт написан Devil-ом специально для LanCom WOW Server*/

#include "ScriptPCH.h"
#include "def.h"

#define GOSSIP_START_EVENT   "Я готов сразиться с Нордскольскими чудовищами."
#define GOSSIP_START_EVENT1  "Я готов сразиться лордом Джараксусом"
#define GOSSIP_START_EVENT2  "Я готов сразиться с чемпионами"
#define GOSSIP_START_EVENT3  "Я готов сразиться с сестрами"
#define GOSSIP_START_EVENT4  "Я готов идти на Анубарака"

#define SAY_START    -1900512
#define ORIENTATION             4.714
#define SPELL_SOUL_FEAST_ALEX  53677
#define EMOTE_LIGHT_OF_DAWN09        -1609277

const Position SpawnPosition1 = {563.53, 213.56, 395.09, 4.66};

const Position Pos [10] =
{
{581.62, 172.62, 394.63, 4.7},
{594.31, 165.51, 394.62, 4.5},
{598.54, 154.54, 394.64, 4.3},
{601.53, 139.53, 394.61, 4.2},
{606.52, 129.54, 394.62, 4.1},
{564.55, 176.55, 394.63, 3.7},
{551.56, 175.58, 394.68, 3.4},
{534.57, 172.50, 394.67, 3.5},
{524.58, 156.58, 394.65, 3.6},
{521.18, 140.57, 394.64, 3.7}
};

struct  npc_toc6_announcerAI : public ScriptedAI
{
    npc_toc6_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    pInstance = pCreature->GetInstanceData();
    
        uiPhase = 0;
    uiTimer = 0;
    uiSummonTimes = 0;
    m_uiDruid = 0;
    m_uiPriestes = 0;
    m_uiPaladin = 0;
    m_uiShaman = 0;
    chmp1 = 0;
    chmp2 = 0;
    chmp3 = 0;
    chmp4 = 0;
    chmp5 = 0;
    chmp6 = 0;
    chmp7 = 0;
    chmp8 = 0;
    chmp9 = 0;
    chmp10 = 0;
    uichmp1 = 0;
    uichmp2 = 0;
    uichmp3 = 0;
    uichmp4 = 0;
    uichmp5 = 0;
    uichmp6 = 0;
    uichmp7 = 0;
    uichmp8 = 0;
    uichmp9 = 0;
    uichmp10 = 0;

    
    me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    me->GetMotionMaster()->MovePoint(0, 563.589294, 139.645645, 393.836578);
    SetDruid();
    SetPaladin();
    SetPriestes();
    SetShaman();
    SelectChampion();
    }

    ScriptedInstance* pInstance;
    
    uint32 uiPhase;
    uint32 uiPhase2;
    uint32 uiTimer;
    uint32 uiTimer2;
    uint32 uiSummonTimes;
    uint64 uiBoss1;
    uint64 uiBoss;
    uint64 uiLord;
    uint64 uiKrik;
    uint64 uiVarian;
    uint64 uiNepopalis;
    uint64 uichmp1;
    uint64 uichmp2;
    uint64 uichmp3;
    uint64 uichmp4;
    uint64 uichmp5;
    uint64 uichmp6;
    uint64 uichmp7;
    uint64 uichmp8;
    uint64 uichmp9;
    uint64 uichmp10;
    
    uint32 m_uiDruid;
    uint32 m_uiPriestes;
    uint32 m_uiPaladin;
    uint32 m_uiShaman;
    
    uint64 chmp1;
    uint64 chmp2;
    uint64 chmp3;
    uint64 chmp4;
    uint64 chmp5;
    uint64 chmp6;
    uint64 chmp7;
    uint64 chmp8;
    uint64 chmp9;
    uint64 chmp10;
    
    void Reset()
    {
    uiBoss1 = NULL;
    uiLord = NULL;
    uiBoss = NULL;
    uiVarian = NULL;
    uiNepopalis = NULL;
    }

  void NextStep(uint32 uiTimerStep,bool bNextStep = true,uint8 uiPhaseStep = 0)
    {
        uiTimer = uiTimerStep;
        if (bNextStep)
            ++uiPhase;
        else
            uiPhase = uiPhaseStep;
    }
    void NextStep2(uint32 uiTimerStep2,bool bNextStep2 = true,uint8 uiPhaseStep2 = 0)
    {
        uiTimer2 = uiTimerStep2;
        if (bNextStep2)
            ++uiPhase2;
        else
            uiPhase2 = uiPhaseStep2;
    }

  void SetData(uint32 uiType, uint32 uiData)
    {
        switch (uiType)
        {
    // Phase 1
    case DATA_START:
         //DoScriptText(SAY_START,me);
         NextStep(2000,false,1);
         break;
    case DATA_START1:
         NextStep(1000,false,6);
         break;
    case DATA_START2:
         NextStep(1000,false,9);
         break;
    case DATA_START3:
         NextStep(1000,false,14);
         break;
         //Phase 2
    case DATA_PHASE2:    
         NextStep2(100,false,1);
         break;
    case DATA_PHASE2_1:
         NextStep2(1000,false,12);
         break;
         //PHase 3
    case DATA_PHASE3:
         NextStep2(1000,false,13);
         break;
    case DATA_PHASE3_1:
         NextStep2(1000,false,23);
         break;
         //Phase 4
    case DATA_PHASE4:
         NextStep2(1000,false,24);
         break;
    case DATA_PHASE4_1:
         NextStep2(1000,false,27);
         break;
    case DATA_PHASE5:
         NextStep2(1000,false,28);
         break;
    case DATA_PHASE5_1:
         NextStep2(1000,false,36);
         break;
    
    default: return;
    }
    }
    
    void SetDruid()
    {
    uint8 uiDruid = urand(0,1);
    switch(uiDruid)
    {
    case 0:
    m_uiDruid = NPC_DRUID_BALANS_H;
    break;
        
    case 1:
    m_uiDruid = NPC_DRUID_RESTO_H;
    break;
    }
    }
    
    void SetPaladin()
    {
    uint8 uiPaladin = urand(0,1);
    switch(uiPaladin)
    {
    case 0:
    m_uiPaladin = NPC_PAL_HOLY_H;
    break;

    case 1:
    m_uiPaladin = NPC_PAL_RETRO_H;
    break;
    }
    }
    
    void SetPriestes()
    {
    uint8 uiPriestes = urand(0,1);
    switch(uiPriestes)
    {
    case 0:
    m_uiPriestes = NPC_PRIST_DIST_H;
    break;
        
    case 1:
    m_uiPriestes = NPC_PRIST_SHAD_H;
    break;
    }
    }
    void SetShaman()
    {
    uint8 uiShaman = urand(0,1);
    switch(uiShaman)
    {
    case 0:
    m_uiShaman = NPC_SHAM_ENH_H;
    break;
        
    case 1:
    m_uiShaman = NPC_SHAM_RESTO_H;
    break;
    }
    }
    void SelectChampion()
    {
    
    uint8 m_chmp1 = urand(0,9);
    switch (m_chmp1)
    {
    case 0: chmp1 = NPC_DK_H; break; 
    case 1: chmp1 = m_uiDruid; break; 
    case 2: chmp1 = m_uiPriestes; break; 
    case 3: chmp1 = NPC_HUNTER_H; break;  
    case 4: chmp1 = m_uiPaladin; break;  
    case 5: chmp1 = NPC_MAGE_H; break;  
    case 6: chmp1 = NPC_ROGUE_H; break;  
    case 7: chmp1 = m_uiShaman; break;  
    case 8: chmp1 = NPC_WARLOCK_H; break;  
    case 9: chmp1 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp2 = urand(0,9);
    switch (m_chmp2)
    {
    case 0: chmp2 = NPC_DK_H; break;  
    case 1: chmp2 = m_uiDruid; break;  
    case 2: chmp2 = m_uiPriestes; break;  
    case 3: chmp2 = NPC_HUNTER_H; break;  
    case 4: chmp2 = m_uiPaladin; break;  
    case 5: chmp2 = NPC_MAGE_H; break;  
    case 6: chmp2 = NPC_ROGUE_H; break;  
    case 7: chmp2 = m_uiShaman; break;  
    case 8: chmp2 = NPC_WARLOCK_H; break;  
    case 9: chmp2 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp3 = urand(0,9);    
    switch (m_chmp3)
    {
    case 0: chmp3 = NPC_DK_H; break;  
    case 1: chmp3 = m_uiDruid; break;  
    case 2: chmp3 = m_uiPriestes; break;  
    case 3: chmp3 = NPC_HUNTER_H; break;  
    case 4: chmp3 = m_uiPaladin; break;  
    case 5: chmp3 = NPC_MAGE_H; break;  
    case 6: chmp3 = NPC_ROGUE_H; break;  
    case 7: chmp3 = m_uiShaman; break;  
    case 8: chmp3 = NPC_WARLOCK_H; break;  
    case 9: chmp3 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp4 = urand(0,9);   
    switch (m_chmp4)
    {
    case 0: chmp4 = NPC_DK_H; break;  
    case 1: chmp4 = m_uiDruid; break;  
    case 2: chmp4 = m_uiPriestes; break;  
    case 3: chmp4 = NPC_HUNTER_H; break;  
    case 4: chmp4 = m_uiPaladin; break;  
    case 5: chmp4 = NPC_MAGE_H; break;  
    case 6: chmp4 = NPC_ROGUE_H; break;  
    case 7: chmp4 = m_uiShaman; break;  
    case 8: chmp4 = NPC_WARLOCK_H; break;  
    case 9: chmp4 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp5 = urand(0,9);    
    switch (m_chmp5)
    {
    case 0: chmp5 = NPC_DK_H; break;  
    case 1: chmp5 = m_uiDruid; break;  
    case 2: chmp5 = m_uiPriestes; break;  
    case 3: chmp5 = NPC_HUNTER_H; break;  
    case 4: chmp5 = m_uiPaladin; break;  
    case 5: chmp5 = NPC_MAGE_H; break;  
    case 6: chmp5 = NPC_ROGUE_H; break;  
    case 7: chmp5 = m_uiShaman; break;  
    case 8: chmp5 = NPC_WARLOCK_H; break;  
    case 9: chmp5 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp6 = urand(0,9);
    switch (m_chmp6)
    {
    case 0: chmp6 = NPC_DK_H; break;  
    case 1: chmp6 = m_uiDruid; break;  
    case 2: chmp6 = m_uiPriestes; break;  
    case 3: chmp6 = NPC_HUNTER_H; break;  
    case 4: chmp6 = m_uiPaladin; break;  
    case 5: chmp6 = NPC_MAGE_H; break;  
    case 6: chmp6 = NPC_ROGUE_H; break;  
    case 7: chmp6 = m_uiShaman; break;  
    case 8: chmp6 = NPC_WARLOCK_H; break;  
    case 9: chmp6 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp7 = urand(0,9);
    switch (m_chmp7)
    {
    case 0: chmp7 = NPC_DK_H; break;  
    case 1: chmp7 = m_uiDruid; break;  
    case 2: chmp7 = m_uiPriestes; break;  
    case 3: chmp7 = NPC_HUNTER_H; break;  
    case 4: chmp7 = m_uiPaladin; break;  
    case 5: chmp7 = NPC_MAGE_H; break;  
    case 6: chmp7 = NPC_ROGUE_H; break;  
    case 7: chmp7 = m_uiShaman; break;  
    case 8: chmp7 = NPC_WARLOCK_H; break;  
    case 9: chmp7 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp8 = urand(0,9);
    switch (m_chmp8)
    {
    case 0: chmp8 = NPC_DK_H; break;  
    case 1: chmp8 = m_uiDruid; break;  
    case 2: chmp8 = m_uiPriestes; break;  
    case 3: chmp8 = NPC_HUNTER_H; break;  
    case 4: chmp8 = m_uiPaladin; break;  
    case 5: chmp8 = NPC_MAGE_H; break;  
    case 6: chmp8 = NPC_ROGUE_H; break;  
    case 7: chmp8 = m_uiShaman; break;  
    case 8: chmp8 = NPC_WARLOCK_H; break;  
    case 9: chmp8 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp9 = urand(0,9);
    switch (m_chmp9)
    {
    case 0: chmp9 = NPC_DK_H; break;  
    case 1: chmp9 = m_uiDruid; break;  
    case 2: chmp9 = m_uiPriestes; break;  
    case 3: chmp9 = NPC_HUNTER_H; break;  
    case 4: chmp9 = m_uiPaladin; break;  
    case 5: chmp9 = NPC_MAGE_H; break;  
    case 6: chmp9 = NPC_ROGUE_H; break;  
    case 7: chmp9 = m_uiShaman; break;  
    case 8: chmp9 = NPC_WARLOCK_H; break;  
    case 9: chmp9 = NPC_WARRIOR_H; break;
    }
    
    uint8 m_chmp10 = urand(0,9);
    switch (m_chmp10)
    {
    case 0: chmp10 = NPC_DK_H; break;  
    case 1: chmp10 = m_uiDruid; break;  
    case 2: chmp10 = m_uiPriestes; break;  
    case 3: chmp10 = NPC_HUNTER_H; break;  
    case 4: chmp10 = m_uiPaladin; break;  
    case 5: chmp10 = NPC_MAGE_H; break;  
    case 6: chmp10 = NPC_ROGUE_H; break;  
    case 7: chmp10 = m_uiShaman; break;  
    case 8: chmp10 = NPC_WARLOCK_H; break;  
    case 9: chmp10 = NPC_WARRIOR_H; break;
    }
  }

void SummonChampions()
{

   if (Creature* pTemp = me->SummonCreature(chmp1, 581.62,172.62,394.63,4.7, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
     {
    me->AddThreat(pTemp, 0.0f);
    uichmp1 = pTemp->GetGUID();
    AggroAllPlayers(pTemp);
    
     for (uint8 i = 0; i < 1; ++i)
     {
      if (chmp2 != chmp1)
      {
      if (Creature* pTrash = me->SummonCreature(chmp2, 598.54,154.54,394.64,4.3, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp2 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,1);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp3 != chmp1 && chmp3 != chmp2)
    {
    if (Creature* pTrash = me->SummonCreature(chmp3, 594.31,165.51,394.62,4.5, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp3 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,2);
       AggroAllPlayers(pTrash);
       }
    }

    if (chmp4 != chmp1 && chmp4 != chmp2 && chmp4 != chmp3)
    {
    if (Creature* pTrash = me->SummonCreature(chmp4, 601.53, 139.53, 394.61, 4.2, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp4 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,3);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp5 != chmp1 && chmp5 != chmp2 && chmp5 != chmp3 && chmp5 != chmp4)
    {
    if (Creature* pTrash = me->SummonCreature(chmp5, 606.52, 129.54, 394.62, 4.1, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp5 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,4);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp6 != chmp1 && chmp6 != chmp2 && chmp6 != chmp3 && chmp6 != chmp4 && chmp6 != chmp5)
    {
    if (Creature* pTrash = me->SummonCreature(chmp6, 564.55, 176.55, 394.63, 3.7, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp6 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,5);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp7 != chmp1 && chmp7 != chmp2 && chmp7 != chmp3 && chmp7 != chmp4 && chmp7 != chmp5 && chmp7 != chmp6)
    {
    if (Creature* pTrash = me->SummonCreature(chmp7, 551.56, 175.58, 394.68, 3.4, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp7 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,6);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp8 != chmp1 && chmp8 != chmp2 && chmp8 != chmp3 && chmp8 != chmp4 && chmp8 != chmp5 && chmp8 != chmp6 && chmp8 != chmp7)
    {
    if (Creature* pTrash = me->SummonCreature(chmp8, 534.57, 172.50, 394.67, 3.5, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp8 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,7);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp9 != chmp1 && chmp9 != chmp2 && chmp9 != chmp3 && chmp9 != chmp4 && chmp9 != chmp5 && chmp9 != chmp6 && chmp9 != chmp7 && chmp9 != chmp8)
    {
    if (Creature* pTrash = me->SummonCreature(chmp9, 524.58, 156.58, 394.65, 3.6, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp9 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,8);
       AggroAllPlayers(pTrash);
       }
    }
    
    if (chmp10 != chmp1 && chmp10 != chmp2 && chmp10 != chmp3 && chmp10 != chmp4 && chmp10 != chmp5 && chmp10 != chmp6 && chmp10 != chmp7 && chmp10 != chmp8 && chmp10 != chmp9)
    {
    if (Creature* pTrash = me->SummonCreature(chmp10, 521.18, 140.57, 394.64, 3.7, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
       {
       me->AddThreat(pTrash, 0.0f);
       uichmp10 = pTrash->GetGUID();
       pTrash->AI()->SetData(i,9);
       AggroAllPlayers(pTrash);
       }
    }
   }
   }
}


 void AggroAllPlayers(Creature* pTemp)
    {
        Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

        if(PlList.isEmpty())
            return;

        for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if(Player* pPlayer = i->getSource())
            {
                if(pPlayer->isGameMaster())
                    continue;

                if(pPlayer->isAlive())
                {
                    pTemp->SetHomePosition(me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation());
                    pTemp->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                    pTemp->SetReactState(REACT_AGGRESSIVE);
                    pTemp->SetInCombatWith(pPlayer);
                    pPlayer->SetInCombatWith(pTemp);
                    pTemp->AddThreat(pPlayer, 0.0f);
                }
            }
        }
    }
    
  void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;
        if (uiPointId == 1)
        {
            me->SetOrientation(ORIENTATION);
            me->SendMovementFlagUpdate();
        }
    }

  void StartEncounter1()
    {
        if (!pInstance)
            return;
    if (pInstance->GetData(BOSS_GORMOK) == NOT_STARTED)
    me->AI()->SetData(DATA_START,0);
    }
  void StartEncounter2()
    {
        if (!pInstance)
            return;
    me->AI()->SetData(DATA_PHASE2,0);
    }
  void StartEncounter3()
    {
    if (!pInstance)
    return;
    me->AI()->SetData(DATA_PHASE3,0);
    }
  void StartEncounter4()
    {
    if (!pInstance)
    return;
    me->AI()->SetData(DATA_PHASE4,0);
    }
  void StartEncounter5()
    {
    if (!pInstance)
    return;
    me->AI()->SetData(DATA_PHASE5,0);
    }

  void UpdateAI(const uint32 uiDiff)
    {
    ScriptedAI::UpdateAI(uiDiff);
    
    if (uiTimer <= uiDiff)
    {
       switch (uiPhase)
       {
      case 0:
      break;
      case 1:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);
        me->SetVisibility(VISIBILITY_OFF);
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,SpawnPosition1,TEMPSUMMON_TIMED_DESPAWN,40000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        pTemp->GetMotionMaster()->MovePoint(1, 555.93, 146.12, 393.91);
        }
        NextStep(10000,true);
        break;
      case 2:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD1,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep(22000,true);
        break;
      case 3:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_GORMOK,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep(2000,true);
        break;
      case 4:
        if (Creature* pBoss=me->SummonCreature(BOSS_GORMOK,SpawnPosition1,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
        {
        me->AddThreat(pBoss, 0.0f);
        uiBoss = pBoss->GetGUID();
        pBoss->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pBoss->SetReactState(REACT_PASSIVE);
        }
        NextStep(6000,true);
        break;
      case 5:
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss))
        {
        pBoss->GetMotionMaster()->MovePoint(1, 563.589294, 139.645645, 393.836578);
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        }
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);    
        NextStep(0,false);
        break;
      case 6:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,555.93, 146.12, 393.91,4.66,TEMPSUMMON_TIMED_DESPAWN,10000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_LORD_DREADSCALE,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep(4000,true);
        break;
      case 7:
        if (Creature* pBoss=me->SummonCreature(BOSS_DREADSCALE,SpawnPosition1,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
        {
        me->AddThreat(pBoss, 0.0f);
        uiBoss = pBoss->GetGUID();
        pBoss->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pBoss->SetReactState(REACT_PASSIVE);
        }
        NextStep(6000,true);
        break;
      case 8:
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss))
        {
        pBoss->GetMotionMaster()->MovePoint(1, 563.589294, 139.645645, 393.836578);
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        }
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);        
        NextStep(0,false);
        break;
      case 9:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);    
        if (Creature* pTemp=me->SummonCreature(NPC_VARIAN,579.16,180.61,395.13,4.7,TEMPSUMMON_TIMED_DESPAWN,17000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiVarian = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        }
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,555.93, 146.12, 393.91,4.66,TEMPSUMMON_TIMED_DESPAWN,17000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        }
        NextStep(2000,true);
        break;
      case 10:
      if (Creature* pTemp = Unit::GetCreature(*me, uiVarian))
      {
        DoScriptText(SAY_VARIAN,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep(3000,true);
        break;
      case 11:
      if (Creature* pTemp = Unit::GetCreature(*me, uiVarian))
      pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
       if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
       {
        DoScriptText(SAY_LORD_ICEHOWL,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep(4000,true);
        break;
      case 12:
        if (Creature* pBoss=me->SummonCreature(BOSS_ICEHOWL,SpawnPosition1,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000))
        {
        me->AddThreat(pBoss, 0.0f);
        uiBoss = pBoss->GetGUID();
        pBoss->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pBoss->SetReactState(REACT_PASSIVE);
        }
        NextStep(8000,true);
        break;
      case 13:
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss))
        {
        pBoss->GetMotionMaster()->MovePoint(1, 563.589294, 139.645645, 393.836578);
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        }
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);    
        NextStep(0,false);
        break;
      case 14:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);
        if (Creature* pTemp=me->SummonCreature(NPC_LORD, 555.93, 146.12, 393.91, 4.66, TEMPSUMMON_TIMED_DESPAWN, 4000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_LORD_WIN,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep(2000,true);
        break;
      case 15:
        me->SetVisibility(VISIBILITY_ON);
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);    
        NextStep(0,false);
        break;

        
    }
    }else uiTimer -= uiDiff;
    
    if (uiTimer2 <= uiDiff)
    {
       switch (uiPhase2)
       {
       case 1:
        me->SetVisibility(VISIBILITY_OFF);
        NextStep2(1000,true);
        break;
       case 2:
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,555.93, 146.12, 393.91,4.66,TEMPSUMMON_TIMED_DESPAWN,54500))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_LORD_PHASE2,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(8000,true);
        break;
       case 3:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp=me->SummonCreature(NPC_NEPOPALIS,563.58,139.64,393.83,4.7,TEMPSUMMON_TIMED_DESPAWN,45500))
        {
        me->AddThreat(pTemp, 0.0f);
        uiNepopalis = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_NEPOPALIS_S,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(11000,true);
        break;
       case 4: //Start retual
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
        {
        pTemp->GetMotionMaster()->MovePoint(1, 565.589294, 165.645645, 393.836578);
        DoScriptText(SAY_NEPOPALIS_Z,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(5000,true);
        break;
       case 5:
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
        {
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        pTemp->CastSpell(pTemp,33271,false);
        }
        NextStep2(4000,true);
        break;
       case 6:
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
         pTemp->CastSpell(pTemp,33271,false);
        if (Creature* pBoss=me->SummonCreature(BOSS_LORD_JARAXXUS,SpawnPosition1,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000))
        {
        me->AddThreat(pBoss, 0.0f);
        uiBoss1 = pBoss->GetGUID();
        pBoss->SetReactState(REACT_PASSIVE);
        pBoss->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        }
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
        {
        DoScriptText(SAY_NEPOPALIS_A,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(13000,true);
        break;
       case 7:
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss1))
        DoScriptText(SAY_JARAXXUS_1,pBoss);
        NextStep2(6000,true);
        break;
       case 8:
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
        {
        DoScriptText(SAY_NEPOPALIS_G,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(2000,true);
        break;
       case 9:
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiBoss1))
        {
        if (Unit* pNepopalis = Unit::GetCreature(*me, uiNepopalis))
        pTemp->CastSpell(pNepopalis, SPELL_SOUL_FEAST_ALEX, false);
        }
        NextStep2(1500,true);
        break;
       case 10:
        if (Creature* pTemp = Unit::GetCreature(*me, uiNepopalis))
        pTemp->SetVisibility(VISIBILITY_OFF);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_KILL,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(5000,true);
        break;
       case 11:
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss1))
        {
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        pBoss->GetMotionMaster()->MovePoint(1, 563.589294, 139.645645, 393.836578);
        }
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);
        NextStep2(0,false);
        break;
       case 12:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);    
        me->SetVisibility(VISIBILITY_ON);
        NextStep2(0,false);
        break;
       //Phase 3
       case 13:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);    
        me->SetVisibility(VISIBILITY_OFF);
        NextStep2(2000,true);
        break;
       case 14:
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,555.93, 146.12, 393.91,4.66,TEMPSUMMON_TIMED_DESPAWN,86000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_LORD_DEAD,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(16000,true);
        break;
       case 15:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp=me->SummonCreature(NPC_KRIK,563.58,139.64,393.83,4.7,TEMPSUMMON_TIMED_DESPAWN,70000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiKrik = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_KRIK_1,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(13000,true);
        break;
       case 16:
        if (Creature* pTemp = Unit::GetCreature(*me, uiKrik))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp=me->SummonCreature(NPC_VARIAN,534.93, 156.12, 394.91,4.66,TEMPSUMMON_TIMED_DESPAWN,57000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiVarian = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_VARIAN_1,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(8000,true);
        break;
       case 17:
       if (Creature* pTemp = Unit::GetCreature(*me, uiVarian))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_SPOK,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(12000,true);
        break;
       case 18:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_NEXT,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(12000,true);
        break;
       case 19:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiKrik))
        {
        DoScriptText(SAY_KRIK_2,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(16000,true);
        break;
       case 20:
        if (Creature* pTemp = Unit::GetCreature(*me, uiKrik))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_WAR,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(5000,true);
        break;
       case 21:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiKrik))
        {
        DoScriptText(SAY_KRIK_3,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(4000,true);
        break;
       case 22:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);
        SummonChampions();
        NextStep2(0,false);
        break;
       case 23:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);    
        me->SetVisibility(VISIBILITY_ON);
        NextStep2(0,false);
        break;
        // Phase 4
       case 24:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);
        me->SetVisibility(VISIBILITY_OFF);
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,555.93, 146.12, 393.91,4.66,TEMPSUMMON_TIMED_DESPAWN,38000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_LORD_PAB,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(20000,true);
        break;
       case 25:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_VAL,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        if (Creature* pBoss=me->SummonCreature(BOSS_EYDIS,581.62,172.62,394.63,4.7,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000))
        {
        me->AddThreat(pBoss, 0.0f);
        uiBoss = pBoss->GetGUID();
        pBoss->SetReactState(REACT_PASSIVE);
        pBoss->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        }
        if (Creature* pBoss=me->SummonCreature(BOSS_FJOLA,594.31,165.51,394.62,4.5,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 600000))
        {
        me->AddThreat(pBoss, 0.0f);
        uiBoss1 = pBoss->GetGUID();
        pBoss->SetReactState(REACT_PASSIVE);
        pBoss->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        }
        NextStep2(13000,true);
        break;
       case 26:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_GO,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }    
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss1))
        {
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        }
        if (Creature* pBoss = Unit::GetCreature(*me, uiBoss))
        {
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        }
        NextStep2(0,false);
        break;
       case 27:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,2);
        me->SetVisibility(VISIBILITY_ON);
        NextStep2(0,false);
        break;
        //Phase 5
       case 28:
        if (pInstance)
        pInstance->SetData64(DATA_MAIN_DOOR,1);
        me->SetVisibility(VISIBILITY_OFF);
        NextStep2(2000,true);
       case 29:
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,555.93, 146.12, 393.91,4.66,TEMPSUMMON_TIMED_DESPAWN,60000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_LORD_POTER,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(20000,true);
        break;
       case 30:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp=me->SummonCreature(NPC_KING,563.58, 139.64, 393.83, 4.7,TEMPSUMMON_TIMED_DESPAWN,40000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiBoss1 = pTemp->GetGUID();
        pTemp->CastSpell(pTemp,68198,false);
        pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        pTemp->SetReactState(REACT_PASSIVE);
        DoScriptText(SAY_KING_1,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(4000,true);
        break;
       case 31:
        NextStep2(2000,true);
        break;
       case 32:
        if (Creature* pTemp = Unit::GetCreature(*me, uiBoss1))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
        {
        DoScriptText(SAY_LORD_ARTAS,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(7000,true);
        break;
       case 33:
        if (Creature* pTemp = Unit::GetCreature(*me, uiLord))
         pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
        if (Creature* pTemp = Unit::GetCreature(*me, uiBoss1))
        {
        DoScriptText(SAY_KING_2,pTemp);
        pTemp->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
        }
        NextStep2(15000,true);
        break;
       case 34:
        if (Creature* pTemp = Unit::GetCreature(*me, uiBoss1))
        {
        DoScriptText(SAY_KING_3,pTemp);
        pTemp->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
        pTemp->CastSpell(pTemp,69016,false);
        }
        NextStep2(6000,true);
        break;
       case 35:
        if (Creature* pBoss=me->SummonCreature(BOSS_ANUBARAK,790.43,131.75,142.62,3.7))
        {
        pBoss->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        AggroAllPlayers(pBoss);
        }
        if (pInstance)
        pInstance->SetData(DATA_LIGHT_KING,DONE);
        NextStep2(0,false);
        break;
       case 36:
        if (Creature* pTemp=me->SummonCreature(NPC_LORD,810.372,135.736,142.369,3.198,TEMPSUMMON_TIMED_DESPAWN,38000))
        {
        me->AddThreat(pTemp, 0.0f);
        uiLord = pTemp->GetGUID();
            pTemp->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        DoScriptText(SAY_LORD_END,pTemp);
        pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
        }
        NextStep2(0,false);
        break;
       }
    }else uiTimer2 -= uiDiff;
        
        
    if (!UpdateVictim()) return;
   }
};

CreatureAI* GetAI_npc_toc6_announcer(Creature* pCreature)
{
    return new npc_toc6_announcerAI(pCreature);
}

bool GossipHello_npc_toc6_announcer(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = pCreature->GetInstanceData();

    if (pInstance &&
        pInstance->GetData(DATA_BOSS_GORMOK) == DONE &&
        pInstance->GetData(DATA_BOSS_DREADSCALE) == DONE &&
        pInstance->GetData(DATA_BOSS_ICEHOWL) == DONE &&
    pInstance->GetData(DATA_BOSS_LORD_JARAXXUS) == DONE &&
    pInstance->GetData(TYPE_CHAMPIONS) == DONE &&
    pInstance->GetData(TYPE_FJOLA) == DONE &&
        pInstance->GetData(DATA_BOSS_ANUBARAK) == DONE)
        return false;

    if (pInstance && pInstance->GetData(DATA_BOSS_ANUBARAK) == NOT_STARTED)
    {
    if (pInstance && pInstance->GetData(TYPE_FJOLA) == NOT_STARTED)
    {
    if (pInstance && pInstance->GetData(TYPE_CHAMPIONS) == NOT_STARTED)
    {
       if (pInstance && pInstance->GetData(DATA_BOSS_LORD_JARAXXUS) == NOT_STARTED)
       {
         if (pInstance && pInstance->GetData(DATA_BOSS_DREADSCALE) == NOT_STARTED)
        {
        if (pInstance && pInstance->GetData(DATA_BOSS_GORMOK) == NOT_STARTED)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (pInstance && pInstance->GetData(DATA_BOSS_GORMOK) == DONE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        }
        if (pInstance && pInstance->GetData(DATA_BOSS_ICEHOWL) == DONE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
       }
       if (pInstance && pInstance->GetData(DATA_BOSS_LORD_JARAXXUS) == DONE)
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
    }
    if (pInstance && pInstance->GetData(TYPE_CHAMPIONS) == DONE)
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
    }
    if (pInstance && pInstance->GetData(TYPE_FJOLA) == DONE)
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
    }
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_toc6_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
    CAST_AI(npc_toc6_announcerAI, pCreature->AI())->StartEncounter1();
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
    CAST_AI(npc_toc6_announcerAI, pCreature->AI())->StartEncounter2();
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+3)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
    CAST_AI(npc_toc6_announcerAI, pCreature->AI())->StartEncounter3();
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+4)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
    CAST_AI(npc_toc6_announcerAI, pCreature->AI())->StartEncounter4();
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+5)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
    CAST_AI(npc_toc6_announcerAI, pCreature->AI())->StartEncounter5();
    }
    return true;
}
void AddSC_npc_toc6_announcer()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc6_announcer";
    NewScript->GetAI = &GetAI_npc_toc6_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc6_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc6_announcer;
    NewScript->RegisterSelf();
}