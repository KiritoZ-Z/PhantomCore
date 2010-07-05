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

/*Comment:
Creared by Lordronn*/

#include "ScriptPCH.h"
#include "icecrown_citadel.h"

enum Yells
{
	SAY_AGGRO                  = -1666016,
	SAY_DEATH                  = -1666024,
	SAY_DEATH_2                = -1666025,
	SAY_KILL                   = -1666022,
	SAY_KILL2                  = -1666021, 
	SAY_BERSERK                = -1666023,
	SAY_SLIME_SPRAY            = -1666017,
	SAY_EXPLOSION              = -1666018,
};

enum Creatures
{
	CREATURE_OOZE_BIG             = 36899,
	CREATURE_LITTLE_OOZE          = 36897,
	CREATURE_PROFFESOR_PUTRICIDE  = 36678,
};

enum Spells
{
	SPELL_OOZE_FLOOD           = 69783,
	SPELL_OOZE_FLOOD1          = 69785,
	SPELL_OOZE_FLOOD2          = 69788,
	SPELL_SLIME_SPRAY          = 69508,
	SPELL_MUTATED_INFECTION    = 71224,
	SPELL_SUMMON_LITTLE_OOZE   = 69706,
	SPELL_SUMMON_BIG_OOZE      = 69540,
	SPELL_BERSERK              = 47008,
	SPELL_STICKY_OOZE          = 71208,
	SPELL_RADIATING_OOZE       = 71212,
	SPELL_UNSTABLE_OOZE        = 69558,
	SPELL_UNSTABLE_EXPLOSION   = 71209,
	SPELL_MERGE_OOZE           = 69889,
};

struct boss_rotfaceAI : public ScriptedAI
{
	boss_rotfaceAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiFloodTimer;
	uint32 m_uiSlimeSprayTimer;
	uint32 m_uiMutatedInfectionTimer;
	uint32 m_uiBerserkTimer;
	uint32 m_uiBigOozeTimer;
	uint32 m_uiLittleOozeTimer;

	void Reset()
	{
		m_uiFloodTimer = 20000;
		m_uiSlimeSprayTimer = 20000;
		m_uiMutatedInfectionTimer = 20000;
		m_uiBerserkTimer = 360000;
		m_uiBigOozeTimer = 30000;
		m_uiLittleOozeTimer = 10000;

		if(m_pInstance)
			m_pInstance->SetData(DATA_ROTFACE_EVENT, NOT_STARTED);  
	}

	void JustDied(Unit* pKiller)
	{
		DoScriptText(SAY_DEATH, me);
		DoScriptText(SAY_DEATH_2, me);

		if(m_pInstance)
			m_pInstance->SetData(DATA_ROTFACE_EVENT, DONE);  
	}

	void EnterCombat(Unit* who)
	{
		DoScriptText(SAY_AGGRO, me);

		if(m_pInstance)
			m_pInstance->SetData(DATA_ROTFACE_EVENT, IN_PROGRESS);  
	}

	void JustReachedHome()
	{
		if(m_pInstance)
			m_pInstance->SetData(DATA_ROTFACE_EVENT, FAIL);  
	}

	void KilledUnit(Unit *pVictim)
	{
		switch(urand(0, 1))
		{
		case 0:
			DoScriptText(SAY_KILL, me);
			break;
		case 1:
			DoScriptText(SAY_KILL2, me);
			break;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (m_uiBigOozeTimer<= diff)  
		{ 
			Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
			DoCast(pTarget, SPELL_SUMMON_BIG_OOZE);
			m_uiBigOozeTimer = 30000;
		}  
		else m_uiBigOozeTimer -= diff;

		if (m_uiSlimeSprayTimer <= diff)
		{
			Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
			DoScriptText(SAY_SLIME_SPRAY, me);
			DoCast(pTarget, SPELL_SLIME_SPRAY);
			m_uiSlimeSprayTimer = 20000;
		} else m_uiSlimeSprayTimer -= diff;

		if (m_uiFloodTimer <= diff)
		{
			DoCast(SPELL_OOZE_FLOOD);
			m_uiFloodTimer = 20000;
		} else m_uiFloodTimer -= diff;

		if (m_uiLittleOozeTimer <= diff)
		{
			DoCast(SPELL_SUMMON_LITTLE_OOZE);
			m_uiLittleOozeTimer = 10000;
	    } else m_uiLittleOozeTimer -= diff;

	if (m_uiMutatedInfectionTimer <= diff)
	{
		Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
			if(pTarget && !pTarget->HasAura(SPELL_MUTATED_INFECTION))
			{
				DoCast(pTarget, SPELL_MUTATED_INFECTION);
			}
		m_uiMutatedInfectionTimer = 20000;
	} else m_uiMutatedInfectionTimer -= diff;

	if (m_uiBerserkTimer <= diff)
	{
		DoScriptText(SAY_BERSERK, me);
		DoCast(me, SPELL_BERSERK);
		m_uiBerserkTimer = 360000;
	} else m_uiBerserkTimer -= diff;

	DoMeleeAttackIfReady();
}
};

struct npc_ooze_bigAI : public ScriptedAI
{
	npc_ooze_bigAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiStickyOozeTimer;
	uint32 m_uiUnstableOozeTimer;
	uint32 m_uiUnstableExplosionTimer;

