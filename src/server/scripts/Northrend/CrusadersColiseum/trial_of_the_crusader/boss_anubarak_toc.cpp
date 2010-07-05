/* Gcore File
SDCategory:trial_of_the_crusader
EndScriptData */
 
#include "ScriptPCH.h"
#include "def.h"
enum eSummons
{
    NPC_FROST_SPHERE     = 34606,
    NPC_BURROWER         = 34607,
    NPC_SCARAB           = 34605,
};

enum eSays
{
   SAY_AGGRO        = -1900554,
   SAY_DEATH        = -1900564,
   SAY_MOGILA        = -1900555,
   SAY_SUMMON        = -1900556,
   SAY_STAYA        = -1900560,
};

enum eSpells
{
   SPELL_COLD           = 66013,
   SPELL_MARK           = 67574,
   SPELL_LEECHING_SWARM = 66118,
   SPELL_LEECHING_HEAL  = 66125,
   SPELL_IMPALE         = 65922,
   SPELL_POUND          = 66012,
   SPELL_SHOUT          = 67730,
   SPELL_SUBMERGE_A     = 53421,
   SPELL_SUBMERGE_M     = 67322,
   SPELL_SUMMON_BEATLES = 66339,
   SPELL_DETERMINATION  = 66092,
   SPELL_ACID_MANDIBLE  = 67861,
   SPELL_SPIDER_FRENZY  = 66129,
   SPELL_EXPOSE_WEAKNESS = 67847,
   SPELL_BERSERK        = 26662,
};

const Position Posit [4] =
{
{731.482, 80.675,142.190,0},
{727.973, 189.955, 142.119,0},
{686.757, 97.445,142.119,0},
{692.077, 184.421, 142.119,0}
};

