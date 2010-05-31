/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

/* ScriptData
SDName: faction_champions
SD%Complete: 60%
SDComment: Scripts by Selector, modified by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "ScriptedPch.h"
#include "trial_of_the_crusader.h"

#define AI_MELEE    0
#define AI_RANGED   1
#define AI_HEALER   2

#define SPELL_ANTI_AOE     68595
#define SPELL_PVP_TRINKET  65547

struct boss_faction_championsAI : public ScriptedAI
{
    boss_faction_championsAI(Creature* pCreature, uint32 aitype) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance *) pCreature->GetInstanceData();
        mAIType = aitype;
        bsw = new BossSpellWorker(this);
        Init();
    }

    ScriptedInstance* m_pInstance;
    BossSpellWorker* bsw;

    uint32 mAIType;
    uint32 ThreatTimer;
    uint32 CCTimer;

    void Init()
    {
        CCTimer = rand()%10000;
        ThreatTimer = 5000;
        bsw->resetTimers();
        me->SetInCombatWithZone();
        me->SetRespawnDelay(DAY);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CRUSADERS, FAIL);
        me->ForcedDespawn();
    }

    float CalculateThreat(float distance, float armor, uint32 health)
    {
        float dist_mod = (mAIType == AI_MELEE) ? 15.0f/(15.0f + distance) : 1.0f;
        float armor_mod = (mAIType == AI_MELEE) ? armor / 16635.0f : 0.0f;
        float eh = (health+1) * (1.0f + armor_mod);
        return dist_mod * 30000.0f / eh;
    }

    void UpdateThreat()
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator itr;
        bool empty = true;
        for(itr = tList.begin(); itr!=tList.end(); ++itr)
        {
            Unit* pUnit = Unit::GetUnit((*me), (*itr)->getUnitGuid());
            if (pUnit && me->getThreatManager().getThreat(pUnit))
            {
                if(pUnit->GetTypeId()==TYPEID_PLAYER)
                {
                    float threat = CalculateThreat(me->GetDistance2d(pUnit), (float)pUnit->GetArmor(), pUnit->GetHealth());
                    me->getThreatManager().modifyThreatPercent(pUnit, -100);
                    me->AddThreat(pUnit, 1000000.0f * threat);
                    empty = false;
                }
            }
        }
    }

    void UpdatePower()
    {
        if(me->getPowerType() == POWER_MANA)
            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
        //else if(me->getPowerType() == POWER_ENERGY)
        //    me->ModifyPower(POWER_ENERGY, 100);
    }

    void RemoveCC()
    {
        me->RemoveAurasByType(SPELL_AURA_MOD_STUN);
        me->RemoveAurasByType(SPELL_AURA_MOD_FEAR);
        me->RemoveAurasByType(SPELL_AURA_MOD_ROOT);
        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY);
        me->RemoveAurasByType(SPELL_AURA_MOD_CONFUSE);
        //DoCast(me, SPELL_PVP_TRINKET);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_CRUSADERS_COUNT, 0);
    }

    void EnterCombat(Unit *who)
    {
        DoCast(me, SPELL_ANTI_AOE, true);
        if(m_pInstance) m_pInstance->SetData(TYPE_CRUSADERS, IN_PROGRESS);
    }

    void Reset()
    {
        if(m_pInstance) m_pInstance->SetData(TYPE_CRUSADERS, NOT_STARTED);
    }

    Creature* SelectRandomFriendlyMissingBuff(uint32 spell)
    {
        std::list<Creature *> lst = DoFindFriendlyMissingBuff(40.0f, spell);
        std::list<Creature *>::const_iterator itr = lst.begin();
        if(lst.empty()) 
            return NULL;
        advance(itr, rand()%lst.size());
        return (*itr);
    }

    Unit* SelectEnemyCaster(bool casting)
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator iter;
        for(iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if(target = Unit::GetUnit((*me),(*iter)->getUnitGuid()))
                if(target->getPowerType() == POWER_MANA)
                    return target;
        }
        return NULL;
    }

    uint32 EnemiesInRange(float distance)
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator iter;
        uint32 count = 0;
        for(iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            Unit *target;
            if(target = Unit::GetUnit((*me),(*iter)->getUnitGuid()))
                if(me->GetDistance2d(target) < distance)
                    ++count;
        }
        return count;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho) return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 10.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);

            if(mAIType==AI_MELEE)
                DoStartMovement(pWho);
            else
                DoStartMovement(pWho, 20.0f);
            SetCombatMovement(true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(ThreatTimer < diff)
        {
            UpdatePower();
            UpdateThreat();
            ThreatTimer = 4000;
        }
        else ThreatTimer -= diff;

        if(CCTimer < diff)
        {
            RemoveCC();
            CCTimer = 8000+rand()%2000;
        }
        else CCTimer -= diff;

        if(mAIType == AI_MELEE) DoMeleeAttackIfReady();
    }
};

/********************************************************************
                            HEALERS
********************************************************************/

