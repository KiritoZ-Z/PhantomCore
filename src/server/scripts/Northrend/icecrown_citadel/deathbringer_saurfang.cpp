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

enum Yells
{
    SAY_AGGRO           = -1665973,
    SAY_FALLENCHAMPION  = -1665974,
    SAY_BLOODBEASTS     = -1665975,
    SAY_KILL1           = -1665976,
    SAY_KILL2           = -1665977,
    SAY_BERSERK         = -1665978,
    SAY_DEATH           = -1665979,
    SAY_INTRO_1_A       = -1665960,
    SAY_INTRO_2_A       = -1665961,
    SAY_INTRO_3_A       = -1665962,
    SAY_INTRO_4_A       = -1665963,
    SAY_INTRO_1_H       = -1665964,
    SAY_INTRO_2_H       = -1665965,
    SAY_INTRO_3_H       = -1665966,
    SAY_INTRO_4_H       = -1665967,
    SAY_INTRO_5_H       = -1665968,
    SAY_INTRO_6_H       = -1665969,
    SAY_INTRO_7_H       = -1665970,
    SAY_INTRO_8_H       = -1665972,
};

enum SaurfangSpells
{
    SPELL_BLOOD_LINK              = 72178,
    SPELL_BLOOD_POWER             = 72371,
    SPELL_SUMMON_BLOOD_BEAST_1    = 72172,
    SPELL_SUMMON_BLOOD_BEAST_2    = 72173,
    SPELL_SUMMON_BLOOD_BEAST_3    = 72356,
    SPELL_SUMMON_BLOOD_BEAST_4    = 72357,
    SPELL_SUMMON_BLOOD_BEAST_5    = 72358,
    SPELL_FRENZY                  = 72737,
    SPELL_TASTE_OF_BLOOD          = 69634,
    SPELL_CLEAVE                  = 15284,
    SPELL_RENDING_THROW           = 70309,
    SPELL_FALLEN_CHAMPION         = 72293,
    SPELL_FALLEN_CHAMPION_AURA    = 72260,
    SPELL_BOILING_BLOOD_10_NORMAL = 72385,
    SPELL_BOILING_BLOOD_25_NORMAL = 72441,
    SPELL_BLOOD_NOVA_10_NORMAL    = 72378,
    SPELL_BLOOD_NOVA_25_NORMAL    = 73058,
    SPELL_BLOOD_NOVA_10_HEROIC    = 72380,
    SPELL_BLOOD_NOVA_25_HEROIC    = 72380,
    SPELL_RUNE_OF_BLOOD_10_NORMAL = 72410,
    SPELL_RUNE_OF_BLOOD_25_NORMAL = 72410,
    SPELL_RUNE_OF_BLOOD_10_HEROIC = 72448,
    SPELL_RUNE_OF_BLOOD_25_HEROIC = 72448,
    SPELL_BLOOD_LINK_BEAST        = 72176,
    SPELL_RESISTAN_SKIN           = 72723,
    SPELL_SCENT_OF_BLOOD          = 72769,
    SPELL_BERSERK                 = 26662,
    SPELL_AGONY                   = 70572,
    SPELL_ZERO_POWER              = 72242,
};

enum Achievements
{
    ACHIEV_MESS_10               = 4537,
    ACHIEV_MESS_25               = 4613,
};

struct boss_saurfangAI : public ScriptedAI
{
    boss_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiBoilingBloodTimer;
    uint32 m_uiBloodNovaTimer;
    uint32 m_uiRuneOfBloodTimer;
    uint32 m_uiSummonBloodBeastTimer;
    uint32 m_uiBerserkTimer;

    bool Achievement;

    void Reset()
    {
        m_uiBoilingBloodTimer    = 20000;
        m_uiBloodNovaTimer = 20000;
        m_uiRuneOfBloodTimer = 30000;
        m_uiSummonBloodBeastTimer = 40000;
        m_uiBerserkTimer = 480000;

        Achievement = false;

        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG, NOT_STARTED);

        me->SetPower(me->getPowerType(), 0);
        me->RemoveAllAuras();

