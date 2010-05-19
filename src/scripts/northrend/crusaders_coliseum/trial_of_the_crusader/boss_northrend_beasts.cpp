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
// Gormok - Firebomb not implemented, timers need correct
// Snakes - Underground phase not worked, timers need correct
// Icehowl - Trample&Crash event not implemented, timers need correct

/* ScriptData
SDName: northrend_beasts
SD%Complete: 90% 
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

#include "ScriptedPch.h"
#include "trial_of_the_crusader.h"

enum Displayids
{
        MODEL_INVISIBLE         =       11686,
};

enum Spells
{
        // Gormok
        SPELL_IMPALE                                    = 66331,
        SPELL_STAGGERING_STOMP                  = 66330,

        N_25_SPELL_IMPALE                               = 67478,
        H_10_SPELL_IMPALE                               = 67477,
        H_25_SPELL_IMPALE                               = 67479,
        N_25_SPELL_STAGGERING_STOMP             = 67647,
        H_10_SPELL_STAGGERING_STOMP             = 67648,
        H_25_SPELL_STAGGERING_STOMP             = 67649,
        SPELL_RISING_ANGER                              = 66636,
        SPELL_BATTER                                    = 66408,
        SPELL_FIRE_BOMB                                 = 66313,
        SPELL_FIRE_BOMB_VISUAL_DAMAGE   = 66318,
        SPELL_HEAD_CRACK                                = 66407,


        // IceHowl
        SPELL_ARTIC_BREATH                      = 66689,
        SPELL_MASSIC_CRASH                      = 66683,
        SPELL_WHIRL                                     = 67345,
        SPELL_FEROCIOUS_BUTT            = 66770,
        N_25_SPELL_ARTIC_BREATH         = 67650,
        H_10_SPELL_ARTIC_BREATH         = 67651,
        H_25_SPELL_ARTIC_BREATH         = 67652,
        N_25_SPELL_MASSIC_CRASH         = 67660,
        H_10_SPELL_MASSIC_CRASH         = 67660,
        H_25_SPELL_MASSIC_CRASH         = 67662,
        N_25_SPELL_WHIRL                        = 67663,
        H_10_SPELL_WHIRL                        = 67663,
        H_25_SPELL_WHIRL                        = 67665,
        N_25_SPELL_FEROCIOUS_BUTT       = 67655,
        H_10_SPELL_FEROCIOUS_BUTT       = 67654,
        H_25_SPELL_FEROCIOUS_BUTT       = 67656,
        SPELL_LOW_ENRAGE                        = 66759,
        SPELL_BERSERK                           = 47008,
        SPELL_TRAMPLE                           = 66734,

        // Worm Twins
        /********* Dreadscale *********/
        SPELL_BURNING_BITE                      = 66879,
        SPELL_MOLTEN_SPEW                       = 66821,
        SPELL_FIRE_SPIT                         = 66796,
        SPELL_BURNING_SPRAY                     = 66902,

        /********* Acidmaw *********/
        SPELL_ACID_SPIT                         = 66880,
        SPELL_PARALYTIC_BITE            = 66824,
        SPELL_PARALYTIC_SPRAY           = 66901,
        SPELL_ACID_SPEW                         = 66818,

        // Difficulty Spells Dreadscale
        N_25_SPELL_BURNING_BITE         = 67624,
        H_10_SPELL_BURNING_BITE     = 67625,
        H_25_SPELL_BURNING_BITE     = 67626,
        N_10_SPELL_MOLTEN_SPEW          = 66820, // Triggered by 66821
        N_25_SPELL_MOLTEN_SPEW          = 67636, // Triggered by 66821
        H_10_SPELL_MOLTEN_SPEW      = 67635, // Triggered by 66821
        H_25_SPELL_MOLTEN_SPEW      = 67637, // Triggered by 66821
        N_25_SPELL_FIRE_SPIT            = 67632,

        //Difficulty Spells Acidmaw
        N_10_SPELL_ACIDIC_SPEW          = 66819, // Triggered by 66818
        N_25_SPELL_ACIDIC_SPEW          = 67610, // Triggered by 66818
        N_25_SPELL_PARALYTIC_BITE       = 67612,
        SPELL_PARALYTIC_TOXIS           = 66823,
        SPELL_ACID_SPIT_25_NH           = 67606,

        //Other Spells
        SPELL_ENRAGE                            = 68335,
        SPELL_SWEAP                                     = 66794,
        SPELL_SLIME_POOL                        = 66882,
        SPELL_SUBMERGE                          = 53421,
        SPELL_ROOT                                      = 42716,
        SPELL_DISENGAGE                         = 61508, // Visual effekt
};

