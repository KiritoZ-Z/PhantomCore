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
	SAY_AGGRO = -1752001,
	SAY_SLAY1 = -1752002,
	SAY_SLAY2 = -1752003,
	SAY_DEATH = -1752004,
	SAY_SPECIAL = -1752005,
	SAY_YELL = -1752006,
	SAY_ATTACK = -1752007,

	XERESTRASZA_HELP = -1752008,
	XERESTRASZA_THX = -1752009,
	XERESTRASZA_1 = -1752010,
	XERESTRASZA_2 = -1752011,
	XERESTRASZA_3 = -1752012,
	XERESTRASZA_4 = -1752013,
	XERESTRASZA_5 = -1752014,
	XERESTRASZA_6 = -1752015,
	XERESTRASZA_7 = -1752016
};

enum eBaltharusSpells
{
	SPELL_BLADE_TEMPEST = 75125,
	SPELL_BLADE_TEMPEST_25 = 75126,
	SPELL_CLEAVE = 40504,
	SPELL_CLEAVE_25 = 40505,
	SPELL_ENERVATING_BRAND = 74502,
	SPELL_ENERVATING_BRAND_25 = 74505,
	SPELL_ENERVATING_BRAND_BUFF = 74507,
	SPELL_RESPELLING_WAVE = 74509,
	SPELL_SUMMON_CLONE = 74511
};

struct boss_baltharusAI : public ScriptedAI
{
	boss_baltharusAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;

	uint32 uiBladeTempestTimer;
	uint32 uiCleaveTimer;
	uint32 uiEnervatingBrandTimer;
	uint32 uiRespellingWaveTimer;

	uint64 uiClone1GUID;
	uint64 uiClone2GUID;
	uint64 uiClone3GUID;

	bool isMode25; // Used for Clone Summoning( 10-Players: Summon at 50%, 25-Players: Summon at 75%, 50% and 25%)
	bool bClone1;
	bool bClone2;
	bool bClone3;

	void Reset()
	{
		uiBladeTempestTimer = 5000;
		uiCleaveTimer = 2000;
		uiEnervatingBrandTimer = 30000;// 30000-45000
		uiRespellingWaveTimer = 20000;// 20000-30000

		uiClone1GUID = 0;
		uiClone2GUID = 0;
		uiClone3GUID = 0;

		if(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
			isMode25 = false;
		else
			isMode25 = true;
		bClone1 = false;
		bClone2 = false;
		bClone3 = false;
	}

	void EnterCombat(Unit* )
	{
		DoScriptText(SAY_AGGRO, me);
		pInstance->SetData(DATA_BALTHARUS, IN_PROGRESS);	
	}

	void UpdateAI(const uint32 diff)
	{
		if(!UpdateVictim())
			return;

		if (uiBladeTempestTimer <= diff)
        {
			DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_BLADE_TEMPEST_25 : SPELL_BLADE_TEMPEST);
			uiBladeTempestTimer = urand(7000,7500);
		} else uiBladeTempestTimer -= diff;

		if (uiCleaveTimer <= diff)
        {
			DoCastVictim(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_CLEAVE_25 : SPELL_CLEAVE);
			uiCleaveTimer = urand(2000,2500);
		} else uiCleaveTimer -= diff;

		if (uiEnervatingBrandTimer <= diff)
        {
			DoCastVictim(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_ENERVATING_BRAND_25 : SPELL_ENERVATING_BRAND);
			DoCast(me, SPELL_ENERVATING_BRAND_BUFF); 
			uiEnervatingBrandTimer = urand(30000,45000);
			DoScriptText(SAY_YELL, me);
		} else uiEnervatingBrandTimer -= diff;

		if (uiRespellingWaveTimer <= diff)
        {
			DoCast(SPELL_RESPELLING_WAVE);
			uiRespellingWaveTimer = urand(20000,30000);
		} else uiRespellingWaveTimer -= diff;

		if(isMode25)
		{
			if(!bClone1)
			{
				if(me->GetHealth() <= ((me->GetMaxHealth() / 100) * 75))
				{
					bClone1 = true;
					DoCast(SPELL_SUMMON_CLONE);
					DoScriptText(SAY_SPECIAL, me);
				}
			}
			else if(!bClone2)
			{
				if(me->GetHealth() <= ((me->GetMaxHealth() / 100) * 50))
				{
					bClone2 = true;
					DoCast(SPELL_SUMMON_CLONE);
					DoScriptText(SAY_SPECIAL, me);
				}
			}
			else if(!bClone3)
			{
				if(me->GetHealth() <= ((me->GetMaxHealth() / 100) * 25))
				{
					bClone3 = true;
					DoCast(SPELL_SUMMON_CLONE);
					DoScriptText(SAY_SPECIAL, me);
				}
			}
		}
		else
		{
			if(!bClone1)
			{
				if(me->GetHealth() <= ((me->GetMaxHealth() / 100) * 50))
				{
					bClone1 = true;
					DoCast(SPELL_SUMMON_CLONE);
					DoScriptText(SAY_SPECIAL, me);
				}
			}
		}

		DoMeleeAttackIfReady();
	}

