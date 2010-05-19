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

enum Says
{
        SAY_AGGRO                       =       -1600218,
        SAY_FLESH                       =       -1600219,
        SAY_SUMMON_SISTER       =       -1600220,
        SAY_INFERNO                     =       -1600221,
        SAY_KILL_1                      =       -1600222,
        SAY_KILL_2                      =       -1600223,
        SAY_DEATH                       =       -1600224,
};

enum Spells
{
         SPELL_INCINERATE_FLESH                         = 66237,
         SPELL_FEL_FIREBALL                                     = 66532,
         SPELL_FEL_LIGHTING                                     = 66528,
         SPELL_LEGION_FLAME                                     = 68124,
         SPELL_NETHER_POWER                                     = 67108,
         SPELL_ROOTET_JARAXXUS                          = 67924,
         SPELL_BERSERK                                          = 47008,

         SPELL_INCINERATE_FLESH_25_NH           = 67049,
         SPELL_INCINERATE_FLESH_10_HC           = 67050,
         SPELL_INCINERATE_FLESH_25_HC           = 67051,
         SPELL_FEL_LIGHTING_25_NH                       = 67029,
         SPELL_FEL_LIGHTING_10_HC                       = 67030,
         SPELL_FEL_LIGHTING_25_HC                       = 67031,
         SPELL_LEGION_FLAME_10_NH                       = 66199,
         SPELL_LEGION_FLAME_10_HC                       = 68127,
         SPELL_LEGION_FLAME_25_NH                       = 68126,
         SPELL_LEGION_FLAME_25_HC                       = 68128,
         SPELL_FEL_FIREBALL_25_NH                       = 66963,
         SPELL_FEL_FIREBALL_10_HC                   = 66964,
         SPELL_FEL_FIREBALL_25_HC                       = 66965,

         // FireTrigger
         SPELL_FIRE_DAMAGE                                      = 66201,

         // Only Heroic 25 Version
         SPELL_TOUCH_OF_JARAXXUS                        = 66209,
         SPELL_CURSE_OF_THE_NETHER                      = 66211,

         // FireAdd
         SPELL_SUMMON                                           = 66252,
         SPELL_SUMMON_NPC_VULCAN                        = 66258,
         SPELL_SUMMON_NPC_VULCAN_HC                     = 67903,
         SPELL_SUMMON_SISTER                            = 67103,

         //Portal Target
         SPELL_PORTAL_SUMMON_GUARDIAN           = 67103,

         SPELL_PORTAL_SUMMON_GUARDIAN_25        = 67103,
         SPELL_PORTAL_SUMMON_GUARDIAN_10HC      = 67104,
         SPELL_PORTAL_SUMMON_GUARDIAN_25HC  = 67105,
         SPELL_PORTAL_EFFECT                            = 36455,

         // Mistress of Pain
         SPELL_SHIVAN_SLAH                                      = 67098,
         SPELL_SPINNING_PAIN_SPIKE                      = 66283
};

enum NPC
{
        NPC_FIRE_TRIGGER                 = 34784,
        NPC_NETHER_PORTAL                = 34825,
        NPC_MISTRESS_OF_PAIN     = 34826,
};