enum Creatures
{
        CREATURE_FIREBOMB       =       34854,
        CREATURE_SNOWBOLD       =       34800,
};


const Position CenterOfArena = {563.331848, 139.412125, 393.836639};

struct boss_gormok_impalerAI : public ScriptedAI
{
    boss_gormok_impalerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
                m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

        uint32 m_uiImpaleTimer;
        uint32 m_uiStaggeringStompTimer;
        uint32 m_uiHealthAmountModifier;
        uint32 m_uiFireBombTimer;
        uint32 m_uiFireBombTriggerTimer;
        uint32 m_uiMaxSnobolds;
        uint32 m_uiSnoboldsLaunched;
        uint32 m_uiSnoboldTimer;
        uint32 m_uiNextEncounterTimer;

        bool m_bIsNextPhase;

    void Reset()
    {
                me->SetSpeed(MOVE_RUN, 1.5f);

                m_uiStaggeringStompTimer = urand(13*IN_MILISECONDS,15*IN_MILISECONDS);
                m_uiImpaleTimer = urand(10*IN_MILISECONDS,11*IN_MILISECONDS);
                m_uiFireBombTimer = urand(15*IN_MILISECONDS,20*IN_MILISECONDS);
                m_uiFireBombTriggerTimer = urand(10*IN_MILISECONDS,18*IN_MILISECONDS);
                m_uiMaxSnobolds = RAID_MODE(4,5,4,5);
                m_uiSnoboldsLaunched = 0;
                m_uiSnoboldTimer = urand(15*IN_MILISECONDS,30*IN_MILISECONDS);
                m_bIsNextPhase = false;

                if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                        m_uiNextEncounterTimer = 180*IN_MILISECONDS;
                }

                if (m_pInstance)
                        m_pInstance->SetData(PHASE_1, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
          if (m_pInstance)
                  m_pInstance->SetData(PHASE_1, IN_PROGRESS);
    }

        void JustSummoned(Unit *snobold)
    {
        if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
        {
            snobold->AddThreat(target, 1000000.0f);
            snobold->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0);
        }
    }

        void JustReachedHome()
    {
                if (m_pInstance)
                {
            m_pInstance->SetData(PHASE_1, NOT_STARTED);
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

                if((m_uiSnoboldsLaunched < m_uiMaxSnobolds) && (m_uiSnoboldTimer < uiDiff))
        {
            if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                if(Creature *snobold = me->SummonCreature(CREATURE_SNOWBOLD,target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0,TEMPSUMMON_DEAD_DESPAWN, 3000))
                    snobold->AddThreat(target, 1000000.0f);
                ++m_uiSnoboldsLaunched;
                DoCast(me, SPELL_RISING_ANGER, true);
            }
            m_uiSnoboldTimer = 30000 + rand()%15000;
        }
        else
            m_uiSnoboldTimer -= uiDiff;

                if (m_uiImpaleTimer <= uiDiff)
                {
                        DoCast(me->getVictim(), SPELL_IMPALE);
                        m_uiImpaleTimer = urand(10*IN_MILISECONDS,11*IN_MILISECONDS);
                }
                else m_uiImpaleTimer -= uiDiff;

                if (m_uiStaggeringStompTimer <= uiDiff)
                {
                        DoCast(me->getVictim(), SPELL_STAGGERING_STOMP);
                        m_uiStaggeringStompTimer = urand(20*IN_MILISECONDS,21*IN_MILISECONDS);
                }
                else m_uiStaggeringStompTimer -= uiDiff;

                if (m_uiFireBombTimer <= uiDiff)
                {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        {
                                DoCast(pTarget, SPELL_FIRE_BOMB);
                                m_uiFireBombTimer = urand(15*IN_MILISECONDS,20*IN_MILISECONDS);
                        }
                }
                else m_uiFireBombTimer -= uiDiff;

                if (m_uiFireBombTriggerTimer <= uiDiff)
                {
                        for(int8 n = 0; n < 4; n++)
                        {
                                Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM);
                                me->SummonCreature(CREATURE_FIREBOMB, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 999999);
                                m_uiFireBombTriggerTimer = urand(15*IN_MILISECONDS,45*IN_MILISECONDS);
                        }
                }
                else m_uiFireBombTriggerTimer -= uiDiff;

                if (getDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || getDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                        if (m_uiNextEncounterTimer <= uiDiff && !m_bIsNextPhase)
                        {
                                m_pInstance->SetData(PHASE_2, IN_PROGRESS);
                                m_bIsNextPhase = true;
                        }
                        else m_uiNextEncounterTimer -= uiDiff;
                }

      DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
                if(m_pInstance)
                m_pInstance->SetData(PHASE_1, DONE);
                m_pInstance->SetData(PHASE_2, IN_PROGRESS);
        }
};

