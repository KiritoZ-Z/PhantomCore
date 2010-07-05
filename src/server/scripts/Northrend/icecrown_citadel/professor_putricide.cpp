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

enum Yells
{
    SAY_AGGRO        =    -1666025,
    SAY_AIRLOCK      =    -1666026,    
    SAY_PHASE_HC     =    -1666027,
    SAY_TRANSFORM_1  =    -1666028,
    SAY_TRANSFORM_2  =    -1666029,
    SAY_KILL_1       =    -1666030,
    SAY_KILL_2       =    -1666031,
    SAY_BERSERK      =    -1666032,
    SAY_DEATH        =    -1666033,
};

enum ProfessorSpells
{
    SPELL_UNSTABLE_EXPERIMENT      =    71968,
    SPELL_TEAR_GAS                 =    71617,
    SPELL_CREATE_CONCOTION         =    71621,
    SPELL_GUZZLE_POTIONS           =    73122,
    SPELL_MALLEABLE_GOO_10_NORMAL  =    72296,
    SPELL_MALLEABLE_GOO_25_NORMAL  =    70852,
    SPELL_MUTATED_STRENGTH         =    71603,
    SPELL_MUTATED_PLAGUE           =    72672,
    SPELL_OOZE_ERUPTION            =    70492,
    SPELL_OOZE_ADHESIV             =    70447,
    SPELL_CHOKING_GAS              =    71278,
	SPELL_SLIME_PUDDLE             =    70341,
    SPELL_CHOKING_GAS_EXPLOSION    =    71279,
	SPELL_CHOKING_GAS_BOMB         =    71255,
};

enum Summons
{
    SUMMON_GASCLOUD         =    37562,
    SUMMON_VOLATILE_OOZE    =    37697,
};

#define EMOTE_UNSTABLE_EXPERIMENT "Professor Putricide begins unstable experiment!"

struct boss_professor_putricideAI : public ScriptedAI
{
    boss_professor_putricideAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiUnstableExperimentTimer;
    uint32 m_uiAddSpawnTimer;

    void Reset()
    {
        m_uiPhase = 1;
        m_uiUnstableExperimentTimer = 10000;
        m_uiAddSpawnTimer = 60000;

        if (m_pInstance)
            m_pInstance->SetData(DATA_PROFESSOR_PUTRICIDE_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);

        if (m_pInstance)
            m_pInstance->SetData(DATA_PROFESSOR_PUTRICIDE_EVENT, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {  
        DoScriptText(SAY_DEATH, me);

        if (m_pInstance)
            m_pInstance->SetData(DATA_PROFESSOR_PUTRICIDE_EVENT, DONE);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_KILL_1,SAY_KILL_2), me);
    }

    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(DATA_PROFESSOR_PUTRICIDE_EVENT, FAIL);  
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
        pSummoned->AI()->AttackStart(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (m_uiUnstableExperimentTimer < uiDiff)
        {
            DoCast(me, SPELL_UNSTABLE_EXPERIMENT);
            me->MonsterTextEmote(EMOTE_UNSTABLE_EXPERIMENT,NULL);
            m_uiUnstableExperimentTimer = 40000;
            m_uiAddSpawnTimer = 5000;
        } else m_uiUnstableExperimentTimer -= uiDiff;

        if (m_uiAddSpawnTimer < uiDiff)
        {
            me->SummonCreature(SUMMON_VOLATILE_OOZE, me->GetPositionX()+20, me->GetPositionY()+20, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 9999999);
            m_uiAddSpawnTimer = 60000;
        } else m_uiAddSpawnTimer -= uiDiff;

        if (m_uiPhase == 1)
        {
			// ToDO
        }

        if (m_uiPhase == 2)
        {
			// ToDO
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_volatile_oozeAI : public ScriptedAI
{
    npc_volatile_oozeAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_OOZE_ADHESIV, true);
    }

    uint64 TargetGUID;

    uint32 OozeAdhesivTimer;
    uint32 OozeExplosion;
    uint32 MovechaseTimer;

    void EnterCombat(Unit *who) 
    {
        DoZoneInCombat();
    }

    void Reset()
    {
        TargetGUID = 0;
        OozeAdhesivTimer = 1000;
        OozeExplosion = 1000;
        MovechaseTimer = 999999;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) 
            return;

        if (!me->HasAura(SPELL_OOZE_ADHESIV))
        {
            if (OozeAdhesivTimer < diff)
            {
                uint32 count = RAID_MODE(1,3,1,3);
                for (uint8 i = 1; i <= count; i++)
                {
                    DoCast(me, SPELL_OOZE_ADHESIV);
                    OozeAdhesivTimer = 999999;
                    MovechaseTimer = 10000;
                }
            } else OozeAdhesivTimer -= diff;
        }

        if (MovechaseTimer < diff)
        {
            MovechaseTimer = 2000;
        }
        else MovechaseTimer -= diff;


        if (OozeAdhesivTimer < diff)
        {
            if (me->IsWithinDistInMap(me, 3))
                DoCast(me, SPELL_OOZE_ERUPTION);
            OozeAdhesivTimer = 10000;
        } else OozeAdhesivTimer -= diff;
    }
};

CreatureAI* GetAI_npc_volatile_ooze(Creature* pCreature)
{
    return new npc_volatile_oozeAI(pCreature);
}

CreatureAI* GetAI_boss_professor_putricide(Creature* pCreature)
{
    return new boss_professor_putricideAI(pCreature);
}

void AddSC_boss_professor_putricide()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_professor_putricide";
    newscript->GetAI = &GetAI_boss_professor_putricide;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_volatile_ooze";
    newscript->GetAI = &GetAI_npc_volatile_ooze;
    newscript->RegisterSelf();
}
