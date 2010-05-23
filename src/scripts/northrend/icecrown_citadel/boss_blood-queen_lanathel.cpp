/* Copyright (C) 2009 - 2010 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Phantom Project <http://phantom-project.org/>
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
 
 /* Script Data Start
Name: Blood-Queen Lana'thel
Commend: Sehr sehr komplexes Script :/
Complet: 20%
Todo: Spell Fixxes! 80% der Spells funktionieren nicht richtig :/
Script Data End */
 
#include "ScriptedPch.h"
#include "instance_icecrown_citadel.h"

enum Yells
{
	SAY_AGGRO			=	-1666053,
	SAY_VAMP_BITE_1		=	-1666054,	
	SAY_VAMP_BITE_2		=	-1666055,
	SAY_SWARM_SHADOWS	=	-1666056,
	SAY_PACT_DARKFALLEN	=	-1666057,
	SAY_MIND_CONTROLL	=	-1666058,
	SAY_AIR_PHASE		=	-1666059,
	SAY_BERSERK			=	-1666060,
	SAY_DEATH			=	-1666061,
};

enum BloodQuennSpells
{
	SPELL_SHROUD_OF_SORROW			=	72982, // Dauerhafter Debuff
	SPELL_DELIRIOUS_SLASH			=	71623,
	SPELL_BLOOD_MIRROR_1			=	70821,
	SPELL_BLOOD_MIRROR_2			=	71510,
	SPELL_VAMPIRIC_BITE				=	71726,
	SPELL_PACT_OF_THE_DARKFALLEN_1	=	71340, // Trigger
	SPELL_PACT_OF_THE_DARKFALLEN_2	=	71341, // Damage
	SPELL_SWARMING_SHADOWS			=	71264, // Spawn Creature 38163 need Visual Spell ID :/
	SPELL_TWILIGHT_BLOODBOLT		=	71446, // Cast on Random targets
	SPELL_BLOODBOLD_WHIRL			=	71772,
	SPELL_BLOODBOLD_SPLASH			=	71481,
	SPELL_BERSERK					=	47008,
	SPELL_INCITE_TERROR				=	73070
};

enum SwarmingShadowSpells
{
	SPELL_SWARMING_SHADOWS_VISUAL		=	71267, // 72890
	N_10_SPELL_SWARMING_SHADOW_DAMAGE	=	71267,
	N_25_SPELL_SWARMING_SHADOW_DAMAGE	=	72635,	
	H_10_SPELL_SWARMING_SHADOW_DAMAGE	=	72636,
	H_25_SPELL_SWARMING_SHADOW_DAMAGE	=	72637,
};

enum Creatures
{
	CREATURE_SWARMING_SHADOWS	=	38163,
};

enum Achievments
{
	ACHIEVMENT_ONCE_BITTEN_TWICE_SHY_10	=	4539,
	ACHIEVMENT_ONCE_BITTEN_TWICE_SHY_25	=	4618,
};

const Position Phase2Position = { 4595.640137, 2769.195557, 400.137054};
const Position FlyPosition = { 4595.904785, 2769.315918, 421.838623};

