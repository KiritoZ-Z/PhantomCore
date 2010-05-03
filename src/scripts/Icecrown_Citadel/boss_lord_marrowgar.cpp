
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

#define SAY_ENTER_ZONE            -1609392
#define SAY_AGGRO                 -1609393
#define SAY_BONE_STORM            -1609394
#define SAY_BONESPIKE_1           -1609395
#define SAY_BONESPIKE_2           -1609396
#define SAY_BONESPIKE_3           -1609397
#define SAY_SLAY_1                -1609398
#define SAY_SLAY_2                -1609399
#define SAY_DEATH                 -1609400
#define SAY_ENRAGE                -1609401

#define SPELL_SABERLASH            RAID_MODE(69055,70814)
#define SPELL_COLDFLAME            RAID_MODE(69146,70823)
#define SPELL_BONESPIKE            RAID_MODE(69057,69057)
#define SPELL_BONESTORM            RAID_MODE(69076,69076)
#define SPELL_ENRAGE               RAID_MODE(26662,26662)

#define EVENT_SABERLASH           1
#define EVENT_COLDFLAME           2
#define EVENT_BONESPIKE           3
#define EVENT_BONESTORM_1         4
#define EVENT_BONESTORM_2         5
#define EVENT_BONESTORM_3         6
#define EVENT_BONESTORM_4         7
#define EVENT_RESET               8
#define EVENT_ENRAGE			  9

struct boss_MarrowgarAI : public ScriptedAI
{
    boss_MarrowgarAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }
    
    ScriptedInstance* pInstance;
    EventMap events;

	bool Enraged;
	bool marrowgarintro;

    void Reset()
    {
		events.Reset();
        
        if (pInstance)
            pInstance->SetData(DATA_MARROWGAR_EVENT, NOT_STARTED);

		Enraged = false;
		marrowgarintro = false;
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
		{
            pInstance->SetData(DATA_MARROWGAR_EVENT, IN_PROGRESS);
		}

		DoScriptText(SAY_AGGRO, me);
		events.ScheduleEvent(EVENT_SABERLASH, 5000);
		events.ScheduleEvent(EVENT_COLDFLAME, 20000);
		events.ScheduleEvent(EVENT_BONESPIKE, 15000);
		events.ScheduleEvent(EVENT_BONESTORM_1, 45000);
		events.ScheduleEvent(EVENT_ENRAGE, 600000);
    }

    void MoveInLineOfSight(Unit *who)
    {
            if (!marrowgarintro && me->IsWithinDistInMap(who,55.0f))
            {
                DoScriptText(SAY_ENTER_ZONE, me);
				marrowgarintro = true;
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

        while(uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_COLDFLAME:
                    if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_COLDFLAME);
                    events.ScheduleEvent(EVENT_COLDFLAME, 15000);
                    return;
                case EVENT_BONESPIKE:
					if (!me->HasAura(SPELL_BONESTORM))
					{
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(pTarget, SPELL_BONESPIKE);
					DoScriptText(RAND(SAY_BONESPIKE_1,SAY_BONESPIKE_2,SAY_BONESPIKE_3), me);
                    events.ScheduleEvent(EVENT_BONESPIKE, 15000);
					}
                    return;
				//Hacky, but dont care. Start.
                case EVENT_BONESTORM_1:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_BONESTORM);
					DoScriptText( SAY_BONE_STORM, me);
                    events.ScheduleEvent(EVENT_BONESTORM_2, 7000);
                    return;
				//Spin 2
                case EVENT_BONESTORM_2:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(pTarget, SPELL_BONESTORM, 1);
                    events.ScheduleEvent(EVENT_BONESTORM_3, 7000);
                    return;
				//Spin 3
                case EVENT_BONESTORM_3:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(pTarget, SPELL_BONESTORM, 1);
                    events.ScheduleEvent(EVENT_BONESTORM_4, 7000);
                    return;
				//Spin 4
                case EVENT_BONESTORM_4:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
					DoCast(pTarget, SPELL_BONESTORM, 1);
                    events.ScheduleEvent(EVENT_RESET, 7000);
                    return;
				case EVENT_RESET:
					me->RemoveAurasDueToSpell(SPELL_BONESTORM);
					events.ScheduleEvent(EVENT_BONESTORM_1, 60000);
					return;
				case EVENT_SABERLASH:
					if (!me->HasAura(SPELL_BONESTORM))
                    DoCast(me->getVictim(), SPELL_SABERLASH);
					events.ScheduleEvent(EVENT_SABERLASH, 5000);
                    return;
				case EVENT_ENRAGE:
                    DoCast(me->getVictim(), SPELL_ENRAGE);
					DoScriptText( SAY_ENRAGE, me);
					events.ScheduleEvent(EVENT_ENRAGE, 300000);
                    return;
			}
		}
        DoMeleeAttackIfReady();
}
    void JustDied(Unit* killer)  
    {
			DoScriptText( SAY_DEATH, me);
			if (pInstance)
            pInstance->SetData(DATA_MARROWGAR_EVENT, DONE);
    }
};

CreatureAI* GetAI_boss_Marrowgar(Creature* pCreature)
{
    return new boss_MarrowgarAI (pCreature);
}

void AddSC_boss_Marrowgar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_Marrowgar";
    newscript->GetAI = &GetAI_boss_Marrowgar;
    newscript->RegisterSelf();
}