        DoCast(me, SPELL_ZERO_POWER);
    }

    void EnterCombat(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);

        DoCast(me, SPELL_BLOOD_LINK);
        DoCast(me, SPELL_BLOOD_POWER);

        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG_EVENT, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG_EVENT, DONE);

        DoScriptText(SAY_DEATH, me);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(DATA_SAURFANG_EVENT, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 1))
        {
        case 0:
            DoScriptText(SAY_KILL1, me);
            break;
        case 1:
            DoScriptText(SAY_KILL2, me);
            break;
        }

        switch(0)
        {
        case 0:
            if (pVictim->HasAura(72293))
            {
                DoCast(me, SPELL_FALLEN_CHAMPION_AURA);

                if(!Achievement)
                m_pInstance->DoCompleteAchievement(RAID_MODE(ACHIEV_MESS_10,ACHIEV_MESS_25));
                Achievement = true;
            }
            break;
        }
    }

    void JustSummoned(Creature *pSummoned)
    {
        Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0); 
        pSummoned->AddThreat(pTarget, 100.0f);
        pSummoned->GetMotionMaster()->MoveChase(pTarget);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!UpdateVictim())
            return;

        if (!m_pInstance || m_pInstance->GetData(DATA_SAURFANG_EVENT) != IN_PROGRESS) 
        {
            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
            {
                if (Player* pPlayer = i->getSource())
                {
                    pPlayer->RemoveAurasDueToSpell(72293);
                }
            }
        }

        if (m_uiBoilingBloodTimer < uiDiff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_BOILING_BLOOD_10_NORMAL);
            m_uiBoilingBloodTimer = 20000;
        } else m_uiBoilingBloodTimer -= uiDiff;


        if (m_uiBloodNovaTimer < uiDiff)
        {
            uint32 count = RAID_MODE(1,3,1,3);
            for (uint8 i = 1; i <= count; i++)
            {
                Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                DoCast(pTarget, RAID_MODE(SPELL_BLOOD_NOVA_10_NORMAL,SPELL_BLOOD_NOVA_25_NORMAL,SPELL_BLOOD_NOVA_10_HEROIC,SPELL_BLOOD_NOVA_25_HEROIC));
                m_uiBloodNovaTimer = 20000;
            }
        } else m_uiBloodNovaTimer -= uiDiff;

        if (m_uiSummonBloodBeastTimer <= uiDiff)
        {
            DoCast(me, SPELL_SUMMON_BLOOD_BEAST_1);
            DoCast(me, SPELL_SUMMON_BLOOD_BEAST_2);
            if (getDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
            {
                DoCast(me, SPELL_SUMMON_BLOOD_BEAST_3);
                DoCast(me, SPELL_SUMMON_BLOOD_BEAST_4);
                DoCast(me, SPELL_SUMMON_BLOOD_BEAST_5);
            }
            DoScriptText(SAY_BLOODBEASTS, me);
            m_uiSummonBloodBeastTimer = 40000;
        } else m_uiSummonBloodBeastTimer -= uiDiff;

        if (me->GetPower(POWER_ENERGY) == 100)
        {
            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            DoCast(pTarget, SPELL_FALLEN_CHAMPION);
            DoScriptText(SAY_FALLENCHAMPION, me);
            me->SetPower(me->getPowerType(), 0);
        }

        if (m_uiRuneOfBloodTimer < uiDiff)
        {
            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);
            DoCast(pTarget, RAID_MODE(SPELL_RUNE_OF_BLOOD_10_NORMAL,SPELL_RUNE_OF_BLOOD_25_NORMAL,SPELL_RUNE_OF_BLOOD_10_HEROIC,SPELL_RUNE_OF_BLOOD_25_HEROIC));
            me->ModifyHealth(me->GetMaxHealth() * 0.05);
            m_uiRuneOfBloodTimer = 30000;
        } else m_uiRuneOfBloodTimer -= uiDiff;

        if(me->GetHealth()*100 / me->GetMaxHealth() < 30)
        {
            if (!me->HasAura(SPELL_FRENZY))
            {
                DoCast(me, SPELL_FRENZY);
            }
        }

        if (m_uiBerserkTimer < uiDiff)
        {
            DoCast(SPELL_BERSERK);
			DoScriptText(SAY_BERSERK, me);
            m_uiBerserkTimer = 480000;
        } else m_uiBerserkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct npc_bloodbeastAI : public ScriptedAI
{
    npc_bloodbeastAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    bool ScentOfBlood;

    uint32 m_uiScentOfBloodTimer;

    void Reset()
    {
        DoCast(me, SPELL_BLOOD_LINK_BEAST);
        DoCast(me, SPELL_RESISTAN_SKIN);

        ScentOfBlood = false;

        m_uiScentOfBloodTimer = 5000;
    }

    void EnterCombat(Unit* pWho)
    {
    }

    void KilledUnit(Unit* pVictim)
    { 
        if (Creature* Saurfang = me->GetCreature(*me, m_pInstance->GetData64(DATA_SAURFANG)))
        {
            Saurfang->ModifyHealth(Saurfang->GetMaxHealth() * 0.01);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!UpdateVictim())
            return;

        if (!m_pInstance || m_pInstance->GetData(DATA_SAURFANG_EVENT) != IN_PROGRESS) 
            me->ForcedDespawn();

     if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
     {
        if (m_uiScentOfBloodTimer < uiDiff)
        {
            if (!ScentOfBlood && (me->GetHealth()*100) / me->GetMaxHealth() < 20)
            {
                if (urand(0,1))
                DoCast(me, SPELL_SCENT_OF_BLOOD);
                ScentOfBlood = true;
                m_uiScentOfBloodTimer = 5000;
            }
        } else m_uiScentOfBloodTimer -= uiDiff;
	 }

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
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_saurfang";
    newscript->GetAI = &GetAI_boss_saurfang;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bloodbeast";
    newscript->GetAI = &GetAI_npc_bloodbeast;
    newscript->RegisterSelf();
}