	void JustDied(Unit*)
	{
		DoScriptText(SAY_DEATH, me);
		if(isMode25)
		{
			if(Creature *Clone1 = pInstance->instance->GetCreature(uiClone1GUID))
				if(Clone1->isAlive())
					Clone1->DisappearAndDie();
			if(Creature *Clone2 = pInstance->instance->GetCreature(uiClone2GUID))
				if(Clone2->isAlive())
					Clone2->DisappearAndDie();
			if(Creature *Clone3 = pInstance->instance->GetCreature(uiClone3GUID))
				if(Clone3->isAlive())
					Clone3->DisappearAndDie();
		}
		else
		{
			if(Creature *Clone1 = pInstance->instance->GetCreature(uiClone1GUID))
				if(Clone1->isAlive())
					Clone1->DisappearAndDie();
		}

		if(pInstance)
		{
			pInstance->SetData(DATA_BALTHARUS, DONE);
		}
	}

	void KilledUnit(Unit* victim)
	{
		if(victim == me)
			return;
		DoScriptText(RAND(SAY_SLAY1,SAY_SLAY2), me);
	}

	void JustSummoned(Creature* pSummon)
    {
        if(!isMode25)
		{
			uiClone1GUID = pSummon->GetGUID();
		}
		else
		{
			if(uiClone1GUID == 0)
				uiClone1GUID = pSummon->GetGUID();
			else if(uiClone2GUID == 0)
				uiClone2GUID = pSummon->GetGUID();
			else if(uiClone3GUID == 0)
				uiClone3GUID = pSummon->GetGUID();
		}
    }
};

CreatureAI* GetAI_boss_baltharus(Creature *pCreature)
{
	return new boss_baltharusAI(pCreature);
}

struct boss_baltharus_cloneAI : public ScriptedAI
{
	boss_baltharus_cloneAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;

	uint32 uiBladeTempestTimer;
	uint32 uiCleaveTimer;
	uint32 uiEnervatingBrandTimer;

	void Reset()
	{
		uiBladeTempestTimer = 5000;
		uiCleaveTimer = 2000;
		uiEnervatingBrandTimer = 30000;// 30000-45000
	}