struct boss_JaraxxusAI : public ScriptedAI
{
    boss_JaraxxusAI(Creature *pCreature) : ScriptedAI(pCreature), summons(me)
    {
                m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

        uint32 uiIncinerateFleshTimer;
        uint32 uiFelFireballTimer;
        uint32 uiLegionFlameTimer;
        uint32 uiFelLightingTimer;
        uint32 uiVolcanSummonTimer;
        uint32 uiNetherPowerTimer;
        uint32 uiNetherPortalTimer;
        uint32 uiBerserkTimer;
        uint32 uiTouchOfJaraxxusTimer;
        uint32 uiCurseOfTheNetherTimer;

        SummonList summons;

        bool m_bIsBerserk;

    void Reset()
    {
                uiIncinerateFleshTimer = 20*IN_MILISECONDS;
                uiFelFireballTimer = 10*IN_MILISECONDS;
                uiLegionFlameTimer = 21*IN_MILISECONDS;
                uiFelLightingTimer = 10*IN_MILISECONDS;
                uiVolcanSummonTimer = 30*IN_MILISECONDS;
                uiNetherPortalTimer = 15*IN_MILISECONDS;
                uiBerserkTimer  = 10*MINUTE*IN_MILISECONDS;

                m_bIsBerserk = false;

                DoCast(me, SPELL_ROOTET_JARAXXUS);

                 summons.DespawnAll();

                 m_pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    void EnterCombat(Unit* who)
    {
                DoScriptText(SAY_AGGRO,me);

                me->RemoveAurasDueToSpell(SPELL_ROOTET_JARAXXUS);
        }

        void SummonedCreatureDespawn(Creature *summon)
        {
                summons.Despawn(summon);
        }

        void JustReachedHome()
    {
        if (m_pInstance)
                {
            m_pInstance->SetData(PHASE_4, NOT_STARTED);
                }

                if(m_pInstance)
                {
                        uint32 count = m_pInstance->GetData(DATA_UPDATE_STATE_UI_COUNT);
                        count--;
                        m_pInstance->SetData(DATA_UPDATE_STATE_UI_COUNT, count);
                        me->ForcedDespawn();
                }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

                if (uiFelFireballTimer < uiDiff)
        {
                        DoCast(me->getVictim(), SPELL_FEL_FIREBALL);
                        uiFelFireballTimer = 15*IN_MILISECONDS,30*IN_MILISECONDS;
        }
                else uiFelFireballTimer -= uiDiff;

                if (uiBerserkTimer < uiDiff && !m_bIsBerserk)
        {
                        DoCast(me, SPELL_BERSERK);
                        m_bIsBerserk = true;
        }
                else uiBerserkTimer -= uiDiff;

                if (uiVolcanSummonTimer < uiDiff)
        {
                        DoCast(me, SPELL_SUMMON_NPC_VULCAN);
                        uiVolcanSummonTimer = 2*MINUTE*IN_MILISECONDS;
        }
                else uiVolcanSummonTimer -= uiDiff;

                if (uiFelLightingTimer <= uiDiff)
                {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                                DoCast(pTarget, SPELL_FEL_LIGHTING);
                                uiFelLightingTimer = urand(15*IN_MILISECONDS,21*IN_MILISECONDS);
                        }
                }
                else uiFelLightingTimer -= uiDiff;

                if (uiIncinerateFleshTimer <= uiDiff)
                {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        {
                                DoCast(pTarget, SPELL_INCINERATE_FLESH);
                                DoScriptText(SAY_FLESH, me);
                                uiIncinerateFleshTimer = urand(20*IN_MILISECONDS,21*IN_MILISECONDS);
                        }
                }
                else uiIncinerateFleshTimer -= uiDiff;

                if (uiNetherPortalTimer <= uiDiff)
        {
                        DoScriptText(SAY_SUMMON_SISTER,me);
            me->SummonCreature(NPC_NETHER_PORTAL, me->GetPositionX(), me->GetPositionY()+5, me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
            uiNetherPortalTimer = 2*MINUTE*IN_MILISECONDS;;
        }
                else uiNetherPortalTimer -= uiDiff;

                if (uiLegionFlameTimer < uiDiff)
        {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        {
                                DoCast(pTarget, SPELL_LEGION_FLAME);
                                uiLegionFlameTimer = urand(25*IN_MILISECONDS,35*IN_MILISECONDS);
                        }
        }
                else uiLegionFlameTimer -= uiDiff;

                if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                if (uiTouchOfJaraxxusTimer < uiDiff)
        {
                        if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        target->CastSpell(target, SPELL_TOUCH_OF_JARAXXUS, true);
                        uiTouchOfJaraxxusTimer = urand(40*IN_MILISECONDS,45*IN_MILISECONDS);
        }
                else uiTouchOfJaraxxusTimer -= uiDiff;

                if (uiCurseOfTheNetherTimer < uiDiff)
        {
                        if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                        target->CastSpell(target, SPELL_CURSE_OF_THE_NETHER, true);
                        uiCurseOfTheNetherTimer = urand(60*IN_MILISECONDS,61*IN_MILISECONDS);
        }
                else uiCurseOfTheNetherTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
      DoScriptText(SAY_DEATH,me);

          if (m_pInstance)
                        m_pInstance->SetData(PHASE_4, DONE);
    }
};

CreatureAI* GetAI_boss_Jaraxxus(Creature* pCreature)
{
    return new boss_JaraxxusAI (pCreature);
}

struct mob_FireTriggerAI : public Scripted_NoMovementAI
{
    mob_FireTriggerAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
                DoCast(me, SPELL_FIRE_DAMAGE);
                me->SetDisplayId(25206);
    }
    void UpdateAI(const uint32 diff)
    {
                if (m_pInstance && m_pInstance->GetData(PHASE_4) == NOT_STARTED)
                {
                        me->ForcedDespawn();
                }
    }
};

CreatureAI* GetAI_mob_FireTrigger(Creature* pCreature)
{
    return new mob_FireTriggerAI (pCreature);
}

struct mob_VulcanAI : public Scripted_NoMovementAI
{
    mob_VulcanAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature), summons(me)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

