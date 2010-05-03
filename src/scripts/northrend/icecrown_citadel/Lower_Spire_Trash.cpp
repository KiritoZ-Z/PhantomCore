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

/***************************************SPELLS*************************************/
//Nerub'ar Broodkeeper
#define SPELL_CRYPTSCARABS                RAID_MODE(70965,70965)
#define SPELL_DARKMENDING                 RAID_MODE(71020,71020)
#define SPELL_WEBWRAP                     RAID_MODE(70980,70980)

//The Damned
#define SPELL_BONEFLURRY                  RAID_MODE(70960,70960)
#define SPELL_SHATTEREDBONES              RAID_MODE(70961,70961)

//Servant of the Throne
#define SPELL_GLACIALBLAST                 RAID_MODE(71029,71029)

//Ancient Skeletal Soldier
#define SPELL_SHIELDBASH                  RAID_MODE(70964,70964)

//Deathbound Ward
#define SPELL_DISRUPTINGSHOUT             RAID_MODE(70122,70122)
#define SPELL_SABERLASH                   RAID_MODE(70121,70121)

//Deathspeaker Attendant
#define SPELL_SHADOWBOLT                  RAID_MODE(69387,69387)
#define SPELL_SHADOWNOVA                  RAID_MODE(69355,71106)

//Deathspeaker Disciple
#define SPELL_DARKBLESSING				  RAID_MODE(69391,69391)
#define SPELL_SHADOWBOLT2                 RAID_MODE(69387,69387)
#define SPELL_SHADOWMEND                  RAID_MODE(69389,71107)

//Deathspeaker High Priest
#define SPELL_AURAOFDARKNESS              RAID_MODE(69491,69491)
#define SPELL_DARKRECKONING               RAID_MODE(69483,69483)

//Deathspeaker Servant
#define SPELL_CHAOSBOLT					  RAID_MODE(69576,71108)
#define SPELL_CONSUMINGSHADOWS            RAID_MODE(69405,69405)
#define SPELL_CURSEOFAGONY                RAID_MODE(69404,71112)

//Deathspeaker Zealot
#define SPELL_SHADOWCLEAVE                RAID_MODE(69492,69492)

//Cult Adherant
#define SPELL_CURSEOFTRPOR                RAID_MODE(71237,71237)
#define SPELL_DARKEMPOWERMENT             RAID_MODE(70901,70901)
#define SPELL_DARKMARTYDOM                RAID_MODE(70903,72498)
#define SPELL_DEATHCHILLBLAST             RAID_MODE(70594,72005)
#define SPELL_DEATHCHILLBLAST_EM          RAID_MODE(70906,72485)
#define SPELL_SHROUDOFTHEOCCULT           RAID_MODE(70768,70768)
#define COSMETIC_TELEPORT                 52096

//Cult Fanatic
#define SPELL_DARKMARTYRDOM               RAID_MODE(71236,72495)
#define SPELL_DARKTRANSFORMATION          RAID_MODE(70900,70900)
#define SPELL_NECROTICSTRIKE              RAID_MODE(70659,72490)
#define SPELL_SHADOWCLEAVE                RAID_MODE(70670,72006)
#define SPELL_VAMPIRICMIGHT               RAID_MODE(70674,70674)

/****************************************EVENTS************************************/
//Nerub'ar Broodkeeper
#define EVENT_CRYPTSCARABS          1
#define EVENT_DARKMENDING           2
#define EVENT_WEBWRAP               3

//The Damned
#define EVENT_BONEFLURRY            4
#define EVENT_SHATTEREDBONES        5

//Servant of the Throne
#define EVENT_GLACIALBLAST          6

//Ancient Skeletal Soldier
#define EVENT_SHIELDBASH            7

//Deathbound Ward
#define EVENT_DISRUPTINGSHOUT       8
#define EVENT_SABERLASH             9

//Deathspeaker Attendant
#define EVENT_SHADOWBOLT            10
#define EVENT_SHADOWNOVA            11

//Deathspeaker Disciple
#define EVENT_SHADOWBOLT2           12
#define EVENT_DARKBLESSING          13
#define EVENT_SHADOWMEND            14

//Deathspeaker High Priest
#define EVENT_DARKRECKONING         15

//Deathspeaker Servant
#define EVENT_CHAOSBOLT             16
#define EVENT_CONSUMINGSHADOWS      17
#define EVENT_CURSEOFAGONY          18

//Deathspeaker Zealot
#define EVENT_SHADOWCLEAVE          19

//Cult Adherant
#define EVENT_CURSEOFTRPOR          20
#define EVENT_DARKEMPOWERMENT       21
#define EVENT_DARKMARTYDOM          22
#define EVENT_DEATHCHILLBLAST       23
#define EVENT_SHROUDOFTHEOCCULT     24