	void UpdateAI(const uint32 diff)
	{
		if(!UpdateVictim())
			return;

		if (uiBladeTempestTimer <= diff)
        {
			DoCast(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL? SPELL_BLADE_TEMPEST_25 : SPELL_BLADE_TEMPEST);
			uiBladeTempestTimer = urand(7000,7500);
		} else uiBladeTempestTimer -= diff;

		if (uiCleaveTimer <= diff)
        {
			DoCastVictim(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_CLEAVE_25 : SPELL_CLEAVE);
			uiCleaveTimer = urand(2000,2500);
		} else uiCleaveTimer -= diff;

		if (uiEnervatingBrandTimer <= diff)
        {
			DoCastVictim(pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL ? SPELL_ENERVATING_BRAND_25 : SPELL_ENERVATING_BRAND);
			DoCast(me, SPELL_ENERVATING_BRAND_BUFF); 
			uiEnervatingBrandTimer = urand(30000,45000);
		} else uiEnervatingBrandTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_baltharus_clone(Creature *pCreature)
{
	return new boss_baltharus_cloneAI(pCreature);
}

bool GossipHello_npc_xerestrasza(Player *pPlayer, Creature *pCreature)
{
	if(pCreature->GetInstanceData()->GetData(DATA_XERESTRASZA_EVENT) == NOT_STARTED)
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Was ist hier vorgefallen?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->PlayerTalkClass->SendGossipMenu(1, pCreature->GetGUID());
	return true;
}

bool GossipSelect_npc_xerestrasza(Player *pPlayer, Creature *pCreature, uint32, uint32 uiAction)
{
	if(uiAction == GOSSIP_ACTION_INFO_DEF+1)
		pCreature->GetInstanceData()->SetData(DATA_XERESTRASZA_EVENT, IN_PROGRESS);
	pPlayer->PlayerTalkClass->CloseGossip();
	return true;
}

struct npc_xerestraszaAI : public ScriptedAI
{
	npc_xerestraszaAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		pInstance = me->GetInstanceData();
	}

	InstanceData* pInstance;
	uint32 Timer;
	uint32 Counter;

	bool bThx;
	bool bHelp;

	void Reset()
	{
		Timer = 9000;
		Counter = 0;
		bThx = false;
		bHelp = false;
	}

	void MoveInLineOfSight(Unit*)
	{
		if(!bHelp)
		{
			DoScriptText(XERESTRASZA_HELP, pInstance->instance->GetCreature(pInstance->GetData64(DATA_XERESTRASZA)));
			bHelp = true;
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if(!bThx)
			if(pInstance->GetData(DATA_XERESTRASZA_EVENT) == NOT_STARTED)
			{
				DoScriptText(XERESTRASZA_THX, pInstance->instance->GetCreature(pInstance->GetData64(DATA_XERESTRASZA)));
				bThx = true;
			}

		if(pInstance->GetData(DATA_XERESTRASZA_EVENT) == IN_PROGRESS)
		{
		switch(Counter)
		{
		case 0:
			DoScriptText(XERESTRASZA_1, me);
			Counter++;
			break;
		case 1:
			if (Timer <= diff)
			{
				DoScriptText(XERESTRASZA_2, me);
				Timer = 11000;
				Counter++;
			} else Timer -= diff;
			break;
		case 2:
			if (Timer <= diff)
			{
				DoScriptText(XERESTRASZA_3, me);
				Timer = 11000;
				Counter++;
			} else Timer -= diff;
			break;
		case 3:
			if (Timer <= diff)
			{
				DoScriptText(XERESTRASZA_4, me);
				Timer = 11000;
				Counter++;
			} else Timer -= diff;
			break;
		case 4:
			if (Timer <= diff)
			{
				DoScriptText(XERESTRASZA_5, me);
				Timer = 11000;
				Counter++;
			} else Timer -= diff;
			break;
		case 5:
			if (Timer <= diff)
			{
				DoScriptText(XERESTRASZA_6, me);
				Timer = 11000;
				Counter++;
			} else Timer -= diff;
			break;
		case 6:
			if (Timer <= diff)
			{
				DoScriptText(XERESTRASZA_7, me);
				pInstance->SetData(DATA_XERESTRASZA_EVENT, DONE);
			} else Timer -= diff;
			break;
		}
		}
	}
};

CreatureAI* GetAI_npc_xerestrasza(Creature *pCreature)
{
	return new npc_xerestraszaAI(pCreature);
}

bool GOHello_go_firefield(Player *pPlayer, GameObject *pGO)
{
	pGO->SetGoState(GO_STATE_ACTIVE);
	pGO->GetInstanceData()->SetData(DATA_XERESTRASZA_EVENT, NOT_STARTED);
	return true;
}

void AddSC_boss_baltharus()
{
	Script *newscript;

	newscript = new Script;
	newscript->Name = "boss_baltharus";
	newscript->GetAI = &GetAI_boss_baltharus;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "boss_baltharus_clone";
	newscript->GetAI = &GetAI_boss_baltharus_clone;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_xerestrasza";
	newscript->pGossipHello = &GossipHello_npc_xerestrasza;
	newscript->pGossipSelect = &GossipSelect_npc_xerestrasza;
	newscript->GetAI = &GetAI_npc_xerestrasza;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_firefield";
	newscript->pGOHello = &GOHello_go_firefield;
	newscript->RegisterSelf();
}