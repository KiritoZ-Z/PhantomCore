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

enum DeathwhisperSpells
{
	SPELL_DEATH_AND_DECAY			=	72108,
	SPELL_DOMINATE_MIND				=	71289,
	SPELL_SHADOW_BOLT				=   71254,
	SPELL_MANA_BARRIER				=	70842,
	SPELL_DARK_TRANSFORMATION		=	70900,
	SPELL_DARK_EMPOWEREMENT			=	70901,
	SPELL_FROST_BOLT				=	71420,
	SPELL_FROST_BOLT_VALLEY			=	72905,
	SPELL_SUMMON_SHADE				=	71363,
	SPELL_INSIGNIFICANCE			=	71204,
	SPELL_ROOT						=	42716,
	SPELL_BERSERK					=	47008,
};

enum CultSpells
{
	SPELL_FROST_FEVER				    =	71129,
	SPELL_DEATHCHILL_BOLT			    =	70594,
	SPELL_DEATHCHILL_BLAST			    =	70906,
	SPELL_DARK_MARTYRDROM			    =	70903,
	SPELL_CURSE_OF_TOPOR			    =	71237,
	SPELL_SHORUD_OF_THE_OCCULUT		    =	70768,
	SPELL_ADHERENTS_DETERMINIATION 	    =	71234,
	SPELL_PORT_VISUAL				    =	41236,
	SPELL_DEATH_AND_DECAY_25			=	72110,
	SPELL_SHADOW_BOLT_25				=	72008,
	H_SPELL_SHADOW_BOLT_10				=	72008,	
	H_SPELL_SHADOW_BOLT_25				=	72504,
	//SPELL_ANIMATED_DEAD				=	?????,
	N_25_SPELL_FROST_BOLT				=	72501,
	H_10_SPELL_FROST_BOLT				=	72007,
	H_25_SPELL_FROST_BOLT				=	72502,
	H_10_SPELL_FROST_BOLT_VALLEY		=	72907,
	N_25_SPELL_FROST_BOLT_VALLEY		=	72906,
	H_25_SPELL_FROST_BOLT_VALLEY		=	72909,
	H_10_SPELl_FROST_FEVER				=	67719,
	N_25_SPELL_FROST_FEVER				=	67934,
	H_25_SPELL_FROST_FEVER				=	67978,
	N_25_SPELL_DEATHCHILL_BOLT			=	72005,
	H_10_SPELL_DEATHCHILL_BOLT			=	72005,	
	H_25_SPELL_DEATHCHILL_BOLT			=	72489,
	H_10_SPELL_DEATHCHILL_BLAST			=	72485,
	N_25_SPELL_DEATHCHILL_BLAST			=	72485,
	H_25_SPELL_DEATHCHILL_BLAST			=	72487,
	H_10_SPELL_DARK_MARTYRDROM			=	72497,
	N_25_SPELL_DARK_MARTYRDROM			=	72497,
    H_25_SPELL_DARK_MARTYRDROM			=   72497,
};

enum Summons
{
	NPC_CULT_ADHERENT          = 37949,
    NPC_CULT_FANATIC           = 37890,
    NPC_VENGEFUL_SHADE         = 38222,
};

enum Yells
{
	SAY_INTRO_1			=	-1665907,
	SAY_INTRO_2			=	-1665908,
	SAY_INTRO_3			=	-1665902,	
	SAY_INTRO_4			=	-1665903,
	SAY_INTRO_5			=	-1665904,
	SAY_INTRO_6			=	-1665905,
	SAY_INTRO_7			=	-1665906,
	SAY_AGGRO			=	-1665909,
	SAY_PHASE_2			=	-1665910,
	SAY_EMPOWERMENT		=	-1665911,
	SAY_TRANSFORMATION	=	-1665912,
	SAY_ANIMATE_DEAD	=	-1665913,
	SAY_KILL_1			=	-1665917,
	SAY_KILL_2			=	-1665918,
	SAY_BERSERK			=	-1665915,
	SAY_DEATH			=	-1665916,	
};

float SpawnLoc[6][3] =
{
    {-620.197449f, 2272.062256f, 50.848679f},
    {-598.636353f, 2272.062256f, 50.848679f},
    {-578.495728f, 2272.062256f, 50.848679f},
    {-578.495728f, 2149.211182f, 50.848679f},
    {-598.636353f, 2149.211182f, 50.848679f},
    {-620.197449f, 2149.211182f, 50.848679f},
};

