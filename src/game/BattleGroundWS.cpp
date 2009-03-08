/*
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundWS.h"
#include "Creature.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Language.h"

BattleGroundWS::BattleGroundWS()
{
    m_BgObjects.resize(BG_WS_OBJECT_MAX);
    m_BgCreatures.resize(BG_CREATURES_MAX_WS);

    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_WS_START_TWO_MINUTES;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;
}

BattleGroundWS::~BattleGroundWS()
{
}

void BattleGroundWS::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if(GetStatus() == STATUS_IN_PROGRESS)
    {
        if(m_FlagState[BG_TEAM_ALLIANCE] == BG_WS_FLAG_STATE_WAIT_RESPAWN)
        {
            m_FlagsTimer[BG_TEAM_ALLIANCE] -= diff;

            if(m_FlagsTimer[BG_TEAM_ALLIANCE] < 0)
            {
                m_FlagsTimer[BG_TEAM_ALLIANCE] = 0;
                RespawnFlag(ALLIANCE, true);
            }
        }
        if(m_FlagState[BG_TEAM_ALLIANCE] == BG_WS_FLAG_STATE_ON_GROUND)
        {
            m_FlagsDropTimer[BG_TEAM_ALLIANCE] -= diff;

            if(m_FlagsDropTimer[BG_TEAM_ALLIANCE] < 0)
            {
                m_FlagsDropTimer[BG_TEAM_ALLIANCE] = 0;
                RespawnFlagAfterDrop(ALLIANCE);
            }
        }
        if(m_FlagState[BG_TEAM_HORDE] == BG_WS_FLAG_STATE_WAIT_RESPAWN)
        {
            m_FlagsTimer[BG_TEAM_HORDE] -= diff;

            if(m_FlagsTimer[BG_TEAM_HORDE] < 0)
            {
                m_FlagsTimer[BG_TEAM_HORDE] = 0;
                RespawnFlag(HORDE, true);
            }
        }
        if(m_FlagState[BG_TEAM_HORDE] == BG_WS_FLAG_STATE_ON_GROUND)
        {
            m_FlagsDropTimer[BG_TEAM_HORDE] -= diff;

            if(m_FlagsDropTimer[BG_TEAM_HORDE] < 0)
            {
                m_FlagsDropTimer[BG_TEAM_HORDE] = 0;
                RespawnFlagAfterDrop(HORDE);
            }
        }
    }
}

void BattleGroundWS::StartingEventCloseDoors()
{
    for(uint32 i = BG_WS_OBJECT_DOOR_A_1; i <= BG_WS_OBJECT_DOOR_H_4; i++)
    {
        DoorClose(i);
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
    }
    for(uint32 i = BG_WS_OBJECT_A_FLAG; i <= BG_WS_OBJECT_BERSERKBUFF_2; i++)
        SpawnBGObject(i, RESPAWN_ONE_DAY);
}

void BattleGroundWS::StartingEventOpenDoors()
{
    for(uint32 i = BG_WS_OBJECT_DOOR_A_1; i <= BG_WS_OBJECT_DOOR_A_4; i++)
        DoorOpen(i);
    for(uint32 i = BG_WS_OBJECT_DOOR_H_1; i <= BG_WS_OBJECT_DOOR_H_2; i++)
        DoorOpen(i);

    SpawnBGObject(BG_WS_OBJECT_DOOR_A_5, RESPAWN_ONE_DAY);
    SpawnBGObject(BG_WS_OBJECT_DOOR_A_6, RESPAWN_ONE_DAY);
    SpawnBGObject(BG_WS_OBJECT_DOOR_H_3, RESPAWN_ONE_DAY);
    SpawnBGObject(BG_WS_OBJECT_DOOR_H_4, RESPAWN_ONE_DAY);

    for(uint32 i = BG_WS_OBJECT_A_FLAG; i <= BG_WS_OBJECT_BERSERKBUFF_2; i++)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
}

void BattleGroundWS::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundWGScore* sc = new BattleGroundWGScore;

    m_PlayerScores[plr->GetGUID()] = sc;
}

void BattleGroundWS::RespawnFlag(uint32 Team, bool captured)
{
    if(Team == ALLIANCE)
    {
        sLog.outDebug("Respawn Alliance flag");
        m_FlagState[BG_TEAM_ALLIANCE] = BG_WS_FLAG_STATE_ON_BASE;
    }
    else
    {
        sLog.outDebug("Respawn Horde flag");
        m_FlagState[BG_TEAM_HORDE] = BG_WS_FLAG_STATE_ON_BASE;
    }

    if(captured)
    {
        //when map_update will be allowed for battlegrounds this code will be useless
        SpawnBGObject(BG_WS_OBJECT_H_FLAG, RESPAWN_IMMEDIATELY);
        SpawnBGObject(BG_WS_OBJECT_A_FLAG, RESPAWN_IMMEDIATELY);
        SendMessageToAll(LANG_BG_WS_F_PLACED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        PlaySoundToAll(BG_WS_SOUND_FLAGS_RESPAWNED);        // flag respawned sound...
    }
}

void BattleGroundWS::RespawnFlagAfterDrop(uint32 team)
{
    if(GetStatus() != STATUS_IN_PROGRESS)
        return;

    RespawnFlag(team,false);
    if(team == ALLIANCE)
    {
        SpawnBGObject(BG_WS_OBJECT_A_FLAG, RESPAWN_IMMEDIATELY);
        SendMessageToAll(LANG_BG_WS_ALLIANCE_FLAG_RESPAWNED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }
    else
    {
        SpawnBGObject(BG_WS_OBJECT_H_FLAG, RESPAWN_IMMEDIATELY);
        SendMessageToAll(LANG_BG_WS_HORDE_FLAG_RESPAWNED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }

    PlaySoundToAll(BG_WS_SOUND_FLAGS_RESPAWNED);

    GameObject *obj = HashMapHolder<GameObject>::Find(GetDroppedFlagGUID(team));
    if(obj)
        obj->Delete();
    else
        sLog.outError("unknown droped flag bg, guid: %u",GUID_LOPART(GetDroppedFlagGUID(team)));

    SetDroppedFlagGUID(0,team);
}

void BattleGroundWS::EventPlayerCapturedFlag(Player *Source)
{
    if(GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint32 winner = 0;

    //TODO FIX reputation and honor gains for low level players!

    Source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
    if(Source->GetTeam() == ALLIANCE)
    {
        if (!IsHordeFlagPickedup())
            return;
        SetHordeFlagPicker(0);                              // must be before aura remove to prevent 2 events (drop+capture) at the same time
                                                            // horde flag in base (but not respawned yet)
        m_FlagState[BG_TEAM_HORDE] = BG_WS_FLAG_STATE_WAIT_RESPAWN;
                                                            // Drop Horde Flag from Player
        Source->RemoveAurasDueToSpell(BG_WS_SPELL_WARSONG_FLAG);
        if(GetTeamScore(ALLIANCE) < BG_WS_MAX_TEAM_SCORE)
            AddPoint(ALLIANCE, 1);
        PlaySoundToAll(BG_WS_SOUND_FLAG_CAPTURED_ALLIANCE);
        RewardReputationToTeam(890, 35, ALLIANCE);          // +35 reputation
        RewardHonorToTeam(40, ALLIANCE);                    // +40 bonushonor
    }
    else
    {
        if (!IsAllianceFlagPickedup())
            return;
        SetAllianceFlagPicker(0);                           // must be before aura remove to prevent 2 events (drop+capture) at the same time
                                                            // alliance flag in base (but not respawned yet)
        m_FlagState[BG_TEAM_ALLIANCE] = BG_WS_FLAG_STATE_WAIT_RESPAWN;
                                                            // Drop Alliance Flag from Player
        Source->RemoveAurasDueToSpell(BG_WS_SPELL_SILVERWING_FLAG);
        if(GetTeamScore(HORDE) < BG_WS_MAX_TEAM_SCORE)
            AddPoint(HORDE, 1);
        PlaySoundToAll(BG_WS_SOUND_FLAG_CAPTURED_HORDE);
        RewardReputationToTeam(889, 35, HORDE);             // +35 reputation
        RewardHonorToTeam(40, HORDE);                       // +40 bonushonor
    }

    SpawnBGObject(BG_WS_OBJECT_H_FLAG, BG_WS_FLAG_RESPAWN_TIME);
    SpawnBGObject(BG_WS_OBJECT_A_FLAG, BG_WS_FLAG_RESPAWN_TIME);

    if(Source->GetTeam() == ALLIANCE)
        SendMessageToAll(LANG_BG_WS_CAPTURED_HF, CHAT_MSG_BG_SYSTEM_ALLIANCE, Source);
    else
        SendMessageToAll(LANG_BG_WS_CAPTURED_AF, CHAT_MSG_BG_SYSTEM_HORDE, Source);

    UpdateFlagState(Source->GetTeam(), 1);                  // flag state none
    UpdateTeamScore(Source->GetTeam());
    // only flag capture should be updated
    UpdatePlayerScore(Source, SCORE_FLAG_CAPTURES, 1);      // +1 flag captures...

    if(GetTeamScore(ALLIANCE) == BG_WS_MAX_TEAM_SCORE)
        winner = ALLIANCE;

    if(GetTeamScore(HORDE) == BG_WS_MAX_TEAM_SCORE)
        winner = HORDE;

    if(winner)
    {
        UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, 0);
        UpdateWorldState(BG_WS_FLAG_UNK_HORDE, 0);
        UpdateWorldState(BG_WS_FLAG_STATE_ALLIANCE, 1);
        UpdateWorldState(BG_WS_FLAG_STATE_HORDE, 1);

        EndBattleGround(winner);
    }
    else
    {
        m_FlagsTimer[GetTeamIndexByTeamId(Source->GetTeam()) ? 0 : 1] = BG_WS_FLAG_RESPAWN_TIME;
    }
}

void BattleGroundWS::EventPlayerDroppedFlag(Player *Source)
{
    if(GetStatus() != STATUS_IN_PROGRESS)
    {
        // if not running, do not cast things at the dropper player (prevent spawning the "dropped" flag), neither send unnecessary messages
        // just take off the aura
        if(Source->GetTeam() == ALLIANCE)
        {
            if(!this->IsHordeFlagPickedup())
                return;
            if(GetHordeFlagPickerGUID() == Source->GetGUID())
            {
                SetHordeFlagPicker(0);
                Source->RemoveAurasDueToSpell(BG_WS_SPELL_WARSONG_FLAG);
            }
        }
        else
        {
            if(!this->IsAllianceFlagPickedup())
                return;
            if(GetAllianceFlagPickerGUID() == Source->GetGUID())
            {
                SetAllianceFlagPicker(0);
                Source->RemoveAurasDueToSpell(BG_WS_SPELL_SILVERWING_FLAG);
            }
        }
        return;
    }

    bool set = false;

    if(Source->GetTeam() == ALLIANCE)
    {
        if(!IsHordeFlagPickedup())
            return;
        if(GetHordeFlagPickerGUID() == Source->GetGUID())
        {
            SetHordeFlagPicker(0);
            Source->RemoveAurasDueToSpell(BG_WS_SPELL_WARSONG_FLAG);
            m_FlagState[BG_TEAM_HORDE] = BG_WS_FLAG_STATE_ON_GROUND;
            Source->CastSpell(Source, BG_WS_SPELL_WARSONG_FLAG_DROPPED, true);
            set = true;
        }
    }
    else
    {
        if(!IsAllianceFlagPickedup())
            return;
        if(GetAllianceFlagPickerGUID() == Source->GetGUID())
        {
            SetAllianceFlagPicker(0);
            Source->RemoveAurasDueToSpell(BG_WS_SPELL_SILVERWING_FLAG);
            m_FlagState[BG_TEAM_ALLIANCE] = BG_WS_FLAG_STATE_ON_GROUND;
            Source->CastSpell(Source, BG_WS_SPELL_SILVERWING_FLAG_DROPPED, true);
            set = true;
        }
    }

    if (set)
    {
        Source->CastSpell(Source, SPELL_RECENTLY_DROPPED_FLAG, true);
        UpdateFlagState(Source->GetTeam(), 1);

        if(Source->GetTeam() == ALLIANCE)
        {
            SendMessageToAll(LANG_BG_WS_DROPPED_HF, CHAT_MSG_BG_SYSTEM_HORDE, Source);
            UpdateWorldState(BG_WS_FLAG_UNK_HORDE, uint32(-1));
        }
        else
        {
            SendMessageToAll(LANG_BG_WS_DROPPED_AF, CHAT_MSG_BG_SYSTEM_ALLIANCE, Source);
            UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, uint32(-1));
        }

        m_FlagsDropTimer[GetTeamIndexByTeamId(Source->GetTeam()) ? 0 : 1] = BG_WS_FLAG_DROP_TIME;
    }
}

void BattleGroundWS::EventPlayerClickedOnFlag(Player *Source, GameObject* target_obj)
{
    if(GetStatus() != STATUS_IN_PROGRESS)
        return;

    int32 message_id = 0;
    ChatMsg type;

    //alliance flag picked up from base
    if(Source->GetTeam() == HORDE && GetFlagState(ALLIANCE) == BG_WS_FLAG_STATE_ON_BASE
        && m_BgObjects[BG_WS_OBJECT_A_FLAG] == target_obj->GetGUID())
    {
        message_id = LANG_BG_WS_PICKEDUP_AF;
        type = CHAT_MSG_BG_SYSTEM_HORDE;
        PlaySoundToAll(BG_WS_SOUND_ALLIANCE_FLAG_PICKED_UP);
        SpawnBGObject(BG_WS_OBJECT_A_FLAG, RESPAWN_ONE_DAY);
        SetAllianceFlagPicker(Source->GetGUID());
        m_FlagState[BG_TEAM_ALLIANCE] = BG_WS_FLAG_STATE_ON_PLAYER;
        //update world state to show correct flag carrier
        UpdateFlagState(HORDE, BG_WS_FLAG_STATE_ON_PLAYER);
        UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, 1);
        Source->CastSpell(Source, BG_WS_SPELL_SILVERWING_FLAG, true);
    }

    //horde flag picked up from base
    if (Source->GetTeam() == ALLIANCE && GetFlagState(HORDE) == BG_WS_FLAG_STATE_ON_BASE
        && m_BgObjects[BG_WS_OBJECT_H_FLAG] == target_obj->GetGUID())
    {
        message_id = LANG_BG_WS_PICKEDUP_HF;
        type = CHAT_MSG_BG_SYSTEM_ALLIANCE;
        PlaySoundToAll(BG_WS_SOUND_HORDE_FLAG_PICKED_UP);
        SpawnBGObject(BG_WS_OBJECT_H_FLAG, RESPAWN_ONE_DAY);
        SetHordeFlagPicker(Source->GetGUID());
        m_FlagState[BG_TEAM_HORDE] = BG_WS_FLAG_STATE_ON_PLAYER;
        //update world state to show correct flag carrier
        UpdateFlagState(ALLIANCE, BG_WS_FLAG_STATE_ON_PLAYER);
        UpdateWorldState(BG_WS_FLAG_UNK_HORDE, 1);
        Source->CastSpell(Source, BG_WS_SPELL_WARSONG_FLAG, true);
    }

    //Alliance flag on ground(not in base) (returned or picked up again from ground!)
    if(GetFlagState(ALLIANCE) == BG_WS_FLAG_STATE_ON_GROUND && Source->IsWithinDistInMap(target_obj, 10))
    {
        if(Source->GetTeam() == ALLIANCE)
        {
            message_id = LANG_BG_WS_RETURNED_AF;
            type = CHAT_MSG_BG_SYSTEM_ALLIANCE;
            UpdateFlagState(HORDE, BG_WS_FLAG_STATE_WAIT_RESPAWN);
            RespawnFlag(ALLIANCE, false);
            SpawnBGObject(BG_WS_OBJECT_A_FLAG, RESPAWN_IMMEDIATELY);
            PlaySoundToAll(BG_WS_SOUND_FLAG_RETURNED);
            UpdatePlayerScore(Source, SCORE_FLAG_RETURNS, 1);
        }
        else
        {
            message_id = LANG_BG_WS_PICKEDUP_AF;
            type = CHAT_MSG_BG_SYSTEM_HORDE;
            PlaySoundToAll(BG_WS_SOUND_ALLIANCE_FLAG_PICKED_UP);
            SpawnBGObject(BG_WS_OBJECT_A_FLAG, RESPAWN_ONE_DAY);
            SetAllianceFlagPicker(Source->GetGUID());
            Source->CastSpell(Source, BG_WS_SPELL_SILVERWING_FLAG, true);
            m_FlagState[BG_TEAM_ALLIANCE] = BG_WS_FLAG_STATE_ON_PLAYER;
            UpdateFlagState(HORDE, BG_WS_FLAG_STATE_ON_PLAYER);
            UpdateWorldState(BG_WS_FLAG_UNK_ALLIANCE, 1);
        }
        //called in HandleGameObjectUseOpcode:
        //target_obj->Delete();
    }

    //Horde flag on ground(not in base) (returned or picked up again)
    if(GetFlagState(HORDE) == BG_WS_FLAG_STATE_ON_GROUND && Source->IsWithinDistInMap(target_obj, 10))
    {
        if(Source->GetTeam() == HORDE)
        {
            message_id = LANG_BG_WS_RETURNED_HF;
            type = CHAT_MSG_BG_SYSTEM_HORDE;
            UpdateFlagState(ALLIANCE, BG_WS_FLAG_STATE_WAIT_RESPAWN);
            RespawnFlag(HORDE, false);
            SpawnBGObject(BG_WS_OBJECT_H_FLAG, RESPAWN_IMMEDIATELY);
            PlaySoundToAll(BG_WS_SOUND_FLAG_RETURNED);
            UpdatePlayerScore(Source, SCORE_FLAG_RETURNS, 1);
        }
        else
        {
            message_id = LANG_BG_WS_PICKEDUP_HF;
            type = CHAT_MSG_BG_SYSTEM_ALLIANCE;
            PlaySoundToAll(BG_WS_SOUND_HORDE_FLAG_PICKED_UP);
            SpawnBGObject(BG_WS_OBJECT_H_FLAG, RESPAWN_ONE_DAY);
            SetHordeFlagPicker(Source->GetGUID());
            Source->CastSpell(Source, BG_WS_SPELL_WARSONG_FLAG, true);
            m_FlagState[BG_TEAM_HORDE] = BG_WS_FLAG_STATE_ON_PLAYER;
            UpdateFlagState(ALLIANCE, BG_WS_FLAG_STATE_ON_PLAYER);
            UpdateWorldState(BG_WS_FLAG_UNK_HORDE, 1);
        }
        //called in HandleGameObjectUseOpcode:
        //target_obj->Delete();
    }

    if (!message_id)
        return;

    SendMessageToAll(message_id, type, Source);
    Source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
}

void BattleGroundWS::RemovePlayer(Player *plr, uint64 guid)
{
    // sometimes flag aura not removed :(
    if(IsAllianceFlagPickedup() && m_FlagKeepers[BG_TEAM_ALLIANCE] == guid)
    {
        if(!plr)
        {
            sLog.outError("BattleGroundWS: Removing offline player who has the FLAG!!");
            SetAllianceFlagPicker(0);
            RespawnFlag(ALLIANCE, false);
        }
        else
            EventPlayerDroppedFlag(plr);
    }
    if(IsHordeFlagPickedup() && m_FlagKeepers[BG_TEAM_HORDE] == guid)
    {
        if(!plr)
        {
            sLog.outError("BattleGroundWS: Removing offline player who has the FLAG!!");
            SetHordeFlagPicker(0);
            RespawnFlag(HORDE, false);
        }
        else
            EventPlayerDroppedFlag(plr);
    }
}

void BattleGroundWS::UpdateFlagState(uint32 team, uint32 value)
{
    if(team == ALLIANCE)
        UpdateWorldState(BG_WS_FLAG_STATE_ALLIANCE, value);
    else
        UpdateWorldState(BG_WS_FLAG_STATE_HORDE, value);
}

void BattleGroundWS::UpdateTeamScore(uint32 team)
{
    if(team == ALLIANCE)
        UpdateWorldState(BG_WS_FLAG_CAPTURES_ALLIANCE, GetTeamScore(team));
    else
        UpdateWorldState(BG_WS_FLAG_CAPTURES_HORDE, GetTeamScore(team));
}

void BattleGroundWS::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if(GetStatus() != STATUS_IN_PROGRESS)
        return;

    //uint32 SpellId = 0;
    //uint64 buff_guid = 0;
    switch(Trigger)
    {
        case 3686:                                          // Alliance elixir of speed spawn. Trigger not working, because located inside other areatrigger, can be replaced by IsWithinDist(object, dist) in BattleGround::Update().
            //buff_guid = m_BgObjects[BG_WS_OBJECT_SPEEDBUFF_1];
            break;
        case 3687:                                          // Horde elixir of speed spawn. Trigger not working, because located inside other areatrigger, can be replaced by IsWithinDist(object, dist) in BattleGround::Update().
            //buff_guid = m_BgObjects[BG_WS_OBJECT_SPEEDBUFF_2];
            break;
        case 3706:                                          // Alliance elixir of regeneration spawn
            //buff_guid = m_BgObjects[BG_WS_OBJECT_REGENBUFF_1];
            break;
        case 3708:                                          // Horde elixir of regeneration spawn
            //buff_guid = m_BgObjects[BG_WS_OBJECT_REGENBUFF_2];
            break;
        case 3707:                                          // Alliance elixir of berserk spawn
            //buff_guid = m_BgObjects[BG_WS_OBJECT_BERSERKBUFF_1];
            break;
        case 3709:                                          // Horde elixir of berserk spawn
            //buff_guid = m_BgObjects[BG_WS_OBJECT_BERSERKBUFF_2];
            break;
        case 3646:                                          // Alliance Flag spawn
            if(m_FlagState[BG_TEAM_HORDE] && !m_FlagState[BG_TEAM_ALLIANCE])
                if(GetHordeFlagPickerGUID() == Source->GetGUID())
                    EventPlayerCapturedFlag(Source);
            break;
        case 3647:                                          // Horde Flag spawn
            if(m_FlagState[BG_TEAM_ALLIANCE] && !m_FlagState[BG_TEAM_HORDE])
                if(GetAllianceFlagPickerGUID() == Source->GetGUID())
                    EventPlayerCapturedFlag(Source);
            break;
        case 3649:                                          // unk1
        case 3688:                                          // unk2
        case 4628:                                          // unk3
        case 4629:                                          // unk4
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }

    //if(buff_guid)
    //    HandleTriggerBuff(buff_guid,Source);
}

bool BattleGroundWS::SetupBattleGround()
{
    // flags
    if(    !AddObject(BG_WS_OBJECT_A_FLAG, BG_OBJECT_A_FLAG_WS_ENTRY, 1540.423f, 1481.325f, 351.8284f, 3.089233f, 0, 0, 0.9996573f, 0.02617699f, BG_WS_FLAG_RESPAWN_TIME/1000)
        || !AddObject(BG_WS_OBJECT_H_FLAG, BG_OBJECT_H_FLAG_WS_ENTRY, 916.0226f, 1434.405f, 345.413f, 0.01745329f, 0, 0, 0.008726535f, 0.9999619f, BG_WS_FLAG_RESPAWN_TIME/1000)
        // buffs
        || !AddObject(BG_WS_OBJECT_SPEEDBUFF_1, BG_OBJECTID_SPEEDBUFF_ENTRY, 1449.93f, 1470.71f, 342.6346f, -1.64061f, 0, 0, 0.7313537f, -0.6819983f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_WS_OBJECT_SPEEDBUFF_2, BG_OBJECTID_SPEEDBUFF_ENTRY, 1005.171f, 1447.946f, 335.9032f, 1.64061f, 0, 0, 0.7313537f, 0.6819984f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_WS_OBJECT_REGENBUFF_1, BG_OBJECTID_REGENBUFF_ENTRY, 1317.506f, 1550.851f, 313.2344f, -0.2617996f, 0, 0, 0.1305263f, -0.9914448f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_WS_OBJECT_REGENBUFF_2, BG_OBJECTID_REGENBUFF_ENTRY, 1110.451f, 1353.656f, 316.5181f, -0.6806787f, 0, 0, 0.333807f, -0.9426414f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_WS_OBJECT_BERSERKBUFF_1, BG_OBJECTID_BERSERKERBUFF_ENTRY, 1320.09f, 1378.79f, 314.7532f, 1.186824f, 0, 0, 0.5591929f, 0.8290376f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_WS_OBJECT_BERSERKBUFF_2, BG_OBJECTID_BERSERKERBUFF_ENTRY, 1139.688f, 1560.288f, 306.8432f, -2.443461f, 0, 0, 0.9396926f, -0.3420201f, BUFF_RESPAWN_TIME)
        // alliance gates
        || !AddObject(BG_WS_OBJECT_DOOR_A_1, BG_OBJECT_DOOR_A_1_WS_ENTRY, 1503.335f, 1493.466f, 352.1888f, 3.115414f, 0, 0, 0.9999143f, 0.01308903f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_A_2, BG_OBJECT_DOOR_A_2_WS_ENTRY, 1492.478f, 1457.912f, 342.9689f, 3.115414f, 0, 0, 0.9999143f, 0.01308903f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_A_3, BG_OBJECT_DOOR_A_3_WS_ENTRY, 1468.503f, 1494.357f, 351.8618f, 3.115414f, 0, 0, 0.9999143f, 0.01308903f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_A_4, BG_OBJECT_DOOR_A_4_WS_ENTRY, 1471.555f, 1458.778f, 362.6332f, 3.115414f, 0, 0, 0.9999143f, 0.01308903f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_A_5, BG_OBJECT_DOOR_A_5_WS_ENTRY, 1492.347f, 1458.34f, 342.3712f, -0.03490669f, 0, 0, 0.01745246f, -0.9998477f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_A_6, BG_OBJECT_DOOR_A_6_WS_ENTRY, 1503.466f, 1493.367f, 351.7352f, -0.03490669f, 0, 0, 0.01745246f, -0.9998477f, RESPAWN_IMMEDIATELY)
        // horde gates
        || !AddObject(BG_WS_OBJECT_DOOR_H_1, BG_OBJECT_DOOR_H_1_WS_ENTRY, 949.1663f, 1423.772f, 345.6241f, -0.5756807f, -0.01673368f, -0.004956111f, -0.2839723f, 0.9586737f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_H_2, BG_OBJECT_DOOR_H_2_WS_ENTRY, 953.0507f, 1459.842f, 340.6526f, -1.99662f, -0.1971825f, 0.1575096f, -0.8239487f, 0.5073641f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_H_3, BG_OBJECT_DOOR_H_3_WS_ENTRY, 949.9523f, 1422.751f, 344.9273f, 0.0f, 0, 0, 0, 1, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_WS_OBJECT_DOOR_H_4, BG_OBJECT_DOOR_H_4_WS_ENTRY, 950.7952f, 1459.583f, 342.1523f, 0.05235988f, 0, 0, 0.02617695f, 0.9996573f, RESPAWN_IMMEDIATELY)
        )
    {
        sLog.outErrorDb("BatteGroundWS: Failed to spawn some object BattleGround not created!");
        return false;
    }

    WorldSafeLocsEntry const *sg = sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_MAIN_ALLIANCE);
    if(!sg || !AddSpiritGuide(WS_SPIRIT_MAIN_ALLIANCE, sg->x, sg->y, sg->z, 3.124139f, ALLIANCE))
    {
        sLog.outErrorDb("BatteGroundWS: Failed to spawn Alliance spirit guide! BattleGround not created!");
        return false;
    }

    sg = sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_MAIN_HORDE);
    if(!sg || !AddSpiritGuide(WS_SPIRIT_MAIN_HORDE, sg->x, sg->y, sg->z, 3.193953f, HORDE))
    {
        sLog.outErrorDb("BatteGroundWS: Failed to spawn Horde spirit guide! BattleGround not created!");
        return false;
    }

    sLog.outDebug("BatteGroundWS: BG objects and spirit guides spawned");

    return true;
}

void BattleGroundWS::Reset()
{
    //call parent's class reset
    BattleGround::Reset();

    m_FlagKeepers[BG_TEAM_ALLIANCE]     = 0;
    m_FlagKeepers[BG_TEAM_HORDE]        = 0;
    m_DroppedFlagGUID[BG_TEAM_ALLIANCE] = 0;
    m_DroppedFlagGUID[BG_TEAM_HORDE]    = 0;
    m_FlagState[BG_TEAM_ALLIANCE]       = BG_WS_FLAG_STATE_ON_BASE;
    m_FlagState[BG_TEAM_HORDE]          = BG_WS_FLAG_STATE_ON_BASE;
    m_TeamScores[BG_TEAM_ALLIANCE]      = 0;
    m_TeamScores[BG_TEAM_HORDE]         = 0;

    /* Spirit nodes is static at this BG and then not required deleting at BG reset.
    if(m_BgCreatures[WS_SPIRIT_MAIN_ALLIANCE])
        DelCreature(WS_SPIRIT_MAIN_ALLIANCE);

    if(m_BgCreatures[WS_SPIRIT_MAIN_HORDE])
        DelCreature(WS_SPIRIT_MAIN_HORDE);
    */
}

