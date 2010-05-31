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
// Jaraxxus - Magic aura (from start?) not fully offlike implemented.
// Legion flame visual effect not imlemented

/* ScriptData
SDName: trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "ScriptedPch.h"
#include "trial_of_the_crusader.h"
enum Equipment
{
    EQUIP_MAIN           = 47266,
    EQUIP_OFFHAND        = 46996,
    EQUIP_RANGED         = 47267,
    EQUIP_DONE           = EQUIP_NO_CHANGE,
};

enum Summons
{
    NPC_LEGION_FLAME     = 34784,
    NPC_INFERNAL_VOLCANO = 34813,
    NPC_FEL_INFERNAL     = 34815,
    NPC_NETHER_PORTAL    = 34825,
    NPC_MISTRESS         = 34826,
};

enum BossSpells
{
SPELL_NETHER_POWER      = 67108,
SPELL_INFERNAL          = 66258,
SPELL_INFERNAL_ERUPTION = 66255,
SPELL_FEL_FIREBALL      = 66532,
SPELL_FEL_LIGHTING      = 66528,
SPELL_INCINERATE_FLESH  = 66237,
SPELL_BURNING_INFERNO   = 66242,
SPELL_NETHER_PORTAL     = 66264,
SPELL_LEGION_FLAME_0    = 66199,
SPELL_LEGION_FLAME_1    = 66197,
SPELL_SHIVAN_SLASH      = 67098,
SPELL_SPINNING_STRIKE   = 66316,
SPELL_FEL_INFERNO       = 67047,
SPELL_FEL_STREAK        = 66494,
SPELL_BERSERK           = 26662,
};

/*######
## boss_jaraxxus
######*/

struct boss_jaraxxusAI : public ScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        bsw = new BossSpellWorker(this);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint8 Difficulty;
    uint8 stage;
    uint8 substage;
    uint8 m_portalsCount;
    uint8 m_volcanoCount;
    BossSpellWorker* bsw;

    void Reset() {
        if(!m_pInstance) return;
        Difficulty = m_pInstance->GetData(TYPE_DIFFICULTY);
        m_pInstance->SetData(TYPE_JARAXXUS, NOT_STARTED);
        SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);
        m_portalsCount = 1;
        if (Difficulty == RAID_DIFFICULTY_10MAN_HEROIC || Difficulty == RAID_DIFFICULTY_25MAN_HEROIC) 
        {
            m_portalsCount = 2;
            m_volcanoCount = 4;
        } else {
            m_portalsCount = 1;
            m_volcanoCount = 4;
        }
        DoScriptText(-1713517,me);
        me->SetRespawnDelay(DAY);
    }

    void JustReachedHome()
    {
        if (!m_pInstance) return;
            m_pInstance->SetData(TYPE_JARAXXUS, FAIL);
            me->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) return;
            DoScriptText(-1713525,me);
            m_pInstance->SetData(TYPE_JARAXXUS, DONE);
            m_pInstance->SetData(TYPE_EVENT,2000);
            m_pInstance->SetData(TYPE_STAGE,0);
    }

    void EnterCombat(Unit* pWho)
    {
        if (!m_pInstance) return;
        me->SetInCombatWithZone();
        m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
        DoScriptText(-1713514,me);
        bsw->doCast(SPELL_NETHER_POWER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        bsw->timedCast(SPELL_FEL_FIREBALL, uiDiff);

        bsw->timedCast(SPELL_FEL_LIGHTING, uiDiff);

        if (bsw->timedQuery(SPELL_INCINERATE_FLESH, uiDiff)) {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,1))
                           {
                           DoScriptText(-1713522,me,pTarget);
                           bsw->doCast(SPELL_INCINERATE_FLESH,pTarget);
                           }
                    }

        if (bsw->timedQuery(SPELL_LEGION_FLAME_1, uiDiff)) {
                    DoScriptText(-1713518,me);
                    bsw->doCast(SPELL_LEGION_FLAME_1);
                    };

        if (bsw->timedQuery(SPELL_INFERNAL_ERUPTION, uiDiff)
                             && m_volcanoCount > 0) {
                DoScriptText(-1713520,me);
                if (bsw->doCast(NPC_INFERNAL_VOLCANO) == CAST_OK) --m_volcanoCount;
                    };

        if (bsw->timedQuery(SPELL_NETHER_PORTAL, uiDiff)
                             && m_portalsCount > 0
                             &&  me->GetHealthPercent() <= 90.0f)
                             {
                DoScriptText(-1713519,me);
                if (bsw->doCast(NPC_NETHER_PORTAL) == CAST_OK) --m_portalsCount;
                };

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}

struct mob_legion_flameAI : public ScriptedAI
{
    mob_legion_flameAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiRangeCheck_Timer;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetInCombatWithZone();
        me->SetRespawnDelay(DAY);

        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0) ) {
                me->GetMotionMaster()->MoveChase(pTarget);
                me->SetSpeed(MOVE_RUN, 0.5);
                }

    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void EnterCombat(Unit *who)
    {
        if (!m_pInstance) return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            me->ForcedDespawn();

        if (!UpdateVictim())
            return;

        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (m_pInstance)
            {
                if (me->IsWithinDist(me->getVictim(), 4.0f, false))
                {
                    DoCast(me->getVictim(), SPELL_LEGION_FLAME_0);
                    me->ForcedDespawn();
                }
            }
            
            m_uiRangeCheck_Timer = 1000;
            
            if (me->getVictim())
            {
                me->GetMotionMaster()->MoveChase(me->getVictim());
                me->SetSpeed(MOVE_RUN, 0.5);
            }
        }
        else m_uiRangeCheck_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_legion_flame(Creature* pCreature)
{
    return new mob_legion_flameAI(pCreature);
}