#define SPELL_LIFEBLOOM        66093
#define SPELL_NOURISH          66066
#define SPELL_REGROWTH         66067
#define SPELL_REJUVENATION     66065
#define SPELL_TRANQUILITY      66086
#define SPELL_BARKSKIN         65860 //1 min cd
#define SPELL_THORNS           66068
#define SPELL_NATURE_GRASP     66071 //1 min cd, self buff

struct mob_toc_druidAI : public boss_faction_championsAI
{
    mob_toc_druidAI(Creature* pCreature) : boss_faction_championsAI(pCreature, AI_HEALER) {Init();}
    
    void Init()
    {
         SetEquipmentSlots(false, 51799, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

          bsw->timedCast(SPELL_NATURE_GRASP, diff);

          bsw->timedCast(SPELL_TRANQUILITY, diff);

          if(bsw->timedQuery(SPELL_BARKSKIN, diff))
                if(me->GetHealthPercent() < 50.0f)
                    bsw->doCast(SPELL_BARKSKIN);

        if(bsw->timedQuery(SPELL_LIFEBLOOM, diff))
            switch(urand(0,4))
            {
                case 0:
                        bsw->doCast(SPELL_LIFEBLOOM);
                    break;
                case 1:
                        bsw->doCast(SPELL_NOURISH);
                    break;
                case 2:
                        bsw->doCast(SPELL_REGROWTH);
                    break;
                case 3:
                        bsw->doCast(SPELL_REJUVENATION);
                    break;
                case 4:
                    if(Creature* target = SelectRandomFriendlyMissingBuff(SPELL_THORNS))
                        bsw->doCast(SPELL_THORNS, target);
                    break;
            }

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_HEALING_WAVE         66055
#define SPELL_RIPTIDE              66053
#define SPELL_SPIRIT_CLEANSE       66056 //friendly only
#define SPELL_HEROISM              65983
#define SPELL_BLOODLUST            65980
#define SPELL_HEX                  66054
#define SPELL_EARTH_SHIELD         66063
#define SPELL_EARTH_SHOCK          65973

struct mob_toc_shamanAI : public boss_faction_championsAI
{
    mob_toc_shamanAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_HEALER) {Init();}
    
    void Init()
    {
         SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_HEROISM, diff);

            bsw->timedCast(SPELL_HEX, diff);

        if(bsw->timedQuery(SPELL_HEALING_WAVE, diff))
        {
            switch(urand(0,5))
            {
                case 0: case 1:
                        bsw->doCast(SPELL_HEALING_WAVE);
                    break;
                case 2:
                        bsw->doCast(SPELL_RIPTIDE);
                    break;
                case 3:
                        bsw->doCast(SPELL_EARTH_SHOCK);
                    break;
                case 4:
                        bsw->doCast(SPELL_SPIRIT_CLEANSE);
                    break;
                case 5:
                    if(Unit *target = SelectRandomFriendlyMissingBuff(SPELL_EARTH_SHIELD))
                        bsw->doCast(target, SPELL_EARTH_SHIELD);
                    break;
            }
        }

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_HAND_OF_FREEDOM    68757 //25 sec cd
#define SPELL_BUBBLE             66010 //5 min cd
#define SPELL_CLEANSE            66116
#define SPELL_FLASH_OF_LIGHT     66113
#define SPELL_HOLY_LIGHT         66112
#define SPELL_HOLY_SHOCK         66114
#define SPELL_HAND_OF_PROTECTION 66009
#define SPELL_HAMMER_OF_JUSTICE  66613

struct mob_toc_paladinAI : public boss_faction_championsAI
{
    mob_toc_paladinAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_HEALER) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 50771, 47079, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

        //cast bubble at 20% hp
        if(me->GetHealthPercent() < 20.0f)
             bsw->timedCast(SPELL_BUBBLE, diff);

            if(Unit *target = DoSelectLowestHpFriendly(40.0f))
                if(target->GetHealthPercent() < 15.0f)
                    bsw->timedCast(SPELL_HAND_OF_PROTECTION, diff);

            bsw->timedCast(SPELL_HOLY_SHOCK, diff);

            if(Unit *target = SelectRandomFriendlyMissingBuff(SPELL_HAND_OF_FREEDOM))
                bsw->timedCast(SPELL_HAND_OF_FREEDOM, diff, target);

            bsw->timedCast(SPELL_HAMMER_OF_JUSTICE, diff);

        if(bsw->timedQuery(SPELL_FLASH_OF_LIGHT, diff))
        {
            switch(urand(0,4))
            {
                case 0: case 1:
                        bsw->doCast(SPELL_FLASH_OF_LIGHT);
                    break;
                case 2: case 3:
                        bsw->doCast(SPELL_HOLY_LIGHT);
                    break;
                case 4:
                        bsw->doCast(SPELL_CLEANSE);
                    break;
            }
        }

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_RENEW            66177
#define SPELL_SHIELD           66099
#define SPELL_FLASH_HEAL       66104
#define SPELL_DISPEL           65546
#define SPELL_PSYCHIC_SCREAM   65543
#define SPELL_MANA_BURN        66100

struct mob_toc_priestAI : public boss_faction_championsAI
{
    mob_toc_priestAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_HEALER) {Init();}
    
    void Init()
    {
         SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            if(EnemiesInRange(10.0f) > 2)
                bsw->timedCast(SPELL_PSYCHIC_SCREAM, diff);

        if(bsw->timedQuery(SPELL_RENEW, diff))
        {
            switch(urand(0,5))
            {
                case 0:
                        bsw->doCast(SPELL_RENEW);
                    break;
                case 1:
                        bsw->doCast(SPELL_SHIELD);
                    break;
                case 2: case 3:
                        bsw->doCast(SPELL_FLASH_HEAL);
                    break;
                case 4:
                    if(Unit *target = urand(0,1) ? SelectUnit(SELECT_TARGET_RANDOM,0) : DoSelectLowestHpFriendly(40.0f))
                        bsw->doCast(target, SPELL_DISPEL);
                    break;
                case 5:
                        bsw->doCast(SPELL_MANA_BURN);
                    break;
            }
        }

        boss_faction_championsAI::UpdateAI(diff);
    }
};

/********************************************************************
                            RANGED
********************************************************************/

#define SPELL_SILENCE          65542
#define SPELL_VAMPIRIC_TOUCH   65490
#define SPELL_SW_PAIN          65541
#define SPELL_MIND_FLAY        65488
#define SPELL_MIND_BLAST       65492
#define SPELL_HORROR           65545
#define SPELL_DISPERSION       65544
#define SPELL_SHADOWFORM       16592

struct mob_toc_shadow_priestAI : public boss_faction_championsAI
{
    mob_toc_shadow_priestAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_RANGED) {Init();}

    void Init()
    {
         SetEquipmentSlots(false, 50040, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void EnterCombat(Unit *who)
    {
        boss_faction_championsAI::EnterCombat(who);
        bsw->doCast(SPELL_SHADOWFORM);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            if(EnemiesInRange(10.0f) > 2)
                bsw->timedCast(SPELL_PSYCHIC_SCREAM, diff);

            if(me->GetHealthPercent() < 20.0f)
                bsw->timedCast(SPELL_DISPERSION, diff);

            if(Unit *target = SelectEnemyCaster(false))
                bsw->timedCast(SPELL_SILENCE, diff, target);

            bsw->timedCast(SPELL_MIND_BLAST, diff);

        if(bsw->timedQuery(SPELL_MIND_FLAY, diff))
        {
            switch(urand(0,4))
            {
                case 0: case 1:
                    bsw->doCast(SPELL_MIND_FLAY);
                    break;
                case 2:
                    bsw->doCast(SPELL_VAMPIRIC_TOUCH);
                    break;
               case 3:
                    bsw->doCast(SPELL_SW_PAIN);
                    break;
               case 4:
                    bsw->doCast(SPELL_DISPEL);
                    break;
            }
        }
        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_HELLFIRE             65816
#define SPELL_CORRUPTION           65810
#define SPELL_Curse_of_Agony       65814
#define SPELL_Curse_of_Exhaustion  65815
#define SPELL_Fear                 65809 //8s
#define SPELL_Searing_Pain         65819
#define SPELL_Shadow_Bolt          65821
#define SPELL_Unstable_Affliction  65812
#define H_SPELL_Unstable_Affliction 68155 //15s

struct mob_toc_warlockAI : public boss_faction_championsAI
{
    mob_toc_warlockAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_RANGED) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_Fear, diff);

            if(EnemiesInRange(10.0f) > 2)
                bsw->timedCast(SPELL_HELLFIRE, diff);

            bsw->timedCast(SPELL_Unstable_Affliction, diff);

        if(bsw->timedQuery(SPELL_Shadow_Bolt, diff))
        {
            switch(urand(0,5))
            {
                case 0:
                    bsw->doCast(SPELL_Searing_Pain);
                    break;
                case 1: case 2:
                    bsw->doCast(SPELL_Shadow_Bolt);
                    break;
                case 3:
                    bsw->doCast(SPELL_CORRUPTION);
                    break;
                case 4:
                    bsw->doCast(SPELL_Curse_of_Agony);
                    break;
                case 5:
                    bsw->doCast(SPELL_Curse_of_Exhaustion);
                    break;
             }
         }
       boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_Arcane_Barrage   65799 //3s
#define SPELL_Arcane_Blast     65791
#define SPELL_Arcane_Explosion 65800
#define SPELL_Blink            65793 //15s
#define SPELL_Counterspell     65790 //24s
#define SPELL_Frost_Nova       65792 //25s
#define SPELL_Frostbolt        65807
#define SPELL_Ice_Block        65802 //5min
#define SPELL_Polymorph        65801 //15s

struct mob_toc_mageAI : public boss_faction_championsAI
{
    mob_toc_mageAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_RANGED) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 47524, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            if(Unit *target = SelectEnemyCaster(false))
                bsw->timedCast(SPELL_Counterspell, diff, target);

            if(me->GetHealthPercent() < 50.0f 
            && EnemiesInRange(10.0f)>3 )
            {
                bsw->timedCast(SPELL_Frost_Nova, diff);
                bsw->timedCast(SPELL_Blink, diff);
            }

            if(me->GetHealthPercent() < 20.0f)
                   bsw->timedCast(SPELL_Ice_Block, diff);

            bsw->timedCast(SPELL_Polymorph, diff);

        if(bsw->timedQuery(SPELL_Arcane_Barrage, diff))
        {
            switch(urand(0,2))
            {
                case 0:
                    bsw->doCast(SPELL_Arcane_Barrage);
                    break;
                case 1:
                    bsw->doCast(SPELL_Arcane_Blast);
                    break;
                case 2:
                    bsw->doCast(SPELL_Frostbolt);
                    break;
            }
        }
        boss_faction_championsAI::UpdateAI(diff);
    }
};


#define SPELL_AIMED_SHOT       65883
#define SPELL_Deterrence       65871 //90s
#define SPELL_Disengage        65869 //30s
#define SPELL_EXPLOSIVE_SHOT   65866
#define SPELL_Frost_Trap       65880 //30s
#define SPELL_SHOOT            65868 //1.7s
#define SPELL_Steady_Shot      65867 //3s
#define SPELL_WING_CLIP        66207 //6s
#define SPELL_Wyvern_Sting     65877 //60s

struct mob_toc_hunterAI : public boss_faction_championsAI
{
    mob_toc_hunterAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_RANGED) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 47156, EQUIP_NO_CHANGE, 48711);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            if(EnemiesInRange(10.0f) > 3)
                bsw->timedCast(SPELL_Disengage, diff);