//Cult Fanatic
#define EVENT_DARKMARTYRDOM         25
#define EVENT_DARKTRANSFORMATION    26
#define EVENT_NECROTICSTRIKE        27
#define EVENT_SHADOWCLEAVE          28
#define EVENT_VAMPIRICMIGHT         29


/*****************************MARROWGAR TRASH****************************/
struct npc_NerubarBroodkeeperAI: public ScriptedAI
{
    npc_NerubarBroodkeeperAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_CRYPTSCARABS, 10000);
	events.ScheduleEvent(EVENT_DARKMENDING, 17800);
	events.ScheduleEvent(EVENT_WEBWRAP, 12000);
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
                case EVENT_CRYPTSCARABS:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CRYPTSCARABS);
                    events.RescheduleEvent(EVENT_CRYPTSCARABS, 12000);
                    return;
                case EVENT_DARKMENDING:
                    DoCast(me, SPELL_DARKMENDING);
                    events.RescheduleEvent(EVENT_DARKMENDING, 17800);
                    return;
                case EVENT_WEBWRAP:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_WEBWRAP);
                    events.RescheduleEvent(EVENT_WEBWRAP, 15000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_TheDamnedAI: public ScriptedAI
{
    npc_TheDamnedAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_BONEFLURRY, 8000);
    }

	void JustDied(Unit* who)
	{
	DoCastAOE(SPELL_SHATTEREDBONES);
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
                case EVENT_BONEFLURRY:
                    DoCast(me, SPELL_BONEFLURRY, 1);
                    events.RescheduleEvent(EVENT_BONEFLURRY, 15000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_ServantoftheThroneAI: public ScriptedAI
{
    npc_ServantoftheThroneAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_GLACIALBLAST, 13000);
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
                case EVENT_GLACIALBLAST:
                    DoCastAOE(SPELL_GLACIALBLAST);
                    events.RescheduleEvent(EVENT_GLACIALBLAST, 8000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_AncientSkeletalSoldierAI: public ScriptedAI
{
    npc_AncientSkeletalSoldierAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_SHIELDBASH, 8000);
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
                case EVENT_SHIELDBASH:
                    DoCast(me->getVictim(), SPELL_SHIELDBASH);
                    events.RescheduleEvent(EVENT_SHIELDBASH, 8000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_DeathboundWardAI: public ScriptedAI
{
    npc_DeathboundWardAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_DISRUPTINGSHOUT, 8000);
    events.ScheduleEvent(EVENT_SABERLASH, 8000);
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
                case EVENT_DISRUPTINGSHOUT:
                    DoCast(me, SPELL_DISRUPTINGSHOUT);
                    events.RescheduleEvent(EVENT_DISRUPTINGSHOUT, 8000);
                    return;
                case EVENT_SABERLASH:
                    DoCast(me->getVictim(), SPELL_SABERLASH);
                    events.RescheduleEvent(EVENT_SABERLASH, 8000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

/*****************************DEATHWHISPER TRASH****************************/
struct npc_DeathspeakerAttedantAI: public ScriptedAI
{
    npc_DeathspeakerAttedantAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_SHADOWBOLT, 8000);
    events.ScheduleEvent(EVENT_SHADOWNOVA, 23000);
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
                case EVENT_SHADOWBOLT:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_SHADOWBOLT);
                    events.RescheduleEvent(EVENT_SHADOWBOLT, 8000);
                    return;
                case EVENT_SHADOWNOVA:
                    DoCastAOE(SPELL_SHADOWNOVA);
                    events.RescheduleEvent(EVENT_SHADOWNOVA, 17000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_DeathspeakerDiscipleAI: public ScriptedAI
{
    npc_DeathspeakerDiscipleAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_SHADOWBOLT2, 8000);
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
                case EVENT_DARKBLESSING:
					if(Unit* pTarget = me->SelectNearestTarget(45))
					if(pTarget->GetHealth() == pTarget->GetHealth() * 100 / pTarget->GetMaxHealth() && pTarget->IsFriendlyTo(me))
                    DoCast(pTarget, SPELL_DARKBLESSING);
                    events.RescheduleEvent(EVENT_DARKBLESSING, 20000);
                    return;
                case EVENT_SHADOWBOLT2:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_SHADOWBOLT2);
                    events.RescheduleEvent(EVENT_SHADOWBOLT2, 8000);
                    return;
                case EVENT_SHADOWMEND:
					if(Unit* pTarget = me->SelectNearestTarget(45))
					if(pTarget->GetHealth() == pTarget->GetHealth() * 100 / pTarget->GetMaxHealth() && pTarget->IsFriendlyTo(me))
                    DoCast(pTarget, SPELL_SHADOWMEND);
                    events.RescheduleEvent(EVENT_SHADOWMEND, 25000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_DeathspeakerHighPriestAI: public ScriptedAI
{
    npc_DeathspeakerHighPriestAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_DARKRECKONING, 10000);
    DoCast(me, SPELL_AURAOFDARKNESS);
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
                case EVENT_DARKRECKONING:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_DARKRECKONING);
                    events.RescheduleEvent(EVENT_DARKRECKONING, 20000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_DeathspeakerServantAI: public ScriptedAI
{
    npc_DeathspeakerServantAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_CHAOSBOLT, 15000);
    events.ScheduleEvent(EVENT_CONSUMINGSHADOWS, 13000);
    events.ScheduleEvent(EVENT_CURSEOFAGONY, 10000);
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
                case EVENT_CHAOSBOLT:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CHAOSBOLT);
                    events.RescheduleEvent(EVENT_CHAOSBOLT, 15000);
                    return;
                case EVENT_CONSUMINGSHADOWS:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CONSUMINGSHADOWS);
                    events.RescheduleEvent(EVENT_CONSUMINGSHADOWS, 13000);
                    return;
                case EVENT_CURSEOFAGONY:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CURSEOFAGONY);
                    events.RescheduleEvent(EVENT_CURSEOFAGONY, 17000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_DeathspeakerZealotAI: public ScriptedAI
{
    npc_DeathspeakerZealotAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_SHADOWCLEAVE, 8000);
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
                case EVENT_SHADOWCLEAVE:
                    DoCast(me->getVictim(), SPELL_SHADOWCLEAVE);
                    events.RescheduleEvent(EVENT_SHADOWCLEAVE, 8000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_CultAdherentAI: public ScriptedAI
{
    npc_CultAdherentAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

	bool bReanimated;
	bool bEmpowered;

    void Reset()
    {
        events.Reset();
		bEmpowered = false;
		bReanimated = false;
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO,0))
            summoned->AI()->AttackStart(target);

        DoCast(COSMETIC_TELEPORT);
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
    }



    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_CURSEOFTRPOR, 10000);
    events.ScheduleEvent(EVENT_DARKEMPOWERMENT, 30000);
    events.ScheduleEvent(EVENT_DEATHCHILLBLAST, 20000);
    events.ScheduleEvent(EVENT_SHROUDOFTHEOCCULT, 15000);

    DoCast(COSMETIC_TELEPORT);
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
                case EVENT_CURSEOFTRPOR:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_CURSEOFTRPOR);
                    events.RescheduleEvent(EVENT_CURSEOFTRPOR, 8000);
                    return;
                    
                case EVENT_DARKEMPOWERMENT:
                    DoCast(me, RAND(SPELL_DARKEMPOWERMENT,SPELL_DARKMARTYDOM));
                    events.RescheduleEvent(EVENT_DARKEMPOWERMENT, 9999999);
					bEmpowered = true;
                    return;
                    
                case EVENT_DEATHCHILLBLAST:
					if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
					if (bEmpowered = false)
					{
                    DoCast(pTarget, SPELL_DEATHCHILLBLAST);
					} else {
					DoCast(pTarget, SPELL_DEATHCHILLBLAST_EM);
					}
                    events.RescheduleEvent(EVENT_DEATHCHILLBLAST, 8000);
                    return;
				case EVENT_SHROUDOFTHEOCCULT:
					DoCast(me, SPELL_SHROUDOFTHEOCCULT);
					events.RescheduleEvent(EVENT_SHROUDOFTHEOCCULT, 8000);
					return;
            }
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_CultFanaticAI: public ScriptedAI
{
    npc_CultFanaticAI(Creature *c) : ScriptedAI(c)
    {
    }
    EventMap events;

	bool Martyr;
	bool Transform;

    void Reset()
    {
        events.Reset();

		Martyr = false;
		Transform = false;
    }

    void Aggro(Unit* pWho)
    {
        me->SetInCombatWithZone();
    }


    void EnterCombat(Unit* who)
    {
    events.ScheduleEvent(EVENT_DARKMARTYRDOM, 45000);
    events.ScheduleEvent(EVENT_NECROTICSTRIKE, 8000);
    events.ScheduleEvent(EVENT_SHADOWCLEAVE, 8000);
    events.ScheduleEvent(EVENT_VAMPIRICMIGHT, 8000);

    DoCast(COSMETIC_TELEPORT);
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* target = SelectUnit(SELECT_TARGET_TOPAGGRO,0))
            summoned->AI()->AttackStart(target);

        DoCast(COSMETIC_TELEPORT);
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
                case EVENT_DARKMARTYRDOM:
					if ((Martyr = false) && (Transform = false))
                    DoCast(me, RAND(SPELL_DARKMARTYRDOM, SPELL_DARKTRANSFORMATION));
                    events.RescheduleEvent(EVENT_DARKMARTYRDOM, 999999);
                    return;
                case EVENT_NECROTICSTRIKE:
                    DoCast(me->getVictim(), SPELL_NECROTICSTRIKE);
                    events.RescheduleEvent(EVENT_NECROTICSTRIKE, 13000);
                    return;
                case EVENT_SHADOWCLEAVE:
                    DoCast(me->getVictim(), SPELL_SHADOWCLEAVE);
                    events.RescheduleEvent(EVENT_SHADOWCLEAVE, 10000);
                    return;
                case EVENT_VAMPIRICMIGHT:
                    DoCast(me, SPELL_VAMPIRICMIGHT);
                    events.RescheduleEvent(EVENT_VAMPIRICMIGHT, 15000);
                    return;
            }
        }
        DoMeleeAttackIfReady();
    }
};


