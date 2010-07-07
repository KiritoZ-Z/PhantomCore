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

enum eTexts
{
	SAY_AGGRO = -1752022,
	SAY_SLAY1 = -1752023,
	SAY_SLAY2 = -1752024,
	SAY_DEATH = -1752025,
	SAY_SPECIAL = -1752026
};

enum eSpells
{
	SPELL_ENRAGE = 78722,
	SPELL_FLAME_BREATH = 74403,
	SPELL_FLAME_BREATH_25 = 74404,
	SPELL_FIRE_NOVA = 78723,
	SPELL_CONFLAGRATION_FIRE_BALL = 74454,
	SPELL_CONFLAGRATION_DMG = 74456,
	SPELL_FLAME_BEACON = 74453
};

struct boss_ragefireAI : public ScriptedAI
{
	boss_ragefireAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;

	uint32 uiFlameBreathTimer;
	uint32 uiEnrageTimer;
	uint32 uiConflagrationTimer;
	uint32 uiAirSteps;
	uint32 uiAirTimer;
	uint32 uiFlameNovaTimer;

	Player* pConflagrationTarget1;
	Player* pConflagrationTarget2;
	Player* pConflagrationTarget3;
	Player* pConflagrationTarget4;
	Player* pConflagrationTarget5;

	bool bAir;

	void Reset()
	{
		uiFlameBreathTimer = 10000;
		uiEnrageTimer = 25000;
		uiConflagrationTimer = 45000;
		uiFlameNovaTimer = 9000;
		uiAirSteps = 0;
		uiAirTimer = 5000;

		bAir = false;
	}

	void EnterCombat(Unit*)
	{
		pInstance->SetData(DATA_RAGEFIRE_EVENT, IN_PROGRESS);
		DoScriptText(SAY_AGGRO, me);
	}

	void UpdateAI(const uint32 diff)
	{
		if(!UpdateVictim())
			return;

		if(!bAir)
		{
			if(uiFlameBreathTimer <= diff)
			{
				DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_FLAME_BREATH_25 : SPELL_FLAME_BREATH);
				uiFlameBreathTimer = urand(10000, 15000);
			}else uiFlameBreathTimer -= diff;

			if(uiEnrageTimer <= diff)
			{
				DoCast(SPELL_ENRAGE);
				uiEnrageTimer = 25000;
			}else uiEnrageTimer -= diff;

			if(uiFlameNovaTimer <= diff)
			{
				DoCast(SPELL_FIRE_NOVA);
				uiFlameNovaTimer = urand(9000, 11000);
			}else uiFlameNovaTimer -= diff;

			if(uiConflagrationTimer <= diff)
			{
				bAir = true;
				uiConflagrationTimer = 45000;
			}else uiConflagrationTimer -= diff;

			DoMeleeAttackIfReady();
		}
		else
		{
			switch(uiAirSteps)
			{
			case 0:
				me->GetMotionMaster()->MovePoint(1, 3159.04, 676.08, 103.05);
				if(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
				{
					pConflagrationTarget1 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
					pConflagrationTarget2 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
				}
				else
				{
					pConflagrationTarget1 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
					pConflagrationTarget2 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
					pConflagrationTarget3 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
					pConflagrationTarget4 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
					pConflagrationTarget5 = Unit::GetPlayer(SelectUnit(SELECT_TARGET_RANDOM, 0)->GetGUID());
				}
				uiAirSteps++;
				break;
			case 1:
				if(uiAirTimer <= diff)
				{
					DoScriptText(SAY_SPECIAL, me);
					if(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
					{
						me->CastSpell(pConflagrationTarget1, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget2, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget1, SPELL_CONFLAGRATION_FIRE_BALL, true);
						me->CastSpell(pConflagrationTarget2, SPELL_CONFLAGRATION_FIRE_BALL, true);
					}
					else
					{
						me->CastSpell(pConflagrationTarget1, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget2, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget3, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget4, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget5, SPELL_FLAME_BEACON, true);
						me->CastSpell(pConflagrationTarget1, SPELL_CONFLAGRATION_FIRE_BALL, true);
						me->CastSpell(pConflagrationTarget2, SPELL_CONFLAGRATION_FIRE_BALL, true);
						me->CastSpell(pConflagrationTarget3, SPELL_CONFLAGRATION_FIRE_BALL, true);
						me->CastSpell(pConflagrationTarget4, SPELL_CONFLAGRATION_FIRE_BALL, true);
						me->CastSpell(pConflagrationTarget5, SPELL_CONFLAGRATION_FIRE_BALL, true);
					}
					uiAirSteps++;
					uiAirTimer = 4000;
				}else uiAirTimer -= diff;
				break;
			case 2:
				if(uiAirTimer <= diff)
				{
					me->GetMotionMaster()->MoveTargetedHome();
					uiAirSteps++;
					uiAirTimer = 1000;
				}else uiAirTimer -= diff;
				break;
			case 3:
				if(uiAirTimer <= diff)
				{
					DoCast(SPELL_CONFLAGRATION_DMG); // Visual in Case of No Target
					if(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
					{
						me->CastSpell(pConflagrationTarget1, SPELL_CONFLAGRATION_DMG, true);
						me->CastSpell(pConflagrationTarget2, SPELL_CONFLAGRATION_DMG, true);
					}
					else
					{
						me->CastSpell(pConflagrationTarget1, SPELL_CONFLAGRATION_DMG, true);
						me->CastSpell(pConflagrationTarget2, SPELL_CONFLAGRATION_DMG, true);
						me->CastSpell(pConflagrationTarget3, SPELL_CONFLAGRATION_DMG, true);
						me->CastSpell(pConflagrationTarget4, SPELL_CONFLAGRATION_DMG, true);
						me->CastSpell(pConflagrationTarget5, SPELL_CONFLAGRATION_DMG, true);
					}
					uiAirSteps = 0;
					bAir = false;
					uiAirTimer = 5000;
				}else uiAirTimer -= diff;
				break;
			}
		}
	}

	void KilledUnit(Unit *victim)
	{
		if(victim == me)
			return;
		DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
	}

	void JustDied(Unit*)
	{
		pInstance->SetData(DATA_RAGEFIRE_EVENT, DONE);
		DoScriptText(SAY_DEATH, me);
	}
};

CreatureAI* GetAI_boss_ragefire(Creature *pCreature)
{
	return new boss_ragefireAI(pCreature);
}

void AddSC_boss_ragefire()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "boss_ragefire";
	newscript->GetAI = &GetAI_boss_ragefire;
	newscript->RegisterSelf();
}