            if(me->GetHealthPercent() < 20.0f)
                bsw->timedCast(SPELL_Deterrence, diff);

            bsw->timedCast(SPELL_Wyvern_Sting, diff);

            bsw->timedCast(SPELL_Frost_Trap, diff );

            if(me->GetDistance2d(me->getVictim()) < 5.0f)
                bsw->timedCast(SPELL_WING_CLIP, diff);

        if(bsw->timedQuery(SPELL_SHOOT, diff))
        {
            switch(urand(0,3))
            {
                case 0: case 1:
                    bsw->doCast(SPELL_SHOOT);
                    break;
                case 2:
                    bsw->doCast(SPELL_EXPLOSIVE_SHOT);
                    break;
                case 3:
                    bsw->doCast(SPELL_AIMED_SHOT);
                    break;
            }
        }
        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_Cyclone          65859 //6s
#define SPELL_Entangling_Roots 65857 //10s
#define SPELL_Faerie_Fire      65863
#define SPELL_Force_of_Nature  65861 //180s
#define SPELL_Insect_Swarm     65855
#define SPELL_Moonfire         65856 //5s
#define SPELL_Starfire         65854
#define SPELL_Wrath            65862

struct mob_toc_boomkinAI : public boss_faction_championsAI
{
    mob_toc_boomkinAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_RANGED) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 50966, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

