/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: boss_anubarak_trial
SD%Complete: 70%
SDComment: by /dev/rsa
SDCategory:
EndScriptData */


#include "ScriptedPch.h"
#include "trial_of_the_crusader.h"

enum Yells
{
        SAY_AGGRO                       =       -1600247,               //This place will serve as your tomb!
        SAY_SUBMERGE            =       -1600248,               //Auum na-l ak-k-k-k, isshhh. Rise, minions. Devour...
        SAY_LEECHING_SWARM      =       -1600249,               //The swarm shall overtake you!
        SAY_KILL_1                      =       -1600250,               //F-lakkh shir!
        SAY_KILL_2                      =       -1600251,               //Another soul to sate the host.
        SAY_DEATH                       =       -1600252,               //I have failed you, master...
        SAY_OUTRO                       =       -1600253,               //Highlord Tirion Fordring yells: Champions, you're alive! Not only have you defeated every challenge of the Trial of the Crusader, but thwarted Arthas directly! Your skill and cunning will prove to be a powerful weapon against the Scourge. Well done! Allow one of my mages to transport you back to the surface!
};

enum AnubarakSpells
{
        SPELL_FREEZING_SLASH                            =       66012,
        SPELL_PENETRATING_COLD_25_NORMAL        =       68510,
        SPELL_PENETRARING_COLD_10_NORMAL        =       67700,
        SPELL_SUBMERGE                                          =       53421,
        SPELL_PURSUED                                           =       67574,
        SPELL_IMPALE_25                                         =       67859,
        SPELL_SUMMON_SCARAB                                     =       66340,
        SPELL_LEECHING_SWARM                            =       67630,
        SPELL_IMPALE_TRIGGER                            =       65921,
        SPELL_IMPALE_RANGE                                      =       65922,
        SPELL_BERSERK                                           =       47008
};

enum NerubianSpells
{
        SPELL_NERUBIAN_GROUND_VISUAL    =       66324
};

enum Summons
{
        CREATURE_NERUBIAN_BURROWERS                     =       34607,
        CREATURE_SWARM_SCARABS                          =       34605,
        CREATURE_FROST_SPHERES                          =       34606,
        CREATURE_IMPALE                                         =       29184,
        CREATURE_NERUBIAN_BARROW_TRIGGER        =       34862,
};

float NerubianSpawnPoints[5][3] =
{
    {735.403015, 75.357597, 142.201996},
        {692.919981, 184.809006, 142.203003},
        {740.544983, 189.113007, 142.197006},
        {688.206970, 102.847000, 142.201996},
        {707.945984, 70.579903, 142.201996}
};

#define SP_LEECHING_SWARM 67630
#define SP_LEECHING_SWARM_DMG 66240
#define SP_LEECHING_SWARM_HEAL 66125