CreatureAI* GetAI_boss_gormok_impaler(Creature* pCreature)
{
    return new boss_gormok_impalerAI(pCreature);
}

struct boss_acidmawAI : public ScriptedAI
{
    boss_acidmawAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
                m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        }

        ScriptedInstance* m_pInstance;

        uint32 m_uiPhase;
        uint32 m_uiPhase1Timer;
        uint32 m_uiPhase2Timer;
        uint32 m_uiRemoveSubmergedTimer;
        uint32 m_uiEnrageTimer;

        /*** Phase 1 ***/
        uint32 m_uiAcidSpitTimer;
        uint32 m_uiParalyticSprayTimer;
        uint32 m_uiSweapTimer;

        /*** Phase 2 ***/
        uint32 m_uiAcidSpewTimer;
        uint32 m_uiParalyticBiteTimer;

        bool m_bIsEnrage;

    void Reset()
    {
                m_uiPhase = 1;
                m_uiPhase2Timer = 50000;
                m_uiEnrageTimer = 2000;

                /*** Phase 1 ***/
                m_uiAcidSpitTimer       = 12000; // Random
                m_uiParalyticSprayTimer = 2000; // Tank
                m_uiSweapTimer  = 10000;
                m_bIsEnrage = false;

                /*** Phase 2 ***/
                m_uiAcidSpewTimer = 16000;
                m_uiParalyticBiteTimer = 5000;
    }

        void KilledUnit(Unit* who)
        {
                if(!m_pInstance)
                        return;
        }

        void JustReachedHome()
    {
        if (m_pInstance)
                {
            m_pInstance->SetData(PHASE_1, NOT_STARTED);
                        m_pInstance->SetData(PHASE_2, NOT_STARTED);
                }

                if(m_pInstance)
                {
                        uint32 count = m_pInstance->GetData(DATA_UPDATE_STATE_UI_COUNT);
                        count--;
                        m_pInstance->SetData(DATA_UPDATE_STATE_UI_COUNT, count);
                        me->ForcedDespawn();
                }
    }

        void EnterCombat(Unit* pWho)
    {
                if (!m_pInstance)
                        return;

                if (m_pInstance->GetData(PHASE_2) == DONE)
                    me->ForcedDespawn();
                else
                    m_pInstance->SetData(PHASE_2, IN_PROGRESS);
    }

        void JustDied(Unit* pKiller)
    {
            if (!m_pInstance)
                        return;

        if(!me->FindNearestCreature(NPC_ACIDMAW, 200, true) && !me->FindNearestCreature(NPC_DREADSCALE, 200, true))
                {
                        m_pInstance->SetData(PHASE_2, DONE);
                        m_pInstance->SetData(PHASE_3, IN_PROGRESS);
                }
        }

        void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

                if (m_uiEnrageTimer <= uiDiff && !m_bIsEnrage)
        {
                        if(!me->FindNearestCreature(NPC_DREADSCALE, 300, true))
                        {
                                DoCast(me, SPELL_ENRAGE );
                                m_bIsEnrage = true;
                        }
                        m_uiEnrageTimer = 2000;
        }
                else m_uiEnrageTimer -= uiDiff;

                if(m_uiPhase == 1)
                {
                        if (!me->HasAura(SPELL_ROOT))
                                DoCast(me, SPELL_ROOT);

                        if (m_uiParalyticSprayTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_PARALYTIC_SPRAY);
                                m_uiParalyticSprayTimer = 5*IN_MILISECONDS;
                        }
                        else m_uiParalyticSprayTimer -= uiDiff;

                        if (m_uiAcidSpitTimer <= uiDiff)
                        {
                                std::list<Unit*> pTargets;
                SelectTargetList(pTargets, 5, SELECT_TARGET_RANDOM, 80, true);
                                for (std::list<Unit*>::const_iterator i = pTargets.begin(); i != pTargets.end(); ++i)
                                DoCast(*i, SPELL_ACID_SPIT);
                                m_uiAcidSpitTimer = 20*IN_MILISECONDS;
                        }
                        else m_uiAcidSpitTimer -= uiDiff;

                        if (m_uiSweapTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_SWEAP);
                                m_uiSweapTimer = 20*IN_MILISECONDS;
                        }
                        else m_uiSweapTimer -= uiDiff;

                        if (m_uiPhase2Timer <= uiDiff)
                        {
                                DoCast(me,      SPELL_SUBMERGE);
                                m_uiPhase = 2;
                                m_uiPhase2Timer = 50*IN_MILISECONDS;
                                m_uiPhase1Timer = 50*IN_MILISECONDS;
                                m_uiRemoveSubmergedTimer = 10000;
                        }
                        else m_uiPhase2Timer -= uiDiff;

                        if (m_uiRemoveSubmergedTimer <= uiDiff)
                        {
                                me->CastSpell(me, 35177, false);
                                me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                                m_uiRemoveSubmergedTimer = 999*IN_MILISECONDS;
                        }
                        else m_uiRemoveSubmergedTimer -= uiDiff;
                }

                if(m_uiPhase == 2)
                {
                        if (me->HasAura(SPELL_ROOT))
                                me->RemoveAurasDueToSpell(SPELL_ROOT);

                        if (m_uiRemoveSubmergedTimer <= uiDiff)
                        {
                                me->CastSpell(me, 35177, false);
                                me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                                m_uiRemoveSubmergedTimer = 999*IN_MILISECONDS;
                        }
                        else m_uiRemoveSubmergedTimer -= uiDiff;

                        if (m_uiAcidSpewTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_ACID_SPEW);
                                m_uiAcidSpewTimer = 20*IN_MILISECONDS;
                        }
                        else m_uiAcidSpewTimer -= uiDiff;

                        if (m_uiParalyticBiteTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_PARALYTIC_BITE);
                                m_uiParalyticBiteTimer = 5*IN_MILISECONDS;
                        }
                        else m_uiParalyticBiteTimer -= uiDiff;

                        if (m_uiPhase1Timer <= uiDiff)
                        {
                                DoCast(me,      SPELL_SUBMERGE);
                                m_uiPhase = 1;
                                m_uiPhase2Timer = 50*IN_MILISECONDS;
                                m_uiPhase1Timer = 50*IN_MILISECONDS;
                                m_uiRemoveSubmergedTimer = 10000;
                        }
                        else m_uiPhase1Timer -= uiDiff;

                        DoMeleeAttackIfReady();
                }
        }
};

