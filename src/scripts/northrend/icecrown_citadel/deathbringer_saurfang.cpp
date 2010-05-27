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

enum Yells
{
    SAY_AGGRO           = -1665973,
    SAY_FALLENCHAMPION  = -1665974,
    SAY_BLOODBEASTS     = -1665975,
    SAY_KILL1           = -1665976,
    SAY_KILL2           = -1665977,
    SAY_BERSERK         = -1665978,
    SAY_DEATH           = -1665979,
};

enum SaurfangSpells
{
    SPELL_BLOOD_LINK			= 72178,
	SPELL_SUMMON_BLOOD_BEAST    = 72172,
	SPELL_FRENZY				= 72737,
	SPELL_BLOOD_NOVA_TRIGGER	= 72378,
	SPELL_TASTE_OF_BLOOD		= 69634,
	SPELL_CLEAVE				= 15284,
	SPELL_RENDING_THROW			= 70309,
    N_10_SPELL_FALLEN_CHAMPION  = 72293,
    SPELL_BOILING_BLOOD			= 72385, // Melees

    N_10_SPELL_BLOOD_NOVA       = 72380,
	N_25_SPELL_BLOOD_NOVA		= 72438,
	H_10_SPELL_BLOOD_NOVA		= 72380,
	H_25_SPELL_BLOOD_NOVA		= 72380,
    N_SPELL_RUNE_OF_BLOOD       = 72408,

};

Creature* pSaurfang;

enum BloodBeastSpells
{
	SPELL_BLOOD_LINK_BEAST	=	72176,
	SPELL_RESISTAN_SKIN		=	72723,
	SPELL_SCENT_OF_BLOOD	=	72769, // Hardmode
};

enum Creatures
{
	CREATURE_BLOOD_BEAST	=	38508,
};

float BloodBeastSpawnLocation[4][3] =
{
    {-494.024536, 2217.747314, 541.114075},
    {-493.856537, 2208.298828, 541.114075},
    {-493.966431, 2214.478760, 541.114075},
    {-493.794312, 2204.799316, 541.114075}
};