void BattleGroundWS::HandleKillPlayer(Player *player, Player *killer)
{
    if(GetStatus() != STATUS_IN_PROGRESS)
        return;

    EventPlayerDroppedFlag(player);

    BattleGround::HandleKillPlayer(player, killer);
}

void BattleGroundWS::UpdatePlayerScore(Player *Source, uint32 type, uint32 value)
{

    std::map<uint64, BattleGroundScore*>::iterator itr = m_PlayerScores.find(Source->GetGUID());

    if(itr == m_PlayerScores.end())                         // player not found
        return;

    switch(type)
    {
        case SCORE_FLAG_CAPTURES:                           // flags captured
            ((BattleGroundWGScore*)itr->second)->FlagCaptures += value;
            break;
        case SCORE_FLAG_RETURNS:                            // flags returned
            ((BattleGroundWGScore*)itr->second)->FlagReturns += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(Source, type, value);
            break;
    }
}

WorldSafeLocsEntry const* BattleGroundWS::GetClosestGraveYard(Player* player)
{
	//if status in progress, it returns main graveyards with spiritguides
	//else it will return the graveyard in the flagroom - this is especially
	//good, if a player dies in preparation phase - then the player can't cheat
	//and teleport to the graveyard outside the flagroom
	//and start running around, while the doors are still closed
	if(player->GetTeam() == ALLIANCE)
	{
		if(GetStatus() == STATUS_IN_PROGRESS)
			return sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_MAIN_ALLIANCE);
		else
			return sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_FLAGROOM_ALLIANCE);
	}
	else
	{
		if(GetStatus() == STATUS_IN_PROGRESS)
			return sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_MAIN_HORDE);
		else
			return sWorldSafeLocsStore.LookupEntry(WS_GRAVEYARD_FLAGROOM_HORDE);
	}
}

