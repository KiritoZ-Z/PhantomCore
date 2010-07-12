#include "ScriptPCH.h"
#include "def.h"

enum Spells
{
SPELL_FEROICIOUS_BUTT    = 66770, //бодание
SPELL_FEROICIOUS_BUTT_H = 67656,
SPELL_MASSIVE_CRASH    = 66683,
SPELL_MASSIVE_CRASH_H    = 67662, //сокрушение
SPELL_TRAMBLE        = 66734, //топот
SPELL_WHIRL        = 67345, //вращение
SPELL_WHIRL_H        = 67665,
SPELL_BERSERK        = 26662,
SPELL_ARCTIC_BREATH    = 66689
};
#define SAY_AGGRO -1900506
#define SAY_DEATH -1900507
#define SAY_MASSIVE -1900508

struct  boss_icehowlAI : public ScriptedAI
{
    boss_icehowlAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        HeroicMode = pCreature->GetMap()->IsHeroic();
        Reset();
    }
 
    ScriptedInstance* pInstance;
 
    bool HeroicMode;

   uint32 Feroicious_butt_Timer;
   uint32 Massive_crash_Timer;
   uint32 Tramble_Timer;
   uint32 Whirl_Timer;
   uint32 ArcticBreachTimer;
   uint32 Berserk_Timer;
   bool Damage;
   bool Razgon;
   uint32 Step;
   uint32 StepTimer;
   
   void Reset()
    {
      me->GetMotionMaster()->MovePoint(0, 563.771, 172.368, 394.315);
    ArcticBreachTimer = 35000;
    Feroicious_butt_Timer = 40000;
    Massive_crash_Timer = 30000;
    Tramble_Timer    = 18000;
    Whirl_Timer        = 14000;
    Berserk_Timer    = 600000;
    Step = 0;
    StepTimer = 100;
    Damage = false;
    Razgon = false;
    }
    
    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO,me);
        if(pInstance)
            pInstance->SetData(DATA_BOSS_ICEHOWL, IN_PROGRESS);
    }
    
    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, me);
    if (pInstance)
            pInstance->SetData(DATA_BOSS_ICEHOWL, DONE);
    }

    void AttackStart(Unit* Who)
    {
       if(!Who) return;
  
       if(Razgon == true) return;

       ScriptedAI::AttackStart(Who);
    }

    void MoveInLineOfSight(Unit* who)
    {
         if(who->GetTypeId() == TYPEID_PLAYER && Damage == true && me->GetDistance2d(who) <= 5)
            me->DealDamage(who, who->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        // ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(const uint32 diff)
    {
    if (!UpdateVictim() && Razgon == false)
        return;

    if(Razgon != true)
    {    
    if (Tramble_Timer < diff)
    {
    DoCast(me, SPELL_TRAMBLE);
    Tramble_Timer = 12000;
    }
    else
    Tramble_Timer -=diff;
    
    if (Feroicious_butt_Timer < diff)
    {
    DoCast(me, HeroicMode ? SPELL_FEROICIOUS_BUTT_H : SPELL_FEROICIOUS_BUTT);
    Feroicious_butt_Timer = 40000;
    }
    else
    Feroicious_butt_Timer -=diff;
    
    if (Massive_crash_Timer < diff)
    {
    DoScriptText(SAY_MASSIVE, me);
      Razgon = true;
    DoCast(me, HeroicMode ? SPELL_MASSIVE_CRASH_H : SPELL_MASSIVE_CRASH);
    Massive_crash_Timer = 30000;
    }
    else
    Massive_crash_Timer -=diff;
    
    if (Whirl_Timer < diff)
    {
    DoCast(me, HeroicMode ? SPELL_WHIRL_H : SPELL_WHIRL);
    Whirl_Timer = 30000;
    }
    else
    Whirl_Timer -=diff;

        if (ArcticBreachTimer < diff)
        {
        DoCast(me, SPELL_ARCTIC_BREATH);
        ArcticBreachTimer = 35000;
        }
        else
        ArcticBreachTimer -=diff;
     
    if (Berserk_Timer < diff)
    {
    DoCast(me, SPELL_BERSERK);
    Berserk_Timer = 300000;
    }
    else
    Berserk_Timer -=diff;
    if ((me->GetHealth()*100 / me->GetMaxHealth()) < 10)
    {
    me->SummonCreature(0, 604.996, 150.775, 138.629, 3.36);
    }
    }
    
  if(Razgon == true)
  {
  if (StepTimer < diff)
     {
     
     switch(Step)
         {
           case 0:
              me->AttackStop();
              me->SetSpeed(MOVE_RUN, 2.0f, true);
              me->GetMotionMaster()->MovePoint(0, 563.805, 139.827, 393.836);
              StepTimer = 7000;
              Step++;
              break;
           case 1:
              if (Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 0))
                 me->GetMotionMaster()->MovePoint(0, pUnit->GetPositionX(), pUnit->GetPositionY(), pUnit->GetPositionZ());
              Damage = true;
              StepTimer = 5000;
              Step++;
              break;
           case 2:
              Damage = false;
              DoScriptText(SAY_MASSIVE, me);
              DoCast(me, HeroicMode ? SPELL_FEROICIOUS_BUTT_H : SPELL_FEROICIOUS_BUTT);
              me->SetSpeed(MOVE_RUN, 1.0f, true);
              StepTimer = 2000;
              Step++;
              break;
           case 3:
              Razgon = false;
              Step = 0;
              StepTimer = 100;
              break;
           }
       } else StepTimer -= diff;
  }

    DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icehowl(Creature* pCreature)
{
    return new boss_icehowlAI(pCreature);
}
 
void AddSC_boss_icehowl()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_icehowl";
    newscript->GetAI = &GetAI_boss_icehowl;
    newscript->RegisterSelf();
}