struct  boss_anub_arak_crusaderAI : public ScriptedAI
{
    boss_anub_arak_crusaderAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }
 
    ScriptedInstance* pInstance;
    bool Event75;
    bool Event50;
    bool Event25;
    bool Event;
 
    // timers
    uint32 SPELL_POUND_Timer;
    uint32 SPELL_COLD_Timer;
    uint32 SUMMON_BORROWER_Timer;
    uint32 SPELL_SUBMERGE_Timer;
    uint32 SPELL_IMPALE_Timer;
    uint32 SUMMON_SCARAB_Timer;
    uint32 Phase_Timer;
    uint32 SPELL_LEECHING_HEAL_Timer;
    uint32 SPELL_LEECHING_SWARM_Timer;
    uint32 SUMMON_FROSTSPHERE_Timer;
    uint32 SPELL_BERSERK_Timer;
    // end timers
    uint32 Phase;
 
    void Reset()
    {
        Event75 = false;
    Event50 = false;
    Event25 = false;
    Event = false;
    SPELL_POUND_Timer = 10000;
    SPELL_COLD_Timer = 15000;
    SUMMON_BORROWER_Timer = 20000;
    SPELL_IMPALE_Timer = 5000;
    SUMMON_SCARAB_Timer = 8000;
    Phase_Timer = 0;
    SPELL_LEECHING_HEAL_Timer = 25000;
    SPELL_LEECHING_SWARM_Timer = 20000;
    SUMMON_FROSTSPHERE_Timer =20000 ;
    SPELL_BERSERK_Timer = 300000;
    Phase = 3;
    me->GetMotionMaster()->MovePoint(0, 788.904, 134.287, 142.625);
    me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
    me->SetReactState(REACT_AGGRESSIVE);
    me->RemoveAurasDueToSpell(SPELL_SUBMERGE_A);
    }
 
    void Aggro(Unit* pWho)
    {
        Phase = 3;
    Event75 = true;
    Event50 = false;
    Event25 = false;
    Event = false;
        DoScriptText(SAY_AGGRO,me);
        if(pInstance)
            pInstance->SetData(DATA_BOSS_ANUBARAK, IN_PROGRESS);
    }
 
    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
        if (pInstance)
            pInstance->SetData(DATA_BOSS_ANUBARAK, DONE);
    }
 
    void JustSummoned(Creature* mob)
    {
        mob->AddThreat(me->getVictim(), 0);
    }
 
    void UpdateAI(const uint32 diff)
    {
    if (!UpdateVictim())
        return;
    
     if (SUMMON_FROSTSPHERE_Timer <= diff)
     {
        me->SummonCreature(NPC_FROST_SPHERE, 750.183, 172.536, 164.498, 0, TEMPSUMMON_TIMED_DESPAWN, 40000);
        me->SummonCreature(NPC_FROST_SPHERE, 758.665, 142.486, 168.611, 0, TEMPSUMMON_TIMED_DESPAWN, 40000);
        me->SummonCreature(NPC_FROST_SPHERE, 763.031, 103.666, 172.110, 0, TEMPSUMMON_TIMED_DESPAWN, 40000);
        SUMMON_FROSTSPHERE_Timer = 20000;
     } else SUMMON_FROSTSPHERE_Timer -= diff;
     
     float health = me->GetHealth()*100 / me->GetMaxHealth();
     
     if (health <= 75)
     {
          Phase = 0;
          Phase_Timer = 1000;
          Event = true;
     }
     if (Event50 && health <= 50)
     {
          Phase = 0;
          Event = true;
          Phase_Timer = 1000;
          Event50 = false;
     }
     if (Event25 && health <= 25)
     {
          Phase = 0;
          Event = true;
          Phase_Timer = 1000;
          Event25 = false;
     }
     if (health <= 20)
         Phase = 4;

       switch(Phase)
         {
        case 3:

           if (SPELL_POUND_Timer <= diff)
           {
              DoCast(me->getVictim(),SPELL_POUND);
          SPELL_POUND_Timer = 10000;
           } else SPELL_POUND_Timer -= diff;
                    
           if (SPELL_COLD_Timer <= diff)
           {
              DoCast(me->getVictim(),SPELL_COLD);
          SPELL_COLD_Timer = 15000;
           } else SPELL_COLD_Timer -= diff;

           if (SUMMON_BORROWER_Timer <= diff)
           {
              DoScriptText(SAY_STAYA, me);
          if (Creature *pBoss=me->SummonCreature(NPC_BURROWER, Posit[2], TEMPSUMMON_CORPSE_DESPAWN, 20000))
          {
             if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                 pBoss->AI()->AttackStart(pTarget);
          }
          
          if (Creature *pBoss=me->SummonCreature(NPC_BURROWER, Posit[0], TEMPSUMMON_CORPSE_DESPAWN, 20000))
          {
                 if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                pBoss->AI()->AttackStart(pTarget);
          }
           SUMMON_BORROWER_Timer = 45000;
         } else SUMMON_BORROWER_Timer -= diff;
     break;
        
     case 4: //10%
        if (SPELL_POUND_Timer <= diff)
        {
           DoCast(me->getVictim(),SPELL_POUND);
           SPELL_POUND_Timer = 10000;
        } else SPELL_POUND_Timer -= diff;

        if (SPELL_COLD_Timer <= diff)
        {
           DoCast(me->getVictim(),SPELL_COLD);
           SPELL_COLD_Timer = 15000;
        } else SPELL_COLD_Timer -= diff;

        if (SPELL_LEECHING_SWARM_Timer <= diff)
        {
           DoCast(me->getVictim(),SPELL_LEECHING_SWARM);
           SPELL_LEECHING_SWARM_Timer = 20000;
        } else SPELL_LEECHING_SWARM_Timer -= diff;

        if (SPELL_LEECHING_HEAL_Timer <= diff)
        {
           DoCast(me, SPELL_LEECHING_HEAL);
           SPELL_LEECHING_HEAL_Timer = 25000;
        } else SPELL_LEECHING_HEAL_Timer -= diff;
     break;
       }

    if (Event)
    {
       if (SUMMON_SCARAB_Timer <= diff)
        {
      DoScriptText(SAY_STAYA, me);
         for (int i=0; i < 4; i++)
         {
           if (Creature *pBoss=me->SummonCreature(NPC_SCARAB, Posit[RAND(0,1,2,3)], TEMPSUMMON_CORPSE_DESPAWN, 50000))
           {
              if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
             pBoss->AI()->AttackStart(pTarget);
           }
         }
        SUMMON_SCARAB_Timer = urand(8000,10000);
        } else SUMMON_SCARAB_Timer -= diff;

    if (Phase_Timer <= diff)
    {
       switch(Phase)
        {
          case 0:
            DoCast(me, SPELL_SUBMERGE_A);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetSpeed(MOVE_RUN, 0.5f, true);
        //DoScriptText(SAY_SUMMON, me);
        Phase_Timer = 2000;
        Phase++;
          break;
          
          case 1:
            if (SPELL_IMPALE_Timer <= diff)
        {
          if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
          {
            if (pTarget->GetTypeId() == TYPEID_PLAYER) 
             me->CastSpell(pTarget, SPELL_MARK,true);
             me->GetMotionMaster()->MovePoint(0, pTarget->GetPositionX(),pTarget->GetPositionY(),pTarget->GetPositionZ());
             DoCastAOE(SPELL_IMPALE);
          }
          SPELL_IMPALE_Timer = 5000;
        } else SPELL_IMPALE_Timer -= diff;
        Phase_Timer = 60000;
        Phase++;
          break;
                
          case 2: 
             me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
         me->RemoveAura(SPELL_SUBMERGE_A);
         Event50 = true;
         if (health <= 50)
            Event25 = true;
         Phase_Timer = 0;
         Phase = 3;
          break;
          
       }
    }else Phase_Timer -= diff;
    }
    
    if (SPELL_BERSERK_Timer <= diff)
    {
       DoCast(me->getVictim(),SPELL_BERSERK);
       SPELL_BERSERK_Timer = 300000;
    } else SPELL_BERSERK_Timer -= diff;
    
    DoMeleeAttackIfReady();
  }
};
 
