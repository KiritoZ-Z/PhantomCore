#include "ScriptPCH.h"
#include "def.h"

// Spell

#define SPELL_BUFF        67108
#define SPELL_FEL_FIREBALL    RAID_MODE(66532,66963)
#define SPELL_FEL_LIGHTING    RAID_MODE(66528,67029)
#define SPELL_INCINERATE_FLESH    RAID_MODE(66237,67049)
#define SPELL_LEGION_FLAME    RAID_MODE(68124,68126)
#define SPELL_INFERNAL        66258
#define SPELL_NETHER        RAID_MODE(66264,68405)
#define SPELL_WRATA        RAID_MODE(66269,67900)            
//Say
#define SAY_START    -1900517
#define SAY_PLAMYA    -1900518
#define SAY_VRATA    -1900519
#define SAY_VULKAN    -1900520
#define SAY_SESTRA    -1900521
#define SAY_PODVERG    -1900522
#define SAY_PLOD    -1900523
#define SAY_INFERNAL    -1900524
#define SAY_DEAD    -1900525



struct  boss_lord_jaraksusAI : public BossAI
{
    boss_lord_jaraksusAI(Creature *pCreature) : BossAI(pCreature, DATA_BOSS_LORD_JARAXXUS)
    {
        pInstance = pCreature->GetInstanceData();
    }

    uint32 BUFF_Timer;

    uint32 FEL_FIREBALL_Timer;
    uint32 FEL_LIGHTING_Timer;
    uint32 INCINERATE_FLESH_Timer;


    uint32 LEGION_FLAME_Timer;
    uint32 INFERNAL_Timer;

    uint32 NETHER_Timer;

    ScriptedInstance* pInstance;

    void Reset()
    {
        me->GetMotionMaster()->MovePoint(0, 563.771, 172.368, 394.315);
    BUFF_Timer = urand(1000,2000);

    FEL_FIREBALL_Timer = urand(5000,11000);
    FEL_LIGHTING_Timer = 13000;
    INCINERATE_FLESH_Timer = 25000;


    LEGION_FLAME_Timer = 25000;
    INFERNAL_Timer = 28000;

    NETHER_Timer = 40000;
    if (pInstance)
            pInstance->SetData(DATA_BOSS_LORD_JARAXXUS, NOT_STARTED);
    }

    void EnterCombat(Unit *who)
    {
        DoZoneInCombat();

        if (pInstance)
            pInstance->SetData(DATA_BOSS_LORD_JARAXXUS, IN_PROGRESS);
        DoScriptText(SAY_START, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

                        if (FEL_FIREBALL_Timer < diff)
                        {
        Unit *target = SelectUnit(SELECT_TARGET_TOPAGGRO, 1);
        if(target && target->isAlive())
    {
        DoCast(target, SPELL_FEL_FIREBALL);
    }
                            FEL_FIREBALL_Timer = urand(15000,25000);
                        } else FEL_FIREBALL_Timer -= diff;

    if (FEL_LIGHTING_Timer < diff)
    {
    Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0);
    if(target && target->isAlive())
    {
    DoCast(target, SPELL_FEL_LIGHTING);
    }
    FEL_LIGHTING_Timer = urand(5000,11000);
    } else FEL_LIGHTING_Timer -= diff;

    if (INCINERATE_FLESH_Timer < diff)
    {
    Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0);
    if(target && target->isAlive())
    {
    DoScriptText(SAY_PLOD, me);
    DoCast(target, SPELL_INCINERATE_FLESH);
    }
    INCINERATE_FLESH_Timer = urand(15000,28000);
    } else INCINERATE_FLESH_Timer -= diff;

    if (LEGION_FLAME_Timer < diff)
    {
    Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0);
    if(target && target->isAlive())
    {
    DoScriptText(SAY_PLAMYA, me);
    DoCast(target, SPELL_LEGION_FLAME);
    }
    LEGION_FLAME_Timer = 28000;
    } else LEGION_FLAME_Timer -= diff;
 /*
    if (INFERNAL_Timer < diff)
    {
    DoScriptText(SAY_INFERNAL, me);
    DoCast(me,SPELL_INFERNAL);
    INFERNAL_Timer = 60000;
    } else INFERNAL_Timer -= diff;*/

    if (NETHER_Timer < diff)
    {
    DoScriptText(SAY_SESTRA, me);
    DoCast(me, SPELL_NETHER);
    NETHER_Timer = 100000;
    } else NETHER_Timer -= diff;

    if (BUFF_Timer < diff)
    {
    DoCast(me,SPELL_BUFF);
    BUFF_Timer = 60000;
    } else BUFF_Timer -= diff;

    DoMeleeAttackIfReady();
    }

        void JustDied(Unit* pKiller)
        {
        if (pInstance)
            pInstance->SetData(DATA_BOSS_LORD_JARAXXUS, DONE);
        DoScriptText(SAY_DEAD, me);
        }

void KilledUnit(Unit *victim){}
};

CreatureAI* GetAI_boss_lord_jaraksus(Creature* pCreature)
{
    return new boss_lord_jaraksusAI (pCreature);
}
void AddSC_boss_lord_jaraksus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lord_jaraksus";
    newscript->GetAI = &GetAI_boss_lord_jaraksus;
    newscript->RegisterSelf();
}
