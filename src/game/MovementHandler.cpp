/*
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Phantom Project <http://phantom-project.org/>
 *
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

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Corpse.h"
#include "Player.h"
#include "Vehicle.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Transports.h"
#include "BattleGround.h"
#include "WaypointMovementGenerator.h"
#include "InstanceSaveMgr.h"
#include "ObjectMgr.h"
#include "World.h"

// Movement anticheat defines
//#define ANTICHEAT_DEBUG
#define ANTICHEAT_EXCEPTION_INFO
// End Movement anticheat defines

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket & /*recv_data*/)
{
    sLog.outDebug("WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // ignore unexpected far teleports
    if (!GetPlayer()->IsBeingTeleportedFar())
        return;

    // get the teleport destination
    WorldLocation &loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc))
    {
        LogoutPlayer(false);
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.GetMapId());
    InstanceTemplate const* mInstance = objmgr.GetInstanceTemplate(loc.GetMapId());

    // reset instance validity, except if going to an instance inside an instance
    if (GetPlayer()->m_InstanceValid == false && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    Map * oldMap = GetPlayer()->GetMap();
    assert(oldMap);
    if (GetPlayer()->IsInWorld())
    {
        sLog.outCrash("Player is still in world when teleported out of map %u! to new map %u", oldMap->GetId(), loc.GetMapId());
        oldMap->Remove(GetPlayer(), false);
    }

    // relocate the player to the teleport destination
    Map * newMap = MapManager::Instance().CreateMap(loc.GetMapId(), GetPlayer(), 0);
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!newMap || !newMap->CanEnter(GetPlayer()))
    {
        sLog.outError("Map %d could not be created for player %d, porting player to homebind", loc.GetMapId(), GetPlayer()->GetGUIDLow());
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }
    else
        GetPlayer()->Relocate(&loc);

    GetPlayer()->ResetMap();
    GetPlayer()->SetMap(newMap);

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    if (!GetPlayer()->GetMap()->Add(GetPlayer()))
    {
        sLog.outError("WORLD: failed to teleport player %s (%d) to map %d because of unknown reason!", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), loc.GetMapId());
        GetPlayer()->ResetMap();
        GetPlayer()->SetMap(oldMap);
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }

    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if (_player->InBattleGround())
    {
        // cleanup setting if outdated
        if (!mEntry->IsBattleGroundOrArena())
        {
            // We're not in BG
            _player->SetBattleGroundId(0, BATTLEGROUND_TYPE_NONE);
            // reset destination bg team
            _player->SetBGTeam(0);
        }
        // join to bg case
        else if (BattleGround *bg = _player->GetBattleGround())
        {
            if (_player->IsInvitedForBattleGroundInstance(_player->GetBattleGroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE)
    {
        if (!_player->InBattleGround())
        {
            // short preparations to continue flight
            FlightPathMovementGenerator* flight = (FlightPathMovementGenerator*)(GetPlayer()->GetMotionMaster()->top());
            flight->Initialize(*GetPlayer());
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->GetMotionMaster()->MovementExpired();
        GetPlayer()->CleanupAfterTaxiFlight();
    }

    // resurrect character at enter into instance where his corpse exist after add to map
    Corpse *corpse = GetPlayer()->GetCorpse();
    if (corpse && corpse->GetType() != CORPSE_BONES && corpse->GetMapId() == GetPlayer()->GetMapId())
    {
        if (mEntry->IsDungeon())
        {
            GetPlayer()->ResurrectPlayer(0.5f,false);
            GetPlayer()->SpawnCorpseBones();
        }
    }

    bool allowMount = !mEntry->IsDungeon() || mEntry->IsBattleGroundOrArena();
    if (mInstance)
    {
        Difficulty diff = GetPlayer()->GetDifficulty(mEntry->IsRaid());
        if (MapDifficulty const* mapDiff = GetMapDifficultyData(mEntry->MapID,diff))
        {
            if (mapDiff->resetTime)
            {
                if (uint32 timeReset = sInstanceSaveManager.GetResetTimeFor(mEntry->MapID,diff))
                {
                    uint32 timeleft = timeReset - time(NULL);
                    GetPlayer()->SendInstanceResetWarning(mEntry->MapID, diff, timeleft);
                }
            }
        }
        allowMount = mInstance->allowMount;
    }

    // mount allow check
    if (!allowMount)
        _player->RemoveAurasByType(SPELL_AURA_MOUNTED);

    // update zone immediately, otherwise leave channel will cause crash in mtmap
    uint32 newzone, newarea;
    GetPlayer()->GetZoneAndAreaId(newzone, newarea);
    GetPlayer()->UpdateZone(newzone, newarea);

    // honorless target
    if (GetPlayer()->pvpInfo.inHostileArea)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true);

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recv_data)
{
    sLog.outDebug("MSG_MOVE_TELEPORT_ACK");
    uint64 guid;

    if (!recv_data.readPackGUID(guid))
        return;

    uint32 flags, time;
    recv_data >> flags >> time;
    DEBUG_LOG("Guid " UI64FMTD, guid);
    DEBUG_LOG("Flags %u, time %u", flags, time/IN_MILISECONDS);

    Unit *mover = _player->m_mover;
    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;

    if (!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if (guid != plMover->GetGUID())
        return;

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();

    plMover->SetPosition(dest,true);

    uint32 newzone, newarea;
    plMover->GetZoneAndAreaId(newzone, newarea);
    plMover->UpdateZone(newzone, newarea);

    // new zone
    if (old_zone != newzone)
    {
        // honorless target
        if (plMover->pvpInfo.inHostileArea)
            plMover->CastSpell(plMover, 2479, true);
    }

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes(WorldPacket & recv_data)
{
    uint16 opcode = recv_data.GetOpcode();
    //sLog.outDebug("WORLD: Recvd %s (%u, 0x%X) opcode", LookupOpcodeName(opcode), opcode, opcode);
    recv_data.hexlike();

    Unit *mover = _player->m_mover;

    assert(mover != NULL);                                  // there must always be a mover

    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;
    Vehicle *vehMover = mover->GetVehicleKit();
    if (vehMover)
        if (mover->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
            if (Unit *charmer = mover->GetCharmer())
                if (charmer->GetTypeId() == TYPEID_PLAYER)
                    plMover = (Player*)charmer;

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (plMover && plMover->IsBeingTeleported())
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    /* extract packet */
    uint64 guid;

    if (!recv_data.readPackGUID(guid))
        return;

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recv_data, &movementInfo);
    /*----------------*/

   /* if (recv_data.size() != recv_data.rpos())
    {
        sLog.outError("MovementHandler: player %s (guid %d, account %u) sent a packet (opcode %u) that is " SIZEFMTD " bytes larger than it should be. Kicked as cheater.", _player->GetName(), _player->GetGUIDLow(), _player->GetSession()->GetAccountId(), recv_data.GetOpcode(), recv_data.size() - recv_data.rpos());
        KickPlayer();*/
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
    /*    return;
    }*/

    if (!Trinity::IsValidMapCoord(movementInfo.x, movementInfo.y, movementInfo.z, movementInfo.o))
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    /* handle special cases */
    if (movementInfo.flags & MOVEMENTFLAG_ONTRANSPORT)
    {
        // transports size limited
        // (also received at zeppelin leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (movementInfo.t_x > 50 || movementInfo.t_y > 50 || movementInfo.t_z > 50)
        {
            recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
            return;
        }

        if (!Trinity::IsValidMapCoord(movementInfo.x+movementInfo.t_x, movementInfo.y + movementInfo.t_y,
            movementInfo.z + movementInfo.t_z, movementInfo.o + movementInfo.t_o))
        {
            recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
            return;
        }

        // if we boarded a transport, add us to it
        if (plMover && !plMover->m_transport && !plMover->m_temp_transport)
        {
            // elevators also cause the client to send MOVEMENTFLAG_ONTRANSPORT - just unmount if the guid can be found in the transport list
            for (MapManager::TransportSet::const_iterator iter = MapManager::Instance().m_Transports.begin(); iter != MapManager::Instance().m_Transports.end(); ++iter)
            {
                if ((*iter)->GetGUID() == movementInfo.t_guid)
                {
                    plMover->m_transport = (*iter);
                    (*iter)->AddPassenger(plMover);
                    break;
                }
            }
            if (!plMover->m_transport)
                if (Map *tempMap = mover->GetMap())
                    if (GameObject *tempTransport = tempMap->GetGameObject(movementInfo.t_guid))
                        if (tempTransport->IsTransport())
                            plMover->m_temp_transport = tempTransport;
        }

        if ((!plMover && !mover->GetTransport() && !mover->GetVehicle()) || (plMover && !plMover->m_vehicle && !plMover->m_transport && !plMover->m_temp_transport)) // Not sure if the first part is needed. Just added it for verbosity.
        {
            GameObject *go = mover->GetMap()->GetGameObject(movementInfo.t_guid);
            if (!go || go->GetGoType() != GAMEOBJECT_TYPE_TRANSPORT)
                movementInfo.flags &= ~MOVEMENTFLAG_ONTRANSPORT;
        }
    }
    else if (plMover && (plMover->m_transport || plMover->m_temp_transport)) // if we were on a transport, leave
    {
        if (plMover->m_transport)
        {
            plMover->m_transport->RemovePassenger(plMover);
            plMover->m_transport = NULL;
        }
        plMover->m_temp_transport = NULL;
        movementInfo.t_x = 0.0f;
        movementInfo.t_y = 0.0f;
        movementInfo.t_z = 0.0f;
        movementInfo.t_o = 0.0f;
        movementInfo.t_time = 0;
        movementInfo.t_seat = -1;
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == MSG_MOVE_FALL_LAND && plMover && !plMover->isInFlight())
    {
        // movement anticheat
        plMover->m_anti_JumpCount = 0;
        plMover->m_anti_JumpBaseZ = 0;
        if (!vehMover)
            plMover->HandleFall(movementInfo);
    }

    if (plMover && ((movementInfo.flags & MOVEMENTFLAG_SWIMMING) != 0) != plMover->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        plMover->SetInWater(!plMover->IsInWater() || plMover->GetBaseMap()->IsUnderWater(movementInfo.x, movementInfo.y, movementInfo.z));
    }

    /*----------------------*/
    // begin anti cheat
    bool check_passed = true;
    #ifdef ANTICHEAT_DEBUG
    sLog.outBasic("AC2-%s > time: %d fall-time: %d | xyzo: %f, %f, %fo(%f) flags[%X] opcode[%s] | transport (xyzo): %f, %f, %fo(%f)",
        plMover->GetName(), movementInfo.time, movementInfo.fallTime, movementInfo.x, movementInfo.y, movementInfo.z, movementInfo.o,
        movementInfo.flags, LookupOpcodeName(opcode), movementInfo.t_x, movementInfo.t_y, movementInfo.t_z, movementInfo.t_o);
    sLog.outBasic("AC2-%s Transport > GUID: (low)%d - (high)%d",
        plMover->GetName(), GUID_LOPART(movementInfo.t_guid), GUID_HIPART(movementInfo.t_guid));
    #endif

    if (plMover)
    {
        if (World::GetEnableMvAnticheat() && !plMover->GetCharmerOrOwnerPlayerOrPlayerItself()->isGameMaster())
        {
            // calc time deltas
            int32 cClientTimeDelta = 1500;
            if (plMover->m_anti_LastClientTime != 0)
            {
                cClientTimeDelta = movementInfo.time - plMover->m_anti_LastClientTime;
                plMover->m_anti_DeltaClientTime += cClientTimeDelta;
                plMover->m_anti_LastClientTime = movementInfo.time;
            }
            else
                plMover->m_anti_LastClientTime = movementInfo.time;

            const uint64 cServerTime = getMSTime();
            uint32 cServerTimeDelta = 1500;
            if (plMover->m_anti_LastServerTime != 0)
            {
                cServerTimeDelta = cServerTime - plMover->m_anti_LastServerTime;
                plMover->m_anti_DeltaServerTime += cServerTimeDelta;
                plMover->m_anti_LastServerTime = cServerTime;
            }
            else
                plMover->m_anti_LastServerTime = cServerTime;

            // resync times on client login (first 15 sec for heavy areas)
            if (plMover->m_anti_DeltaServerTime < 15000 && plMover->m_anti_DeltaClientTime < 15000)
                plMover->m_anti_DeltaClientTime = plMover->m_anti_DeltaServerTime;

            const int32 sync_time = plMover->m_anti_DeltaClientTime - plMover->m_anti_DeltaServerTime;

            #ifdef ANTICHEAT_DEBUG
            sLog.outBasic("AC2-%s Time > cClientTimeDelta: %d, cServerTime: %d | deltaC: %d - deltaS: %d | SyncTime: %d", plMover->GetName(), cClientTimeDelta, cServerTime, plMover->m_anti_DeltaClientTime, plMover->m_anti_DeltaServerTime, sync_time);
            #endif

            // mistiming checks
            const int32 GetMistimingDelta = abs(int32(World::GetMistimingDelta()));
            if (sync_time > GetMistimingDelta)
            {
                cClientTimeDelta = cServerTimeDelta;
                ++(plMover->m_anti_MistimingCount);

                const bool bMistimingModulo = plMover->m_anti_MistimingCount % 50 == 0;

                if (bMistimingModulo)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    sLog.outError("AC2-%s, mistiming exception #%d, mistiming: %dms", plMover->GetName(), plMover->m_anti_MistimingCount, sync_time);
                    #endif
                    check_passed = false;
                }                   
                if (vehMover)
                    vehMover->Die();
                // Tell the player "Sure, you can fly!"
                {
                    WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                    data.append(plMover->GetPackGUID());
                    data << uint32(0);
                    SendPacket(&data);
                }
                // Then tell the player "Wait, no, you can't."
                {
                    WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                    data.append(plMover->GetPackGUID());
                    data << uint32(0);
                    SendPacket(&data);
                }
                plMover->FallGround(2);

                /* Disabled, not passive at all, and apparently causing crashes:
                if (plMover->m_anti_MistimingCount > World::GetMistimingAlarms())
                {
                    sWorld.SendWorldText(3, strcat("Kicking cheater: ", plMover->GetName()));
                    KickPlayer();
                    return;
                } */ 
            }
            // end mistiming checks

            const uint32 curDest = plMover->m_taxi.GetTaxiDestination(); // check taxi flight
            if (!curDest)
            {
                UnitMoveType move_type;

                // calculating section
                // current speed
                if (movementInfo.flags & MOVEMENTFLAG_FLYING)
                    move_type = movementInfo.flags & MOVEMENTFLAG_BACKWARD ? MOVE_FLIGHT_BACK : MOVE_FLIGHT;
                else if (movementInfo.flags & MOVEMENTFLAG_SWIMMING)
                    move_type = movementInfo.flags & MOVEMENTFLAG_BACKWARD ? MOVE_SWIM_BACK : MOVE_SWIM;
                else if (movementInfo.flags & MOVEMENTFLAG_WALK_MODE)
                    move_type = MOVE_WALK;
                // hmm... in first time after login player has MOVE_SWIMBACK instead MOVE_WALKBACK
                else
                    move_type = movementInfo.flags & MOVEMENTFLAG_BACKWARD ? MOVE_SWIM_BACK : MOVE_RUN;

                const float current_speed = mover->GetSpeed(move_type);
                // end current speed

                // movement distance
                const float delta_x = plMover->m_transport || plMover->m_temp_transport ? 0 : plMover->GetPositionX() - movementInfo.x;
                const float delta_y = plMover->m_transport || plMover->m_temp_transport ? 0 : plMover->GetPositionY() - movementInfo.y;
                const float delta_z = plMover->m_transport || plMover->m_temp_transport ? 0 : plMover->GetPositionZ() - movementInfo.z;
                const float real_delta = plMover->m_transport || plMover->m_temp_transport ? 0 : pow(delta_x, 2) + pow(delta_y, 2);
                // end movement distance

                const bool no_fly_auras = !(plMover->HasAuraType(SPELL_AURA_FLY) || plMover->HasAuraType(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED)
                    || plMover->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || plMover->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)
                    || plMover->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS) || plMover->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK));
                const bool no_fly_flags = (movementInfo.flags & (MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLY_MODE | MOVEMENTFLAG_FLYING)) == 0;

                const bool no_swim_flags = (movementInfo.flags & MOVEMENTFLAG_SWIMMING) == 0;
                const bool no_swim_in_water = !mover->IsInWater();
                const bool no_swim_above_water = movementInfo.z-7.0f >= mover->GetBaseMap()->GetWaterLevel(movementInfo.x,movementInfo.y);
                const bool no_swim_water = no_swim_in_water && no_swim_above_water;

                const bool no_waterwalk_flags = (movementInfo.flags & MOVEMENTFLAG_WATERWALKING) == 0;
                const bool no_waterwalk_auras = !(plMover->HasAuraType(SPELL_AURA_WATER_WALK) || plMover->HasAuraType(SPELL_AURA_GHOST));

                if (cClientTimeDelta < 0)
                    cClientTimeDelta = 0;
                const float time_delta = cClientTimeDelta < 1500 ? float(cClientTimeDelta)/1000.0f : 1.5f; // normalize time - 1.5 second allowed for heavy loaded server

                const float tg_z = (real_delta != 0 && no_fly_auras && no_swim_flags) ? (pow(delta_z, 2) / real_delta) : -99999; // movement distance tangents

                if (current_speed < plMover->m_anti_Last_HSpeed && plMover->m_anti_LastSpeedChangeTime == 0)
                    plMover->m_anti_LastSpeedChangeTime = movementInfo.time + uint32(floor(((plMover->m_anti_Last_HSpeed / current_speed) * 1500)) + 100); // 100ms above for random fluctuation

                const float allowed_delta = plMover->m_transport || plMover->m_temp_transport ? 2 : // movement distance allowed delta
                    pow(std::max(current_speed, plMover->m_anti_Last_HSpeed) * time_delta, 2)
                    + 2                                                                             // minimum allowed delta
                    + (tg_z > 2.2 ? pow(delta_z, 2)/2.37f : 0);                                     // mountain fall allowed delta

                if (movementInfo.time > plMover->m_anti_LastSpeedChangeTime)
                {
                    plMover->m_anti_Last_HSpeed = current_speed;                                    // store current speed
                    plMover->m_anti_Last_VSpeed = -2.3f;
                    plMover->m_anti_LastSpeedChangeTime = 0;
                }
                // end calculating section

                // AntiGravity (thanks to Meekro)
                const float JumpHeight = plMover->m_anti_JumpBaseZ - movementInfo.z;
                if (no_fly_auras && no_swim_in_water && plMover->m_anti_JumpBaseZ != 0 && JumpHeight < plMover->m_anti_Last_VSpeed)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    sLog.outError("AC2-%s, AntiGravity exception. JumpHeight = %f, Allowed Vertical Speed = %f",
                        plMover->GetName(), JumpHeight, plMover->m_anti_Last_VSpeed);
                    #endif
                    check_passed = false;
                    if (vehMover)
                        vehMover->Die();
                    // Tell the player "Sure, you can fly!"
                    {
                        WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                        data.append(plMover->GetPackGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    // Then tell the player "Wait, no, you can't."
                    {
                        WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                        data.append(plMover->GetPackGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    plMover->FallGround(2);
                }

                // multi jump checks
                if (opcode == MSG_MOVE_JUMP)
                {
                    if (no_fly_auras && no_swim_water)
                    {
                        if (plMover->m_anti_JumpCount >= 1)
                        {
                            // don't process new jump packet
                            check_passed = false;
                            if (vehMover)
                                vehMover->Die();
                            // Tell the player "Sure, you can fly!"
                            {
                                WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                                data.append(plMover->GetPackGUID());
                                data << uint32(0);
                                SendPacket(&data);
                            }
                            // Then tell the player "Wait, no, you can't."
                            {
                                WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                                data.append(plMover->GetPackGUID());
                                data << uint32(0);
                                SendPacket(&data);
                            }
                            plMover->FallGround(2);
                            plMover->m_anti_JumpCount = 0;
                        }
                        else
                        {
                            plMover->m_anti_JumpCount += 1;
                            plMover->m_anti_JumpBaseZ = movementInfo.z;
                        }
                    } else
                        plMover->m_anti_JumpCount = 0;
                }

                // speed and teleport hack checks
                if (real_delta > allowed_delta)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    if (real_delta < 4900.0f)
                        sLog.outError("AC2-%s, speed exception | cDelta=%f aDelta=%f | cSpeed=%f lSpeed=%f deltaTime=%f", plMover->GetName(), real_delta, allowed_delta, current_speed, plMover->m_anti_Last_HSpeed, time_delta);
                    else
                        sLog.outError("AC2-%s, teleport exception | cDelta=%f aDelta=%f | cSpeed=%f lSpeed=%f deltaTime=%f", plMover->GetName(), real_delta, allowed_delta, current_speed, plMover->m_anti_Last_HSpeed, time_delta);
                    #endif
                    check_passed = false;
                    if (vehMover)
                        vehMover->Die();
                    plMover->FallGround(2);
                }

                // mountain hack checks // 1.56f (delta_z < GetPlayer()->m_anti_Last_VSpeed))
                if (delta_z < plMover->m_anti_Last_VSpeed && plMover->m_anti_JumpCount == 0 && tg_z > 2.37f)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    sLog.outError("AC2-%s, mountain exception | tg_z=%f", plMover->GetName(), tg_z);
                    #endif
                    check_passed = false;
                    if (vehMover)
                        vehMover->Die();
                }

                // Fly hack checks
                if (no_fly_auras && !no_fly_flags)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO // Aura numbers: 201, 206, 207, 208, 209, 211
                    sLog.outError("AC2-%s, flight exception. {SPELL_AURA_FLY=[%X]} {SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED=[%X]} {SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED=[%X]} {SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK=[%X]} {plMover->GetVehicle()=[%X]}",
                        plMover->GetName(),
                        plMover->HasAuraType(SPELL_AURA_FLY), plMover->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED),
                        plMover->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED), plMover->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS),
                        plMover->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK), plMover->GetVehicle());
                    #endif
                    check_passed = false;
                    if (vehMover)
                        vehMover->Die();
                    // Tell the player "Sure, you can fly!"
                    {
                        WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                        data.append(plMover->GetPackGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    // Then tell the player "Wait, no, you can't."
                    {
                        WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                        data.append(plMover->GetPackGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    plMover->FallGround(2);
                }

                // Waterwalk checks
                if (no_waterwalk_auras && !no_waterwalk_flags)
                {
                    #ifdef ANTICHEAT_EXCEPTION_INFO
                    sLog.outError("AC2-%s, waterwalk exception. [%X]{SPELL_AURA_WATER_WALK=[%X]}",
                        plMover->GetName(), movementInfo.flags, plMover->HasAuraType(SPELL_AURA_WATER_WALK));
                    #endif
                    check_passed = false;
                    if (vehMover)
                        vehMover->Die();
                    // Tell the player "Sure, you can fly!"
                    {
                        WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
                        data.append(plMover->GetPackGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    // Then tell the player "Wait, no, you can't."
                    {
                        WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
                        data.append(plMover->GetPackGUID());
                        data << uint32(0);
                        SendPacket(&data);
                    }
                    plMover->FallGround(2);
                }

                // Teleport To Plane checks
                if (no_swim_in_water && movementInfo.z < 0.0001f && movementInfo.z > -0.0001f)
                {
                    if (const Map *map = plMover->GetMap())
                    {
                        float plane_z = map->GetHeight(movementInfo.x, movementInfo.y, MAX_HEIGHT) - movementInfo.z;
                        plane_z = (plane_z < -500.0f) ? 0.0f : plane_z; // check holes in height map
                        if (plane_z > 0.1f || plane_z < -0.1f)
                        {
                            #ifdef ANTICHEAT_DEBUG
                            sLog.outDebug("AC2-%s, teleport to plane exception. plane_z: %f", plMover->GetName(), plane_z);
                            #endif
                            #ifdef ANTICHEAT_EXCEPTION_INFO
                            if (plMover->m_anti_TeleToPlane_Count > World::GetTeleportToPlaneAlarms())
                            {
                                sLog.outError("AC2-%s, teleport to plane exception. Exception count: %d", plMover->GetName(), plMover->m_anti_TeleToPlane_Count);
                                /* Disabled, not passive at all, and apparently causing crashes:
                                sWorld.SendWorldText(3, strcat("Kicking cheater: ", plMover->GetName()));
                                KickPlayer();
                                return; */
                            }
                            #endif
                            ++(plMover->m_anti_TeleToPlane_Count);
                            check_passed = false;
                            if (vehMover)
                                vehMover->Die();
                        }
                    }
                }
                else
                    plMover->m_anti_TeleToPlane_Count = 0;
            }
        }
    }	
    /* process position-change */
    if (check_passed)
    {
    WorldPacket data(opcode, recv_data.size());
    movementInfo.time = getMSTime();
    movementInfo.guid = mover->GetGUID();
    WriteMovementInfo(&data, &movementInfo);
    mover->SendMessageToSet(&data, _player);

    mover->m_movementInfo = movementInfo;

    // this is almost never true (not sure why it is sometimes, but it is), normally use mover->IsVehicle()
    if (mover->GetVehicle())
    {
        mover->SetOrientation(movementInfo.o);
        return;
    }

    mover->SetPosition(movementInfo.x, movementInfo.y, movementInfo.z, movementInfo.o);

    if (plMover && !vehMover)                               // nothing is charmed, or player charmed
    {
        plMover->UpdateFallInformationIfNeed(movementInfo, opcode);

        if (movementInfo.z < -500.0f)
        {
            if (plMover->InBattleGround()
                && plMover->GetBattleGround()
                && plMover->GetBattleGround()->HandlePlayerUnderMap(_player))
            {
                // do nothing, the handle already did if returned true
            }
            else
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                // TODO: discard movement packets after the player is rooted
                if (plMover->isAlive())
                {
                    plMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, GetPlayer()->GetMaxHealth());
                    // pl can be alive if GM/etc
                    if (!plMover->isAlive())
                    {
                        // change the death state to CORPSE to prevent the death timer from
                        // starting in the next player update
                        plMover->KillPlayer();
                        plMover->BuildPlayerRepop();
                    }
                }

                // cancel the death timer here if started
                plMover->RepopAtGraveyard();
            }
        }
        // movement anticheat
        if (plMover->m_anti_AlarmCount > 0)
        {
            sLog.outError("AC2-%s produce %d anticheat alarms", plMover->GetName(), plMover->m_anti_AlarmCount);
            plMover->m_anti_AlarmCount = 0;
        }
        // end movement anticheat
    }
    /*else                                                    // creature charmed
    {
        if (mover->canFly())
        {
            bool flying = mover->IsFlying();
            if (flying != ((mover->GetByteValue(UNIT_FIELD_BYTES_1, 3) & 0x02) ? true : false))
                mover->SetFlying(flying);
        }
    }*/

    //sLog.outString("Receive Movement Packet %s:", opcodeTable[recv_data.GetOpcode()]);
    //mover->OutMovementInfo();
    }
    else if (plMover)
    {
        if (plMover->m_transport)
        {
            plMover->m_transport->RemovePassenger(plMover);
            plMover->m_transport = NULL;
        }
        plMover->m_temp_transport = NULL;
        ++(plMover->m_anti_AlarmCount);
        WorldPacket data;
        plMover->SetUnitMovementFlags(0);
        plMover->SendTeleportAckPacket();
        plMover->BuildHeartBeatMsg(&data);
        plMover->SendMessageToSet(&data, true);
    }
}