CreatureAI* GetAI_boss_acidmaw(Creature* pCreature)
{
    return new boss_acidmawAI(pCreature);
}

struct boss_dreadscaleAI : public ScriptedAI
{
    boss_dreadscaleAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
                m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        }

        ScriptedInstance* m_pInstance;

        uint32 m_uiPhase;
        uint32 m_uiPhase1Timer;
        uint32 m_uiPhase2Timer;
        uint32 m_uiEnrageTimer;

        /*** Phase 1 ***/
        uint32 m_uiBurningBiteTimer;
        uint32 m_uiMoltenSpewTimer;
        uint32 m_uiSubmergeTimer;

        /*** Phase 2 ***/
        uint32 m_uiFireSpitTimer;
        uint32 m_uiBurningSprayTimer;
        uint32 m_uiSlimePoolTimer;
        uint32 m_uiSweapTimer;

        bool m_bIsEnrage;

    void Reset()
    {
                m_uiPhase = 1;
                m_uiPhase2Timer = 40000;
                m_uiEnrageTimer = 2000;

                /*** Phase 1 ***/
                m_uiBurningBiteTimer = 2000;
                m_uiMoltenSpewTimer = 15000;
                m_bIsEnrage = false;

                /*** Phase 2 ***/
                m_uiFireSpitTimer       = 5000;
                m_uiBurningSprayTimer = 15000;
                m_uiSlimePoolTimer = 0;
                m_uiSweapTimer = 10000;
    }

        void KilledUnit(Unit* who)
        {
                if(!m_pInstance)
                        return;
        }

        void JustReachedHome()
    {
        if (m_pInstance)
                {
            m_pInstance->SetData(PHASE_1, NOT_STARTED);
                        m_pInstance->SetData(PHASE_2, NOT_STARTED);
                }

                if(m_pInstance)
                {
                        uint32 count = m_pInstance->GetData(DATA_UPDATE_STATE_UI_COUNT);
                        count--;
                        m_pInstance->SetData(DATA_UPDATE_STATE_UI_COUNT, count);
                        me->ForcedDespawn();
                }
    }

        void EnterCombat(Unit* pWho)
    {
                if (!m_pInstance)
                        return;

                if (m_pInstance->GetData(PHASE_2) == DONE)
                    me->ForcedDespawn();
                else
                    m_pInstance->SetData(PHASE_2, IN_PROGRESS);
    }

        void JustDied(Unit* pKiller)
    {
            if (!m_pInstance)
                return;

        if(!me->FindNearestCreature(NPC_ACIDMAW, 200, true)&& !me->FindNearestCreature(NPC_DREADSCALE, 200, true))
                {
                        m_pInstance->SetData(PHASE_2, DONE);
                        m_pInstance->SetData(PHASE_3, IN_PROGRESS);
                }
        }

        void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

                if (m_uiEnrageTimer <= uiDiff && !m_bIsEnrage)
        {
                        if(!me->FindNearestCreature(NPC_ACIDMAW, 300, true))
                        {
                                DoCast(me, SPELL_ENRAGE );
                                m_bIsEnrage = true;
                        }
                        m_uiEnrageTimer = 2000;
        }
                else m_uiEnrageTimer -= uiDiff;

                if(m_uiPhase == 1)
                {
                        if (me->HasAura(SPELL_ROOT))
                                me->RemoveAurasDueToSpell(SPELL_ROOT);

                        if (m_uiMoltenSpewTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_MOLTEN_SPEW);
                                m_uiMoltenSpewTimer = 20*IN_MILISECONDS;
                        }
                        else m_uiMoltenSpewTimer -= uiDiff;

                        if (m_uiBurningBiteTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_BURNING_BITE);
                                m_uiBurningBiteTimer = 8*IN_MILISECONDS;
                        }
                        else m_uiBurningBiteTimer -= uiDiff;

                        if (m_uiPhase2Timer <= uiDiff)
                        {
                                m_uiPhase = 2;
                                m_uiPhase2Timer = 40*IN_MILISECONDS;
                                m_uiPhase1Timer = 40*IN_MILISECONDS;
                        }
                        else m_uiPhase2Timer -= uiDiff;

                        DoMeleeAttackIfReady();
                }

                if(m_uiPhase == 2)
                {
                        if (!me->HasAura(SPELL_ROOT))
                                DoCast(me, SPELL_ROOT);

                        if (m_uiBurningSprayTimer <= uiDiff)
                        {
                                std::list<Unit*> pTargets;
                SelectTargetList(pTargets, 5, SELECT_TARGET_RANDOM, 80, true);
                                for (std::list<Unit*>::const_iterator i = pTargets.begin(); i != pTargets.end(); ++i)
                                DoCast(*i, SPELL_BURNING_SPRAY);
                                m_uiBurningSprayTimer = 15*IN_MILISECONDS;
                        }
                        else m_uiBurningSprayTimer -= uiDiff;

                        if (m_uiFireSpitTimer <= uiDiff)
                        {
                                DoCast(me->getVictim(), SPELL_FIRE_SPIT);
                                m_uiFireSpitTimer = 5*IN_MILISECONDS;
                        }
                        else m_uiFireSpitTimer -= uiDiff;

                        if (m_uiSweapTimer <= uiDiff)
                        {
                                DoCast(me, SPELL_SWEAP);
                                m_uiSweapTimer = 20*IN_MILISECONDS;
                        }
                        else m_uiSweapTimer -= uiDiff;

                        if (m_uiPhase1Timer <= uiDiff)
                        {
                                m_uiPhase = 1;
                                m_uiPhase1Timer = 40*IN_MILISECONDS;
                                m_uiPhase2Timer = 40*IN_MILISECONDS;
                        }
                        else m_uiPhase1Timer -= uiDiff;
                }
        }
};

