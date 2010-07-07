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
	SAY_SPAWN = -1752027,
	SAY_AGGRO = -1752028,
	SAY_SLAY1 = -1752029,
	SAY_SLAY2 = -1752030,
	SAY_DEATH = -1752031,
	SAY_BERSERK = -1752032,
	SAY_SPECIAL1 = -1752033,
	SAY_SPECIAL2 = -1752033,
	SAY_PHASE2 = -1752033,
	SAY_PHASE3 = -1752033

};

struct boss_halionAI : public ScriptedAI
{
	boss_halionAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;

	bool bIntro;

	void Reset()
	{
		bIntro = false;
	}

	void EnterCombat(Unit*)
	{
		pInstance->SetData(DATA_HALION_EVENT, IN_PROGRESS);
		DoScriptText(SAY_AGGRO, me);
	}

	void UpdateAI(const uint32 diff)
	{
		if(!bIntro)
			if((pInstance->GetData(DATA_BALTHARUS_EVENT) == DONE && pInstance->GetData(DATA_ZARITHRIAN_EVENT) == DONE && pInstance->GetData(DATA_RAGEFIRE_EVENT) == DONE) || (pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC))
			{
				DoScriptText(SAY_SPAWN, me);
				bIntro = true;
			}

		if(!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}

	void JustDied(Unit*)
	{
		pInstance->SetData(DATA_HALION_EVENT, DONE);
		DoScriptText(SAY_DEATH, me);
	}

	void KilledUnit(Unit*)
	{
		DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
	}

	void EnterEvadeMode()
	{
		pInstance->SetData(DATA_HALION, NOT_STARTED);
	}
};

CreatureAI* GetAI_boss_halion(Creature *pCreature)
{
	return new boss_halionAI(pCreature);
}

struct boss_twilight_halionAI : public ScriptedAI
{
	boss_twilight_halionAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;

	void UpdateAI(const uint32 diff)
	{
		if(!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_twilight_halion(Creature *pCreature)
{
	return new boss_halionAI(pCreature);
}

void AddSC_boss_halion()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "boss_halion";
	newscript->GetAI = &GetAI_boss_halion;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "boss_twilight_halion";
	newscript->GetAI = &GetAI_boss_twilight_halion;
	newscript->RegisterSelf();
}