	void Reset()
	{
		m_uiStickyOozeTimer = 1000;
		m_uiUnstableOozeTimer = 5000;
		DoCast(SPELL_RADIATING_OOZE);
		me->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
		me->SetSpeed(MOVE_WALK, 0.5f);
	}

	void EnterCombat(Unit* who)
	{
	}

	void KilledUnit(Unit *pVictim)
	{
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (!m_pInstance || m_pInstance->GetData(DATA_ROTFACE_EVENT) != IN_PROGRESS) 
			me->ForcedDespawn();

		if (m_uiStickyOozeTimer <= diff)
		{
			DoCast(SPELL_STICKY_OOZE);
			m_uiStickyOozeTimer = 1000;
		} else m_uiStickyOozeTimer -= diff;

		if (m_uiUnstableOozeTimer <= diff)
		{
			DoCast(me, SPELL_UNSTABLE_OOZE);
			m_uiUnstableOozeTimer = 5000;
		} else m_uiUnstableOozeTimer -= diff;

		if (Aura *UnstableAura = me->GetAura(SPELL_UNSTABLE_OOZE))
		{
			if (UnstableAura->GetStackAmount() == 5)
			{
				DoCast(SPELL_UNSTABLE_EXPLOSION);
				me->ForcedDespawn();
			}
		}

		DoMeleeAttackIfReady();
	}
};

struct npc_ooze_littleAI : public ScriptedAI
{
	npc_ooze_littleAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiStickyOozeTimer;

	void Reset()
	{
		m_uiStickyOozeTimer = 1000;
		DoCast(SPELL_RADIATING_OOZE);
		me->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
		me->SetSpeed(MOVE_WALK, 0.5f);
	}

	void EnterCombat(Unit* who)
	{
	}

	void KilledUnit(Unit *victim)
	{
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (!m_pInstance || m_pInstance->GetData(DATA_ROTFACE_EVENT) != IN_PROGRESS) 
			me->ForcedDespawn();

		if (m_uiStickyOozeTimer <= diff)
		{
			DoCast(SPELL_STICKY_OOZE);
			m_uiStickyOozeTimer = 1000;
		} else m_uiStickyOozeTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_rotface(Creature* pCreature)
{
	return new boss_rotfaceAI(pCreature);
}

CreatureAI* GetAI_npc_ooze_big(Creature* pCreature)
{
	return new npc_ooze_bigAI(pCreature);
}

CreatureAI* GetAI_npc_ooze_little(Creature* pCreature)
{
	return new npc_ooze_littleAI(pCreature);
}

void AddSC_boss_rotface()
{
	Script* NewScript;

	NewScript = new Script;
	NewScript->Name = "boss_rotface";
	NewScript->GetAI = &GetAI_boss_rotface;
	NewScript->RegisterSelf();

	NewScript = new Script;
	NewScript->Name = "npc_ooze_big";
	NewScript->GetAI = &GetAI_npc_ooze_big;
	NewScript->RegisterSelf();

	NewScript = new Script;
	NewScript->Name = "npc_ooze_little";
	NewScript->GetAI = &GetAI_npc_ooze_little;
	NewScript->RegisterSelf();
}