CreatureAI* GetAI_boss_dreadscale(Creature* pCreature)
{
    return new boss_dreadscaleAI(pCreature);
}

struct boss_icehowlAI : public ScriptedAI
{
    boss_icehowlAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
                m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        }

        ScriptedInstance* m_pInstance;

        /********** Phase 1  ************/
        uint32 m_uiArticBreathTimer;
        uint32 m_uiWhirlTimer;
        uint32 m_uiFerociusButtTimer;

        /*********** Phase 2 ************/
        uint32 m_uiMassivCrashTimer;
        uint32 m_uiTramplePhaseTimer;
        uint32 Phase;
        uint32 PhaseTimer;

        bool m_bIsTrample;
        bool m_bIsTrampleCast;

    void Reset()
    {
                m_uiArticBreathTimer = 25*IN_MILISECONDS;
                m_uiWhirlTimer  = 15*IN_MILISECONDS;
                m_uiFerociusButtTimer = 10*IN_MILISECONDS;
                m_uiTramplePhaseTimer = 5*IN_MILISECONDS;
                Phase = 0;

                me->SetSpeed(MOVE_WALK, 1.5f);
                me->SetSpeed(MOVE_RUN, 1.5f);
                m_bIsTrample = false;
                m_bIsTrampleCast = false;
    }

        void KilledUnit(Unit* who)
        {
                if(!m_pInstance)
                        return;
        }

        void JustReachedHome()
    {
        if (m_pInstance)
                {
            m_pInstance->SetData(PHASE_1, NOT_STARTED);
                        m_pInstance->SetData(PHASE_2, NOT_STARTED);
                        m_pInstance->SetData(PHASE_3, NOT_STARTED);
                }

                if(m_pInstance)
                {
                        uint32 count = m_pInstance->GetData(DATA_UPDATE_STATE_UI_COUNT);
                        count--;
                        m_pInstance->SetData(DATA_UPDATE_STATE_UI_COUNT, count);
                        me->ForcedDespawn();
                }
    }

        void EnterCombat(Unit* pWho)
    {
                if (!m_pInstance)
                        return;

                if (m_pInstance->GetData(PHASE_3) == DONE)
                    me->ForcedDespawn();
                else
                    m_pInstance->SetData(PHASE_3, IN_PROGRESS);
    }

        void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

                if (m_uiWhirlTimer <= uiDiff)
                {
                        DoCast(me->getVictim(), SPELL_WHIRL);
                        m_uiWhirlTimer = urand (15*IN_MILISECONDS,25*IN_MILISECONDS);
                }
                else m_uiWhirlTimer -= uiDiff;

                if (m_uiFerociusButtTimer <= uiDiff)
                {
                        DoCast(me->getVictim(), SPELL_FEROCIOUS_BUTT);
                        m_uiFerociusButtTimer = urand (25*IN_MILISECONDS,35*IN_MILISECONDS);
                }
                else m_uiFerociusButtTimer -= uiDiff;

                if (m_uiArticBreathTimer <= uiDiff)
                {
                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        {
                                DoCast(pTarget, SPELL_ARTIC_BREATH);
                                m_uiArticBreathTimer = urand (20*IN_MILISECONDS,21*IN_MILISECONDS);
            }
                }
                else m_uiArticBreathTimer -= uiDiff;

                /*if (m_uiTramplePhaseTimer <= uiDiff)
        {
            switch(Phase)
            {
               case 0:
                                        me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(0, CenterOfArena);
                                        m_bIsTrample = false;
                    m_uiTramplePhaseTimer = 5000;
                    Phase = 1;
                    break;
                case 1:
                    DoCastAOE(SPELL_MASSIC_CRASH);
                    m_uiTramplePhaseTimer = 4000;
                    Phase = 2;
                    break;
                case 2:
                    DoCast(me, SPELL_DISENGAGE);
                                        me->SetSpeed(MOVE_WALK, 5.5f);
                                        me->SetSpeed(MOVE_RUN, 5.5f);
                    m_uiTramplePhaseTimer = 3000;
                    Phase = 3;
                    break;
                case 3:
                                        if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                        {
                                                float x, y, z;
                                                pTarget->GetPosition(x,y,z);
                                                me->GetMotionMaster()->MovePoint(1, x,y,z);
                                                m_bIsTrample = true;
                                        }
                                        m_uiTramplePhaseTimer = 3000;
                    Phase = 3;
                    break;
                                case 4:
                                        if(m_bIsTrample)
                                        {
                                                Map* pMap = me->GetMap();
                                                if (pMap && pMap->IsDungeon())
                                                {
                                                        Map::PlayerList const &PlayerList = pMap->GetPlayers();
                                                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                                        {
                                                                Unit* pPlayer = i->getSource();
                                                                if (!pPlayer) continue;
                                                                if (pPlayer->isAlive() && pPlayer->IsWithinDistInMap(me, 5.0f))
                                                                {
                                                                        m_bIsTrampleCast = true;
                                                                        me->CastSpell(pPlayer, SPELL_TRAMPLE, true);
                                                                        me->GetMotionMaster()->MovementExpired();
                                                                        me->GetMotionMaster()->MoveChase(me->getVictim());
                                                                }
                                                        }
                                                }
                                        }
                                        m_uiTramplePhaseTimer = 3000;
                    Phase = 4;
                    break;
                                case 5:
                                        if(!m_bIsTrampleCast)
                                        {
                                                m_bIsTrampleCast = false;
                                                me->SetSpeed(MOVE_WALK, 1.5f);
                                                me->SetSpeed(MOVE_RUN, 1.5f);
                                                me->GetMotionMaster()->MovementExpired();
                                                me->GetMotionMaster()->MoveChase(me->getVictim());
                                        }
                                        m_uiTramplePhaseTimer = 3000;
                    Phase = 5;
                    break;
                default:
                    break;
            }
        } else m_uiTramplePhaseTimer -= uiDiff;*/

                DoMeleeAttackIfReady();
        }

        void JustDied(Unit* pKiller)
    {
            if (!m_pInstance)
                return;

        m_pInstance->SetData(PHASE_1, DONE);
                m_pInstance->SetData(PHASE_2, DONE);
                m_pInstance->SetData(PHASE_3, DONE);
        }
};

