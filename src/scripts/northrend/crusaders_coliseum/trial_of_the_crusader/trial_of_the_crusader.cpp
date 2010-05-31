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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Trial Of the crusader
SD%Complete: 60%
SDComment: event script by /dev/rsa
SDCategory: trial_of_the_crusader
EndScriptData */

#include "ScriptedPch.h"
#include "trial_of_the_crusader.h"

struct _Messages
{
    AnnounserMessages msgnum;
    uint32 id;
    bool state;
    uint32 encounter;
};

static _Messages _GossipMessage[]=
{
    {MSG_BEASTS,GOSSIP_ACTION_INFO_DEF+1,false,TYPE_BEASTS},
    {MSG_JARAXXUS,GOSSIP_ACTION_INFO_DEF+2,false,TYPE_JARAXXUS},
    {MSG_CRUSADERS,GOSSIP_ACTION_INFO_DEF+3,false,TYPE_CRUSADERS},
    {MSG_VALKIRIES,GOSSIP_ACTION_INFO_DEF+4,false,TYPE_VALKIRIES},
    {MSG_LICH_KING,GOSSIP_ACTION_INFO_DEF+5,false,TYPE_ANUBARAK},
    {MSG_ANUBARAK,GOSSIP_ACTION_INFO_DEF+6,true,TYPE_ANUBARAK}
};

enum
{
         NUM_MESSAGES = 6,
};


struct npc_toc_announcerAI : public ScriptedAI
{
    npc_toc_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    uint32 DelayTimer;
    uint32 substage;

    void Reset()
    {
    if (!pInstance) return;
    pInstance->SetData(TYPE_STAGE,0);
    DelayTimer = 0;
    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    if(Creature *pAlly = GetClosestCreatureWithEntry(me, NPC_THRALL, 300.0f))
                      pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    if(Creature *pAlly = GetClosestCreatureWithEntry(me, NPC_PROUDMOORE, 300.0f))
                      pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    me->SetRespawnDelay(DAY);
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance) return;

        if (pInstance->IsEncounterInProgress() && me->GetVisibility() == VISIBILITY_ON)
            me->SetVisibility(VISIBILITY_OFF);
        else if (!pInstance->IsEncounterInProgress() && me->GetVisibility() == VISIBILITY_OFF)
            me->SetVisibility(VISIBILITY_ON);

