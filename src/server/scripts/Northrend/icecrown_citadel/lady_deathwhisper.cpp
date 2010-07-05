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

enum DeathwhisperSpells
{
	SPELL_DEATH_AND_DECAY_10_NORMAL    =    71001,
	SPELL_DEATH_AND_DECAY_25_NORMAL    =    72108,
	SPELL_DEATH_AND_DECAY_10_HEROIC    =    71001,
	SPELL_DEATH_AND_DECAY_25_HEROIC    =    72110,
	SPELL_DOMINATE_MIND                =    71289,
	SPELL_SHADOW_BOLT_10_NORMAL        =    71254,
	SPELL_SHADOW_BOLT_25_NORMAL        =    72008,
	SPELL_SHADOW_BOLT_10_HEROIC        =    72008,
	SPELL_SHADOW_BOLT_25_HEROIC        =    72504,
	SPELL_MANA_BARRIER                 =    70842,
	SPELL_DARK_TRANSFORMATION          =    70900,
	SPELL_DARK_EMPOWEREMENT            =    70901,
	SPELL_FROST_BOLT_10_NORMAL         =    71420,
	SPELL_FROST_BOLT_25_NORMAL         =    72007,
	SPELL_FROST_BOLT_10_HEROIC         =    72007,
	SPELL_FROST_BOLT_25_HEROIC         =    72502,
	SPELL_FROST_BOLT_VALLEY_10_NORMAL  =    72905,
	SPELL_FROST_BOLT_VALLEY_25_NORMAL  =    72906,
	SPELL_FROST_BOLT_VALLEY_10_HEROIC  =    72907,
	SPELL_FROST_BOLT_VALLEY_25_HEROIC  =    72908,
	SPELL_SUMMON_SHADE                 =    71363,
	SPELL_INSIGNIFICANCE               =    71204,
	SPELL_ROOT                         =    42716,
	SPELL_BERSERK                      =    47008,
	SPELL_BLAST_10_NORMAL              =    72011,
	SPELL_BLAST_10_HEROIC              =    72010,
	SPELL_BLAST_25_NORMAL              =    72010,
	SPELL_BLAST_25_HEROIC              =    72012,
	SPELL_PORT_VISUAL                  =    41236,
	//SPELL_ANIMATED_DEAD                =    ?????,
};
enum Summons
{
	NPC_CULT_ADHERENT          = 37949,
	NPC_CULT_FANATIC           = 37890,
	NPC_VENGEFUL_SHADE         = 38222,
};

enum Yells
{
	SAY_INTRO_1            =    -1665902,
	SAY_INTRO_2            =    -1665903,
	SAY_INTRO_3            =    -1665904,    
	SAY_INTRO_4            =    -1665905,
	SAY_INTRO_5            =    -1665906,
	SAY_INTRO_6            =    -1665907,
	SAY_INTRO_7            =    -1665908,
	SAY_AGGRO              =    -1665909,
	SAY_PHASE_2            =    -1665910,
	SAY_EMPOWERMENT        =    -1665911,
	SAY_TRANSFORMATION     =    -1665912,
	SAY_ANIMATE_DEAD       =    -1665913,
	SAY_KILL_1             =    -1665917,
	SAY_KILL_2             =    -1665918,
	SAY_BERSERK            =    -1665915,
	SAY_DEATH              =    -1665916,    
};

struct boss_lady_deathwisperAI : public ScriptedAI
{
	boss_lady_deathwisperAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	uint32 m_uiPhase;
	uint32 m_uiDominateMindTimer;
	uint32 m_uiSummonWaveTimer;
	uint32 m_uiDeathandDecayTimer;
	uint8  m_uiIntroText;
	uint32 m_uiIntroTextTimer;
	uint32 m_uiFrostBoltTimer;
	uint32 m_uiFrostValleyTimer;
	uint32 m_uiShadowBoltTimer;
	uint32 m_uiBerserkTimer;
	uint32 m_uiInsignificanceTimer;
	uint32 m_uiShadeTimer;

	void Reset()
	{
		m_uiPhase = 1;
		m_uiSummonWaveTimer = 60000;
		m_uiDeathandDecayTimer = 10000;
		m_uiIntroText = 0;
		m_uiIntroTextTimer = 15000;
		m_uiFrostBoltTimer = 16000;
		m_uiFrostValleyTimer = 25000;
		m_uiShadowBoltTimer = 7000;
		m_uiBerserkTimer = 600000;
		m_uiInsignificanceTimer = 30000;
		m_uiShadeTimer = 10000;
		m_uiDominateMindTimer = 12000;

		if (m_pInstance)
			m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, NOT_STARTED);