struct Boss_Blood_Queen_LanathelAI : public ScriptedAI
{
    Boss_Blood_Queen_LanathelAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = pCreature->GetInstanceData();
    }

	ScriptedInstance* m_pInstance;

	/************ Phase 1 **********/ 
	uint32 m_uiTwilightBloodboltTimer;
	uint32 m_uiPhaseTimer;
	uint32 m_uiVampBiteTimer;
	uint32 m_uiSwarmingShadowsTimer;
	uint32 m_uiPactofDarkfallenTimer;
    uint32 m_uiPactofDarkfallenExplodeTimer;
	uint32 m_uiPactofDarkfallenExplosionCount;

	/************ Other **********/ 
	uint32 m_uiPhase;
	uint32 m_uiBerserkTimer;
		
	/************ Phase 2 **********/ 
	uint32 m_uiSetHoverTimer;
	uint32 m_uiLandingTimer;
	uint32 m_uiFlyingFalseTimer;
	uint32 m_uiBloodboldSplashTimer;

	void Reset()
	{
		/************ Phase Switch **********/ 
		m_uiPhaseTimer = 120000; // Erste mal 2 Min danach immer 1:30 Min.

		/************ Phase 1 **********/ 
		m_uiTwilightBloodboltTimer = 10*IN_MILISECONDS;
		m_uiVampBiteTimer	= 15*IN_MILISECONDS;
		m_uiSwarmingShadowsTimer = 30*IN_MILISECONDS;
		m_uiPactofDarkfallenTimer	= 5*IN_MILISECONDS;
		/************ Phase 2 **********/ 
		m_uiSetHoverTimer = 900*IN_MILISECONDS;
		m_uiBloodboldSplashTimer = 900*IN_MILISECONDS;
		m_uiLandingTimer = 900*IN_MILISECONDS;;
		m_uiFlyingFalseTimer = 900*IN_MILISECONDS;;
		
		/************ Other **********/
		m_uiPhase = 1;
		m_uiBerserkTimer	= 330000; // 5:30 Min
		me->SetFlying(false);

		me->SetReactState(REACT_AGGRESSIVE);
	}

	void EnterCombat(Unit *who)
	{
		DoScriptText(SAY_AGGRO, me);
	}

	void KilledUnit(Unit* victim)
	{
		//DoScriptText(RAND(SAY_SLAY_1,SAY_SLAY_2), me);
	}

	void JustDied(Unit* Killer)
	{
		DoScriptText(SAY_DEATH, me);
	}

     void UpdateAI(const uint32 uiDiff)
     {
          if (!UpdateVictim())
               return;

		if (!me->HasAura(SPELL_SHROUD_OF_SORROW))
			DoCast(me, SPELL_SHROUD_OF_SORROW);

			if(m_uiPhase == 1)
			{

				if (m_uiPactofDarkfallenTimer <= uiDiff)
				{
//					PactFallen();

					DoScriptText(SAY_PACT_DARKFALLEN, me);
					m_uiPactofDarkfallenExplosionCount = 0;
					m_uiPactofDarkfallenExplodeTimer = 2*IN_MILISECONDS;
					m_uiPactofDarkfallenTimer = 25*IN_MILISECONDS;
				} 
				else m_uiPactofDarkfallenTimer -= uiDiff;

				if (m_uiSwarmingShadowsTimer < uiDiff)
				{
					if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(target, SPELL_SWARMING_SHADOWS);
					m_uiSwarmingShadowsTimer  = 30*IN_MILISECONDS;
				}
				else m_uiSwarmingShadowsTimer -= uiDiff;

				if (m_uiTwilightBloodboltTimer < uiDiff)
				{
					if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(target, SPELL_TWILIGHT_BLOODBOLT);
					m_uiTwilightBloodboltTimer  = urand(6*IN_MILISECONDS,12*IN_MILISECONDS);
				}
				else m_uiTwilightBloodboltTimer -= uiDiff;

				if (m_uiVampBiteTimer < uiDiff)
				{
					DoScriptText(RAND(SAY_VAMP_BITE_1,SAY_VAMP_BITE_2), me);
					if (Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO/*SELECT_TARGET_RANDOM*/, 0))
					DoCast(target, SPELL_VAMPIRIC_BITE);
					m_uiVampBiteTimer	= urand(15*IN_MILISECONDS,60*IN_MILISECONDS);
				}
				else m_uiVampBiteTimer -= uiDiff;
				
				if (m_uiPhaseTimer < uiDiff)
				{	
					DoScriptText(SAY_AIR_PHASE, me);
					DoCast(me, SPELL_INCITE_TERROR);
					me->GetMotionMaster()->MovePoint(0, Phase2Position);
					me->SetReactState(REACT_PASSIVE);
					me->AttackStop();
					m_uiPhase = 2;
					m_uiPhaseTimer	= 90*IN_MILISECONDS;
					m_uiSetHoverTimer = 5*IN_MILISECONDS;
					m_uiBloodboldSplashTimer = 10*IN_MILISECONDS;
					m_uiLandingTimer = 26*IN_MILISECONDS;
					m_uiFlyingFalseTimer = 30*IN_MILISECONDS;
				}
				else m_uiPhaseTimer -= uiDiff;
			}
		
		if(m_uiPhase == 2)
		{
			if (m_uiSetHoverTimer < uiDiff)
			{
				me->GetMotionMaster()->MovePoint(0, FlyPosition);
				me->SetUnitMovementFlags(MOVEMENTFLAG_JUMPING);
				me->SetFlying(true);
				m_uiSetHoverTimer	= 90*IN_MILISECONDS;
			}
			else m_uiSetHoverTimer -= uiDiff;

			if (m_uiBloodboldSplashTimer < uiDiff)
			{
               std::list<Unit*> pTargets;
                    SelectTargetList(pTargets, 5, SELECT_TARGET_RANDOM, 80, true);
                    for (std::list<Unit*>::const_iterator i = pTargets.begin(); i != pTargets.end(); ++i)
                        DoCast(*i, RAID_MODE(SPELL_TWILIGHT_BLOODBOLT,SPELL_TWILIGHT_BLOODBOLT));
				m_uiBloodboldSplashTimer = 2*IN_MILISECONDS;
			}
			else m_uiBloodboldSplashTimer -= uiDiff;

			if (m_uiLandingTimer < uiDiff)
			{
				me->GetMotionMaster()->MovePoint(0, Phase2Position);
				me->SetUnitMovementFlags(MOVEMENTFLAG_JUMPING);
				m_uiLandingTimer	= 900*IN_MILISECONDS;
				m_uiBloodboldSplashTimer = 900*IN_MILISECONDS;
			}
			else m_uiLandingTimer -= uiDiff;

			if (m_uiFlyingFalseTimer < uiDiff)
			{
				me->SetFlying(false);
				me->RemoveAllAuras();
				me->SetReactState(REACT_AGGRESSIVE);
				me->GetMotionMaster()->MoveChase(me->getVictim());
				m_uiPhase = 1;
				m_uiFlyingFalseTimer = 900*IN_MILISECONDS;
			}
			else m_uiFlyingFalseTimer -= uiDiff;
		}

		DoMeleeAttackIfReady();
	}
};