    if(DelayTimer < diff) {
        switch (pInstance->GetData(TYPE_STAGE)) {
        case 0: break;
        case 1: {
            if (pInstance->GetData(TYPE_NORTHREND_BEASTS) == GORMOK_DONE) {
                         pInstance->SetData(TYPE_STAGE,2);
                         pInstance->SetData(TYPE_EVENT,200);
                         pInstance->SetData(TYPE_NORTHREND_BEASTS,IN_PROGRESS);
                         pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                         };
            if (pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL) {
                         pInstance->SetData(TYPE_STAGE,0);
                         pInstance->SetData(TYPE_EVENT,666);
                         pInstance->SetData(TYPE_BEASTS,NOT_STARTED);
                         };
                 break;
                 };
        case 2: {
            if (pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_DONE) {
                         pInstance->SetData(TYPE_STAGE,3);
                         pInstance->SetData(TYPE_EVENT,300);
                         pInstance->SetData(TYPE_NORTHREND_BEASTS,IN_PROGRESS);
                         pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                 };
            if (pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL) {
                         pInstance->SetData(TYPE_STAGE,0);
                         pInstance->SetData(TYPE_EVENT,666);
                         pInstance->SetData(TYPE_BEASTS,NOT_STARTED);
                         };
                 break;
                 }
        case 3: {
            if (pInstance->GetData(TYPE_NORTHREND_BEASTS) == ICEHOWL_DONE) {
                         pInstance->SetData(TYPE_STAGE,0);
                         pInstance->SetData(TYPE_BEASTS,DONE);
                         pInstance->SetData(TYPE_EVENT,400);
                         pInstance->SetData(TYPE_NORTHREND_BEASTS,DONE);
                        }
            if (pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL) {
                         pInstance->SetData(TYPE_STAGE,0);
                         pInstance->SetData(TYPE_EVENT,666);
                         pInstance->SetData(TYPE_BEASTS,NOT_STARTED);
                         };
                 break;
                 };

        case 4: {
                 break;
                 };

        case 5: {
                break;
                };

        case 6: {
                 if (pInstance->GetData(TYPE_CRUSADERS_COUNT) == 0
                     && pInstance->GetData(TYPE_CRUSADERS) == IN_PROGRESS)
                     {
                        pInstance->SetData(TYPE_STAGE,0);
                        pInstance->SetData(TYPE_CRUSADERS,DONE);
                        pInstance->SetData(TYPE_EVENT,3100);
                     }
                 if (pInstance->GetData(TYPE_CRUSADERS) == NOT_STARTED)
                     {
                        pInstance->SetData(TYPE_STAGE,0);
                        pInstance->SetData(TYPE_EVENT,0);
                     }
                break;
                };

        case 7: {
                 if (pInstance->GetData(TYPE_VALKIRIES) == DONE) {
                                 pInstance->SetData(TYPE_STAGE,0);
                                 pInstance->SetData(TYPE_EVENT,4020);
                                 }
                 if (pInstance->GetData(TYPE_VALKIRIES) == FAIL) {
                                 pInstance->SetData(TYPE_STAGE,0);
                                 pInstance->SetData(TYPE_EVENT,0);
                                 }
                break;
                };
        case 8: {
                 break;
                 };
        case 9: {
             if (pInstance->GetData(TYPE_ANUBARAK) == DONE) {
                                 pInstance->SetData(TYPE_STAGE,10);
                                 pInstance->SetData(TYPE_EVENT,6000);
                                 }
             if (pInstance->GetData(TYPE_ANUBARAK) == FAIL) {
                                 pInstance->SetData(TYPE_STAGE,0);
                                 pInstance->SetData(TYPE_EVENT,0);
                                 }
                 break;
                 };
        case 10: {
//                 me->ForcedDespawn();
                 break;
                 };

          }
        } else DelayTimer -= diff;
    }
};

CreatureAI* GetAI_npc_toc_announcer(Creature* pCreature)
{
    return new npc_toc_announcerAI(pCreature);
}