struct  npc_borrowerAI : public ScriptedAI
{
    npc_borrowerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
       pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }
 
      ScriptedInstance* pInstance;
 
      bool mobSubmerged;
      bool HeroicMode;
 
      uint32 ShadowStrike_Timer;
      uint32 Submerge_Timer;
      uint32 Emerge_Timer;
 
    void Reset()
    {
      ShadowStrike_Timer = 5500;
      Submerge_Timer = 35000;
      Emerge_Timer = 10000;
      mobSubmerged = false;
      me->RemoveAurasDueToSpell(SPELL_SUBMERGE_M);
    }
 
    void UpdateAI(const uint32 diff)
    {
    if (!UpdateVictim())
        return;
 
    Unit* pTarget = me->getVictim();
 
    if (!mobSubmerged && ShadowStrike_Timer <= diff)
    {
      DoCast(pTarget, SPELL_EXPOSE_WEAKNESS);
      ShadowStrike_Timer = 5500;
    } else ShadowStrike_Timer -= diff;
 
    if (!mobSubmerged && Submerge_Timer <= diff)
    {
      me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
      me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
      DoCast(me, SPELL_SUBMERGE_M, true);
      Submerge_Timer = 35000;
      mobSubmerged = true;
    } else Submerge_Timer -= diff;
 
    if (mobSubmerged && Emerge_Timer <= diff)
    {
      me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
      me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
      me->RemoveAura(SPELL_SUBMERGE_M);
      Emerge_Timer = 10000;
      mobSubmerged = false;
    } else Emerge_Timer -= diff;
 
       if (!mobSubmerged)
    DoMeleeAttackIfReady();
    }
};
 
CreatureAI* GetAI_boss_anub_arak_crusader(Creature* pCreature)
{
    return new boss_anub_arak_crusaderAI(pCreature);
}
 
CreatureAI* GetAI_npc_borrower(Creature* pCreature)
{
    return new npc_borrowerAI(pCreature);
}
 
void AddSC_boss_anub_arak_crusader()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_anub_arak_crusader";
    newscript->GetAI = &GetAI_boss_anub_arak_crusader;
    newscript->RegisterSelf();
 
newscript = new Script;
    newscript->Name = "npc_borrower";
    newscript->GetAI = &GetAI_npc_borrower;
    newscript->RegisterSelf();
}