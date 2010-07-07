/* Copyright (C) 2010 /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ScriptPCH.h"
#include "ruby_sanctum.h"

enum eSpells
{
	SPELL_CLEAVE_ARMOR = 74367,
	SPELL_INTIMIDATING_ROAR = 74384,
	SPELL_SUMMON_FLAMECALLER = 74398,

	SPELL_ONYX_LAVA_ATTACK = 74394,
	SPELL_ONYX_LAVA_ATTACK_25 = 74395,
	SPELL_BLAST_NOVA = 74392,
	SPELL_BLAST_NOVA_25 = 74393
};

enum eTexts
{
	SAY_AGGRO = -1752017,
	SAY_SLAY1 = -1752018,
	SAY_SLAY2 = -1752019,
	SAY_DEATH = -1752020, 
	SAY_SPECIAL = -1752021
};

struct boss_zarithrianAI : public ScriptedAI
{
	boss_zarithrianAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData *pInstance;

	uint32 uiCleaveArmorTimer;
	uint32 uiIntimidatingRoarTimer;
	uint32 uiSummonFlamecallerTimer;

	Creature* Trigger1;
	Creature* Trigger2;

	bool SpawnTargetSpawned;

	void Reset()
	{
		uiCleaveArmorTimer = 5500;
		uiIntimidatingRoarTimer = 10000;
		uiSummonFlamecallerTimer = 25000;

		SpawnTargetSpawned = false;
	}

	void EnterCombat(Unit*)
	{
		pInstance->SetData(DATA_ZARITHRIAN_EVENT, IN_PROGRESS);
		DoScriptText(SAY_AGGRO, me);
	}

	void UpdateAI(const uint32 diff)
	{
		if(!UpdateVictim())
			return;


		if(!SpawnTargetSpawned)
		{
			Trigger1 = me->SummonCreature(NPC_ZARITHRIAN_SPAWN_STALKER,3020.61, 546.444, 89.49, 5.668, TEMPSUMMON_DEAD_DESPAWN);
			Trigger2 = me->SummonCreature(NPC_ZARITHRIAN_SPAWN_STALKER,3016.48, 507.349, 90.16, 0.52, TEMPSUMMON_DEAD_DESPAWN);
			SpawnTargetSpawned = true;
		}

		if(uiCleaveArmorTimer <= diff)
		{
			DoCastVictim(SPELL_CLEAVE_ARMOR, true);
			uiCleaveArmorTimer = urand(3500,4500);
		}else uiCleaveArmorTimer -= diff;

		if(uiIntimidatingRoarTimer <= diff)
		{
			DoCastVictim(SPELL_INTIMIDATING_ROAR, true);
			uiIntimidatingRoarTimer = urand(9500,11000);
		}else uiIntimidatingRoarTimer -= diff;

		if(uiSummonFlamecallerTimer <= diff)
		{
			Trigger1->AI()->DoCast(SPELL_SUMMON_FLAMECALLER);
			Trigger2->AI()->DoCast(SPELL_SUMMON_FLAMECALLER);
			DoScriptText(SAY_SPECIAL, me);
			uiSummonFlamecallerTimer = urand(25000,45000);
		}else uiSummonFlamecallerTimer -= diff;

		DoMeleeAttackIfReady();
	}

	void KilledUnit(Unit *victim)
	{
		if(victim == me)
			return;
		DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
	}

	void EnterEvadeMode()
	{
		if(Trigger1)
		{
			Trigger1->DisappearAndDie();
		}
		if(Trigger2)
		{
			Trigger2->DisappearAndDie();
		}
		ScriptedAI::EnterEvadeMode();
	}

	void JustDied(Unit*)
	{
		if(!pInstance)
			return;
		if(Trigger1)
		{
			Trigger1->DisappearAndDie();
		}
		if(Trigger2)
		{
			Trigger2->DisappearAndDie();
		}
		DoScriptText(SAY_DEATH, me);
		pInstance->SetData(DATA_ZARITHRIAN_EVENT, DONE);
	}
};

CreatureAI* GetAI_boss_zarithrian(Creature *pCreature)
{
	return new boss_zarithrianAI(pCreature);
}

struct npc_onyx_flamecallerAI : public ScriptedAI
{
	npc_onyx_flamecallerAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;

	uint32 uiLavaAttackTimer;
	uint32 uiBlastNovaTimer;

	void Reset()
	{
		uiLavaAttackTimer = 3000;
		uiBlastNovaTimer = 8000;
	}

	void MoveInLineOfSight(Unit* victim)
	{
		me->Attack(victim, true);
		ScriptedAI::AttackStart(victim);
	}

	void UpdateAI(const uint32 diff)
	{
		if(!pInstance->instance->GetCreature(pInstance->GetData64(DATA_ZARITHRIAN))->isAlive())
			me->DisappearAndDie();

		if(!UpdateVictim())
			return;

		if(uiLavaAttackTimer <= diff)
		{
			DoCastVictim(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_ONYX_LAVA_ATTACK_25 : SPELL_ONYX_LAVA_ATTACK);
			uiLavaAttackTimer = urand(2000, 5000);
		}else uiLavaAttackTimer -= diff;

		if(uiBlastNovaTimer <= diff)
		{
			DoCastVictim(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_BLAST_NOVA_25 : SPELL_BLAST_NOVA);
			uiBlastNovaTimer = urand(8000, 10000);
		}else uiBlastNovaTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_onyx_flamecaller(Creature *pCreature)
{
	return new npc_onyx_flamecallerAI(pCreature);
}

void AddSC_boss_zarithrian()
{
	Script *newscript;

	newscript = new Script;
	newscript->Name = "boss_zarithrian";
	newscript->GetAI = &GetAI_boss_zarithrian;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_onyx_flamecaller";
	newscript->GetAI = &GetAI_npc_onyx_flamecaller;
	newscript->RegisterSelf();
}