bool GossipHello_npc_toc_announcer(Player* pPlayer, Creature* pCreature)
{
 
    ScriptedInstance* pInstance;
    pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    
    char const* _message = "We are ready!";
    uint8 i;

    if (!pInstance) return false;

    if( !pPlayer->getAttackers().empty() ||
        //pInstance->IsEncounterInProgress() || Hack to fix feezes of announcer
        pInstance->GetData(TYPE_EVENT)) 
           return true;

    for(i = 0; i < NUM_MESSAGES; i++) {
    if (!_GossipMessage[i].state && (pInstance->GetData(_GossipMessage[i].encounter) != DONE )) {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,_GossipMessage[i].id);
        break;
        }
    if (_GossipMessage[i].state && pInstance->GetData(_GossipMessage[i].encounter) == DONE) {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,_GossipMessage[i].id);
        break;
        }
    };

    pPlayer->SEND_GOSSIP_MENU(_GossipMessage[i].msgnum, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_toc_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* pInstance;
    pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!pInstance) return false;

    pPlayer->CLOSE_GOSSIP_MENU();

    switch(uiAction) {
        case GOSSIP_ACTION_INFO_DEF+1:
            if (pInstance->GetData(TYPE_BEASTS) != DONE)
            {
                pInstance->SetData(TYPE_EVENT,110);
                pInstance->SetData(TYPE_NORTHREND_BEASTS,NOT_STARTED);
            }
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
            if (pInstance->GetData(TYPE_JARAXXUS) != DONE) 
                pInstance->SetData(TYPE_EVENT,1010);
            break;

        case GOSSIP_ACTION_INFO_DEF+3:
            if (pInstance->GetData(TYPE_CRUSADERS) != DONE)
            {
                if (pPlayer->GetTeam() == ALLIANCE) pInstance->SetData(TYPE_EVENT,3000);
                else pInstance->SetData(TYPE_EVENT,3001);
            }
            break;

        case GOSSIP_ACTION_INFO_DEF+4:
            if (pInstance->GetData(TYPE_VALKIRIES) != DONE)
                pInstance->SetData(TYPE_EVENT,4000);
            break;

        case GOSSIP_ACTION_INFO_DEF+5:
        {
           if (pInstance->GetData(TYPE_LICH_KING) != DONE)
               return false;

           if (GameObject* pGoFloor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
                  pGoFloor->DestroyForNearbyPlayers();

            pCreature->CastSpell(pCreature,69016,false);

            Creature* pTemp = Unit::GetCreature((*pCreature),pInstance->GetData64(NPC_ANUBARAK));
            if (!pTemp || !pTemp->isAlive())
                pCreature->SummonCreature(NPC_ANUBARAK, SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[20].x, SpawnLoc[20].y, SpawnLoc[20].z);
                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                pTemp->SetInCombatWithZone();
            }

            pInstance->SetData(TYPE_STAGE,9);
            pInstance->SetData(TYPE_ANUBARAK,IN_PROGRESS);

            if (pCreature->GetVisibility() == VISIBILITY_ON)
                pCreature->SetVisibility(VISIBILITY_OFF);
            break;
         }
        case GOSSIP_ACTION_INFO_DEF+6:
            pInstance->SetData(TYPE_STAGE,10);
            break;

  };

return true;
}

