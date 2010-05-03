/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
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

#define SAY_INTRO_1                   -1609402
#define SAY_INTRO_2                   -1609403
#define SAY_INTRO_3                   -1609404
#define SAY_INTRO_4                   -1609405
#define SAY_INTRO_5                   -1609406
#define SAY_INTRO_6                   -1609407
#define SAY_INTRO_7                   -1609408
#define SAY_AGGRO                     -1609409
#define SAY_PHASE_2                   -1609410
#define SAY_DARK_EMPOWERMENT          -1609411
#define SAY_DARK_TRANSFORMATION       -1609412
#define SAY_RAISE_DEAD                -1609413
#define SAY_DOMINATE_MIND             -1609414
#define SAY_ENRAGE                    -1609415
#define SAY_DEATH                     -1609416
#define SAY_SLAY_1                    -1609417
#define SAY_SLAY_2                    -1609418


#define SPELL_ENRAGE                   RAID_MODE(26662,26662)
#define SPELL_DAD                      RAID_MODE(71001,72108)
#define SPELL_FROSTBOLT                RAID_MODE(71420,72007)
#define SPELL_FROSTBOLT_VOLLEY         RAID_MODE(72905,72906)
#define SPELL_MANA_BARRIER             RAID_MODE(70842,70842)
#define SPELL_SHADOW_BOLT              RAID_MODE(71254,72008)
#define SPELL_TOUCH_OF_INSIGINIFANCE   RAID_MODE(71204,71204)
#define SPELL_DARK_EMPOWERMENT         RAID_MODE(70901,70901)
#define SPELL_DOMINATE_MIND            71289
#define SPELL_HACK_HEAL                15068 // This is not right, but im just hackin it for now.

#define EVENT_ENRAGE              1
#define EVENT_DAD                 2
#define EVENT_FROSTBOLT           3
#define EVENT_FROSTBOLT_VOLLEY    4
#define EVENT_MANA_BARRIER        5
#define EVENT_SHADOW_BOLT         6
#define EVENT_TOUCH_OF_INSI       7
#define EVENT_DARK_EMPOWERMENT    8
#define EVENT_DOMINATE_MIND  	  9
#define EVENT_INTRO_1             10
#define EVENT_INTRO_2             11
#define EVENT_INTRO_3             12
#define EVENT_INTRO_4             13
#define EVENT_INTRO_5             14
#define EVENT_INTRO_6             15
#define EVENT_INTRO_7             16
#define EVENT_SPAWN_LEFT          17
#define EVENT_SPAWN_RIGHT         18

// Not like I couldnt have just checked the NPCs in the DB, but thanks scriptdev2 people for the coords
#define ADD_1X -619.006
#define ADD_1Y 2158.104
#define ADD_1Z 50.848

#define ADD_2X -598.697
#define ADD_2Y 2157.767
#define ADD_2Z 50.848

#define ADD_3X -577.992
#define ADD_3Y 2156.989
#define ADD_3Z 50.848

#define ADD_4X -618.748
#define ADD_4Y 2266.648
#define ADD_4Z 50.849

#define ADD_5X -598.573
#define ADD_5Y 2266.870
#define ADD_5Z 50.849

#define ADD_6X -578.360
#define ADD_6Y 2267.210
#define ADD_6Z 50.849

struct boss_DeathwhisperAI : public ScriptedAI
{
    boss_DeathwhisperAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();