        if(me->GetHealthPercent() < 50.0f)
                bsw->timedCast(SPELL_BARKSKIN, diff);

        bsw->timedCast(SPELL_Cyclone, diff);

        bsw->timedCast(SPELL_Entangling_Roots, diff);

        bsw->timedCast(SPELL_Faerie_Fire, diff);

        if(bsw->timedQuery(SPELL_Moonfire, diff))
        {
            switch(urand(0,6))
            {
                case 0: case 1:
                      bsw->doCast(SPELL_Moonfire);
                      break;
                case 2:
                      bsw->doCast(SPELL_Insect_Swarm);
                      break;
                case 3:
                      bsw->doCast(SPELL_Starfire);
                      break;
                case 4: case 5: case 6:
                      bsw->doCast(SPELL_Wrath);
                      break;
            }
        }
        boss_faction_championsAI::UpdateAI(diff);
    }
};

/********************************************************************
                            MELEE
********************************************************************/

#define SPELL_BLADESTORM           65947
#define SPELL_INTIMIDATING_SHOUT   65930
#define SPELL_MORTAL_STRIKE        65926
#define SPELL_CHARGE               68764
#define SPELL_DISARM               65935
#define SPELL_OVERPOWER            65924
#define SPELL_SUNDER_ARMOR         65936
#define SPELL_SHATTERING_THROW     65940
#define SPELL_RETALIATION          65932