		if (!me->HasAura(SPELL_ROOT))
			DoCast(me, SPELL_ROOT);
	}

	void EnterCombat(Unit* who)
	{
		DoScriptText(SAY_AGGRO, me);

		if (m_pInstance)
			m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, IN_PROGRESS);

		if (!me->HasAura(SPELL_MANA_BARRIER))
		{
			DoCast(me, SPELL_MANA_BARRIER); 
		}
	}

	void JustDied(Unit* killer)
	{  
		DoScriptText(SAY_DEATH, me);

		if (m_pInstance)
			m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, DONE);
	}

	void JustReachedHome()
	{
		if(m_pInstance)
			m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, FAIL);  
	}

	void KilledUnit(Unit *victim)
	{
		switch (urand(0,1)) 
		{
		case 0:
			DoScriptText(SAY_KILL_1,me);
			break;
		case 1:
			DoScriptText(SAY_KILL_1,me);
			break;
		}
	}

	void MoveInLineOfSight(Unit* pWho)
	{
		if (me->IsWithinDistInMap(pWho, 90.0f) && m_uiIntroText == 0)
		{
			DoScriptText(SAY_INTRO_1, me);
			m_uiIntroText++;
		}
	}

	void JustSummoned(Creature* pSummoned)
	{
		DoCast(pSummoned, SPELL_PORT_VISUAL);  

		Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
		pSummoned->AI()->AttackStart(pTarget);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!UpdateVictim())
			return;

		if (m_uiIntroText == 1 || m_uiIntroText == 2 || 
			m_uiIntroText == 3 || m_uiIntroText == 4 ||
			m_uiIntroText == 5 || m_uiIntroText == 6)
		{
			if (m_uiIntroTextTimer < uiDiff)
			{
				switch(m_uiIntroText)
				{
				case 1: DoScriptText(SAY_INTRO_2, me); break;
				case 2: DoScriptText(SAY_INTRO_3, me); break;
				case 3: DoScriptText(SAY_INTRO_4, me); break;
				case 4: DoScriptText(SAY_INTRO_5, me); break;
				case 5: DoScriptText(SAY_INTRO_6, me); break;
				case 6: DoScriptText(SAY_INTRO_7, me); break;
				}
				m_uiIntroText++;
				m_uiIntroTextTimer = 10500;
			} else m_uiIntroTextTimer -= uiDiff;
		}

		if (m_uiBerserkTimer < uiDiff)
		{
			DoCast(me, SPELL_BERSERK);
			DoScriptText(SAY_BERSERK, me);
			m_uiBerserkTimer = 600000;
		} else m_uiBerserkTimer -= uiDiff;

		if (m_uiDeathandDecayTimer < uiDiff)
		{
			Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
			DoCast(pTarget, RAID_MODE(SPELL_DEATH_AND_DECAY_10_NORMAL,SPELL_DEATH_AND_DECAY_25_NORMAL,SPELL_DEATH_AND_DECAY_10_HEROIC,SPELL_DEATH_AND_DECAY_25_HEROIC));
			m_uiDeathandDecayTimer = 11000;
		} else m_uiDeathandDecayTimer -= uiDiff;

		if (m_uiDominateMindTimer < uiDiff)
		{
			uint32 count = RAID_MODE(0,1,1,3);
			for (uint8 i = 1; i <= count; i++)
			{
				Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 200, true);
				if (pTarget && !pTarget->HasAura(71289))
				{
					DoCast(pTarget, SPELL_DOMINATE_MIND);
				}
			}
			m_uiDominateMindTimer = 12000;
		} else m_uiDominateMindTimer -= uiDiff;

		if (m_uiPhase == 1)
		{
			if (m_uiShadowBoltTimer < uiDiff)
			{
				Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
				DoCast(pTarget, RAID_MODE(SPELL_SHADOW_BOLT_10_NORMAL,SPELL_SHADOW_BOLT_25_NORMAL,SPELL_SHADOW_BOLT_10_HEROIC,SPELL_SHADOW_BOLT_25_HEROIC));
				m_uiShadowBoltTimer = 7000;
			} else m_uiShadowBoltTimer -= uiDiff;

			if (m_uiSummonWaveTimer < uiDiff)
			{
				Unit* pSummoned= NULL;
				me->SummonCreature(NPC_CULT_FANATIC,-619.006,2158.104,50.848,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				me->SummonCreature(NPC_CULT_ADHERENT,-598.697,2157.767,50.848,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				me->SummonCreature(NPC_CULT_FANATIC,-577.992,2156.989,50.848,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);

				if (getDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				{
					me->SummonCreature(NPC_CULT_ADHERENT,-618.748,2266.648,50.849,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
					me->SummonCreature(NPC_CULT_FANATIC,-598.573,2266.870,50.849,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
					me->SummonCreature(NPC_CULT_ADHERENT,-578.360,2267.210,50.849,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
					me->SummonCreature(RAND(NPC_CULT_FANATIC, NPC_CULT_ADHERENT), -555.643,2211.281,49.476,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				}
				m_uiSummonWaveTimer = 60000;
			} else m_uiSummonWaveTimer -= uiDiff;

			if (!me->HasAura(SPELL_MANA_BARRIER))
			{
				DoScriptText(SAY_PHASE_2, me);
				DoResetThreat();
				m_uiPhase = 2;
			}
		}

		if (m_uiPhase == 2)
		{
			if (me->HasAura(SPELL_ROOT))
				me->RemoveAurasDueToSpell(SPELL_ROOT);

			if (m_uiFrostBoltTimer < uiDiff)
			{
				Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM,1);
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false); 
				DoCast(pTarget, RAID_MODE(SPELL_FROST_BOLT_10_NORMAL,SPELL_FROST_BOLT_25_NORMAL,SPELL_FROST_BOLT_10_HEROIC,SPELL_FROST_BOLT_25_HEROIC));
				m_uiFrostBoltTimer = 16000;
			} else m_uiFrostBoltTimer -= uiDiff;

			if (m_uiFrostValleyTimer < uiDiff)
			{
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true); 
				DoCastAOE(RAID_MODE(SPELL_FROST_BOLT_VALLEY_10_NORMAL,SPELL_FROST_BOLT_VALLEY_25_NORMAL,SPELL_FROST_BOLT_VALLEY_10_HEROIC,SPELL_FROST_BOLT_VALLEY_25_HEROIC));
				m_uiFrostValleyTimer = 25000;
			} else m_uiFrostValleyTimer -= uiDiff;

			if (m_uiInsignificanceTimer < uiDiff)
			{
				Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM,1);
				DoCast(pTarget, SPELL_INSIGNIFICANCE);
				m_uiInsignificanceTimer = 30000;
			} else m_uiInsignificanceTimer -= uiDiff;

			if (m_uiSummonWaveTimer < uiDiff)
			{
				if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
			    {
			        Unit* pSummoned= NULL;
			        me->SummonCreature(RAND(NPC_CULT_FANATIC, NPC_CULT_ADHERENT), -555.643,2211.281,49.476,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				}
				if (getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
			    {
				    Unit* pSummoned= NULL;
				    me->SummonCreature(NPC_CULT_FANATIC,-619.006,2158.104,50.848,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				    me->SummonCreature(NPC_CULT_ADHERENT,-598.697,2157.767,50.848,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				    me->SummonCreature(NPC_CULT_FANATIC,-577.992,2156.989,50.848,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,10000);
				}
				m_uiSummonWaveTimer = 60000;
			} else m_uiSummonWaveTimer -= uiDiff;

			if (m_uiShadeTimer < uiDiff)
			{
				uint32 count = RAID_MODE(0,1,1,1);
				for (uint8 i = 1; i <= count; i++)
				{
					Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
					DoCast(pTarget, SPELL_SUMMON_SHADE);
					m_uiShadeTimer = 15000;
				}
			} else m_uiShadeTimer -= uiDiff;

			DoMeleeAttackIfReady();
		}
	}
};

struct npc_shadeAI : public ScriptedAI //Dont work
{
	npc_shadeAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = pCreature->GetInstanceData();
	}

	ScriptedInstance* m_pInstance;

	void Reset()
	{
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetSpeed(MOVE_WALK, 1.1f, true);
	}

	void EnterCombat(Unit* who)
	{
	}

	void KilledUnit(Unit *victim)
	{
	}

	void MoveInLineOfSight(Unit *who)
	{
		if (!me->IsWithinDistInMap(who,3.0f))
		{
			Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM,0); 
			DoCast(pTarget, RAID_MODE(SPELL_BLAST_10_NORMAL,SPELL_BLAST_25_NORMAL,SPELL_BLAST_10_HEROIC,SPELL_BLAST_25_HEROIC));
			me->ForcedDespawn();
		}

		ScriptedAI::MoveInLineOfSight(who);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_lady_deathwisper(Creature* pCreature)
{
	return new boss_lady_deathwisperAI(pCreature);
}

CreatureAI* GetAI_npc_shade(Creature* pCreature)
{
	return new npc_shadeAI(pCreature);
}

void AddSC_boss_deahtwisper()
{
	Script *newscript;

	newscript = new Script;
	newscript->Name = "boss_lady_deathwisper";
	newscript->GetAI = &GetAI_boss_lady_deathwisper;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shade";
	newscript->GetAI = &GetAI_npc_shade;
	newscript->RegisterSelf();
}
