/*
 * Copyright (C) 2009-2010 TrinityCore <http://www.trinitycore.org/>
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
#include "trial_of_the_crussader.h"

#define GOSSIP_START_EVENT1     "Los Fordering lass uns Starten"
#define GOSSIP_START_EVENT2     "Ich bin bereit für die nachste Herausforderung"

enum Spells
{
	 SPELL_ROOTET_JARAXXUS				= 67924,
	 SPELL_WILFRED_PORTAL				= 68424,
	 SPELL_WILFRED_CAST_PORTAL			= 46589,
};

enum Mobs
{
	CREATURE_JARAXXUS_PORTAL_TRIGGER	=	41000,
};

static uint32 ChampHealers[2][4] =
{
    //Druid, Pala, Priest, Shammy
    {34469, 34465, 34466, 34470}, //Alliance
    {34459, 34445, 34447, 34444} //Horde
};

static uint32 ChampRanged[2][5] =
{
    //Boomkin, Hunter, Mage, SPriest, Warlock
    {34460, 34467, 34468, 34473, 34474}, //Alliance
    {34451, 34448, 34449, 34441, 34450} //Horde
};

static uint32 ChampMelee[2][5] =
{
    //DK, RetPala, Rogue, Enh.Shammy, Warrior
    {34461, 34471, 34472, 34463, 34475}, //Alliance
    {34458, 34456, 34454, 34455, 34453} //Horde
};

/* Champions Coordinates
r h h h r 0 4 5 6 2
r m m m r 1 7 8 9 3
*/
static float ChampSpawnLocs[2][10][4] =
{
    {
        //Alliance (North Side)
        {610.74f, 143.20f, 395.14f, 3.14f}, {605.38f, 143.81f, 394.67f, 3.13f},
        {607.97f, 121.16f, 395.14f, 3.07f}, {602.02f, 122.15f, 395.66f, 2.97f},
        {611.24f, 138.07f, 395.13f, 2.90f}, {610.11f, 132.49f, 395.14f, 2.91f},
        {608.55f, 127.28f, 395.14f, 2.93f}, {605.62f, 138.90f, 394.65f, 3.06f},
        {604.59f, 133.98f, 394.66f, 2.89f}, {603.48f, 128.42f, 394.66f, 3.00f}
    },
    {
        //Horde (South Side)
        {520.48f, 120.54f, 395.14f, 0.26f}, {525.19f, 121.89f, 394.66f, 0.18f},
        {516.03f, 140.94f, 395.13f, 6.28f}, {521.19f, 141.24f, 394.67f, 0.00f},
        {519.24f, 124.74f, 395.14f, 0.28f}, {518.05f, 129.36f, 395.14f, 0.14f},
        {517.23f, 135.03f, 395.14f, 0.04f}, {523.98f, 125.91f, 394.66f, 0.25f},
        {522.65f, 130.43f, 394.67f, 0.18f}, {521.40f, 136.07f, 394.67f, 0.18f}
    }
};

struct npc_tcrus_announcerAI : public ScriptedAI
{
    npc_tcrus_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    InstanceData* pInstance;

	Creature* Fjola;
	Creature* Eydis;

	uint32 mMaxChampions;
    uint32 mHChampRnd1;
    uint32 mHChampRnd2;
    uint32 mRChampRnd1;
    uint32 mRChampRnd2;
    uint32 mMChampRnd1;
    uint32 mMChampRnd2;
	uint32 mPlayerTeam;
	uint64 mChampionsGUID[10];
	uint32 mChampionsDead;

    void Reset()
    {
		mChampionsDead = 0;
        mMaxChampions = RAID_MODE(6,10,6,10);
        mHChampRnd1 = urand(0,3);
        mHChampRnd2 = urand(1,3);
        mRChampRnd1 = urand(0,4);
        mRChampRnd2 = urand(1,4);
        mMChampRnd1 = urand(0,4);
        mMChampRnd2 = urand(1,4);
    }