void BattleGroundWS::FillInitialWorldStates(WorldPacket& data)
{
    data << uint32(BG_WS_FLAG_CAPTURES_ALLIANCE) << uint32(GetTeamScore(ALLIANCE));
    data << uint32(BG_WS_FLAG_CAPTURES_HORDE) << uint32(GetTeamScore(HORDE));

    if(m_FlagState[BG_TEAM_ALLIANCE] == BG_WS_FLAG_STATE_ON_GROUND)
        data << uint32(BG_WS_FLAG_UNK_ALLIANCE) << uint32(-1);
    else if (m_FlagState[BG_TEAM_ALLIANCE] == BG_WS_FLAG_STATE_ON_PLAYER)
        data << uint32(BG_WS_FLAG_UNK_ALLIANCE) << uint32(1);
    else
        data << uint32(BG_WS_FLAG_UNK_ALLIANCE) << uint32(0);

    if(m_FlagState[BG_TEAM_HORDE] == BG_WS_FLAG_STATE_ON_GROUND)
        data << uint32(BG_WS_FLAG_UNK_HORDE) << uint32(-1);
    else if (m_FlagState[BG_TEAM_HORDE] == BG_WS_FLAG_STATE_ON_PLAYER)
        data << uint32(BG_WS_FLAG_UNK_HORDE) << uint32(1);
    else
        data << uint32(BG_WS_FLAG_UNK_HORDE) << uint32(0);

    data << uint32(BG_WS_FLAG_CAPTURES_MAX) << uint32(BG_WS_MAX_TEAM_SCORE);

    if (m_FlagState[BG_TEAM_HORDE] == BG_WS_FLAG_STATE_ON_PLAYER)
        data << uint32(BG_WS_FLAG_STATE_HORDE) << uint32(2);
    else
        data << uint32(BG_WS_FLAG_STATE_HORDE) << uint32(1);

    if (m_FlagState[BG_TEAM_ALLIANCE] == BG_WS_FLAG_STATE_ON_PLAYER)
        data << uint32(BG_WS_FLAG_STATE_ALLIANCE) << uint32(2);
    else
        data << uint32(BG_WS_FLAG_STATE_ALLIANCE) << uint32(1);

}