float HeroicSpawnLoc[3][3] =
{
	{-517.652466f, 2216.611328f, 62.823681f},
    {-517.652466f, 2211.611328f, 62.823681f},
    {-517.652466f, 2206.611328f, 62.823681f}, 
};

struct NotCharmedTargetSelector : public std::unary_function<Unit *, bool> 
{
    NotCharmedTargetSelector() {}

    bool operator() (const Unit *pTarget) {
        return (!pTarget->isCharmed());
    }
};

struct Boss_Lady_DeathwisperAI : public ScriptedAI
{
	Boss_Lady_DeathwisperAI(Creature *pCreature) : ScriptedAI(pCreature)
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
	uint32 m_uiDarkEmpoweredTimer;

    void Reset()
    {
		m_uiPhase = 1;
		m_uiSummonWaveTimer	= 15000;
		m_uiDeathandDecayTimer = 10000;
		m_uiIntroText = 0;
        m_uiIntroTextTimer = 12000;
		m_uiFrostBoltTimer = 15000;
		m_uiFrostValleyTimer = 5000;
		m_uiShadowBoltTimer = urand(5000,10000);
		m_uiDarkEmpoweredTimer	= 30000;

		if (!getDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
		m_uiDominateMindTimer = 15000;

		if (m_pInstance)
            m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
		DoScriptText(SAY_AGGRO, me);

		if (m_pInstance)
            m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, IN_PROGRESS);
    }

	void JustDied(Unit* killer)
    {  
		DoScriptText(SAY_DEATH, me);

		if (m_pInstance)
            m_pInstance->SetData(DATA_DEATHWHISPER_EVENT, DONE);
    }