        SpellEntry *TempSpell;
        TempSpell = GET_SPELL(SPELL_MANA_BARRIER);
        if (TempSpell && TempSpell->EffectTriggerSpell[0] != SPELL_HACK_HEAL)
        {
            TempSpell->EffectTriggerSpell[0] = SPELL_HACK_HEAL;
        }
    }
    
    ScriptedInstance* pInstance;
    EventMap events;

	bool Enraged;
	bool deathwhisperintro;
    uint8 Phase;

    void Reset()
    {
		events.Reset();
        
        if (pInstance)
            pInstance->SetData(DATA_DEATHWHISPER_EVENT, NOT_STARTED);

		Enraged = false;
		deathwhisperintro = false;
        Phase = 1;
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
		{
            pInstance->SetData(DATA_DEATHWHISPER_EVENT, IN_PROGRESS);
		}
		
		Phase = 1;
		DoScriptText(SAY_AGGRO, me);
		events.ScheduleEvent(EVENT_ENRAGE, 600000);
        events.ScheduleEvent(EVENT_SPAWN_LEFT, 20000);
        events.ScheduleEvent(EVENT_DAD, 15000);
        events.ScheduleEvent(EVENT_SHADOW_BOLT, 1);
    }

    // Mana Barrier is broken, it appears as though mana shields effects dont work either(?)
    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (me->HasAura(SPELL_MANA_BARRIER))
        {
            me->SetHealth(me->GetHealth()+damage);
            me->SetPower(POWER_MANA,me->GetPower(POWER_MANA)-damage);
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!deathwhisperintro && me->IsWithinDistInMap(who,105.0f))
        {
			deathwhisperintro = true;
			events.ScheduleEvent(EVENT_INTRO_1, 1);
		}
        ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit *victim)
    {
        if (victim == me)
            return;
        DoScriptText(RAND(SAY_SLAY_1,SAY_SLAY_2), me);
    }
    
    void UpdateAI(const uint32 diff)
    {
         //Return since we have no target
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->hasUnitState(UNIT_STAT_CASTING))
            return;

		if ((!me->HasAura(SPELL_MANA_BARRIER)) && (Phase = 1))
		{
			DoCast(me, SPELL_MANA_BARRIER);
            DoStartNoMovement(me->getVictim());
		}

        if (Phase == 1)
        {
            if ((me->GetPower(POWER_MANA)*100 / me->GetMaxPower(POWER_MANA)) < 10)
            {
                Phase = 2;
                return;
            }
        }

        if (Phase == 2)
        {
            if (me->HasAura(SPELL_MANA_BARRIER))
                me->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
        }

        while(uint32 eventId = events.ExecuteEvent())
        {
            Creature* prightmob1= NULL;
            Creature* prightmob2= NULL;
            Creature* prightmob3= NULL;
            Creature* pleftmob1= NULL;
            Creature* pleftmob2= NULL;
            Creature* pleftmob3= NULL;
            switch(eventId)
            {
                case EVENT_INTRO_1:
                    DoScriptText(SAY_INTRO_1, me);
                    events.ScheduleEvent(EVENT_INTRO_2, 1500);
                    return;
                case EVENT_INTRO_2:
                    DoScriptText(SAY_INTRO_2, me);
					events.ScheduleEvent(EVENT_INTRO_3, 1500);
                    return;
                case EVENT_INTRO_3:
                    DoScriptText(SAY_INTRO_3, me);
					events.ScheduleEvent(EVENT_INTRO_4, 1500);
                    return;
                case EVENT_INTRO_4:
                    DoScriptText(SAY_INTRO_4, me);
					events.ScheduleEvent(EVENT_INTRO_5, 1500);
                    return;
                case EVENT_INTRO_5:
                    DoScriptText(SAY_INTRO_5, me);
					events.ScheduleEvent(EVENT_INTRO_6, 1500);
                    return;
                case EVENT_INTRO_6:
                    DoScriptText(SAY_INTRO_6, me);
					events.ScheduleEvent(EVENT_INTRO_7, 1500);
                    return;
                case EVENT_INTRO_7:
                    DoScriptText(SAY_INTRO_7, me);
                    return;
				case EVENT_ENRAGE:
                    DoCast(me->getVictim(), SPELL_ENRAGE);
					DoScriptText( SAY_ENRAGE, me);
					events.ScheduleEvent(EVENT_ENRAGE, 300000);
                    return;
                // Why wouldnt this work with just pleftmobs?
                case EVENT_SPAWN_LEFT:
                    if (Phase = 1)
                    {
                    pleftmob1 = me->SummonCreature(NPC_CULT_FANATIC,ADD_1X,ADD_1Y,ADD_1Z,1.532153,TEMPSUMMON_CORPSE_DESPAWN);
                    pleftmob2 = me->SummonCreature(NPC_CULT_ADHERENT,ADD_2X,ADD_2Y,ADD_2Z,1.532153,TEMPSUMMON_CORPSE_DESPAWN);
                    pleftmob3 = me->SummonCreature(NPC_CULT_FANATIC,ADD_3X,ADD_3Y,ADD_3Z,1.532153,TEMPSUMMON_CORPSE_DESPAWN);
                    if (pleftmob1)
                    {
                    pleftmob1->setActive(true);
                    pleftmob1->AI()->DoZoneInCombat();
                    pleftmob2->setActive(true);
                    pleftmob2->AI()->DoZoneInCombat();
                    pleftmob3->setActive(true);
                    pleftmob3->AI()->DoZoneInCombat();
                    }
                    events.ScheduleEvent(EVENT_SPAWN_RIGHT, 60000);
                    }
                    return;
                // Why wouldn't this work just prightmobs?
                case EVENT_SPAWN_RIGHT:
                    if (Phase = 1)
                    {
                    prightmob1 = me->SummonCreature(NPC_CULT_ADHERENT,ADD_4X,ADD_4Y,ADD_4Z,4.669165,TEMPSUMMON_CORPSE_DESPAWN);
                    prightmob2 = me->SummonCreature(NPC_CULT_FANATIC,ADD_5X,ADD_5Y,ADD_5Z,4.669165,TEMPSUMMON_CORPSE_DESPAWN);
                    prightmob3 = me->SummonCreature(NPC_CULT_ADHERENT,ADD_6X,ADD_6Y,ADD_6Z,4.669165,TEMPSUMMON_CORPSE_DESPAWN);
                    if (prightmob2)
                    {
                    prightmob1->setActive(true);
                    prightmob1->AI()->DoZoneInCombat();
                    prightmob2->setActive(true);
                    prightmob2->AI()->DoZoneInCombat();
                    prightmob3->setActive(true);
                    prightmob3->AI()->DoZoneInCombat();
                    }
                    events.ScheduleEvent(EVENT_SPAWN_LEFT, 60000);
                    }
                    return;
				case EVENT_DAD:
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget, SPELL_DAD);
					events.ScheduleEvent(EVENT_DAD, 20000);
                    return;
				case EVENT_SHADOW_BOLT:
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget, SPELL_SHADOW_BOLT);
					events.ScheduleEvent(EVENT_SHADOW_BOLT, 5000);
                    return;
			}
		}
    }

    void JustDied(Unit* killer)  
    {
			DoScriptText( SAY_DEATH, me);
		 	if (pInstance)
            pInstance->SetData(DATA_DEATHWHISPER_EVENT, DONE);
    }
};

CreatureAI* GetAI_boss_Deathwhisper(Creature* pCreature)
{
    return new boss_DeathwhisperAI (pCreature);
}

void AddSC_boss_Deathwhisper()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_Deathwhisper";
    newscript->GetAI = &GetAI_boss_Deathwhisper;
    newscript->RegisterSelf();
}
