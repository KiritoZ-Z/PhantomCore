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

const Position FlyPosition = { 4523.889160, 2486.907227, 280.249878, 3.155010};
const Position LandPosition = { 4407.439453, 2484.90541,  203.374374, 3.166791};
const Position SpawnPosition = {4408.052734f, 2484.825439f, 203.374207f, 3.166791};

enum Yells
{
    SAY_AGGRO              =    -1666071,
    SAY_UNCHAIND_MAGIC     =    -1666072,
    SAY_BLISTERING_COLD    =    -1666073,
    SAY_BREATH             =    -1666074,
    SAY_AIR_PHASE          =    -1666075,
    SAY_PHASE_3            =    -1666076,
    SAY_KILL_1             =    -1666077,
    SAY_KILL_2             =    -1666078,
    SAY_BERSERK            =    -1666079,
    SAY_DEATH              =    -1666080,
};

enum Spells
{
    SPELL_FROST_AURA_10_NORMAL      =    70084,
    SPELL_FROST_AURA_25_NORMAL      =    71050,
    SPELL_CLEAVE                    =    19983,
    SPELL_TAIL_SMASH                =    71077,
    SPELL_FROST_BREATH_10_NORMAL    =    73061,
    SPELL_FROST_BREATH_25_NORMAL    =    71056,
    SPELL_PERMEATING_CHILL          =    70109,
    SPELL_UNCHAINED_MAGIC           =    69762,
    SPELL_ICY_GRIP_TRIGGER          =    70117,
    SPELL_ICY_TRIP_PULL             =    70122,
    SPELL_BLISTERING_COLD_10_NORMAL =    70123,
    SPELL_BLISTERING_COLD_25_NORMAL =    71047,
    SPELL_FROST_BOMB_TRIGGER        =    69846,
    SPELL_FROST_BEACON              =    70126,
    SPELL_ICE_TOMB                  =    69712, 
    SPELL_ICE_TOMB_TRIGGER          =    69675, 
    SPELL_FROST_BOMB                =    71053,
    SPELL_MYSTIC_BUFFED             =    70128,
    SPELL_BERSERK                   =    47008,
};

enum Mobs
{
    NPC_ICE_TOMB    =    36980,
    NPC_FROST_BOMB  =    37186,
};

enum Phase
{
    PHASE_LAND   = 1,
    PHASE_FLY    = 2, 
    PHASE_ENRAGE = 3,  
};

struct boss_sindragosaAI : public ScriptedAI
{
    boss_sindragosaAI(Creature*pCreature) : ScriptedAI (pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    uint8 Phase;

    uint32 m_uiBreathTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiTailSmashTimer;
    uint32 m_uiBlisteringColdTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiMarkTimer;
    uint32 m_uiIceBoltTriggerTimer;
    uint32 FlyTimer;

    void Reset()
    {
        Phase = PHASE_LAND;

        m_uiBreathTimer = 15000;
        m_uiTailSmashTimer = 10000;
        m_uiBlisteringColdTimer = 30000;
        m_uiMarkTimer = 20000;

        me->SetSpeed(MOVE_WALK, 1.5f, true);
        me->SetSpeed(MOVE_RUN, 1.5f, true);
        me->SetSpeed(MOVE_FLIGHT, 2.5f, true);

        if (m_pInstance)
            m_pInstance->SetData(DATA_SINDRAGOSA_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        DoCast(me, RAID_MODE(SPELL_FROST_AURA_10_NORMAL,SPELL_FROST_AURA_25_NORMAL));

        if (m_pInstance)
            m_pInstance->SetData(DATA_SINDRAGOSA_EVENT, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {  
        DoScriptText(SAY_DEATH, me);

        if (m_pInstance)
            m_pInstance->SetData(DATA_SINDRAGOSA_EVENT, DONE);
    }

    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(DATA_SINDRAGOSA_EVENT, FAIL);  
    }

    void MarkPlayer()
    { 
        Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);
        if (pTarget && !pTarget->HasAura(SPELL_FROST_BEACON))
        {
            DoCast(pTarget, SPELL_FROST_BEACON, true);
        }
    }

    void CastIceBlockTrigger()
    {
        Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);

        if (pTarget && pTarget->HasAura(SPELL_FROST_BEACON))
        {
            DoCast(pTarget, SPELL_ICE_TOMB_TRIGGER);
        }
    }

    void BlisteringCold()
    {
        Map* pMap = me->GetMap();
        Map::PlayerList const &PlayerList = pMap->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* i_pl = i->getSource())
                if (i_pl->isAlive())
                    i_pl->TeleportTo(me->GetMapId(), me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(), 0, TELE_TO_NOT_LEAVE_COMBAT);
        DoCast(me, SPELL_ICY_GRIP_TRIGGER);
        DoCast(me, RAID_MODE(SPELL_BLISTERING_COLD_10_NORMAL,SPELL_BLISTERING_COLD_25_NORMAL));
    }

    void TakeOff()
    {
        me->InterruptSpell(CURRENT_GENERIC_SPELL);
        me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);

        FlyTimer = 52000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if(Phase == PHASE_LAND || Phase == PHASE_ENRAGE)
        {
            if (m_uiBreathTimer <= uiDiff)
            {
                DoCast(me, RAID_MODE(SPELL_FROST_BREATH_10_NORMAL,SPELL_FROST_BREATH_25_NORMAL));
                m_uiBreathTimer = 15000;
            } else m_uiBreathTimer -= uiDiff;

            if (m_uiCleaveTimer <= uiDiff)
            {
                DoCast(me, SPELL_CLEAVE);
                m_uiCleaveTimer = 10000;
            } else m_uiCleaveTimer -= uiDiff;

            if (m_uiTailSmashTimer <= uiDiff)
            {
                TakeOff();//DoCast(me, SPELL_TAIL_SMASH);
                m_uiTailSmashTimer = 8000;
            } else m_uiTailSmashTimer -= uiDiff;

            if (m_uiBlisteringColdTimer <= uiDiff)
            {
                BlisteringCold();
                m_uiBlisteringColdTimer = 30000;
            } else m_uiBlisteringColdTimer -= uiDiff;
        }

        if(Phase == PHASE_LAND)
        {
            // Todo: Flyphase
        }

        if(Phase == PHASE_FLY)
        {    
            if (m_uiMarkTimer < uiDiff)
            {
                MarkPlayer();
                m_uiMarkTimer = 20000;
                m_uiIceBoltTriggerTimer = 5000;
            } else m_uiMarkTimer -= uiDiff;

            if (m_uiIceBoltTriggerTimer < uiDiff)
            {
                CastIceBlockTrigger();
                m_uiIceBoltTriggerTimer = 31000;
            } else m_uiIceBoltTriggerTimer -= uiDiff;
        }

        if(Phase == PHASE_ENRAGE)
        {
        }

        if(Phase == PHASE_LAND || Phase == PHASE_FLY)
        {
            if((me->GetHealth()*100) / me->GetMaxHealth() < 35)
            {
                Phase = PHASE_ENRAGE;
                DoCast(me, SPELL_MYSTIC_BUFFED);
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

void AddSC_boss_sindragosa()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sindragosa";
    newscript->GetAI = &GetAI_boss_sindragosa;
    newscript->RegisterSelf();
}