/*****************************MARROWGAR TRASH****************************/
CreatureAI* GetAI_npc_NerubarBroodkeeperAI(Creature* pCreature)
{
    return new npc_NerubarBroodkeeperAI (pCreature);
}

CreatureAI* GetAI_npc_TheDamnedAI(Creature* pCreature)
{
    return new npc_TheDamnedAI (pCreature);
}

CreatureAI* GetAI_npc_ServantoftheThroneAI(Creature* pCreature)
{
    return new npc_ServantoftheThroneAI (pCreature);
}

CreatureAI* GetAI_npc_AncientSkeletalSoldierAI(Creature* pCreature)
{
    return new npc_AncientSkeletalSoldierAI (pCreature);
}

CreatureAI* GetAI_npc_DeathboundWardAI(Creature* pCreature)
{
    return new npc_DeathboundWardAI (pCreature);
}

/*****************************DEATHWHISPER TRASH**************************/
CreatureAI* GetAI_npc_DeathspeakerAttedantAI(Creature* pCreature)
{
    return new npc_DeathspeakerAttedantAI (pCreature);
}

CreatureAI* GetAI_npc_DeathspeakerDiscipleAI(Creature* pCreature)
{
    return new npc_DeathspeakerDiscipleAI (pCreature);
}

CreatureAI* GetAI_npc_DeathspeakerHighPriestAI(Creature* pCreature)
{
    return new npc_DeathspeakerHighPriestAI (pCreature);
}

