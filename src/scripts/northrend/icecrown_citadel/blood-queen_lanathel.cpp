 /* Copyright (C) 2009 - 2010 TrinityCore <http://www.trinitycore.org/>
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
	SPELL_SHROUD_OF_SORROW			=	72982,
	SPELL_DELIRIOUS_SLASH			=	71623,
	SPELL_BLOOD_MIRROR_1			=	70821,
	SPELL_BLOOD_MIRROR_2			=	71510,
	SPELL_VAMPIRIC_BITE				=	71726,
	SPELL_PACT_OF_THE_DARKFALLEN_1	=	71340, 
	SPELL_PACT_OF_THE_DARKFALLEN_2	=	71341, 
	SPELL_SWARMING_SHADOWS			=	71264, // Spawn Creature 38163 need Visual Spell ID :/
	SPELL_TWILIGHT_BLOODBOLT		=	71446, 
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

struct boss_blood_queen_lanathelAI : public ScriptedAI
{
    boss_blood_queen_lanathelAI(Creature *pCreature) : ScriptedAI(pCreature)
	{
        m_pInstance = pCreature->GetInstanceData();
    }

	ScriptedInstance* m_pInstance;

	uint32 m_uiTwilightBloodboltTimer;
	uint32 m_uiPhaseTimer;
	uint32 m_uiVampBiteTimer;
	uint32 m_uiSwarmingShadowsTimer;
	uint32 m_uiPactofDarkfallenTimer;
    uint32 m_uiPactofDarkfallenExplodeTimer;
	uint32 m_uiPactofDarkfallenExplosionCount;
	uint32 m_uiPhase;
	uint32 m_uiBerserkTimer;		
	uint32 m_uiSetHoverTimer;
	uint32 m_uiLandingTimer;
	uint32 m_uiFlyingFalseTimer;
	uint32 m_uiBloodboldSplashTimer;

	void Reset()
	{
		m_uiPhaseTimer = 90000;

		m_uiTwilightBloodboltTimer = 10000;
		m_uiVampBiteTimer	= 15000;
		m_uiSwarmingShadowsTimer = 30000;
		m_uiPactofDarkfallenTimer	= 5000;
		m_uiSetHoverTimer = 90000;
		m_uiBloodboldSplashTimer = 900000;
		m_uiLandingTimer = 900000;
		m_uiFlyingFalseTimer = 900000;
		
		/************ Other **********/
		m_uiPhase = 1;
		m_uiBerserkTimer	= 330000;
		me->SetFlying(false);

		me->SetReactState(REACT_AGGRESSIVE);

		if (m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_QUEEN_LANATHEL_EVENT, NOT_STARTED);
	}

	void EnterCombat(Unit *who)
	{
		DoScriptText(SAY_AGGRO, me);

		if (m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_QUEEN_LANATHEL_EVENT, IN_PROGRESS);
	}

	void KilledUnit(Unit* victim)
	{
	}

	void JustDied(Unit* Killer)
	{
		DoScriptText(SAY_DEATH, me);

		if (m_pInstance)
            m_pInstance->SetData(DATA_BLOOD_QUEEN_LANATHEL_EVENT, DONE);
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
					m_uiPactofDarkfallenExplodeTimer = 2000;
					m_uiPactofDarkfallenTimer = 25000;
				} 
				else m_uiPactofDarkfallenTimer -= uiDiff;

				if (m_uiSwarmingShadowsTimer < uiDiff)
				{
					if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(target, SPELL_SWARMING_SHADOWS);
					m_uiSwarmingShadowsTimer  = 30000;
				}
				else m_uiSwarmingShadowsTimer -= uiDiff;

				if (m_uiTwilightBloodboltTimer < uiDiff)
				{
					if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(target, SPELL_TWILIGHT_BLOODBOLT);
					m_uiTwilightBloodboltTimer  = 9000;
				}
				else m_uiTwilightBloodboltTimer -= uiDiff;

				if (m_uiVampBiteTimer < uiDiff)
				{
					DoScriptText(RAND(SAY_VAMP_BITE_1,SAY_VAMP_BITE_2), me);
					if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1))
					DoCast(target, SPELL_VAMPIRIC_BITE);
					m_uiVampBiteTimer	= 45000+rand()%20000;
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
					m_uiPhaseTimer	= 90000;
					m_uiSetHoverTimer = 5000;
					m_uiBloodboldSplashTimer = 10000;
					m_uiLandingTimer = 26000;
					m_uiFlyingFalseTimer = 30000;
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
				m_uiSetHoverTimer	= 90000;
			}
			else m_uiSetHoverTimer -= uiDiff;

			if (m_uiBloodboldSplashTimer < uiDiff)
			{
               std::list<Unit*> pTargets;
                    SelectTargetList(pTargets, 5, SELECT_TARGET_RANDOM, 80, true);
                    for (std::list<Unit*>::const_iterator i = pTargets.begin(); i != pTargets.end(); ++i)
                        DoCast(*i, RAID_MODE(SPELL_TWILIGHT_BLOODBOLT,SPELL_TWILIGHT_BLOODBOLT));
				m_uiBloodboldSplashTimer = 2000;
			}
			else m_uiBloodboldSplashTimer -= uiDiff;

			if (m_uiLandingTimer < uiDiff)
			{
				me->GetMotionMaster()->MovePoint(0, Phase2Position);
				me->SetUnitMovementFlags(MOVEMENTFLAG_JUMPING);
				m_uiLandingTimer	= 900000;
				m_uiBloodboldSplashTimer = 900000;
			}
			else m_uiLandingTimer -= uiDiff;

			if (m_uiFlyingFalseTimer < uiDiff)
			{
				me->SetFlying(false);
				me->RemoveAllAuras();
				me->SetReactState(REACT_AGGRESSIVE);
				me->GetMotionMaster()->MoveChase(me->getVictim());
				m_uiPhase = 1;
				m_uiFlyingFalseTimer = 900000;
			}
			else m_uiFlyingFalseTimer -= uiDiff;
		}

		DoMeleeAttackIfReady();
	}
};

struct npc_swarming_shadowsAI : public Scripted_NoMovementAI
{
    npc_swarming_shadowsAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
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

CreatureAI* GetAIboss_blood_queen_lanathelAI(Creature* pCreature)
{
    return new boss_blood_queen_lanathelAI (pCreature);
}

CreatureAI* GetAInpc_swarming_shadowsAI(Creature* pCreature)
{
    return new npc_swarming_shadowsAI (pCreature);
}

void AddSC_boss_blood_queen_lanathel()
{
     Script *newscript;

     newscript = new Script;
     newscript->Name = "boss_blood_queen_lanathel";
     newscript->GetAI = &GetAIboss_blood_queen_lanathelAI;
     newscript->RegisterSelf();

     newscript = new Script;
     newscript->Name = "npc_swarming_shadows";
     newscript->GetAI = &GetAInpc_swarming_shadowsAI;
     newscript->RegisterSelf();
}