struct Boss_Raid_AnubarakAI : public ScriptedAI
{
Boss_Raid_AnubarakAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
                pInstance = pCreature->GetInstanceData();
    }
        ScriptedInstance* pInstance;

        uint32 m_uiPhase;
        uint32 m_uiSwarmTickTimer;
        uint32 m_uiNerubianTriggerTimer;
        uint32 m_uiNerubianSummonTimer;

        int32 SwarmDamage;
    int32 SwarmDamageTotal;

        bool m_bIsTriggerSpawned;

    void Reset()
    {
                m_uiPhase =     1;
                m_uiSwarmTickTimer = 2000;
                m_uiNerubianTriggerTimer = 1000;
                m_uiNerubianSummonTimer = 10000;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveAura(SPELL_SUBMERGE);

                m_bIsTriggerSpawned = false;
    }

    void EnterCombat(Unit* who)
    {
                DoScriptText(SAY_AGGRO, me);
                DoZoneInCombat(me);
    }

        void NerubianSummon()
        {
                for (uint8 i = 0; i < RAID_MODE(2,2,4,4); ++i)
                {
                        if (Creature *Nerubian = me->SummonCreature(CREATURE_NERUBIAN_BURROWERS, NerubianSpawnPoints[i][0], NerubianSpawnPoints [i][1], NerubianSpawnPoints[i][2],0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        {
                                Nerubian->AddThreat(me->getVictim(), 0.0f);
                                DoZoneInCombat(Nerubian);
                        }
                }
        }

        void SwarmTick()
        {
                SwarmDamageTotal = 0;
                std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
                std::list<HostileReference*>::const_iterator i = m_threatlist.begin();
                        for (i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
                        {
                                Unit* plr = Unit::GetUnit((*me), (*i)->getUnitGuid());
                                if (plr && plr->isAlive())
                                if(plr && plr->GetTypeId()==TYPEID_PLAYER && plr->isAlive())
                                {
                                        SwarmDamage = plr->GetHealth() / RAID_MODE(10,20,30,30);
                                        if(SwarmDamage < 250) SwarmDamage = 250;
                                        SwarmDamageTotal += SwarmDamage;
                                        me->CastCustomSpell(plr, SP_LEECHING_SWARM_DMG, &SwarmDamage, NULL, NULL, true);
                                }
                        }
                me->CastCustomSpell(me, SP_LEECHING_SWARM_HEAL, &SwarmDamageTotal, NULL, NULL, true);
        }

        void KilledUnit(Unit *victim)
    {
                switch(rand()%2)
        {
            case 0: DoScriptText(SAY_KILL_1, me); break;
            case 1: DoScriptText(SAY_KILL_2, me); break;
                }
    }

        void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

                if (m_uiNerubianTriggerTimer <= uiDiff)
                {
                        for (uint8 i = 0; i < 5; ++i)
                        {
                                me->SummonCreature(CREATURE_NERUBIAN_BARROW_TRIGGER, NerubianSpawnPoints[i][0], NerubianSpawnPoints[i][1], NerubianSpawnPoints[i][2],0,TEMPSUMMON_CORPSE_DESPAWN, 10000);
                                m_uiNerubianTriggerTimer = 999999999;
                        }
                }
                else m_uiNerubianTriggerTimer -= uiDiff;

                if (m_uiPhase == 1)
                {
                        if (m_uiNerubianSummonTimer <= uiDiff)
                        {
                                NerubianSummon();
                                m_uiNerubianSummonTimer = 30000;
                        }
                        else m_uiNerubianSummonTimer -= uiDiff;
                }

                if (m_uiPhase == 2)
                {
                }

                if (m_uiPhase == 1 && m_uiPhase == 3)
                {

                }

                if (m_uiPhase == 3)
                {
                        if (m_uiSwarmTickTimer <= uiDiff)
                        {
                                SwarmTick();
                                m_uiSwarmTickTimer = 2*IN_MILISECONDS;
                        }
                        else m_uiSwarmTickTimer -= uiDiff;

                        if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                        if (m_uiNerubianSummonTimer <= uiDiff)
                        {
                                NerubianSummon();
                                m_uiNerubianSummonTimer = 30000;
                        }
                        else m_uiNerubianSummonTimer -= uiDiff;
                }

                if(m_uiPhase == 1)
        {
                        if( (me->GetHealth()*100) / me->GetMaxHealth() < 30)
                        {
                                m_uiPhase = 3;
                                DoCastAOE(SPELL_LEECHING_SWARM);
                        }
                }

                if (m_uiPhase != 2)
            DoMeleeAttackIfReady();
        }

    void JustDied(Unit* killer)
    {
    }
};

CreatureAI* GetAI_Raid_Anubarak(Creature* pCreature)
{
    return new Boss_Raid_AnubarakAI (pCreature);
}

struct mob_NerubianTriggerAI : public Scripted_NoMovementAI
{
    mob_NerubianTriggerAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
        {
                DoCast(me, SPELL_NERUBIAN_GROUND_VISUAL);
        }

    void UpdateAI(const uint32 uiDiff)
    {
    }
};

CreatureAI* GetAI_mob_NerubianTrigger(Creature* pCreature)
{
    return new mob_NerubianTriggerAI (pCreature);
}

void AddSC_Raid_Anubarak()
{
    Script *newscript;

        newscript = new Script;
    newscript->Name = "Boss_Raid_Anubarak";
    newscript->GetAI = &GetAI_Raid_Anubarak;
    newscript->RegisterSelf();

        newscript = new Script;
    newscript->Name = "Mob_NerubianTrigger";
    newscript->GetAI = &GetAI_mob_NerubianTrigger;
    newscript->RegisterSelf();
};
/*
UPDATE `creature_template` SET `ScriptName`='Boss_Raid_Anubarak' WHERE (`entry`='34564');
UPDATE `creature_template` SET `ScriptName`='Mob_NerubianTrigger' WHERE (`entry`='34862');
*/