CreatureAI* GetAI_npc_DeathspeakerServantAI(Creature* pCreature)
{
    return new npc_DeathspeakerServantAI (pCreature);
}

CreatureAI* GetAI_npc_DeathspeakerZealotAI(Creature* pCreature)
{
    return new npc_DeathspeakerZealotAI (pCreature);
}

CreatureAI* GetAI_npc_CultAdherentAI(Creature* pCreature)
{
    return new npc_CultAdherentAI (pCreature);
}

CreatureAI* GetAI_npc_CultFanaticAI(Creature* pCreature)
{
    return new npc_CultFanaticAI (pCreature);
}

void AddSC_LowerSpireTrash()
{
/*****************************MARROWGAR TRASH****************************/
    Script *newscript;
    newscript = new Script;
    newscript->Name="npc_NerubarBroodkeeper";
    newscript->GetAI = &GetAI_npc_NerubarBroodkeeperAI;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name="npc_TheDamned";
    newscript->GetAI = &GetAI_npc_TheDamnedAI;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name="npc_ServantoftheThrone";
    newscript->GetAI = &GetAI_npc_ServantoftheThroneAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_AncientSkeletalSoldier";
    newscript->GetAI = &GetAI_npc_AncientSkeletalSoldierAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_DeathboundWard";
    newscript->GetAI = &GetAI_npc_DeathboundWardAI;
    newscript->RegisterSelf();

/*****************************DEATHWHISPER TRASH**************************/
    newscript = new Script;
    newscript->Name="npc_DeathspeakerAttedant";
    newscript->GetAI = &GetAI_npc_DeathspeakerAttedantAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_DeathspeakerDisciple";
    newscript->GetAI = &GetAI_npc_DeathspeakerDiscipleAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_DeathspeakerHighPriest";
    newscript->GetAI = &GetAI_npc_DeathspeakerHighPriestAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_DeathspeakerServant";
    newscript->GetAI = &GetAI_npc_DeathspeakerServantAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_DeathspeakerZealot";
    newscript->GetAI = &GetAI_npc_DeathspeakerZealotAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_CultAdherent";
    newscript->GetAI = &GetAI_npc_CultAdherentAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_CultFanatic";
    newscript->GetAI = &GetAI_npc_CultFanaticAI;
    newscript->RegisterSelf();
}