struct boss_lich_king_tocAI : public ScriptedAI
{
    boss_lich_king_tocAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        WayPointList.clear();
        JustRespawned();
    }

    ScriptedInstance* pInstance;
    uint32 UpdateTimer;
    uint32 event_state_lich_king;
    bool Event;
    bool MovementStarted;
    std::list<WayPoints> WayPointList;
    std::list<WayPoints>::iterator WayPoint;
    uint32 WalkTimer;
    bool IsWalking;
    Creature* pPortal;

    void Reset()
    {
        UpdateTimer = 0;
        event_state_lich_king = 0;
        Event = false;
        MovementStarted = false;
        me->SetRespawnDelay(DAY);
        pPortal = me->SummonCreature(NPC_TRIGGER, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
        pPortal->SetRespawnDelay(DAY);
        pPortal->CastSpell(pPortal, 51807, false);
        pPortal->SetDisplayId(17612);
        if(pInstance) pInstance->SetData(TYPE_LICH_KING,IN_PROGRESS);
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void JustRespawned()
    {
        Reset();
    }

    void MoveInLineOfSight(Unit *who)
    {
    }

    void StartMovement()
    {
        if(!WayPointList.empty() || MovementStarted)
            return;

        AddWaypoint(0, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z);
        AddWaypoint(1, SpawnLoc[17].x, SpawnLoc[17].y, SpawnLoc[17].z);
        AddWaypoint(2, SpawnLoc[18].x, SpawnLoc[18].y, SpawnLoc[18].z);
        me->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
        WayPoint = WayPointList.begin();
        MovementStarted = true;
        IsWalking = true;
        WalkTimer = 200;
        event_state_lich_king = 1;
        UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
    }

    void AddWaypoint(uint32 id, float x, float y, float z)
    {
        WayPoints wp(id, x, y, z);
        WayPointList.push_back(wp);
    }

    void UpdateAI(const uint32 diff)
    {

    if(!pInstance) return;
    if (pInstance->GetData(TYPE_EVENT_NPC) != NPC_LICH_KING_1) return;
    if (!MovementStarted) StartMovement();

        if (IsWalking && WalkTimer)
        {
            if (WalkTimer <= diff)
            {
                if (WayPoint != WayPointList.end())
                {
                    me->GetMotionMaster()->MovePoint(WayPoint->id, WayPoint->x, WayPoint->y,WayPoint->z);
                    WalkTimer = 0;
                }
            }else WalkTimer -= diff;
        }

        UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
        switch (pInstance->GetData(TYPE_EVENT))
        {
        case 5010:
               DoScriptText(-1713550,me);
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,5020);
               break;
        case 5030:
               DoScriptText(-1713552,me);
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
               UpdateTimer =  10000;
               pInstance->SetData(TYPE_EVENT,5040);
               break;
        case 5040:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
               UpdateTimer =  1000;
               pInstance->SetData(TYPE_EVENT,5050);
               break;
        case 5050:
               me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
               UpdateTimer =  3000;
               pInstance->SetData(TYPE_EVENT,5060);
               break;
        case 5060: if (Event) {
               DoScriptText(-1713553,me);
               me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
               UpdateTimer =  2500;
               pInstance->SetData(TYPE_EVENT,5070);
               }
               break;
        case 5070:
               me->CastSpell(me,68198,false);
               UpdateTimer = 1500;
               pInstance->SetData(TYPE_EVENT,5080);
               break;
        case 5080:
               if (GameObject* pGoFloor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
                  {
                      pGoFloor->DestroyForNearbyPlayers();
                  }
                me->CastSpell(me,69016,false);
                if(pInstance) pInstance->SetData(TYPE_LICH_KING,DONE);

               pInstance->SetData(TYPE_ANUBARAK,IN_PROGRESS);
                  me->SummonCreature(NPC_ANUBARAK, SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                  if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_ANUBARAK))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[20].x, SpawnLoc[20].y, SpawnLoc[20].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
               pInstance->SetData(TYPE_STAGE,9);
               Event=false;
               me->ForcedDespawn();
               pPortal->ForcedDespawn();
               pInstance->SetData(TYPE_EVENT,0);
               UpdateTimer = 20000;
               break;
        }
        } else UpdateTimer -= diff;
       pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);

    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(pInstance)
        {
            if(id == 2)
            {
                Event = true;
            }
        }
        if(type != POINT_MOTION_TYPE) return;
        if(WayPoint->id != id) return;
        ++WayPoint;
        WalkTimer = 200;
    }
};

CreatureAI* GetAI_boss_lich_king_toc(Creature* pCreature)
{
    return new boss_lich_king_tocAI(pCreature);
}