CreatureAI* GetAI_boss_icehowl(Creature* pCreature)
{
    return new boss_icehowlAI(pCreature);
}

struct mob_FireBombAI : public Scripted_NoMovementAI
{
    mob_FireBombAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* pInstance;

        uint32 m_uiDespawnTimer;
        uint32 m_uiFireBombTimer;

    void Reset()
        {
                m_uiDespawnTimer = 65000;
                m_uiFireBombTimer = 2000;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(MODEL_INVISIBLE);
        }

    void UpdateAI(const uint32 uiDiff)
    {
                if (m_uiFireBombTimer <= uiDiff)
                {
                        DoCast(me, SPELL_FIRE_BOMB_VISUAL_DAMAGE);
                        m_uiFireBombTimer = 70000;
                }
                else m_uiFireBombTimer -= uiDiff;


                if (m_uiDespawnTimer < uiDiff)
                {
                        me->SetVisibility(VISIBILITY_OFF);
                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE,
                        SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        m_uiDespawnTimer = 70000;
        }
                else m_uiDespawnTimer -= uiDiff;
    }
};

struct mob_SnowboldAI : public ScriptedAI
{
    mob_SnowboldAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

        uint32 m_uiBatterTimer;
        uint32 m_uiHeadCrackTimer;

    void Reset()
        {
                m_uiBatterTimer = 5000;
                m_uiHeadCrackTimer = 10000;
        }