struct mob_infernal_volcanoAI : public ScriptedAI
{
    mob_infernal_volcanoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        bsw = new BossSpellWorker(this);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint8 Difficulty;
    uint8 m_Count;
    uint32 m_Timer;
    BossSpellWorker* bsw;

    void Reset()
    {
        Difficulty = m_pInstance->GetData(TYPE_DIFFICULTY);
        m_Timer = 15000;
        me->SetRespawnDelay(DAY);
        if (Difficulty != RAID_DIFFICULTY_10MAN_HEROIC && Difficulty != RAID_DIFFICULTY_25MAN_HEROIC) 
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_Count = 3;
        } else
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_Count = 6;
        }
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void JustDied(Unit* Killer)
    {
    }

    void EnterCombat(Unit *who)
    {
        if (!m_pInstance) return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            me->ForcedDespawn();

        if (bsw->timedQuery(SPELL_INFERNAL_ERUPTION,diff) && m_Count > 0) {
            bsw->doCast(SPELL_INFERNAL_ERUPTION);
            DoScriptText(-1713524,me);
            --m_Count;
            }

        if (!UpdateVictim())
            return;

    }
};

CreatureAI* GetAI_mob_infernal_volcano(Creature* pCreature)
{
    return new mob_infernal_volcanoAI(pCreature);
}

struct mob_fel_infernalAI : public ScriptedAI
{
    mob_fel_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        bsw = new BossSpellWorker(this);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    BossSpellWorker* bsw;

    void Reset()
    {
        me->SetInCombatWithZone();
        me->SetRespawnDelay(DAY);

        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0)) {
            me->Attack(pTarget, true);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void JustDied(Unit* Killer)
    {
    }

    void EnterCombat(Unit *who)
    {
        if (!m_pInstance) return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            me->ForcedDespawn();

        if (!UpdateVictim())
            return;

        bsw->timedCast(SPELL_FEL_INFERNO, uiDiff);

        bsw->timedCast(SPELL_FEL_STREAK, uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_fel_infernal(Creature* pCreature)
{
    return new mob_fel_infernalAI(pCreature);
}

struct mob_nether_portalAI : public ScriptedAI
{
    mob_nether_portalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint8 Difficulty;
    uint32 m_Timer;
    uint8  m_Count;

    void Reset()
    {
        Difficulty = m_pInstance->GetData(TYPE_DIFFICULTY);
        m_Timer = 10000;
        me->SetRespawnDelay(DAY);
        if (Difficulty != RAID_DIFFICULTY_10MAN_HEROIC && Difficulty != RAID_DIFFICULTY_25MAN_HEROIC) 
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_Count = 1;
        } else
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_Count = 2;
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void JustDied(Unit* Killer)
    {
    }

    void EnterCombat(Unit *who)
    {
        if (!m_pInstance) return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            me->ForcedDespawn();

        if (m_Timer < diff && m_Count > 0) {
            DoCast(me,SPELL_NETHER_PORTAL,false);
            DoScriptText(-1713521,me);
            --m_Count;
            m_Timer = 60000;
            } else m_Timer -= diff;

        if (!UpdateVictim())
            return;

    }
};

CreatureAI* GetAI_mob_nether_portal(Creature* pCreature)
{
    return new mob_nether_portalAI(pCreature);
}

struct mob_mistress_of_painAI : public ScriptedAI
{
    mob_mistress_of_painAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        bsw = new BossSpellWorker(this);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    BossSpellWorker* bsw;

    void Reset()
    {
        me->SetInCombatWithZone();
        me->SetRespawnDelay(DAY);

        if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
        {
                me->GetMotionMaster()->MoveChase(pTarget);
                me->SetSpeed(MOVE_RUN, 1);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) return;
    }

    void JustDied(Unit* Killer)
    {
    }

    void EnterCombat(Unit *who)
    {
        if (!m_pInstance) return;
        DoScriptText(-1713523,me, who);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) return;
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            me->ForcedDespawn();

        if (!UpdateVictim())
            return;

        bsw->timedCast(SPELL_SHIVAN_SLASH, uiDiff);

        bsw->timedCast(SPELL_SPINNING_STRIKE, uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_mistress_of_pain(Creature* pCreature)
{
    return new mob_mistress_of_painAI(pCreature);
}

void AddSC_boss_jaraxxus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_jaraxxus";
    newscript->GetAI = &GetAI_boss_jaraxxus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_legion_flame";
    newscript->GetAI = &GetAI_mob_legion_flame;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_infernal_volcano";
    newscript->GetAI = &GetAI_mob_infernal_volcano;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_fel_infernal";
    newscript->GetAI = &GetAI_mob_fel_infernal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nether_portal";
    newscript->GetAI = &GetAI_mob_nether_portal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mistress_of_pain";
    newscript->GetAI = &GetAI_mob_mistress_of_pain;
    newscript->RegisterSelf();
}
