/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptedPch.h"
#include "instance_icecrown_citadel.h"

enum Yells
{
	SAY_STINKI_DIES			=	-1666004,
	SAY_AGGRO				=	-1666005,
	SAY_GAS_CLOUD			=	-1666006,
	SAY_GAS_SPORES			=	-1666007,
	SAY_PUNGENT_BLIGHT_1	=	-1666008,
	SAY_PUNGENT_BLIGHT_2	=	-1666009,
	SAY_KILL_1				=	-1666010,
	SAY_KILL_2				=	-1666011,
	SAY_BERSERK				=	-1666012,
	SAY_DEATH_1				=	-1666013,
	SAY_DEATH_2				=	-1666014, 
};

enum Spells
{
	N_10_SPELL_PUNGENT_BLIGHT		=	69195, // 
	N_10_SPELL_GASTRIC_EXPLOSION	=	72227, //
	SPELL_INHALE_BLIGHT				=	69165, //
	N_10_SPELL_VILE_GAS				=	72272, // Random
	N_10_SPELL_GASTRIC_BLOAT		=	72219, // Tank
	SPELL_GAS_VISUAL_SMAL			=	69154, //
	SPELL_GAS_VISUAL_MIDDEL			=	69152, //
	SPELL_GAS_VISUAL_BIG			=	69126, // 69157 ?? :/
	SPELL_GAS_SPORES				=	69279, // 10 Version 2 Player 25 Version 3 Player
	SPELL_BERSERK					=	47008, // 5 Min :O
	SPELL_INOCULATED				=	72103, // Impfung
	SPELL_BLIGHTED_SPORES			=	69290, 
};

struct Boss_FestergutAI : public ScriptedAI
{
    Boss_FestergutAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = pCreature->GetInstanceData();
    }
    
    ScriptedInstance* m_pInstance;

	uint32 m_uiPungentBlightTimer;
	uint32 m_uiGastricExplosionTimer;
	uint32 m_uiInhaleBlightTimer;
	uint32 m_uiGasSporesTimer;
	uint32 m_uiVileGasTimer;
	uint32 m_uiGastricBloatTimer;

    void Reset()
    {
		m_uiPungentBlightTimer	= 120000;
		m_uiInhaleBlightTimer 	= 33000;
		m_uiVileGasTimer = urand(10000,15000);
		m_uiGasSporesTimer = 20000;
		m_uiGastricBloatTimer = 10000;
    }

    void EnterCombat(Unit* who)
    {
		DoScriptText(SAY_AGGRO, me);
    }

	void JustDied(Unit* killer)
    {  
		DoScriptText(SAY_DEATH_1, me);
    }

	void KilledUnit(Unit *victim)
    {
       DoScriptText(RAND(SAY_KILL_1,SAY_KILL_2), me);
    }

	void SpellHitTarget(Unit *pTarget,const SpellEntry* spell)
    {
        switch(spell->Id)
        {
			case SPELL_GAS_SPORES:
            HandleTouchedSpells(pTarget, SPELL_BLIGHTED_SPORES);
            break;
        }
    }

	void HandleTouchedSpells(Unit *pTarget, uint32 TouchedType)
    {
        switch(TouchedType)
        {
        case SPELL_GAS_SPORES:
                if (pTarget->HasAura(SPELL_BLIGHTED_SPORES))
                {
                    pTarget->CastSpell(pTarget, SPELL_INOCULATED, true);
                } else pTarget->CastSpell(pTarget, SPELL_INOCULATED, true);
				break;
		 }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;
	
	if (m_uiGastricBloatTimer < uiDiff)
	{
		DoCast(me->getVictim(), N_10_SPELL_GASTRIC_BLOAT);
		m_uiGastricBloatTimer = 15000;
	}
	else m_uiGastricBloatTimer -= uiDiff;

	if(m_uiInhaleBlightTimer < uiDiff)
	{
		DoCast(me, SPELL_INHALE_BLIGHT);
		m_uiInhaleBlightTimer = 33000;
	}
	else m_uiInhaleBlightTimer -= uiDiff;

	if (m_uiVileGasTimer < uiDiff)
	{
		if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
		DoCast(target, N_10_SPELL_VILE_GAS);
		m_uiVileGasTimer = 30000;
	}
	else m_uiVileGasTimer -= uiDiff;

	if (m_uiGasSporesTimer < uiDiff)
	{
		if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
		target->CastSpell(target, SPELL_GAS_SPORES, true);
		m_uiGasSporesTimer = 35000;
	}
	else m_uiGasSporesTimer -= uiDiff;

	if (m_uiPungentBlightTimer < uiDiff)
	{
		DoCastAOE(N_10_SPELL_PUNGENT_BLIGHT);
		m_uiPungentBlightTimer = 120000;
		m_uiInhaleBlightTimer = 33000;
		me->RemoveAllAuras();
	}
	else m_uiPungentBlightTimer -= uiDiff;

	DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_Boss_Festergut(Creature* pCreature)
{
    return new Boss_FestergutAI(pCreature);
}

void AddSC_Boss_Festergut()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "Boss_Festergut";
    NewScript->GetAI = &GetAI_Boss_Festergut;
    NewScript->RegisterSelf();
}