    void UpdateAI(const uint32 uiDiff)
    {
                if (!UpdateVictim())
            return;

                if (m_uiBatterTimer <= uiDiff)
                {
                        DoCast(me->getVictim(), SPELL_BATTER);
                        m_uiBatterTimer = urand (15*IN_MILISECONDS,25*IN_MILISECONDS);
                }
                else m_uiBatterTimer -= uiDiff;

                if (m_uiHeadCrackTimer <= uiDiff)
                {
                        DoCast(me->getVictim(), SPELL_HEAD_CRACK);
                        m_uiHeadCrackTimer = urand (15*IN_MILISECONDS,25*IN_MILISECONDS);
                }
                else m_uiHeadCrackTimer -= uiDiff;

                DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_FireBomb(Creature* pCreature)
{
    return new mob_FireBombAI (pCreature);
}

void AddSC_boss_northrend_beasts()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_gormok_impaler";
        NewScript->GetAI = &GetAI_boss_gormok_impaler;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_acidmaw";
        NewScript->GetAI = &GetAI_boss_acidmaw;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_dreadscale";
        NewScript->GetAI = &GetAI_boss_dreadscale;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_icehowl";
        NewScript->GetAI = &GetAI_boss_icehowl;
    NewScript->RegisterSelf();

        NewScript = new Script;
    NewScript->Name = "mob_firebomb_trigger";
        NewScript->GetAI = &GetAI_mob_FireBomb;
    NewScript->RegisterSelf();
}