struct npc_fizzlebang_tocAI : public ScriptedAI
{
    npc_fizzlebang_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
        Reset();
    }

    InstanceData* pInstance;
    uint32 UpdateTimer;
    Creature* pPortal;

    void JustDied(Unit* pKiller)
    {
        DoScriptText(-1713715, me, pKiller);
        pInstance->SetData(TYPE_EVENT, 1180);
        if (pPortal) pPortal->ForcedDespawn();
    }

    void Reset()
    {
        me->SetRespawnDelay(DAY);
        me->GetMotionMaster()->MovePoint(1, SpawnLoc[27].x, SpawnLoc[27].y, SpawnLoc[27].z);
        pPortal = NULL;
    }

    void UpdateAI(const uint32 diff)
    {
      if(!pInstance) return;

      if (pInstance->GetData(TYPE_EVENT_NPC) != NPC_FIZZLEBANG) return;

        UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
        if(UpdateTimer <= diff)
        {
        switch(pInstance->GetData(TYPE_EVENT))
              {
               case 1110:
                    pInstance->SetData(TYPE_EVENT, 1120);
                    UpdateTimer = 4000;
                    break;
               case 1120:
                    DoScriptText(-1713511, me);
                    pInstance->SetData(TYPE_EVENT, 1130);
                    UpdateTimer = 12000;
                    break;
               case 1130:
                    me->GetMotionMaster()->MovementExpired();
                    pPortal = me->SummonCreature(NPC_PORTAL, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (pPortal) pPortal->SetRespawnDelay(DAY);
                    DoScriptText(-1713512, me);
                    pInstance->SetData(TYPE_EVENT, 1135);
                    UpdateTimer = 4000;
                    break;
               case 1135:
                    me->GetMotionMaster()->MovementExpired();
                    if (pPortal) pPortal->SetDisplayId(15900);
                    pInstance->SetData(TYPE_EVENT, 1140);
                    UpdateTimer = 4000;
                    break;
               case 1140:
                    me->CastSpell(pPortal,69016,false);
                    pInstance->SetData(TYPE_STAGE,4);
                    pInstance->SetData(TYPE_JARAXXUS,IN_PROGRESS);
                          me->SummonCreature(NPC_JARAXXUS, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                          if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_JARAXXUS))) {
                                pTemp->SetInCombatWithZone();
                                me->SetInCombatWith(pTemp);
                                pTemp->AddThreat(me, 1000.0f);
                                pTemp->AI()->AttackStart(me);
                                }
                    pInstance->SetData(TYPE_EVENT, 1150);
                    UpdateTimer = 500;
                    break;
               case 1150:
                    DoScriptText(-1713513, me);
                    pInstance->SetData(TYPE_EVENT, 1160);
                    UpdateTimer = 1000;
                    break;
               case 1160:
                    DoScriptText(-1713515, me);
                    pInstance->SetData(TYPE_EVENT, 1170);
                    UpdateTimer = 1000;
                    break;
               case 1170:
                    pInstance->SetData(TYPE_EVENT, 1175);
                    UpdateTimer = 1000;
                    break;
              }
        } else UpdateTimer -= diff;
        pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }
};

CreatureAI* GetAI_npc_fizzlebang_toc(Creature* pCreature)
{
    return new npc_fizzlebang_tocAI(pCreature);
}

struct npc_tirion_tocAI : public ScriptedAI
{
    npc_tirion_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance) return;
        if (pInstance->GetData(TYPE_EVENT_NPC) != NPC_TIRION) return;

        UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
        switch (pInstance->GetData(TYPE_EVENT))
        {
        case 110:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
               DoScriptText(-1713500, me);
               UpdateTimer = 12000;
               pInstance->SetData(TYPE_EVENT,120);
               //pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_WEST_PORTCULLIS));
               break;
        case 140:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
               DoScriptText(-1713501, me);
               UpdateTimer = 10000;
               pInstance->SetData(TYPE_EVENT,150);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;
        case 150:
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                if (pInstance->GetData(TYPE_BEASTS) != DONE && pInstance->GetData(TYPE_NORTHREND_BEASTS) != GORMOK_DONE) {
                      me->SummonCreature(NPC_GORMOK, SpawnLoc[26].x, SpawnLoc[26].y, SpawnLoc[26].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                          if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_GORMOK))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
                        }
                UpdateTimer = 10000;
                pInstance->SetData(TYPE_EVENT,160);
                pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                pInstance->SetData(TYPE_STAGE,1);
                pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                break;

        case 200:
               DoScriptText(-1713503, me);
               UpdateTimer = 10000;
               pInstance->SetData(TYPE_EVENT,205);
               break;

        case 205:
               UpdateTimer = 8000;
               pInstance->SetData(TYPE_EVENT,210);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;

        case 210:
                        if (pInstance->GetData(TYPE_BEASTS) != DONE && pInstance->GetData(TYPE_NORTHREND_BEASTS) != SNAKES_DONE){
                        me->SummonCreature(NPC_DREADSCALE, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                        me->SummonCreature(NPC_ACIDMAW, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                          if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_DREADSCALE))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
                          if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_ACIDMAW))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
                        }
               UpdateTimer = 10000;
               pInstance->SetData(TYPE_EVENT,220);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;
        case 300:
               DoScriptText(-1713505, me);
               UpdateTimer = 15000;
               pInstance->SetData(TYPE_EVENT,305);
               break;
        case 305:
               UpdateTimer = 8000;
               pInstance->SetData(TYPE_EVENT,310);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;
        case 310:
                if (pInstance->GetData(TYPE_BEASTS) != DONE && pInstance->GetData(TYPE_NORTHREND_BEASTS) != ICEHOWL_DONE) {
                        me->SummonCreature(NPC_ICEHOWL, SpawnLoc[26].x, SpawnLoc[26].y, SpawnLoc[26].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_ICEHOWL))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
                        }
               UpdateTimer = 10000;
               pInstance->SetData(TYPE_EVENT,320);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;
        case 400:
               DoScriptText(-1713509, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,0);
               //pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_WEST_PORTCULLIS));
               break;

        case 666:
               DoScriptText(-1713709, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,0);
               //pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_WEST_PORTCULLIS));
               break;

        case 1010:
               DoScriptText(-1713510, me);
               UpdateTimer = 5000;
               me->SummonCreature(NPC_FIZZLEBANG, SpawnLoc[21].x, SpawnLoc[21].y, SpawnLoc[21].z, 2, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               pInstance->SetData(TYPE_EVENT,1110);
               break;

        case 1180:
               DoScriptText(-1713516, me);
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,0);
               break;

        case 2000:
               DoScriptText(-1713526, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,2010);
               break;
        case 2030:
               DoScriptText(-1713529, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,0);
               break;
        case 3000:
               DoScriptText(-1713530, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3050);
               break;
        case 3001:
               DoScriptText(-1713530, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3051);
               break;
        case 3060:
               DoScriptText(-1713532, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3070);
               break;
        case 3061:
               DoScriptText(-1713532, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3071);
               break;