void WorldSession::HandleForceSpeedChangeAck(WorldPacket &recv_data)
{
    uint32 opcode = recv_data.GetOpcode();
    sLog.outDebug("WORLD: Recvd %s (%u, 0x%X) opcode", LookupOpcodeName(opcode), opcode, opcode);

    /* extract packet */
    uint64 guid;
    uint32 unk1;
    float  newspeed;

    if (!recv_data.readPackGUID(guid))
        return;

    // now can skip not our packet
    if (_player->GetGUID() != guid)
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    // continue parse packet

    recv_data >> unk1;                                      // counter or moveEvent

    MovementInfo movementInfo;
    movementInfo.guid = guid;
    ReadMovementInfo(recv_data, &movementInfo);

    recv_data >> newspeed;
    /*----------------*/

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack", "PitchRate" };

    switch(opcode)
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;          force_move_type = MOVE_WALK;        break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;           force_move_type = MOVE_RUN;         break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;      force_move_type = MOVE_RUN_BACK;    break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;          force_move_type = MOVE_SWIM;        break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;     force_move_type = MOVE_SWIM_BACK;   break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;     force_move_type = MOVE_TURN_RATE;   break;
        case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;        force_move_type = MOVE_FLIGHT;      break;
        case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK;   force_move_type = MOVE_FLIGHT_BACK; break;
        case CMSG_FORCE_PITCH_RATE_CHANGE_ACK:          move_type = MOVE_PITCH_RATE;    force_move_type = MOVE_PITCH_RATE;  break;
        default:
            sLog.outError("WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", opcode);
            return;
    }

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if (_player->m_forced_speed_changes[force_move_type] > 0)
    {
        --_player->m_forced_speed_changes[force_move_type];
        if (_player->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (_player->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            sLog.outError("%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName(), _player->GetSpeed(move_type), newspeed);
            _player->SetSpeed(move_type,_player->GetSpeedRate(move_type),true);
        }
        else                                                // must be lesser - cheating
        {
            sLog.outBasic("Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName(),_player->GetSession()->GetAccountId(),_player->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    uint64 guid;
    recv_data >> guid;

    if (GetPlayer()->IsInWorld())
       if (Unit *mover = ObjectAccessor::GetUnit(*GetPlayer(), guid))
       {
          GetPlayer()->SetMover(mover);
          if (mover != GetPlayer() && mover->canFly())
          {
              WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
              data.append(mover->GetPackGUID());
              data << uint32(0);
              SendPacket(&data);
          }
       }
    else
    {
        sLog.outError("HandleSetActiveMoverOpcode: incorrect mover guid: mover is " UI64FMTD " and should be " UI64FMTD, guid, _player->m_mover->GetGUID());
        GetPlayer()->SetMover(GetPlayer());
    }
}

void WorldSession::HandleMoveNotActiveMover(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");

    uint64 old_mover_guid;
    if (!recv_data.readPackGUID(old_mover_guid))
        return;

    /*if (_player->m_mover->GetGUID() == old_mover_guid)
    {
        sLog.outError("HandleMoveNotActiveMover: incorrect mover guid: mover is " I64FMT " and should be " I64FMT " instead of " I64FMT, _player->m_mover->GetGUID(), _player->GetGUID(), old_mover_guid);
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }*/

    MovementInfo mi;
    mi.guid = old_mover_guid;
    ReadMovementInfo(recv_data, &mi);

    //ReadMovementInfo(recv_data, &_player->m_mover->m_movementInfo);
    _player->m_movementInfo = mi;
}

void WorldSession::HandleDismissControlledVehicle(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_DISMISS_CONTROLLED_VEHICLE");
    recv_data.hexlike();

    uint64 vehicleGUID = _player->GetCharmGUID();

    if (!vehicleGUID)                                        // something wrong here...
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    uint64 guid;

    if (!recv_data.readPackGUID(guid))
        return;

    MovementInfo mi;
    mi.guid = guid;
    ReadMovementInfo(recv_data, &mi);

    _player->m_movementInfo = mi;

    /*
    ReadMovementInfo(recv_data, &_player->m_mover->m_movementInfo);*/
    _player->ExitVehicle();
}

void WorldSession::HandleChangeSeatsOnControlledVehicle(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE");
    recv_data.hexlike();

    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
        return;

    switch (recv_data.GetOpcode())
    {
    case CMSG_REQUEST_VEHICLE_PREV_SEAT:
        GetPlayer()->ChangeSeat(-1, false);
        break;
    case CMSG_REQUEST_VEHICLE_NEXT_SEAT:
        GetPlayer()->ChangeSeat(-1, true);
        break;
    case CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE:
        {
            uint64 guid;        // current vehicle guid
            if (!recv_data.readPackGUID(guid))
                return;

            ReadMovementInfo(recv_data, &vehicle_base->m_movementInfo);

            uint64 accessory;        //  accessory guid
            if (!recv_data.readPackGUID(accessory))
                return;

            int8 seatId;
            recv_data >> seatId;

            if (vehicle_base->GetGUID() != guid)
                return;

            if (!accessory)
                GetPlayer()->ChangeSeat(-1, seatId > 0); // prev/next
            else if (Unit *vehUnit = Unit::GetUnit(*GetPlayer(), accessory))
            {
                if (Vehicle *vehicle = vehUnit->GetVehicleKit())
                    if (vehicle->HasEmptySeat(seatId))
                        GetPlayer()->EnterVehicle(vehicle, seatId);
            }
        }
        break;
    case CMSG_REQUEST_VEHICLE_SWITCH_SEAT:
        {
            uint64 guid;        // current vehicle guid
            if (!recv_data.readPackGUID(guid))
                return;

            int8 seatId;
            recv_data >> seatId;

            if (vehicle_base->GetGUID() == guid)
                GetPlayer()->ChangeSeat(seatId);
            else if (Unit *vehUnit = Unit::GetUnit(*GetPlayer(), guid))
                if (Vehicle *vehicle = vehUnit->GetVehicleKit())
                    if (vehicle->HasEmptySeat(seatId))
                        GetPlayer()->EnterVehicle(vehicle, seatId);
        }
        break;
    default:
        break;
    }
}

void WorldSession::HandleEnterPlayerVehicle(WorldPacket &data)
{
    // Read guid
    uint64 guid;
    data >> guid;

    if (Player* pl=ObjectAccessor::FindPlayer(guid))
    {
        if (!pl->GetVehicleKit())
            return;
        if (!pl->IsInRaidWith(_player))
            return;
        if (!pl->IsWithinDistInMap(_player,INTERACTION_DISTANCE))
            return;
        _player->EnterVehicle(pl);
    }
}

void WorldSession::HandleEjectPasenger(WorldPacket &data)
{
    if (data.GetOpcode() == CMSG_EJECT_PASSENGER)
    {
        if (Vehicle* Vv= _player->GetVehicleKit())
        {
            uint64 guid;
            data >> guid;
            if (Player* Pl=ObjectAccessor::FindPlayer(guid))
                Pl->ExitVehicle();
        }
    }
}

void WorldSession::HandleRequestVehicleExit(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_REQUEST_VEHICLE_EXIT");
    recv_data.hexlike();
    GetPlayer()->ExitVehicle();
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvdata*/)
{
    //sLog.outDebug("WORLD: Recvd CMSG_MOUNTSPECIAL_ANIM");

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket & recv_data)
{
    sLog.outDebug("CMSG_MOVE_KNOCK_BACK_ACK");

    uint64 guid;                                            // guid - unused
    if (!recv_data.readPackGUID(guid))
        return;

    recv_data.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recv_data, &movementInfo);

    // Save movement flags
    _player->SetUnitMovementFlags(movementInfo.flags);
    #ifdef ANTICHEAT_DEBUG
    sLog.outBasic("%s CMSG_MOVE_KNOCK_BACK_ACK: time: %d, fall time: %d | xyzo: %f,%f,%fo(%f) flags[%X]", GetPlayer()->GetName(), movementInfo.time, movementInfo.fallTime, movementInfo.x, movementInfo.y, movementInfo.z, movementInfo.o, movementInfo.flags);
    sLog.outBasic("%s CMSG_MOVE_KNOCK_BACK_ACK additional: Vspeed: %f, Hspeed: %f", GetPlayer()->GetName(), movementInfo.j_unk, movementInfo.j_xyspeed);
    #endif

    _player->m_movementInfo = movementInfo;
    _player->m_anti_Last_HSpeed = movementInfo.j_xyspeed;
    _player->m_anti_Last_VSpeed = movementInfo.j_zspeed < 3.2f ? movementInfo.j_zspeed - 1.0f : 3.2f;

    const uint32 dt = (_player->m_anti_Last_VSpeed < 0) ? int(ceil(_player->m_anti_Last_VSpeed/-25)*1000) : int(ceil(_player->m_anti_Last_VSpeed/25)*1000);
    _player->m_anti_LastSpeedChangeTime = movementInfo.time + dt + 1000;
}

void WorldSession::HandleMoveHoverAck(WorldPacket& recv_data)
{
    sLog.outDebug("CMSG_MOVE_HOVER_ACK");

    uint64 guid;                                            // guid - unused
    if (!recv_data.readPackGUID(guid))
        return;

    recv_data.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recv_data, &movementInfo);

    recv_data.read_skip<uint32>();                          // unk2
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recv_data)
{
    sLog.outDebug("CMSG_MOVE_WATER_WALK_ACK");

    uint64 guid;                                            // guid - unused
    if (!recv_data.readPackGUID(guid))
        return;

    recv_data.read_skip<uint32>();                          // unk

    MovementInfo movementInfo;
    ReadMovementInfo(recv_data, &movementInfo);

    recv_data.read_skip<uint32>();                          // unk2
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recv_data)
{
    if (!_player->isAlive() || _player->isInCombat())
        return;

    uint64 summoner_guid;
    bool agree;
    recv_data >> summoner_guid;
    recv_data >> agree;

    _player->SummonIfPossible(agree);
}

