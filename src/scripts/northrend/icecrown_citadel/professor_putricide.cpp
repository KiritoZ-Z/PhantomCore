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
#include "ScriptedPch.h"
#include "icecrown_citadel.h"

enum Yells
{
	SAY_AGGRO		=	-1666025,
	SAY_AIRLOCK		=	-1666026,	
	SAY_PHASE_HC	=	-1666027,
	SAY_TRANSFORM_1	=	-1666028,
	SAY_TRANSFORM_2	=	-1666029,
	SAY_KILL_1		=	-1666030,
	SAY_KILL_2		=	-1666031,
	SAY_BERSERK		=	-1666032,
	SAY_DEATH		=	-1666033,
};

enum Spells
{
	// Professor All Phase Spells
	SPELL_UNSTABLE_EXPERIMENT			=	71968,

	// Phase Change Spells
	N_SPELL_TEAR_GAS					=	71617,	// Phasenchange Spell
	N_SPELL_CREATE_CONCOTION			=	71621,	// Phase 1 to 2.
	N_SPELL_GUZZLE_POTIONS				=	73122,	// Phase 2 to 3.

	// 80% - 35%
	N_10_SPELL_MALLEABLE_GOO			=	72296,
	N_25_SPELL_MALLEABLE_GOO			=	70852,

	// 35% - 0%
	N_10_SPELL_MUTATED_STRENGTH			=	71603,
	N_10_MUTATED_PLAGUE					=	72672,

	// Ooze Spells
	N_10_SPELL_OOZE_ERUPTION			=	70492,
	N_10_SPELL_OOZE_ADHESIV				=	70447,
	// Gas Cloude Spells
	// Choking Gas Spells
	N_10_SPELL_CHOKING_GAS				=	71278, // Wenn Gas Spawnt.
	N_10_SPELL_CHOKING_GAS_EXPLOSION	=	71279, // 20 Sek.
};

enum Summons
{
	SUMMON_GASCLOUD			=	37562,
	SUMMON_VOLATILE_OOZE	=	37697,
};

enum Achievements
{
};

#define EMOTE_UNSTABLE_EXPERIMENT "Professor Seuchenmord beginnt Unstabiles Experiment zu wirken!"

struct Boss_ProfessorPutricideAI : public ScriptedAI
{
    Boss_ProfessorPutricideAI(Creature *pCreature) : ScriptedAI(pCreature)
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

	void JustDied(Unit* killer)
    {  
		DoScriptText(SAY_DEATH, me);

		if (m_pInstance)
            m_pInstance->SetData(DATA_PROFESSOR_PUTRICIDE_EVENT, DONE);
    }

	void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_KILL_1,SAY_KILL_2), me);
    }

	void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
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
        }
		else m_uiUnstableExperimentTimer -= uiDiff;

		if (m_uiAddSpawnTimer < uiDiff)
        {
			me->SummonCreature(SUMMON_VOLATILE_OOZE, me->GetPositionX()+20, me->GetPositionY()+20, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 9999999);
			m_uiAddSpawnTimer = 60000;
        }
		else m_uiAddSpawnTimer -= uiDiff;

		if (m_uiPhase == 1)
        {
		}

		if (m_uiPhase == 2)
        {
		}
			DoMeleeAttackIfReady();
		}
};

struct VolatileOozeAI : public ScriptedAI
{
    VolatileOozeAI(Creature *pCreature) : ScriptedAI(pCreature) 
	{
		me->ApplySpellImmune(0, IMMUNITY_ID, N_10_SPELL_OOZE_ADHESIV, true);
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
		OozeAdhesivTimer	= 1000;
		OozeExplosion		= 1000;
		MovechaseTimer		= 999999;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) 
			return;

        if (me->getVictim()->GetTypeId() != TYPEID_PLAYER) 
			return; // Only cast the below on players.

        if (!me->getVictim()->HasAura(N_10_SPELL_OOZE_ADHESIV))
        {
			if (OozeAdhesivTimer < diff)
			{
				uint32 count = RAID_MODE(1,1,1,1); // 10 Normal x1 / 25 Normal 3x / 10 Heroic 1x / 25 Heroic 3x
                for (uint8 i = 1; i <= count; i++)
                {
					DoCast(me->getVictim(), N_10_SPELL_OOZE_ADHESIV, true);
					me->GetMotionMaster()->MoveChase(me->getVictim());
					OozeAdhesivTimer = 999999;
					MovechaseTimer = 10000;
				}
			}
		else OozeAdhesivTimer -= diff;
        }

		if (MovechaseTimer < diff)
		{
			me->AddThreat(me->getVictim(), 10000000.0f);
			me->GetMotionMaster()->MoveChase(me->getVictim());
			MovechaseTimer = 2000;
		}
		else MovechaseTimer -= diff;
		

		if (OozeAdhesivTimer < diff)
		{
			if (me->IsWithinDistInMap(me->getVictim(), 3))
            DoCast(me->getVictim(), N_10_SPELL_OOZE_ERUPTION);
			OozeAdhesivTimer = 10000;
		}
		else OozeAdhesivTimer -= diff;
    }
};

CreatureAI* GetAI_VolatileOoze(Creature* pCreature)
{
    return new VolatileOozeAI(pCreature);
}

CreatureAI* GetAI_Boss_ProfessorPutricide(Creature* pCreature)
{
    return new Boss_ProfessorPutricideAI(pCreature);
}

void AddSC_Boss_ProfessorPutricide()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "Boss_ProfessorPutricide";
    NewScript->GetAI = &GetAI_Boss_ProfessorPutricide;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "Mob_VolatileOoze";
    NewScript->GetAI = &GetAI_VolatileOoze;
    NewScript->RegisterSelf();
}