//Summoning crusaders
        case 3091:
               pInstance->SetData(TYPE_STAGE,6);
               if (pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_NORMAL 
                              ||  pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                              {
                              pInstance->SetData(TYPE_CRUSADERS_COUNT,12);
                              me->SummonCreature(NPC_CRUSADER_1_7, SpawnLoc[9].x, SpawnLoc[9].y, SpawnLoc[9].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_7))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_1_8, SpawnLoc[10].x, SpawnLoc[10].y, SpawnLoc[10].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_8))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_1_9, SpawnLoc[11].x, SpawnLoc[11].y, SpawnLoc[11].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_9))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_1_10, SpawnLoc[12].x, SpawnLoc[12].y, SpawnLoc[12].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_10))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_0_1, SpawnLoc[13].x, SpawnLoc[13].y, SpawnLoc[13].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_0_1))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           pTemp->SetRespawnDelay(DAY);
                                           }
                              me->SummonCreature(NPC_CRUSADER_0_2, SpawnLoc[14].x, SpawnLoc[14].y, SpawnLoc[14].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_0_2))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           pTemp->SetRespawnDelay(DAY);
                                           }
                              }
                              else pInstance->SetData(TYPE_CRUSADERS_COUNT,6);
               me->SummonCreature(NPC_CRUSADER_1_1, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_1))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
               me->SummonCreature(NPC_CRUSADER_1_2, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_2))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
               me->SummonCreature(NPC_CRUSADER_1_3, SpawnLoc[5].x, SpawnLoc[5].y, SpawnLoc[5].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_3))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
               me->SummonCreature(NPC_CRUSADER_1_4, SpawnLoc[6].x, SpawnLoc[6].y, SpawnLoc[6].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_4))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
               me->SummonCreature(NPC_CRUSADER_1_5, SpawnLoc[7].x, SpawnLoc[7].y, SpawnLoc[7].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_5))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
               me->SummonCreature(NPC_CRUSADER_1_6, SpawnLoc[8].x, SpawnLoc[8].y, SpawnLoc[8].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
               if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_1_6))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,3095);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               pInstance->SetData(TYPE_CRUSADERS,IN_PROGRESS);
               break;