struct mob_toc_warriorAI : public boss_faction_championsAI
{
    mob_toc_warriorAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 47427, 46964, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_BLADESTORM, diff);

            bsw->timedCast(SPELL_INTIMIDATING_SHOUT, diff);

            bsw->timedCast(SPELL_MORTAL_STRIKE, diff);

            bsw->timedCast(SPELL_SUNDER_ARMOR, diff);

            bsw->timedCast(SPELL_CHARGE, diff);

            bsw->timedCast(SPELL_RETALIATION, diff);

            bsw->timedCast(SPELL_OVERPOWER, diff);

            bsw->timedCast(SPELL_SHATTERING_THROW, diff);

            bsw->timedCast(SPELL_DISARM, diff);

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_Chains_of_Ice      66020 //8sec
#define SPELL_Death_Coil         66019    //5sec
#define SPELL_Death_Grip         66017 //35sec
#define SPELL_Frost_Strike       66047  //6sec
#define SPELL_Icebound_Fortitude 66023 //1min
#define SPELL_Icy_Touch          66021  //8sec
#define SPELL_Strangulate        66018 //2min

struct mob_toc_dkAI : public boss_faction_championsAI
{
    mob_toc_dkAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 47518, 51021, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            if(me->GetHealthPercent() < 50.0f)
                bsw->timedCast(SPELL_Icebound_Fortitude, diff);

            bsw->timedCast(SPELL_Chains_of_Ice, diff);