struct mob_swarming_shadowsAI : public Scripted_NoMovementAI
{
    mob_swarming_shadowsAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

	uint32 m_uiSwarmingShadowTimer;

    void Reset()
    {
		me->SetReactState(REACT_PASSIVE);
		m_uiSwarmingShadowTimer = 1200;
    }
    void UpdateAI(const uint32 uiDiff)
    {
		if (!me->HasAura(SPELL_SWARMING_SHADOWS_VISUAL))
			DoCast(me, SPELL_SWARMING_SHADOWS_VISUAL);

		if (m_uiSwarmingShadowTimer < uiDiff)
		{
			DoCast(me, RAID_MODE(N_10_SPELL_SWARMING_SHADOW_DAMAGE,N_25_SPELL_SWARMING_SHADOW_DAMAGE,H_10_SPELL_SWARMING_SHADOW_DAMAGE,H_25_SPELL_SWARMING_SHADOW_DAMAGE));
			m_uiSwarmingShadowTimer	= 2000;
		}
		else m_uiSwarmingShadowTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_swarming_shadows(Creature* pCreature)
{
     return new mob_swarming_shadowsAI (pCreature);
}

CreatureAI* GetAI_Boss_Blood_Queen_Lanathel(Creature* pCreature)
{
     return new Boss_Blood_Queen_LanathelAI (pCreature);
}

void AddSC_Blood_Queen_Lanathel()
{
     Script *newscript;

     newscript = new Script;
     newscript->Name = "npc_Blood_Queen_Lanathel";
     newscript->GetAI = &GetAI_Boss_Blood_Queen_Lanathel;
     newscript->RegisterSelf();

	 newscript = new Script;
     newscript->Name = "mob_swarming_shadows";
     newscript->GetAI = &GetAI_mob_swarming_shadows;
     newscript->RegisterSelf();
}