//summoning crusaders
        case 3090:
               pInstance->SetData(TYPE_STAGE,6);
               if (pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_NORMAL 
                              ||  pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                              {
                              pInstance->SetData(TYPE_CRUSADERS_COUNT,12);

                              me->SummonCreature(NPC_CRUSADER_2_7, SpawnLoc[9].x, SpawnLoc[9].y, SpawnLoc[9].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_7))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_2_8, SpawnLoc[10].x, SpawnLoc[10].y, SpawnLoc[10].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_8))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_2_9, SpawnLoc[11].x, SpawnLoc[11].y, SpawnLoc[11].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_9))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_2_10, SpawnLoc[12].x, SpawnLoc[12].y, SpawnLoc[12].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_10))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_0_1, SpawnLoc[13].x, SpawnLoc[13].y, SpawnLoc[13].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_0_1))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              me->SummonCreature(NPC_CRUSADER_0_2, SpawnLoc[14].x, SpawnLoc[14].y, SpawnLoc[14].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                              if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_0_2))) {
                                           pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                           pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                           }
                              }
                              else pInstance->SetData(TYPE_CRUSADERS_COUNT,6);

                me->SummonCreature(NPC_CRUSADER_2_1, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_1))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
                me->SummonCreature(NPC_CRUSADER_2_2, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_2))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
                me->SummonCreature(NPC_CRUSADER_2_3, SpawnLoc[5].x, SpawnLoc[5].y, SpawnLoc[5].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_3))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
                me->SummonCreature(NPC_CRUSADER_2_4, SpawnLoc[6].x, SpawnLoc[6].y, SpawnLoc[6].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_4))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
                me->SummonCreature(NPC_CRUSADER_2_5, SpawnLoc[7].x, SpawnLoc[7].y, SpawnLoc[7].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                          if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_5))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                }
                me->SummonCreature(NPC_CRUSADER_2_6, SpawnLoc[8].x, SpawnLoc[8].y, SpawnLoc[8].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                          if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_CRUSADER_2_6))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,3095);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               pInstance->SetData(TYPE_CRUSADERS,IN_PROGRESS);
               break;

//Crusaders battle end
        case 3100:
               DoScriptText(-1713535, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,0);
               break;

        case 4000:
               DoScriptText(-1713536, me);
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,4010);
               break;
        case 4010:
               DoScriptText(-1713537, me);
               UpdateTimer = 10000;
               pInstance->SetData(TYPE_EVENT,4015);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;

        case 4015:
               pInstance->SetData(TYPE_STAGE,7);
               pInstance->SetData(TYPE_VALKIRIES,IN_PROGRESS);
                      me->SummonCreature(NPC_LIGHTBANE, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                      if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_LIGHTBANE))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
                      me->SummonCreature(NPC_DARKBANE, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                      if (Creature* pTemp = Unit::GetCreature((*me),pInstance->GetData64(NPC_DARKBANE))) {
                                pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
                                pTemp->SetInCombatWithZone();
                                }
               UpdateTimer = 10000;
               pInstance->SetData(TYPE_EVENT,4016);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;

        case 4040:
               UpdateTimer = 60000;
               pInstance->SetData(TYPE_EVENT,5000);
               break;

        case 5000:
               DoScriptText(-1713549, me);
               UpdateTimer = 8000;
               pInstance->SetData(TYPE_EVENT,5005);
               break;
        case 5005:
               UpdateTimer = 8000;
               pInstance->SetData(TYPE_EVENT,5010);
               pInstance->SetData(TYPE_STAGE,8);
                    me->SummonCreature(NPC_LICH_KING_1, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 5, TEMPSUMMON_MANUAL_DESPAWN, 0);
               break;
        case 5020:
               DoScriptText(-1713551, me);
               UpdateTimer = 8000;
               pInstance->SetData(TYPE_EVENT,5030);
               break;
        case 6000:
               me->NearTeleportTo(SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 4.0f);
               UpdateTimer = 20000;
               pInstance->SetData(TYPE_EVENT,6005);
               break;
        case 6005:
               DoScriptText(-1713565, me);
               UpdateTimer = 20000;
               pInstance->SetData(TYPE_EVENT,6010);
               break;
        case 6010:
              if (pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_10MAN_HEROIC 
                  ||  pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
               DoScriptText(-1713566, me);
               UpdateTimer = 60000;
               pInstance->SetData(TYPE_EVENT,6020);
               break;
        case 6020:
               pInstance->SetData(TYPE_STAGE,10);
               me->ForcedDespawn();
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,6030);
               break;
        }
        } else UpdateTimer -= diff;
       pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);

    }
};