            bsw->timedCast(SPELL_Death_Coil, diff);

            if(Unit *target = SelectEnemyCaster(false))
                bsw->timedCast(SPELL_Strangulate, diff, target);

            bsw->timedCast(SPELL_Frost_Strike, diff);

            bsw->timedCast(SPELL_Icy_Touch, diff);

            if(me->IsInRange(me->getVictim(), 10.0f, 30.0f, false))
                bsw->timedCast(SPELL_Death_Grip, diff); 

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_FAN_OF_KNIVES        65955 //2sec
#define SPELL_BLIND                65960 //2min
#define SPELL_CLOAK                65961 //90sec
#define SPELL_Blade_Flurry         65956 //2min
#define SPELL_SHADOWSTEP           66178 //30sec
#define SPELL_HEMORRHAGE           65954
#define SPELL_EVISCERATE           65957

struct mob_toc_rogueAI : public boss_faction_championsAI
{
    mob_toc_rogueAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 47422, 49982, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

        if(EnemiesInRange(15.0f) > 2)
            bsw->timedCast(SPELL_FAN_OF_KNIVES, diff);

        bsw->timedCast(SPELL_HEMORRHAGE, diff);

        bsw->timedCast(SPELL_EVISCERATE, diff);

        if(me->IsInRange(me->getVictim(), 10.0f, 40.0f))
            bsw->timedCast(SPELL_SHADOWSTEP, diff);