	void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_KILL_1,SAY_KILL_2), me);
    }

	void MoveInLineOfSight(Unit* pWho)
    {
        if (me->IsWithinDistInMap(pWho, 50.0f) && m_uiIntroText == 0)
        {
            DoScriptText(SAY_INTRO_1, me);
            m_uiIntroText++;
        }
    }

	void JustSummoned(Creature* pSummoned)
    {
		pSummoned->CastSpell(pSummoned, SPELL_PORT_VISUAL, false);  

        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
            pSummoned->AI()->AttackStart(pTarget);
    }

	void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (me->HasAura(SPELL_MANA_BARRIER))
        {
            me->SetHealth(me->GetHealth()+damage);
            me->SetPower(POWER_MANA, me->GetPower(POWER_MANA)>damage ? me->GetPower(POWER_MANA)-damage : 0);
        }
    }
	
	void AdherentList(Unit* me)
	{
		me->FindNearestCreature(NPC_CULT_ADHERENT, 200.0f);

        std::list<Creature*> pAdherentList;
        Trinity::AllCreaturesOfEntryInRange checker(me, NPC_CULT_ADHERENT, 200.0f);
        Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, pAdherentList, checker);
        me->VisitNearbyObject(200.0f, searcher);

        if(pAdherentList.empty())
            return;

        std::list<Creature*>::iterator itr = pAdherentList.begin();
        uint32 rnd = rand()%pAdherentList.size();

        for(uint32 i = 0; i < rnd; ++i)
            ++itr;

        (*itr)->CastSpell(me, SPELL_DARK_EMPOWEREMENT, true);
	}

	void FanaticList(Unit* me)
	{
		me->FindNearestCreature(NPC_CULT_FANATIC, 200.0f);

        std::list<Creature*> pFanaticList;
        Trinity::AllCreaturesOfEntryInRange checker(me, NPC_CULT_FANATIC, 200.0f);
        Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, pFanaticList, checker);
        me->VisitNearbyObject(200.0f, searcher);

        if(pFanaticList.empty())
            return;

        std::list<Creature*>::iterator itr = pFanaticList.begin();
        uint32 rnd = rand()%pFanaticList.size();

        for(uint32 i = 0; i < rnd; ++i)
            ++itr;

        (*itr)->CastSpell(me, SPELL_DARK_EMPOWEREMENT, true);
	}
	
    void UpdateAI(const uint32 uiDiff)
    {
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
            }
            else m_uiIntroTextTimer -= uiDiff;
        }

        if (!UpdateVictim())
            return;

		if (m_uiPhase == 1)
        {
			if (m_uiShadowBoltTimer < uiDiff)
            {
				Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM);
				DoCast(pTarget, SPELL_SHADOW_BOLT);
				m_uiShadowBoltTimer = urand(5000,10000);
            }
			else m_uiShadowBoltTimer -= uiDiff;

			if (m_uiDarkEmpoweredTimer < uiDiff)
            {
				DoScriptText(SAY_EMPOWERMENT, me);
				switch(rand() % 2)
				{
					case 0: AdherentList(me); break;
					case 1: FanaticList(me); break;
				}
				m_uiDarkEmpoweredTimer = urand(25000,30000);
            }
            else m_uiDarkEmpoweredTimer -= uiDiff;

			if (m_uiSummonWaveTimer < uiDiff)
			{
				for (uint8 i = 0; i < RAID_MODE(3,6,3,6); ++i)
				{
					me->SummonCreature(RAND(NPC_CULT_FANATIC, NPC_CULT_ADHERENT), SpawnLoc[i][0], SpawnLoc [i][1], SpawnLoc[i][2],0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
				}
				m_uiSummonWaveTimer = 60000;
            }
            else m_uiSummonWaveTimer -= uiDiff;

			if (!me->HasAura(SPELL_MANA_BARRIER))
                DoCast(me, SPELL_MANA_BARRIER);
			if (!me->HasAura(SPELL_ROOT))
                DoCast(me, SPELL_ROOT);

			if (m_uiPhase == 1 || m_uiPhase == 2)
			{
			if (m_uiDominateMindTimer < uiDiff)
            {
				uint32 count = RAID_MODE(0,1,1,3);
                for (uint8 i = 1; i <= count; i++)
                {
					Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 200, true);
					if (pTarget && !pTarget->isCharmed())
					{
						DoCast(pTarget, SPELL_DOMINATE_MIND);
					}
				}
				m_uiDominateMindTimer = urand(15000,30000);
            }
            else m_uiDominateMindTimer -= uiDiff;

			if (m_uiDeathandDecayTimer < uiDiff)
            {
				Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM);
				DoCast(pTarget, SPELL_DEATH_AND_DECAY);
				m_uiDeathandDecayTimer = urand(10000,12000);
            }
            else m_uiDeathandDecayTimer -= uiDiff;

			if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
			if (m_uiSummonWaveTimer < uiDiff)
            {
                for (uint8 i = 0; i < RAID_MODE(3,3,3,3); ++i)
				{
					me->SummonCreature(RAND(NPC_CULT_FANATIC, NPC_CULT_ADHERENT), HeroicSpawnLoc[i][0], HeroicSpawnLoc [i][1], HeroicSpawnLoc[i][2],0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
				}
                m_uiSummonWaveTimer = 60000;
            }
            else m_uiSummonWaveTimer -= uiDiff;
		}

			if ((me->GetPower(POWER_MANA)*100 / me->GetMaxPower(POWER_MANA)) < 1)
			{
				DoScriptText(SAY_PHASE_2, me);
                m_uiPhase = 2;
            }
		}

		if (m_uiPhase == 2)
        {
            if (me->HasAura(SPELL_MANA_BARRIER))
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);

			if (me->HasAura(SPELL_ROOT))
                me->RemoveAurasDueToSpell(SPELL_ROOT);

			if (m_uiFrostBoltTimer < uiDiff)
            {
				DoCast(me->getVictim(), SPELL_FROST_BOLT);
				m_uiFrostBoltTimer = urand(5000,10000);
            }
            else m_uiFrostBoltTimer -= uiDiff;

			if (m_uiFrostValleyTimer < uiDiff)
            {
				DoCastAOE(SPELL_FROST_BOLT_VALLEY);
				m_uiFrostValleyTimer = urand(30000,35000);
            }
            else m_uiFrostValleyTimer -= uiDiff;

			DoMeleeAttackIfReady();
		}
	}
};

CreatureAI* GetAI_Boss_Lady_Deathwisper(Creature* pCreature)
{
    return new Boss_Lady_DeathwisperAI(pCreature);
}

void AddSC_boss_deahtwisper()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "Boss_Lady_Deathwisper";
    NewScript->GetAI = &GetAI_Boss_Lady_Deathwisper;
    NewScript->RegisterSelf();
}