	 void SummonChampions()
    {
        Map::PlayerList const &players = pInstance->instance->GetPlayers();
        uint32 TeamInInstance = 0;
		Difficulty diffic = getDifficulty();

        uint8 team = (mPlayerTeam==ALLIANCE) ? 1 : 0;
        uint8 coordsIdx = 0;
        uint8 guidIdx = 0;
        uint8 entryIdx = 0;
        uint8 r = RAID_MODE(2,4);
        uint8 hm = RAID_MODE(2,3);

        //Ranged
        entryIdx = mRChampRnd1;
        for(coordsIdx=0; coordsIdx < r; ++coordsIdx)
        {
            if(Unit *champ = me->SummonCreature(ChampRanged[team][entryIdx],
                ChampSpawnLocs[team][coordsIdx][0],
                ChampSpawnLocs[team][coordsIdx][1],
                ChampSpawnLocs[team][coordsIdx][2],
                ChampSpawnLocs[team][coordsIdx][3],
                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mChampionsGUID[guidIdx++] = champ->GetGUID();
            }
            entryIdx = (entryIdx + mRChampRnd2) % 5;
        }
        //Healers
        entryIdx = mHChampRnd1;
        for(coordsIdx=4; coordsIdx < hm+4; ++coordsIdx)
        {
            if(Unit *champ = me->SummonCreature(ChampHealers[team][entryIdx],
                ChampSpawnLocs[team][coordsIdx][0],
                ChampSpawnLocs[team][coordsIdx][1],
                ChampSpawnLocs[team][coordsIdx][2],
                ChampSpawnLocs[team][coordsIdx][3],
                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mChampionsGUID[guidIdx++] = champ->GetGUID();
            }
            entryIdx = (entryIdx + mHChampRnd2) % 4;
        }
        //Melee
        entryIdx = mMChampRnd1;
        for(coordsIdx=7; coordsIdx < hm+7; ++coordsIdx)
        {
            if(Unit *champ = me->SummonCreature(ChampMelee[team][entryIdx],
                ChampSpawnLocs[team][coordsIdx][0],
                ChampSpawnLocs[team][coordsIdx][1],
                ChampSpawnLocs[team][coordsIdx][2],
                ChampSpawnLocs[team][coordsIdx][3],
                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
            {
                mChampionsGUID[guidIdx++] = champ->GetGUID();
            }
            entryIdx = (entryIdx + mMChampRnd2) % 5;
        }
        
        mChampionsDead = 0;
    }

    void StartEvent()
    {
		pInstance = (ScriptedInstance*)me->GetInstanceData();

        if(pInstance && pInstance->GetData(PHASE_1) == NOT_STARTED
			&& pInstance->GetData(PHASE_2) == NOT_STARTED
			&& pInstance->GetData(PHASE_3) == NOT_STARTED
			&& pInstance->GetData(PHASE_4) == NOT_STARTED
			&& pInstance->GetData(PHASE_5) == NOT_STARTED
			&& pInstance->GetData(PHASE_6) == NOT_STARTED
			&& pInstance->GetData(PHASE_7) == NOT_STARTED)
        {
			pInstance->SetData(PHASE_1, IN_PROGRESS);
		}

        if(pInstance && pInstance->GetData(PHASE_1) == DONE
			&& pInstance->GetData(PHASE_2) == DONE
			&& pInstance->GetData(PHASE_3) == DONE
			&& pInstance->GetData(PHASE_4) == NOT_STARTED
			&& pInstance->GetData(PHASE_5) == NOT_STARTED
			&& pInstance->GetData(PHASE_6) == NOT_STARTED
			&& pInstance->GetData(PHASE_7) == NOT_STARTED)
		{
            pInstance->SetData(PHASE_4, IN_PROGRESS);
        }

        if(pInstance && pInstance->GetData(PHASE_1) == DONE
			&& pInstance->GetData(PHASE_2) == DONE
			&& pInstance->GetData(PHASE_3) == DONE
			&& pInstance->GetData(PHASE_4) == DONE
			&& pInstance->GetData(PHASE_5) == NOT_STARTED
			&& pInstance->GetData(PHASE_6) == NOT_STARTED
			&& pInstance->GetData(PHASE_7) == NOT_STARTED)
        {
			pInstance->SetData(PHASE_5, IN_PROGRESS);
			SummonChampions();
        }

        if(pInstance && pInstance->GetData(PHASE_1) == DONE
			&& pInstance->GetData(PHASE_2) == DONE
			&& pInstance->GetData(PHASE_3) == DONE
			&& pInstance->GetData(PHASE_4) == DONE
			&& pInstance->GetData(PHASE_5) == DONE
			&& pInstance->GetData(PHASE_6) == NOT_STARTED
			&& pInstance->GetData(PHASE_7) == NOT_STARTED)
        {
            Fjola = me->SummonCreature(NPC_FJOLA, 565.11, 260.0, 397.09+4, 4.72, TEMPSUMMON_MANUAL_DESPAWN, 0);
			Fjola->SetFlying(true);
			Fjola->GetMotionMaster()->MovePoint(0, 579.90f, 172.35f, 394.68f);

            Eydis = me->SummonCreature(NPC_EYDIS, 563.11, 260.0, 397.09+4, 4.72, TEMPSUMMON_MANUAL_DESPAWN, 0);
			Eydis->SetFlying(true);
			Eydis->GetMotionMaster()->MovePoint(0, 548.57f, 172.59f, 394.68f);
            pInstance->SetData(PHASE_6, IN_PROGRESS);
        }

        if(pInstance && pInstance->GetData(PHASE_1) == DONE
			&& pInstance->GetData(PHASE_2) == DONE
			&& pInstance->GetData(PHASE_3) == DONE
			&& pInstance->GetData(PHASE_4) == DONE
			&& pInstance->GetData(PHASE_5) == DONE
			&& pInstance->GetData(PHASE_6) == DONE
			&& pInstance->GetData(PHASE_7) == NOT_STARTED)
        {
			pInstance->SetData(PHASE_7, IN_PROGRESS);
			pInstance->HandleGameObject(pInstance->GetData64(20), true);
        }
    } 

    void UpdateAI(const uint32 diff)
    {
    }
};

CreatureAI* GetAI_npc_tcrus_announcer(Creature* pCreature)
{
    return new npc_tcrus_announcerAI(pCreature);
}

bool GossipHello_npc_tcrus_announcer(Player* pPlayer, Creature* pCreature)
{
    InstanceData* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

	bool inProgress = false;
    for(uint8 i = 0; i < 7; ++i)
    {
		if (pInstance && pInstance->GetData(i) == IN_PROGRESS)
			inProgress = true;
    }

    if(pInstance && pInstance->GetData(PHASE_1) == NOT_STARTED
		&& pInstance->GetData(PHASE_2) == NOT_STARTED
		&& pInstance->GetData(PHASE_3) == NOT_STARTED
		&& pInstance->GetData(PHASE_4) == NOT_STARTED
		&& pInstance->GetData(PHASE_5) == NOT_STARTED
		&& pInstance->GetData(PHASE_6) == NOT_STARTED
		&& pInstance->GetData(PHASE_7) == NOT_STARTED && !inProgress)
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    else if(!inProgress)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pCreature->GetEntry(), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_tcrus_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	InstanceData* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
	if(!pInstance)
		return false;

    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
		pInstance->SetData(PHASE_SPECHIAL, 1);
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
		CAST_AI(npc_tcrus_announcerAI, pCreature->AI())->StartEvent();
    }

    return true;
}

struct npc_tcrus_tirionAI : public ScriptedAI
{
    npc_tcrus_tirionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    InstanceData* pInstance;
	uint32 UpdateTimer;

	Creature* Fizzle;
	Creature* Jaraxxus;
	Creature* Gormok;
	Creature* Lichking;
	Creature* Dreadscale;
	Creature* Acidmaw;
	Creature* IceHowl;

	uint64 JaraxxusGUID;
	uint64 GormokGUID;
	uint64 LichkingGUID;
	uint64 DreadscaleGUID;
	uint64 AcidmawGUID;
	uint64 IceHowlGUID;

    void Reset()
    {
		pInstance->SetData(SPECHIAL_TIMER, 2000);
    }

    void UpdateAI(const uint32 diff)
    {
		if(!pInstance)
			return;

		ScriptedAI::UpdateAI(diff);

		UpdateTimer = pInstance->GetData(SPECHIAL_TIMER);

        if(UpdateTimer <= diff)
        {
			switch(pInstance->GetData(PHASE_SPECHIAL))
			{
				case 1:
					if(pInstance->GetData(PHASE_1)==0)
					{
						DoScriptText(SAY_INTRO_START, me);
						pInstance->SetData(PHASE_SPECHIAL, 2);
						pInstance->SetData(SPECHIAL_TIMER, 70000);
					}
				break;

				case 2:
					if(pInstance->GetData(PHASE_1)==0)
					{
						DoScriptText(SAY_INTRO_BEGINN, me);
						pInstance->SetData(PHASE_SPECHIAL, 3);
						pInstance->SetData(SPECHIAL_TIMER, 15000);
					}
				break;

				case 3:
					if(pInstance->GetData(PHASE_1)==0)
					{
						DoScriptText(SAY_INTRO_3, me);
						pInstance->SetData(PHASE_SPECHIAL, 4);
						pInstance->SetData(SPECHIAL_TIMER, 15000);
					}
				break;

				case 4:	
					if(pInstance->GetData(PHASE_1)==0)
					{
						Gormok = me->SummonCreature(NPC_GORMOK_IMPALER, 563.14f, 215.38f, 395.08f, 4.68f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
						Gormok->GetMotionMaster()->MovePoint(0, 563.74f, 173.53f, 394.32f);
						Gormok->SetReactState(REACT_PASSIVE);
						Gormok->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						pInstance->SetData(PHASE_SPECHIAL, 5);
						pInstance->SetData(SPECHIAL_TIMER, 15000);
					}
				break;

				case 6:	
					if(pInstance->GetData(PHASE_1)==0)
					{
						
						pInstance->SetData(PHASE_SPECHIAL, 7);
						pInstance->SetData(SPECHIAL_TIMER, 5000);
					}
				break;

				case 7:	
					if(pInstance->GetData(PHASE_1)==0)
					{	
						Creature* cre = Unit::GetCreature(*me, pInstance->GetData64(NPC_ANONSER));
						CAST_AI(npc_tcrus_announcerAI, cre->AI())->StartEvent();
						Gormok->SetReactState(REACT_AGGRESSIVE);
						Gormok->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						pInstance->SetData(PHASE_SPECHIAL, 8);
						pInstance->SetData(SPECHIAL_TIMER, 6000);
					}
				break;

				case 8:	
					if(pInstance->GetData(PHASE_1)==IN_PROGRESS)
					{
						pInstance->SetData(PHASE_SPECHIAL, 9);
						pInstance->SetData(SPECHIAL_TIMER, 500);
					}
					break;

				case 9:	
					if(pInstance->GetData(PHASE_2)==IN_PROGRESS)
					{
						DoScriptText(SAY_INTRO_DREADSCALE, me);
						Dreadscale = me->SummonCreature(NPC_DREADSCALE,562.354675, 215.781113, 395.087036, 4.725680,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
						Dreadscale->SetReactState(REACT_PASSIVE);
						Dreadscale->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						Dreadscale->GetMotionMaster()->MovePoint(0, 563.74f, 173.53f, 394.32f);
						Acidmaw = me->SummonCreature(NPC_ACIDMAW,546.526184, 157.534195, 394.320557, 4.725680,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000);
						Acidmaw->SetVisibility(VISIBILITY_OFF);
						Acidmaw->SetReactState(REACT_PASSIVE);
						pInstance->SetData(PHASE_SPECHIAL, 10);
						pInstance->SetData(SPECHIAL_TIMER, 25000);
					}
				break;

				case 10:
					if(pInstance->GetData(PHASE_2)==IN_PROGRESS)
					{
						Acidmaw->SetVisibility(VISIBILITY_ON);
						Acidmaw->CastSpell(Acidmaw, 35177, false);
						Acidmaw->SetReactState(REACT_AGGRESSIVE);
						Dreadscale->SetReactState(REACT_AGGRESSIVE);
						Dreadscale->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						pInstance->SetData(PHASE_SPECHIAL, 11);
						pInstance->SetData(SPECHIAL_TIMER, 5000);
					}
				break;

				case 11:
					if(pInstance->GetData(PHASE_3)==IN_PROGRESS)
					{
						IceHowl = me->SummonCreature(NPC_ICEHOWL, 563.14f, 215.38f, 395.08f, 4.68f, TEMPSUMMON_DEAD_DESPAWN, 600000);
						IceHowl->GetMotionMaster()->MovePoint(0, 563.74f, 173.53f, 394.32f);
						IceHowl->SetReactState(REACT_PASSIVE);
						IceHowl->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						DoScriptText(SAY_INTRO_EISHEULER, me);
						pInstance->SetData(PHASE_SPECHIAL, 12);
						pInstance->SetData(SPECHIAL_TIMER, 20000);
					}
				break;

				case 12:
					if(pInstance->GetData(PHASE_3)==IN_PROGRESS)
					{
						IceHowl->SetReactState(REACT_AGGRESSIVE);
						IceHowl->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
						pInstance->SetData(PHASE_SPECHIAL, 13);
						pInstance->SetData(SPECHIAL_TIMER, 500);
					}
				break;

				case 13:
					if(pInstance->GetData(PHASE_3)==DONE)
					{
						DoScriptText(-1999930, me);
						pInstance->SetData(PHASE_SPECHIAL, 14);
						pInstance->SetData(SPECHIAL_TIMER, 500);
					}
				break;

				case 15:
					DoScriptText(SAY_JARAXXUS_INTRO_1, me);
					pInstance->SetData(PHASE_SPECHIAL, 16);
					Fizzle = me->SummonCreature(NPC_FIZZLEBANG, 564.008057, 176.066330, 394.372772, 4.719572, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
					pInstance->SetData(SPECHIAL_TIMER, 500);
				break;

				case 29:
					DoScriptText(SAY_JARAXXUS_INTRO_7, me);
					pInstance->SetData(PHASE_SPECHIAL, 30);
					pInstance->SetData(SPECHIAL_TIMER, 10000);
				break;

				case 30:
					if (Creature* pJaraxxus = GetClosestCreatureWithEntry(me, NPC_LORD_JARAXXUS, 100.0f))
					Jaraxxus = pJaraxxus;
					Jaraxxus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
					Jaraxxus->SetReactState(REACT_AGGRESSIVE);
					pInstance->SetData(PHASE_SPECHIAL, 31);
					pInstance->SetData(SPECHIAL_TIMER, 5000);
				break;

				case 31:
					if(pInstance->GetData(PHASE_4)==IN_PROGRESS)
					{
						pInstance->SetData(PHASE_SPECHIAL, 32);
						pInstance->SetData(SPECHIAL_TIMER, 5000);
					}
				break;
					
				case 32:
					if(pInstance->GetData(PHASE_4)==DONE)
					{
						DoScriptText(SAY_JARAXXUS_OUTRO_1, me);
						pInstance->SetData(PHASE_SPECHIAL, 33);
						pInstance->SetData(SPECHIAL_TIMER, 45500);
					}
				break;

				case 35:
					if(pInstance->GetData(PHASE_4)==DONE)
					{
						DoScriptText(SAY_JARAXXUS_OUTRO_4, me);
						pInstance->SetData(PHASE_SPECHIAL, 36);
						pInstance->SetData(SPECHIAL_TIMER, 15500);
					}
				break;

				case 37:
						DoScriptText(SAY_INTRO_LICHTKING_1, me);
						pInstance->SetData(PHASE_SPECHIAL, 38);
						pInstance->SetData(SPECHIAL_TIMER, 36000);
				break;

				case 38:
						Lichking = me->SummonCreature(NPC_LICH_KING, 564.008057, 176.066330, 394.372772, 4.719572);
						Lichking->GetMotionMaster()->MovePoint(0, 563.955444f, 140.563583f, 393.836548f);
						Lichking->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
						pInstance->SetData(PHASE_SPECHIAL, 39);
						pInstance->SetData(SPECHIAL_TIMER, 500);
				break;

				case 40:
						DoScriptText(SAY_INTRO_LICHTKING_3, me);
						pInstance->SetData(PHASE_SPECHIAL, 41);
						pInstance->SetData(SPECHIAL_TIMER, 10500);
				break;

				case 50:
				if(pInstance->GetData(PHASE_5)==IN_PROGRESS)
				{
					DoScriptText(SAY_INTRO_FACTION_HORDE_1, me);
					pInstance->SetData(PHASE_SPECHIAL, 51);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				}
				break;
				
				case 52:
					DoScriptText(SAY_INTRO_FACTION_HORDE_3, me);
					pInstance->SetData(PHASE_SPECHIAL, 53);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				break;

				case 54:
				if(pInstance->GetData(PHASE_5)==DONE)
				{
					DoScriptText(SAY_OUTRO_FACTION, me);
					pInstance->SetData(PHASE_SPECHIAL, 55);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				}
				break;

				case 57:
					DoScriptText(SAY_INTRO_TWINS, me);
					pInstance->SetData(PHASE_SPECHIAL, 58);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				break;

				case 58:
					DoScriptText(SAY_INTRO_BEGINN, me);
					pInstance->SetData(PHASE_SPECHIAL, 59);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				break;
			}
        }
		else
		{
			UpdateTimer -= diff;
			pInstance->SetData(SPECHIAL_TIMER, UpdateTimer);
		}
    }
};

CreatureAI* GetAI_npc_tcrus_tirion(Creature* pCreature)
{
    return new npc_tcrus_tirionAI(pCreature);
}

struct npc_GarroshAI : public ScriptedAI
{
    npc_GarroshAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    InstanceData* pInstance;
	uint32 UpdateTimer;

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
		if(!pInstance)
			return;

		ScriptedAI::UpdateAI(diff);

		UpdateTimer = pInstance->GetData(SPECHIAL_TIMER);

        if(UpdateTimer <= diff)
        {
			switch(pInstance->GetData(PHASE_SPECHIAL))
			{
				case 5:
					DoScriptText(SAY_INTRO_GOMROK_START_1, me);
					pInstance->SetData(PHASE_SPECHIAL, 6);
					pInstance->SetData(SPECHIAL_TIMER, 5000);
					break;

				case 33:
					if(pInstance->GetData(PHASE_4)==DONE)
					{
						DoScriptText(SAY_JARAXXUS_OUTRO_2, me);
						pInstance->SetData(PHASE_SPECHIAL, 34);
						pInstance->SetData(SPECHIAL_TIMER, 30500);
					}
				break;
			}
        }
		else
		{
			UpdateTimer -= diff;
			pInstance->SetData(SPECHIAL_TIMER, UpdateTimer);
		}
    }
};

CreatureAI* GetAI_npc_Garrosh(Creature* pCreature)
{
    return new npc_GarroshAI(pCreature);
}

struct npc_tcrus_fizzlebangAI : public ScriptedAI
{
    npc_tcrus_fizzlebangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    InstanceData* pInstance;
	uint32 UpdateTimer;

	Creature* Jaraxxus;
	Creature* WilfredPortal;

	uint64 JaraxxusGUID;
	uint64 WilfredPortalGUID;

    void Reset()
    {
		me->setFaction(14); // Jaraxxus Faction verhintert den Reset und Aggro Bug ;)
		me->SetReactState(REACT_PASSIVE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
		me->SetSpeed(MOVE_WALK, 1.0f);
    }

    void UpdateAI(const uint32 diff)
    {
		if(!pInstance)
			return;

		ScriptedAI::UpdateAI(diff);

		UpdateTimer = pInstance->GetData(SPECHIAL_TIMER);

        if(UpdateTimer <= diff)
        {
			switch(pInstance->GetData(PHASE_SPECHIAL))
			{
				case 16:
					pInstance->SetData(PHASE_SPECHIAL, 17);
					pInstance->SetData(SPECHIAL_TIMER, 11000);
				break;

				case 17:
					me->GetMotionMaster()->MovePoint(0, 563.296753,147.057114,394.149200);
					pInstance->SetData(PHASE_SPECHIAL, 18);
					pInstance->SetData(SPECHIAL_TIMER, 30000);
				break;

				case 18:
					me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 1);
					DoScriptText(SAY_JARAXXUS_INTRO_2, me);
					pInstance->SetData(PHASE_SPECHIAL, 19);
					pInstance->SetData(SPECHIAL_TIMER, 45000);
				break;

				case 19:
					me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 27);
					DoScriptText(SAY_JARAXXUS_INTRO_3, me);
					pInstance->SetData(PHASE_SPECHIAL, 20);
					pInstance->SetData(SPECHIAL_TIMER, 16000);
				break;

				case 20:
					me->SummonGameObject(184006, 563.296753,147.057114,394.149200,4.709139, 0, 0, 0, 0, 150);
					pInstance->SetData(PHASE_SPECHIAL, 21);
					pInstance->SetData(SPECHIAL_TIMER, 16000);
				break;

				case 21:
					//WilfredPortal = me->SummonCreature(CREATURE_JARAXXUS_PORTAL_TRIGGER, 563.830933, 127.890533, 393.918182,1.579980,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,180000);
					pInstance->SetData(PHASE_SPECHIAL, 22);
					pInstance->SetData(SPECHIAL_TIMER, 16000);
				break;

				case 22:
					//me->CastSpell(WilfredPortal, SPELL_WILFRED_CAST_PORTAL, true);
					pInstance->SetData(PHASE_SPECHIAL, 23);
					pInstance->SetData(SPECHIAL_TIMER, 16000);
				break;

				case 23:
					//me->CastSpell(WilfredPortal, SPELL_WILFRED_CAST_PORTAL, true);
					pInstance->SetData(PHASE_SPECHIAL, 24);
					pInstance->SetData(SPECHIAL_TIMER, 14000);
				break;

				case 24:
					//WilfredPortal->CastSpell(WilfredPortal, SPELL_WILFRED_PORTAL, false);
					pInstance->SetData(PHASE_SPECHIAL, 25);
					pInstance->SetData(SPECHIAL_TIMER, 9000);
				break;

				case 25:
					Jaraxxus = me->SummonCreature(NPC_LORD_JARAXXUS,563.830933, 127.890533, 393.918182,1.579980,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,180000);
					Jaraxxus->RemoveAurasDueToSpell(SPELL_ROOTET_JARAXXUS);
					Jaraxxus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
					Jaraxxus->GetMotionMaster()->MovePoint(0, 563.780945, 132.052063, 393.918182);
					Jaraxxus->SetReactState(REACT_PASSIVE);
					pInstance->SetData(PHASE_SPECHIAL, 26);
					pInstance->SetData(SPECHIAL_TIMER, 35000);
				break;

				case 26:
					//WilfredPortal->ForcedDespawn();
					me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 1);
					DoScriptText(SAY_JARAXXUS_INTRO_4, me);
					pInstance->SetData(PHASE_SPECHIAL, 27);
					pInstance->SetData(SPECHIAL_TIMER, 45000);
				break;

				case 27:
					DoScriptText(SAY_JARAXXUS_INTRO_5, Jaraxxus);   
					me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
					pInstance->SetData(PHASE_SPECHIAL, 28);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				break;
			
				case 28:
					Jaraxxus->CastSpell(me, 67888, true);
					Jaraxxus->Kill(me);
					DoScriptText(SAY_JARAXXUS_INTRO_6, me);
					pInstance->SetData(PHASE_SPECHIAL, 29);
					pInstance->SetData(SPECHIAL_TIMER, 25000);
				break;
			}
        }
		else
		{
			UpdateTimer -= diff;
			pInstance->SetData(SPECHIAL_TIMER, UpdateTimer);
		}
    }
};

CreatureAI* GetAI_npc_tcrus_fizzlebang(Creature* pCreature)
{
    return new npc_tcrus_fizzlebangAI(pCreature);
}

struct npc_KingVyrnAI : public ScriptedAI
{
    npc_KingVyrnAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    InstanceData* pInstance;
	uint32 UpdateTimer;


    void Reset()
    {
	
    }

    void UpdateAI(const uint32 diff)
    {
		if(!pInstance)
			return;

		ScriptedAI::UpdateAI(diff);

		UpdateTimer = pInstance->GetData(SPECHIAL_TIMER);

        if(UpdateTimer <= diff)
        {
			switch(pInstance->GetData(PHASE_SPECHIAL))
			{
				case 34:
					if(pInstance->GetData(PHASE_4)==DONE)
					{
						DoScriptText(SAY_JARAXXUS_OUTRO_3, me);
						pInstance->SetData(PHASE_SPECHIAL, 35);
						pInstance->SetData(SPECHIAL_TIMER, 27500);
					}
				break;

				case 51:
					if(pInstance->GetData(PHASE_5)==IN_PROGRESS)
					{
						DoScriptText(SAY_INTRO_FACTION_HORDE_2, me);
						pInstance->SetData(PHASE_SPECHIAL, 52);
						pInstance->SetData(SPECHIAL_TIMER, 25000);
					}
				break;

				case 53:
						DoScriptText(SAY_INTRO_FACTION_HORDE_4, me);
						pInstance->SetData(PHASE_SPECHIAL, 54);
						pInstance->SetData(SPECHIAL_TIMER, 25000);
				break;
			}
        }
		else
		{
			UpdateTimer -= diff;
			pInstance->SetData(SPECHIAL_TIMER, UpdateTimer);
		}
    }
};

CreatureAI* GetAI_npc_KingVyrn(Creature* pCreature)
{
    return new npc_KingVyrnAI(pCreature);
}

struct npc_LichKingAI : public ScriptedAI
{
    npc_LichKingAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
    }

    InstanceData* pInstance;
	uint32 UpdateTimer;

	Creature* Jaraxxus;

	uint64 JaraxxusGUID;

    void Reset()
    {
		me->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
		me->SetSpeed(MOVE_WALK, 1.4f);
    }

    void UpdateAI(const uint32 diff)
    {
		if(!pInstance)
			return;

		ScriptedAI::UpdateAI(diff);

		UpdateTimer = pInstance->GetData(SPECHIAL_TIMER);

        if(UpdateTimer <= diff)
        {
			switch(pInstance->GetData(PHASE_SPECHIAL))
			{
				case 39:
						DoScriptText(SAY_INTRO_LICHTKING_2, me);
						pInstance->SetData(PHASE_SPECHIAL, 40);
						pInstance->SetData(SPECHIAL_TIMER, 24500);
				break;

				case 41:
						me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
						pInstance->SetData(PHASE_SPECHIAL, 42);
						pInstance->SetData(SPECHIAL_TIMER, 31500);
				break;

				case 42:
						me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
						DoScriptText(SAY_INTRO_LICHTKING_4, me);
						pInstance->SetData(PHASE_SPECHIAL, 43);
						pInstance->SetData(SPECHIAL_TIMER, 70000);
				break;

				case 43:
						me->CastSpell(me,72262,false);
						pInstance->SetData(PHASE_SPECHIAL, 44);
						pInstance->SetData(SPECHIAL_TIMER, 12000);
				break;

				case 44:
						pInstance->SetData(PHASE_SPECHIAL, 45);
						pInstance->SetData(SPECHIAL_TIMER, 18000);
				break;

				case 45:
						pInstance->HandleGameObject(pInstance->GetData64(20), true);
						pInstance->SetData(PHASE_SPECHIAL, 46);
						pInstance->SetData(SPECHIAL_TIMER, 2500);
				break;
					
				case 46:
						DoScriptText(SAY_INTRO_LICHTKING_5, me);
						me->ForcedDespawn();
						pInstance->SetData(PHASE_SPECHIAL, 47);
						pInstance->SetData(SPECHIAL_TIMER, 15500);
				break;
			}
        }
		else
		{
			UpdateTimer -= diff;
			pInstance->SetData(SPECHIAL_TIMER, UpdateTimer);
		}
    }
};

CreatureAI* GetAI_npc_LichKing(Creature* pCreature)
{
    return new npc_LichKingAI(pCreature);
}

void AddSC_trial_of_the_crussader()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_tcrus_announcer";
	NewScript->GetAI = &GetAI_npc_tcrus_announcer;
    NewScript->pGossipHello = &GossipHello_npc_tcrus_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_tcrus_announcer;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_tcrus_tirion";
	NewScript->GetAI = &GetAI_npc_tcrus_tirion;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_tcrus_fizzlebang";
	NewScript->GetAI = &GetAI_npc_tcrus_fizzlebang;
    NewScript->RegisterSelf();

	NewScript = new Script;
    NewScript->Name = "npc_Garrosh";
	NewScript->GetAI = &GetAI_npc_Garrosh;
    NewScript->RegisterSelf();
	
	NewScript = new Script;
    NewScript->Name = "npc_LichKing";
	NewScript->GetAI = &GetAI_npc_LichKing;
    NewScript->RegisterSelf();
	
	NewScript = new Script;
    NewScript->Name = "npc_KingVyrn";
	NewScript->GetAI = &GetAI_npc_KingVyrn;
    NewScript->RegisterSelf();
}