        if(Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
            if(me->IsInRange(target, 0.0f, 15.0f, false))
                bsw->timedCast(SPELL_BLIND, diff, target);

        if(me->GetHealthPercent() < 50.0f)
            bsw->timedCast(SPELL_CLOAK, diff);

        bsw->timedCast(SPELL_Blade_Flurry, diff);

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_EARTH_SHOCK      65973
#define SPELL_LAVA_LASH        65974
#define SPELL_STORMSTRIKE      65970

struct mob_toc_enh_shamanAI : public boss_faction_championsAI
{
    mob_toc_enh_shamanAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 51803, 48013, EQUIP_NO_CHANGE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_HEROISM, diff);

            bsw->timedCast(SPELL_EARTH_SHOCK, diff);

            bsw->timedCast(SPELL_STORMSTRIKE, diff);

            bsw->timedCast(SPELL_LAVA_LASH, diff);

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_Avenging_Wrath       66011 //3min cd
#define SPELL_Crusader_Strike      66003 //6sec cd
#define SPELL_Divine_Shield        66010 //5min cd
#define SPELL_Divine_Storm         66006 //10sec cd
#define SPELL_Hammer_of_Justice    66007 //40sec cd
#define SPELL_Hand_of_Protection   66009 //5min cd
#define SPELL_Judgement_of_Command 66005 //8sec cd
#define SPELL_REPENTANCE           66008 //60sec cd
#define SPELL_Seal_of_Command      66004 //no cd

struct mob_toc_retro_paladinAI : public boss_faction_championsAI
{
    mob_toc_retro_paladinAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void Init()
    {
        SetEquipmentSlots(false, 47519, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
    }

    void EnterCombat(Unit *who)
    {
        boss_faction_championsAI::EnterCombat(who);
        bsw->doCast(SPELL_Seal_of_Command);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_REPENTANCE, diff);

            bsw->timedCast(SPELL_Crusader_Strike, diff);

            bsw->timedCast(SPELL_Avenging_Wrath, diff);

            if(me->GetHealthPercent() < 20.0f)
                 bsw->timedCast(SPELL_Divine_Shield, diff);

            bsw->timedCast(SPELL_Divine_Storm, diff);

            bsw->timedCast(SPELL_Judgement_of_Command, diff);

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_WPET0 67518
#define SPELL_WPET1 67519

struct mob_toc_pet_warlockAI : public boss_faction_championsAI
{
    mob_toc_pet_warlockAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void EnterCombat(Unit *who)
    {
        boss_faction_championsAI::EnterCombat(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_WPET0, diff);

            bsw->timedCast(SPELL_WPET1, diff);

        boss_faction_championsAI::UpdateAI(diff);
    }
};

#define SPELL_HPET0 67793
struct mob_toc_pet_hunterAI : public boss_faction_championsAI
{
    mob_toc_pet_hunterAI(Creature *pCreature) : boss_faction_championsAI(pCreature, AI_MELEE) {Init();}

    void EnterCombat(Unit *who)
    {
        boss_faction_championsAI::EnterCombat(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim()) return;

            bsw->timedCast(SPELL_HPET0, diff);

        boss_faction_championsAI::UpdateAI(diff);
    }
};


/*========================================================*/
CreatureAI* GetAI_mob_toc_druid(Creature *pCreature) {
    return new mob_toc_druidAI (pCreature);
}
CreatureAI* GetAI_mob_toc_shaman(Creature *pCreature) {
    return new mob_toc_shamanAI (pCreature);
}
CreatureAI* GetAI_mob_toc_paladin(Creature *pCreature) {
    return new mob_toc_paladinAI (pCreature);
}
CreatureAI* GetAI_mob_toc_priest(Creature *pCreature) {
    return new mob_toc_priestAI (pCreature);
}
CreatureAI* GetAI_mob_toc_shadow_priest(Creature *pCreature) {
    return new mob_toc_shadow_priestAI (pCreature);
}
CreatureAI* GetAI_mob_toc_warlock(Creature *pCreature) {
    return new mob_toc_warlockAI (pCreature);
}
CreatureAI* GetAI_mob_toc_mage(Creature *pCreature) {
    return new mob_toc_mageAI (pCreature);
}
CreatureAI* GetAI_mob_toc_hunter(Creature *pCreature) {
    return new mob_toc_hunterAI (pCreature);
}
CreatureAI* GetAI_mob_toc_boomkin(Creature *pCreature) {
    return new mob_toc_boomkinAI (pCreature);
}
CreatureAI* GetAI_mob_toc_warrior(Creature *pCreature) {
    return new mob_toc_warriorAI (pCreature);
}
CreatureAI* GetAI_mob_toc_dk(Creature *pCreature) {
    return new mob_toc_dkAI (pCreature);
}
CreatureAI* GetAI_mob_toc_rogue(Creature *pCreature) {
    return new mob_toc_rogueAI (pCreature);
}
CreatureAI* GetAI_mob_toc_enh_shaman(Creature *pCreature) {
    return new mob_toc_enh_shamanAI (pCreature);
}
CreatureAI* GetAI_mob_toc_retro_paladin(Creature *pCreature) {
    return new mob_toc_retro_paladinAI (pCreature);
}
CreatureAI* GetAI_mob_toc_pet_warlock(Creature *pCreature) {
    return new mob_toc_pet_warlockAI (pCreature);
}
CreatureAI* GetAI_mob_toc_pet_hunter(Creature *pCreature) {
    return new mob_toc_pet_hunterAI (pCreature);
}

void AddSC_boss_faction_champions()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_toc_druid";
    newscript->GetAI = &GetAI_mob_toc_druid;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_shaman";
    newscript->GetAI = &GetAI_mob_toc_shaman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_paladin";
    newscript->GetAI = &GetAI_mob_toc_paladin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_priest";
    newscript->GetAI = &GetAI_mob_toc_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_shadow_priest";
    newscript->GetAI = &GetAI_mob_toc_shadow_priest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_mage";
    newscript->GetAI = &GetAI_mob_toc_mage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_warlock";
    newscript->GetAI = &GetAI_mob_toc_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_hunter";
    newscript->GetAI = &GetAI_mob_toc_hunter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_boomkin";
    newscript->GetAI = &GetAI_mob_toc_boomkin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_warrior";
    newscript->GetAI = &GetAI_mob_toc_warrior;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_dk";
    newscript->GetAI = &GetAI_mob_toc_dk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_rogue";
    newscript->GetAI = &GetAI_mob_toc_rogue;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_enh_shaman";
    newscript->GetAI = &GetAI_mob_toc_enh_shaman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_retro_paladin";
    newscript->GetAI = &GetAI_mob_toc_retro_paladin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_pet_warlock";
    newscript->GetAI = &GetAI_mob_toc_pet_warlock;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_toc_pet_hunter";
    newscript->GetAI = &GetAI_mob_toc_pet_hunter;
    newscript->RegisterSelf();
}