struct boss_saurfangAI : public ScriptedAI
{
    boss_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature), summons(me)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		pSaurfang = me;
    }

    ScriptedInstance* m_pInstance;
 
	uint32 m_uiFallenChampionTimer;
	uint32 m_uiBoilingBloodTimer;
	uint32 m_uiBloodNovaChannelTimer;
	uint32 m_uiBloodNovaDamageTimer;
	uint32 m_uiRuneOfBloodTimer;
	uint32 m_uiSummonBloodBeastTimer;
	uint32 m_uiBerserkTimer;

	bool m_bIsEnrage;

	SummonList summons;

    void Reset()
    {
		m_uiFallenChampionTimer = RAID_MODE(60000,30000,60000,30000);
		m_uiBoilingBloodTimer	= 15000;
		m_uiBloodNovaChannelTimer = 22000;
		m_uiBloodNovaDamageTimer = 25000;
		m_uiRuneOfBloodTimer = urand (20000,25000);
		m_uiSummonBloodBeastTimer = 40000;

		m_bIsEnrage = false;

		if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG, NOT_STARTED);
    }

    void EnterCombat(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG_EVENT, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, me);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG_EVENT, DONE);

        DoScriptText(SAY_DEATH, me);

    Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

    if (PlList.isEmpty())
            return;

    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
    {
        if (Player* pPlayer = i->getSource())
        {
		    if (pPlayer && pPlayer->HasAura(72293))
                pPlayer->RemoveAurasDueToSpell(72293);
        }
	}
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG_EVENT, NOT_STARTED);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,1))
        {
            case 0: DoScriptText(SAY_KILL1, me); break;
            case 1: DoScriptText(SAY_KILL2, me); break;
        }
		me->ModifyHealth(me->GetMaxHealth() * 0.10);
		if (pVictim && pVictim->HasAura(72293))
			pVictim->RemoveAurasDueToSpell(72293);
    }

	void JustSummoned(Creature *summon)
    {
		if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
        summon->AI()->AttackStart(pTarget);

		summons.Summon(summon);
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if(!UpdateVictim())
            return;

	   if (!me->HasAura(SPELL_BLOOD_LINK))
			DoCast(me, SPELL_BLOOD_LINK);

		if (m_uiBoilingBloodTimer < uiDiff)
		{
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1))
			DoCast(target, SPELL_BOILING_BLOOD);
			me->ModifyHealth(me->GetMaxHealth() * 0.02);
		    //me->SetPower(me->getPowerType(), 10);
			//me->ModifyPower(POWER_MANA, -mana);
			m_uiBoilingBloodTimer = 20000;
		}
		else m_uiBoilingBloodTimer -= uiDiff;

		if (m_uiBloodNovaChannelTimer < uiDiff)
		{
			DoCast(me, SPELL_BLOOD_NOVA_TRIGGER);
			m_uiBloodNovaChannelTimer = 22000;
			m_uiBloodNovaDamageTimer = 2000;
		}
		else m_uiBloodNovaChannelTimer -= uiDiff;
	
		if (m_uiBloodNovaDamageTimer < uiDiff)
		{
			uint32 count = RAID_MODE(1,3,1,3);
            for (uint8 i = 1; i <= count; i++)
            {
				if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
				DoCast(target, RAID_MODE(N_10_SPELL_BLOOD_NOVA,N_25_SPELL_BLOOD_NOVA,H_10_SPELL_BLOOD_NOVA,H_25_SPELL_BLOOD_NOVA));
				me->ModifyHealth(me->GetMaxHealth() * 0.02);
				m_uiBloodNovaDamageTimer = 90000;
			}
		}
		else m_uiBloodNovaDamageTimer -= uiDiff;

		if (m_uiSummonBloodBeastTimer <= uiDiff)
        {
			for (uint8 i = 0; i < RAID_MODE(1,4,1,4); ++i)
			{
				DoCast(me, SPELL_SUMMON_BLOOD_BEAST);
				me->SummonCreature(CREATURE_BLOOD_BEAST, BloodBeastSpawnLocation[i][0],BloodBeastSpawnLocation[i][1],BloodBeastSpawnLocation[i][2],BloodBeastSpawnLocation[i][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000);
				DoScriptText(SAY_BLOODBEASTS, me);
				me->ModifyHealth(me->GetMaxHealth() * 0.02);
				m_uiSummonBloodBeastTimer = 40000;
			}
        } 
		else m_uiSummonBloodBeastTimer -= uiDiff;

		if (m_uiFallenChampionTimer < uiDiff)
		{
			DoScriptText(SAY_FALLENCHAMPION, me);
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
			DoCast(target, N_10_SPELL_FALLEN_CHAMPION);
			me->ModifyHealth(me->GetMaxHealth() * 0.09);
			m_uiFallenChampionTimer	= RAID_MODE(60000,30000,60000,30000);
		}
		else m_uiFallenChampionTimer -= uiDiff;

		if (m_uiRuneOfBloodTimer < uiDiff)
		{
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
			DoCast(target, N_SPELL_RUNE_OF_BLOOD);
			me->ModifyHealth(me->GetMaxHealth() * 0.05);
			m_uiRuneOfBloodTimer = 25000;
		}
		else m_uiRuneOfBloodTimer -= uiDiff;

		if(!m_bIsEnrage && (me->GetHealth()*100) / me->GetMaxHealth() < 30)
		{
			DoCast(me, SPELL_FRENZY);
			m_bIsEnrage = true;
		}

        DoMeleeAttackIfReady();
    }
};

struct npc_bloodbeastAI : public ScriptedAI
{
    npc_bloodbeastAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		DoCast(me, SPELL_BLOOD_LINK_BEAST);
		DoCast(me, SPELL_RESISTAN_SKIN);
    }

    ScriptedInstance* m_pInstance;

		uint32 m_uiSetPowerTimer;

    void Reset()
    {
		m_uiSetPowerTimer = 2000;
    }

    void EnterCombat(Unit* pWho)
    {
    }

    void KilledUnit(Unit* pVictim)
    { 
		if (Creature* Saurfang = me->GetCreature(*me, m_pInstance->GetData64(DATA_SAURFANG)))
        {
			Saurfang->ModifyHealth(Saurfang->GetMaxHealth() * 0.10);
		}
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if(!UpdateVictim())
            return;

	   		if (m_uiSetPowerTimer <= uiDiff)
		{
			pSaurfang->ModifyPower(pSaurfang->getPowerType(), +1);
			m_uiSetPowerTimer = 2000;
		} else m_uiSetPowerTimer -= uiDiff;

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_saurfang(Creature* pCreature)
{
    return new boss_saurfangAI(pCreature);
}

CreatureAI* GetAI_npc_bloodbeast(Creature* pCreature)
{
    return new npc_bloodbeastAI(pCreature);
}

void AddSC_boss_saurfang()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_saurfang";
    NewScript->GetAI = &GetAI_boss_saurfang;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "npc_bloodbeast";
    NewScript->GetAI = &GetAI_npc_bloodbeast;
    NewScript->RegisterSelf();
}