        SummonList summons;

    void Reset()
    {
                me->SetReactState(REACT_PASSIVE);

                if (getDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || getDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
    }

        void JustSummoned(Creature *summon)
        {
                summons.Summon(summon);
                summon->AI()->AttackStart(me->getVictim());
        summon->AI()->DoZoneInCombat();
        }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance && m_pInstance->GetData(PHASE_4) == NOT_STARTED)
                {
                        me->ForcedDespawn();
                }
    }
};

CreatureAI* GetAI_mob_Vulcan(Creature* pCreature)
{
    return new mob_VulcanAI (pCreature);
}

struct mob_NetherportalAI : public Scripted_NoMovementAI
{
    mob_NetherportalAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature), summons(me)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

        uint32 uiNetherPortalEffectTimer;
        uint32 uiSpellNetherPortal;

        bool m_bIsSummonGuardian;

        SummonList summons;

    void Reset()
    {
                uiNetherPortalEffectTimer = urand (1000,2000);
                uiSpellNetherPortal = urand (1000,2000);
                me->SetDisplayId(25206);
                me->SetReactState(REACT_PASSIVE);
                m_bIsSummonGuardian = false;

                if (getDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || getDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
    }

        void JustSummoned(Creature *summon)
        {
                if (summon->GetEntry() == NPC_MISTRESS_OF_PAIN)
                summon->AI()->AttackStart(me->getVictim());
        summon->AI()->DoZoneInCombat();
                summons.Summon(summon);
        }

    void UpdateAI(const uint32 uiDiff)
    {
                if (m_pInstance && m_pInstance->GetData(PHASE_4) == NOT_STARTED)
                {
                        me->ForcedDespawn();
                }

                if (uiSpellNetherPortal <= uiDiff && !m_bIsSummonGuardian)
                {
                        DoCast(me, SPELL_PORTAL_SUMMON_GUARDIAN);
                        m_bIsSummonGuardian = true;
                }
                else uiSpellNetherPortal -= uiDiff;
    }
};

CreatureAI* GetAI_mob_Netherportal(Creature* pCreature)
{
    return new mob_NetherportalAI (pCreature);

}

struct mob_mistressofpainAI : public ScriptedAI
{
    mob_mistressofpainAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

        uint32 uiShivanSlashTimer;
        uint32 uiSpinningPainSpikeTimer;

        void Reset()
        {
                uiShivanSlashTimer = urand(5000,6000);
                uiSpinningPainSpikeTimer = urand(10000,11000);
        }

    void UpdateAI(const uint32 uiDiff)
        {

        if (m_pInstance && m_pInstance->GetData(PHASE_4) == NOT_STARTED)
    {
                me->ForcedDespawn();
        }

        if (!UpdateVictim())
                return;

        if (uiShivanSlashTimer < uiDiff)
    {
                DoCast(me->getVictim(), SPELL_SHIVAN_SLAH);
                uiShivanSlashTimer = urand(5000,6000);
        }
        else uiShivanSlashTimer -= uiDiff;

        if (uiSpinningPainSpikeTimer <= uiDiff)
        {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                {
                        DoCast(pTarget, SPELL_SPINNING_PAIN_SPIKE);
                        uiSpinningPainSpikeTimer = urand(15000,16000);
        }
        }
        else uiSpinningPainSpikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_mistressofpain(Creature* pCreature)
{
    return new mob_mistressofpainAI (pCreature);
}

void AddSC_boss_lord_jaraxxus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "Boss_Jaraxxus";
    newscript->GetAI = &GetAI_boss_Jaraxxus;
    newscript->RegisterSelf();

        newscript = new Script;
    newscript->Name = "Mob_FireTrigger";
    newscript->GetAI = &GetAI_mob_FireTrigger;
    newscript->RegisterSelf();

        newscript = new Script;
    newscript->Name = "Mob_Vulcan";
    newscript->GetAI = &GetAI_mob_Vulcan;
    newscript->RegisterSelf();

        newscript = new Script;
    newscript->Name = "Mob_Netherportal";
    newscript->GetAI = &GetAI_mob_Netherportal;
    newscript->RegisterSelf();

        newscript = new Script;
    newscript->Name = "Mob_MistressOfPain";
    newscript->GetAI = &GetAI_mob_mistressofpain;
    newscript->RegisterSelf();
};