CreatureAI* GetAI_npc_tirion_toc(Creature* pCreature)
{
    return new npc_tirion_tocAI(pCreature);
}

struct npc_garrosh_tocAI : public ScriptedAI
{
    npc_garrosh_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance) return;
        if (pInstance->GetData(TYPE_EVENT_NPC) != NPC_GARROSH) return;

        UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
        switch (pInstance->GetData(TYPE_EVENT))
        {
        case 120:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
               DoScriptText(-1713702, me);
               UpdateTimer = 2000;
               pInstance->SetData(TYPE_EVENT,122);
               break;
        case 122:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,130);
               break;
        case 2010:
               DoScriptText(-1713527, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,2020);
               break;
        case 3050:
               DoScriptText(-1713531, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3060);
               break;
        case 3070:
               DoScriptText(-1713533, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3080);
               break;
        case 3081:
               DoScriptText(-1713734, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3091);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;
        case 4030:
               DoScriptText(-1713748, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,4040);
               break;
        }
        } else UpdateTimer -= diff;
       pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }
};

CreatureAI* GetAI_npc_garrosh_toc(Creature* pCreature)
{
    return new npc_garrosh_tocAI(pCreature);
};

struct npc_rinn_tocAI : public ScriptedAI
{
    npc_rinn_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)me->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance) return;
        if (pInstance->GetData(TYPE_EVENT_NPC) != NPC_RINN) return;

        UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
        switch (pInstance->GetData(TYPE_EVENT))
        {
        case 130:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
               DoScriptText(-1713502, me);
               UpdateTimer = 2000;
               pInstance->SetData(TYPE_EVENT,132);
               break;
        case 132:
               me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
               UpdateTimer = 3000;
               pInstance->SetData(TYPE_EVENT,140);
               break;
        case 2020:
               DoScriptText(-1713528, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,2030);
               break;
        case 3051:
               DoScriptText(-1713731, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3061);
               break;
        case 3071:
               DoScriptText(-1713733, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3081);
               break;
        case 3080:
               DoScriptText(-1713534, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,3090);
               pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
               break;
        case 4020:
               DoScriptText(-1713548, me);
               UpdateTimer = 5000;
               pInstance->SetData(TYPE_EVENT,4030);
               break;
        }
        } else UpdateTimer -= diff;
        pInstance->SetData(TYPE_EVENT_TIMER,UpdateTimer);
    }
};

CreatureAI* GetAI_npc_rinn_toc(Creature* pCreature)
{
    return new npc_rinn_tocAI(pCreature);
};

void AddSC_trial_of_the_crusader()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc_announcer";
    NewScript->GetAI = &GetAI_npc_toc_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc_announcer;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_lich_king_toc";
    NewScript->GetAI = &GetAI_boss_lich_king_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_fizzlebang_toc";
    NewScript->GetAI = &GetAI_npc_fizzlebang_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_tirion_toc";
    NewScript->GetAI = &GetAI_npc_tirion_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_garrosh_toc";
    NewScript->GetAI = &GetAI_npc_garrosh_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_rinn_toc";
    NewScript->GetAI = &GetAI_npc_rinn_toc;
    NewScript->RegisterSelf();
}