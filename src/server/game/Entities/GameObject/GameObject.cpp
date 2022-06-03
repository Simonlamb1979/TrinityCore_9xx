/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GameObject.h"
#include "ArtifactPackets.h"
#include "AzeriteItem.h"
#include "AzeritePackets.h"
#include "Battleground.h"
#include "BattlegroundPackets.h"
#include "CellImpl.h"
#include "CreatureAISelector.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "G3DPosition.hpp"
#include "GameEventSender.h"
#include "GameObjectAI.h"
#include "GameObjectModel.h"
#include "GameObjectPackets.h"
#include "GameTime.h"
#include "GossipDef.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Item.h"
#include "Log.h"
#include "LootMgr.h"
#include "Map.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "OutdoorPvPMgr.h"
#include "PhasingHandler.h"
#include "PoolMgr.h"
#include "QueryPackets.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "Transport.h"
#include "World.h"
#include <G3D/Box.h>
#include <G3D/CoordinateFrame.h>
#include <G3D/Quat.h>
#include <sstream>

void GameObjectTemplate::InitializeQueryData()
{
    for (uint8 loc = LOCALE_enUS; loc < TOTAL_LOCALES; ++loc)
        QueryData[loc] = BuildQueryData(static_cast<LocaleConstant>(loc));
}

WorldPacket GameObjectTemplate::BuildQueryData(LocaleConstant loc) const
{
    WorldPackets::Query::QueryGameObjectResponse queryTemp;

    queryTemp.GameObjectID = entry;

    queryTemp.Allow = true;
    WorldPackets::Query::GameObjectStats& stats = queryTemp.Stats;

    stats.Type = type;
    stats.DisplayID = displayId;

    stats.Name[0] = name;
    stats.IconName = IconName;
    stats.CastBarCaption = castBarCaption;
    stats.UnkString = unk1;

    if (loc != LOCALE_enUS)
        if (GameObjectLocale const* gameObjectLocale = sObjectMgr->GetGameObjectLocale(entry))
        {
            ObjectMgr::GetLocaleString(gameObjectLocale->Name, loc, stats.Name[0]);
            ObjectMgr::GetLocaleString(gameObjectLocale->CastBarCaption, loc, stats.CastBarCaption);
            ObjectMgr::GetLocaleString(gameObjectLocale->Unk1, loc, stats.UnkString);
        }

    stats.Size = size;

    if (std::vector<uint32> const* items = sObjectMgr->GetGameObjectQuestItemList(entry))
        for (int32 item : *items)
            stats.QuestItems.push_back(item);

    memcpy(stats.Data, raw.data, MAX_GAMEOBJECT_DATA * sizeof(int32));
    stats.ContentTuningId = ContentTuningId;

    queryTemp.Write();
    queryTemp.ShrinkToFit();
    return queryTemp.Move();
}

bool QuaternionData::isUnit() const
{
    return fabs(x * x + y * y + z * z + w * w - 1.0f) < 1e-5f;
}

void QuaternionData::toEulerAnglesZYX(float& Z, float& Y, float& X) const
{
    G3D::Matrix3(G3D::Quat(x, y, z, w)).toEulerAnglesZYX(Z, Y, X);
}

QuaternionData QuaternionData::fromEulerAnglesZYX(float Z, float Y, float X)
{
    G3D::Quat quat(G3D::Matrix3::fromEulerAnglesZYX(Z, Y, X));
    return QuaternionData(quat.x, quat.y, quat.z, quat.w);
}

GameObjectTypeBase::CustomCommand::~CustomCommand() = default;

namespace GameObjectType
{
//11 GAMEOBJECT_TYPE_TRANSPORT
class Transport : public GameObjectTypeBase, public TransportBase
{
public:
    static constexpr Milliseconds PositionUpdateInterval = 50ms;

    explicit Transport(GameObject& owner) : GameObjectTypeBase(owner), _animationInfo(sTransportMgr->GetTransportAnimInfo(owner.GetGOInfo()->entry)),
        _pathProgress(GameTime::GetGameTimeMS() % GetTransportPeriod()), _stateChangeTime(GameTime::GetGameTimeMS()), _stateChangeProgress(_pathProgress),
        _autoCycleBetweenStopFrames(false)
    {
        GameObjectTemplate const* goInfo = _owner.GetGOInfo();
        if (goInfo->transport.Timeto2ndfloor > 0)
        {
            _stopFrames.push_back(goInfo->transport.Timeto2ndfloor);
            if (goInfo->transport.Timeto3rdfloor > 0)
            {
                _stopFrames.push_back(goInfo->transport.Timeto3rdfloor);
                if (goInfo->transport.Timeto4thfloor > 0)
                {
                    _stopFrames.push_back(goInfo->transport.Timeto4thfloor);
                    if (goInfo->transport.Timeto5thfloor > 0)
                    {
                        _stopFrames.push_back(goInfo->transport.Timeto5thfloor);
                        if (goInfo->transport.Timeto6thfloor > 0)
                        {
                            _stopFrames.push_back(goInfo->transport.Timeto6thfloor);
                            if (goInfo->transport.Timeto7thfloor > 0)
                            {
                                _stopFrames.push_back(goInfo->transport.Timeto7thfloor);
                                if (goInfo->transport.Timeto8thfloor > 0)
                                {
                                    _stopFrames.push_back(goInfo->transport.Timeto8thfloor);
                                    if (goInfo->transport.Timeto9thfloor > 0)
                                    {
                                        _stopFrames.push_back(goInfo->transport.Timeto9thfloor);
                                        if (goInfo->transport.Timeto10thfloor > 0)
                                            _stopFrames.push_back(goInfo->transport.Timeto10thfloor);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        _positionUpdateTimer.Reset(PositionUpdateInterval);
    }

    void Update(uint32 diff) override
    {
        if (!_animationInfo)
            return;

        _positionUpdateTimer.Update(diff);
        if (!_positionUpdateTimer.Passed())
            return;

        _positionUpdateTimer.Reset(PositionUpdateInterval);

        uint32 now = GameTime::GetGameTimeMS();
        uint32 period = GetTransportPeriod();
        uint32 newProgress = 0;
        if (_stopFrames.empty())
            newProgress = now % period;
        else
        {
            int32 stopTargetTime = 0;
            if (_owner.GetGoState() == GO_STATE_TRANSPORT_ACTIVE)
                stopTargetTime = 0;
            else
                stopTargetTime = _stopFrames[_owner.GetGoState() - GO_STATE_TRANSPORT_STOPPED];

            if (now < uint32(*_owner.m_gameObjectData->Level))
            {
                int32 timeToStop = _owner.m_gameObjectData->Level - _stateChangeTime;
                float stopSourcePathPct = float(_stateChangeProgress) / float(period);
                float stopTargetPathPct = float(stopTargetTime) / float(period);
                float timeSinceStopProgressPct = float(now - _stateChangeTime) / float(timeToStop);

                float progressPct;
                if (!_owner.HasDynamicFlag(GO_DYNFLAG_LO_INVERTED_MOVEMENT))
                {
                    if (_owner.GetGoState() == GO_STATE_TRANSPORT_ACTIVE)
                        stopTargetPathPct = 1.0f;

                    float pathPctBetweenStops = stopTargetPathPct - stopSourcePathPct;
                    if (pathPctBetweenStops < 0.0f)
                        pathPctBetweenStops += 1.0f;

                    progressPct = pathPctBetweenStops * timeSinceStopProgressPct + stopSourcePathPct;
                    if (progressPct > 1.0f)
                        progressPct = progressPct - 1.0f;
                }
                else
                {
                    float pathPctBetweenStops = stopSourcePathPct - stopTargetPathPct;
                    if (pathPctBetweenStops < 0.0f)
                        pathPctBetweenStops += 1.0f;

                    progressPct = stopSourcePathPct - pathPctBetweenStops * timeSinceStopProgressPct;
                    if (progressPct < 0.0f)
                        progressPct += 1.0f;
                }

                newProgress = uint32(float(period) * progressPct) % period;
            }
            else
                newProgress = stopTargetTime;

            if (int32(newProgress) == stopTargetTime && newProgress != _pathProgress)
            {
                uint32 eventId = [&]()
                {
                    switch (_owner.GetGoState() - GO_STATE_TRANSPORT_ACTIVE)
                    {
                        case 0:
                            return _owner.GetGOInfo()->transport.Reached1stfloor;
                        case 1:
                            return _owner.GetGOInfo()->transport.Reached2ndfloor;
                        case 2:
                            return _owner.GetGOInfo()->transport.Reached3rdfloor;
                        case 3:
                            return _owner.GetGOInfo()->transport.Reached4thfloor;
                        case 4:
                            return _owner.GetGOInfo()->transport.Reached5thfloor;
                        case 5:
                            return _owner.GetGOInfo()->transport.Reached6thfloor;
                        case 6:
                            return _owner.GetGOInfo()->transport.Reached7thfloor;
                        case 7:
                            return _owner.GetGOInfo()->transport.Reached8thfloor;
                        case 8:
                            return _owner.GetGOInfo()->transport.Reached9thfloor;
                        case 9:
                            return _owner.GetGOInfo()->transport.Reached10thfloor;
                        default:
                            return 0u;
                    }
                }();
                if (eventId)
                    GameEvents::Trigger(eventId, &_owner, nullptr);

                if (_autoCycleBetweenStopFrames)
                {
                    GOState currentState = _owner.GetGoState();
                    GOState newState;
                    if (currentState == GO_STATE_TRANSPORT_ACTIVE)
                        newState = GO_STATE_TRANSPORT_STOPPED;
                    else if (currentState - GO_STATE_TRANSPORT_ACTIVE == int32(_stopFrames.size()))
                        newState = GOState(currentState - 1);
                    else if (_owner.HasDynamicFlag(GO_DYNFLAG_LO_INVERTED_MOVEMENT))
                        newState = GOState(currentState - 1);
                    else
                        newState = GOState(currentState + 1);

                    _owner.SetGoState(newState);
                }
            }
        }

        if (_pathProgress == newProgress)
            return;

        _pathProgress = newProgress;

        TransportAnimationEntry const* oldAnimation = _animationInfo->GetPrevAnimNode(newProgress);
        TransportAnimationEntry const* newAnimation = _animationInfo->GetNextAnimNode(newProgress);
        if (oldAnimation && newAnimation)
        {
            G3D::Matrix3 pathRotation = G3D::Quat(_owner.m_gameObjectData->ParentRotation->x, _owner.m_gameObjectData->ParentRotation->y,
                _owner.m_gameObjectData->ParentRotation->z, _owner.m_gameObjectData->ParentRotation->w).toRotationMatrix();

            G3D::Vector3 prev(oldAnimation->Pos.X, oldAnimation->Pos.Y, oldAnimation->Pos.Z);
            G3D::Vector3 next(newAnimation->Pos.X, newAnimation->Pos.Y, newAnimation->Pos.Z);

            float animProgress = float(newProgress - oldAnimation->TimeIndex) / float(newAnimation->TimeIndex - oldAnimation->TimeIndex);

            G3D::Vector3 dst = prev.lerp(next, animProgress) * pathRotation;

            dst += PositionToVector3(&_owner.GetStationaryPosition());

            _owner.GetMap()->GameObjectRelocation(&_owner, dst.x, dst.y, dst.z, _owner.GetOrientation());
        }

        TransportRotationEntry const* oldRotation = _animationInfo->GetPrevAnimRotation(newProgress);
        TransportRotationEntry const* newRotation = _animationInfo->GetNextAnimRotation(newProgress);
        if (oldRotation && newRotation)
        {
            G3D::Quat prev(oldRotation->Rot[0], oldRotation->Rot[1], oldRotation->Rot[2], oldRotation->Rot[3]);
            G3D::Quat next(newRotation->Rot[0], newRotation->Rot[1], newRotation->Rot[2], newRotation->Rot[3]);

            float animProgress = float(newProgress - oldRotation->TimeIndex) / float(newRotation->TimeIndex - oldRotation->TimeIndex);

            G3D::Quat rotation = prev.slerp(next, animProgress);

            _owner.SetLocalRotation(rotation.x, rotation.y, rotation.z, rotation.w);
            _owner.UpdateModelPosition();
        }

        // update progress marker for client
        _owner.SetPathProgressForClient(float(_pathProgress) / float(period));
    }

    void OnStateChanged(GOState oldState, GOState newState) override
    {
        ASSERT(newState >= GO_STATE_TRANSPORT_ACTIVE);

        // transports without stop frames just keep animating in state 24
        if (_stopFrames.empty())
        {
            if (newState != GO_STATE_TRANSPORT_ACTIVE)
                _owner.SetGoState(GO_STATE_TRANSPORT_ACTIVE);
            return;
        }

        int32 stopPathProgress = 0;

        if (newState != GO_STATE_TRANSPORT_ACTIVE)
        {
            ASSERT(newState < GOState(GO_STATE_TRANSPORT_STOPPED + MAX_GO_STATE_TRANSPORT_STOP_FRAMES));
            uint32 stopFrame = newState - GO_STATE_TRANSPORT_STOPPED;
            ASSERT(stopFrame < _stopFrames.size());
            stopPathProgress = _stopFrames[stopFrame];
        }

        _stateChangeTime = GameTime::GetGameTimeMS();
        _stateChangeProgress = _pathProgress;
        uint32 timeToStop = std::abs(int32(_pathProgress) - stopPathProgress);
        _owner.SetLevel(GameTime::GetGameTimeMS() + timeToStop);
        _owner.SetPathProgressForClient(float(_pathProgress) / float(GetTransportPeriod()));

        if (oldState == GO_STATE_ACTIVE || oldState == newState)
        {
            // initialization
            if (int32(_pathProgress) > stopPathProgress)
                _owner.SetDynamicFlag(GO_DYNFLAG_LO_INVERTED_MOVEMENT);
            else
                _owner.RemoveDynamicFlag(GO_DYNFLAG_LO_INVERTED_MOVEMENT);

             return;
        }

        int32 pauseTimesCount = _stopFrames.size();
        int32 newToOldStateDelta = newState - oldState;
        if (newToOldStateDelta < 0)
            newToOldStateDelta += pauseTimesCount + 1;

        int32 oldToNewStateDelta = oldState - newState;
        if (oldToNewStateDelta < 0)
            oldToNewStateDelta += pauseTimesCount + 1;

        if (oldToNewStateDelta < newToOldStateDelta)
            _owner.SetDynamicFlag(GO_DYNFLAG_LO_INVERTED_MOVEMENT);
        else
            _owner.RemoveDynamicFlag(GO_DYNFLAG_LO_INVERTED_MOVEMENT);
    }

    void OnRelocated() override
    {
        UpdatePassengerPositions();
    }

    void UpdatePassengerPositions()
    {
        for (WorldObject* passenger : _passengers)
        {
            float x, y, z, o;
            passenger->m_movementInfo.transport.pos.GetPosition(x, y, z, o);
            CalculatePassengerPosition(x, y, z, &o);
            UpdatePassengerPosition(_owner.GetMap(), passenger, x, y, z, o, true);
        }
    }

    uint32 GetTransportPeriod() const
    {
        if (_animationInfo)
            return _animationInfo->TotalTime;

        return 1;
    }

    std::vector<uint32> const* GetPauseTimes() const
    {
        return &_stopFrames;
    }

    ObjectGuid GetTransportGUID() const override { return _owner.GetGUID(); }

    float GetTransportOrientation() const override { return _owner.GetOrientation(); }

    void AddPassenger(WorldObject* passenger) override
    {
        if (!_owner.IsInWorld())
            return;

        if (_passengers.insert(passenger).second)
        {
            passenger->SetTransport(this);
            passenger->m_movementInfo.transport.guid = GetTransportGUID();
            TC_LOG_DEBUG("entities.transport", "Object %s boarded transport %s.", passenger->GetName().c_str(), _owner.GetName().c_str());
        }
    }

    TransportBase* RemovePassenger(WorldObject* passenger) override
    {
        if (_passengers.erase(passenger) > 0)
        {
            passenger->SetTransport(nullptr);
            passenger->m_movementInfo.transport.Reset();
            TC_LOG_DEBUG("entities.transport", "Object %s removed from transport %s.", passenger->GetName().c_str(), _owner.GetName().c_str());

            if (Player* plr = passenger->ToPlayer())
                plr->SetFallInformation(0, plr->GetPositionZ());
        }

        return this;
    }

    void CalculatePassengerPosition(float& x, float& y, float& z, float* o) const override
    {
        TransportBase::CalculatePassengerPosition(x, y, z, o, _owner.GetPositionX(), _owner.GetPositionY(), _owner.GetPositionZ(), _owner.GetOrientation());
    }

    void CalculatePassengerOffset(float& x, float& y, float& z, float* o) const override
    {
        TransportBase::CalculatePassengerOffset(x, y, z, o, _owner.GetPositionX(), _owner.GetPositionY(), _owner.GetPositionZ(), _owner.GetOrientation());
    }

    int32 GetMapIdForSpawning() const override
    {
        return _owner.GetGOInfo()->transport.SpawnMap;
    }

    void SetAutoCycleBetweenStopFrames(bool on)
    {
        _autoCycleBetweenStopFrames = on;
    }

private:
    TransportAnimation const* _animationInfo;
    uint32 _pathProgress;
    uint32 _stateChangeTime;
    uint32 _stateChangeProgress;
    std::vector<uint32> _stopFrames;
    bool _autoCycleBetweenStopFrames;
    TimeTracker _positionUpdateTimer;
    std::unordered_set<WorldObject*> _passengers;
};

SetTransportAutoCycleBetweenStopFrames::SetTransportAutoCycleBetweenStopFrames(bool on) : _on(on)
{
}

void SetTransportAutoCycleBetweenStopFrames::Execute(GameObjectTypeBase& type) const
{
    if (Transport* transport = dynamic_cast<Transport*>(&type))
        transport->SetAutoCycleBetweenStopFrames(_on);
}
}

GameObject::GameObject() : WorldObject(false), MapObject(),
    m_model(nullptr), m_goValue(), m_AI(nullptr), m_respawnCompatibilityMode(false), _animKitId(0), _worldEffectID(0)
{
    m_objectType |= TYPEMASK_GAMEOBJECT;
    m_objectTypeId = TYPEID_GAMEOBJECT;

    m_updateFlag.Stationary = true;
    m_updateFlag.Rotation = true;

    m_respawnTime = 0;
    m_respawnDelayTime = 300;
    m_despawnDelay = 0;
    m_despawnRespawnTime = 0s;
    m_restockTime = 0;
    m_lootState = GO_NOT_READY;
    m_spawnedByDefault = true;
    m_usetimes = 0;
    m_spellId = 0;
    m_cooldownTime = 0;
    m_prevGoState = GO_STATE_ACTIVE;
    m_goInfo = nullptr;
    m_goData = nullptr;
    m_packedRotation = 0;
    m_goTemplateAddon = nullptr;

    m_spawnId = UI64LIT(0);

    m_groupLootTimer = 0;
    m_lootGenerationTime = 0;

    ResetLootMode(); // restore default loot mode
    m_stationaryPosition.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
}

GameObject::~GameObject()
{
    delete m_AI;
    delete m_model;
}

void GameObject::AIM_Destroy()
{
    delete m_AI;
    m_AI = nullptr;
}

bool GameObject::AIM_Initialize()
{
    AIM_Destroy();

    m_AI = FactorySelector::SelectGameObjectAI(this);

    if (!m_AI)
        return false;

    m_AI->InitializeAI();
    return true;
}

std::string const& GameObject::GetAIName() const
{
    return sObjectMgr->GetGameObjectTemplate(GetEntry())->AIName;
}

void GameObject::CleanupsBeforeDelete(bool finalCleanup)
{
    WorldObject::CleanupsBeforeDelete(finalCleanup);

    RemoveFromOwner();
}

void GameObject::RemoveFromOwner()
{
    ObjectGuid ownerGUID = GetOwnerGUID();
    if (!ownerGUID)
        return;

    if (Unit* owner = ObjectAccessor::GetUnit(*this, ownerGUID))
    {
        owner->RemoveGameObject(this, false);
        ASSERT(!GetOwnerGUID());
        return;
    }

    // This happens when a mage portal is despawned after the caster changes map (for example using the portal)
    TC_LOG_DEBUG("misc", "Removed GameObject (%s Entry: %u SpellId: %u LinkedGO: %u) that just lost any reference to the owner (%s) GO list",
        GetGUID().ToString().c_str(), GetGOInfo()->entry, m_spellId, GetGOInfo()->GetLinkedGameObjectEntry(), ownerGUID.ToString().c_str());
    SetOwnerGUID(ObjectGuid::Empty);
}

void GameObject::AddToWorld()
{
    ///- Register the gameobject for guid lookup
    if (!IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnGameObjectCreate(this);

        GetMap()->GetObjectsStore().Insert<GameObject>(GetGUID(), this);
        if (m_spawnId)
            GetMap()->GetGameObjectBySpawnIdStore().insert(std::make_pair(m_spawnId, this));

        // The state can be changed after GameObject::Create but before GameObject::AddToWorld
        bool toggledState = GetGoType() == GAMEOBJECT_TYPE_CHEST ? getLootState() == GO_READY : (GetGoState() == GO_STATE_READY || IsTransport());
        if (m_model)
        {
            if (Transport* trans = ToTransport())
                trans->SetDelayedAddModelToMap();
            else
                GetMap()->InsertGameObjectModel(*m_model);
        }

        EnableCollision(toggledState);
        WorldObject::AddToWorld();
    }
}

void GameObject::RemoveFromWorld()
{
    ///- Remove the gameobject from the accessor
    if (IsInWorld())
    {
        if (m_zoneScript)
            m_zoneScript->OnGameObjectRemove(this);

        RemoveFromOwner();
        if (m_model)
            if (GetMap()->ContainsGameObjectModel(*m_model))
                GetMap()->RemoveGameObjectModel(*m_model);

        // If linked trap exists, despawn it
        if (GameObject* linkedTrap = GetLinkedTrap())
            linkedTrap->DespawnOrUnsummon();

        WorldObject::RemoveFromWorld();

        if (m_spawnId)
            Trinity::Containers::MultimapErasePair(GetMap()->GetGameObjectBySpawnIdStore(), m_spawnId, this);
        GetMap()->GetObjectsStore().Remove<GameObject>(GetGUID());
    }
}

bool GameObject::Create(uint32 entry, Map* map, Position const& pos, QuaternionData const& rotation, uint32 animProgress, GOState goState, uint32 artKit, bool dynamic, ObjectGuid::LowType spawnid)
{
    ASSERT(map);
    SetMap(map);

    Relocate(pos);
    m_stationaryPosition.Relocate(pos);
    if (!IsPositionValid())
    {
        TC_LOG_ERROR("misc", "Gameobject (Spawn id: " UI64FMTD " Entry: %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", GetSpawnId(), entry, pos.GetPositionX(), pos.GetPositionY());
        return false;
    }

    // Set if this object can handle dynamic spawns
    if (!dynamic)
        SetRespawnCompatibilityMode();

    UpdatePositionData();

    SetZoneScript();
    if (m_zoneScript)
    {
        entry = m_zoneScript->GetGameObjectEntry(m_spawnId, entry);
        if (!entry)
            return false;
    }

    GameObjectTemplate const* goInfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!goInfo)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (Spawn id: " UI64FMTD " Entry: %u) not created: non-existing entry in `gameobject_template`. Map: %u (X: %f Y: %f Z: %f)", GetSpawnId(), entry, map->GetId(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
        return false;
    }

    if (goInfo->type == GAMEOBJECT_TYPE_MAP_OBJ_TRANSPORT)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (Spawn id: " UI64FMTD " Entry: %u) not created: gameobject type GAMEOBJECT_TYPE_MAP_OBJ_TRANSPORT cannot be manually created.", GetSpawnId(), entry);
        return false;
    }

    ObjectGuid guid;
    if (goInfo->type != GAMEOBJECT_TYPE_TRANSPORT)
        guid = ObjectGuid::Create<HighGuid::GameObject>(map->GetId(), goInfo->entry, map->GenerateLowGuid<HighGuid::GameObject>());
    else
    {
        guid = ObjectGuid::Create<HighGuid::Transport>(map->GenerateLowGuid<HighGuid::Transport>());
        m_updateFlag.ServerTime = true;
    }

    Object::_Create(guid);

    m_goInfo = goInfo;
    m_goTemplateAddon = sObjectMgr->GetGameObjectTemplateAddon(entry);

    if (goInfo->type >= MAX_GAMEOBJECT_TYPE)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (%s Spawn id: " UI64FMTD " Entry: %u) not created: non-existing GO type '%u' in `gameobject_template`. It will crash client if created.", guid.ToString().c_str(), GetSpawnId(), entry, goInfo->type);
        return false;
    }

    SetLocalRotation(rotation.x, rotation.y, rotation.z, rotation.w);
    GameObjectAddon const* gameObjectAddon = sObjectMgr->GetGameObjectAddon(GetSpawnId());

    // For most of gameobjects is (0, 0, 0, 1) quaternion, there are only some transports with not standard rotation
    QuaternionData parentRotation;
    if (gameObjectAddon)
        parentRotation = gameObjectAddon->ParentRotation;

    SetParentRotation(parentRotation);

    SetObjectScale(goInfo->size);

    if (GameObjectOverride const* goOverride = GetGameObjectOverride())
    {
        SetFaction(goOverride->Faction);
        ReplaceAllFlags(GameObjectFlags(goOverride->Flags));
    }

    if (m_goTemplateAddon)
    {
        if (m_goTemplateAddon->WorldEffectID)
        {
            m_updateFlag.GameObject = true;
            SetWorldEffectID(m_goTemplateAddon->WorldEffectID);
        }

        if (m_goTemplateAddon->AIAnimKitID)
            _animKitId = m_goTemplateAddon->AIAnimKitID;
    }

    SetEntry(goInfo->entry);

    // set name for logs usage, doesn't affect anything ingame
    SetName(goInfo->name);

    SetDisplayId(goInfo->displayId);

    CreateModel();
    // GAMEOBJECT_BYTES_1, index at 0, 1, 2 and 3
    SetGoType(GameobjectTypes(goInfo->type));
    m_prevGoState = goState;
    SetGoState(goState);
    SetGoArtKit(artKit);

    SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::SpawnTrackingStateAnimID), sDB2Manager.GetEmptyAnimStateID());

    switch (goInfo->type)
    {
        case GAMEOBJECT_TYPE_FISHINGHOLE:
            SetGoAnimProgress(animProgress);
            m_goValue.FishingHole.MaxOpens = urand(GetGOInfo()->fishingHole.minRestock, GetGOInfo()->fishingHole.maxRestock);
            break;
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
        {
            // TODO: Get the values somehow, no longer in gameobject_template
            m_goValue.Building.Health = 20000/*goinfo->destructibleBuilding.intactNumHits + goinfo->destructibleBuilding.damagedNumHits*/;
            m_goValue.Building.MaxHealth = m_goValue.Building.Health;
            SetGoAnimProgress(255);
            // yes, even after the updatefield rewrite this garbage hack is still in client
            QuaternionData reinterpretId;
            memcpy(&reinterpretId.x, &m_goInfo->destructibleBuilding.DestructibleModelRec, sizeof(float));
            SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::ParentRotation), reinterpretId);
            break;
        }
        case GAMEOBJECT_TYPE_TRANSPORT:
        {
            m_goTypeImpl = std::make_unique<GameObjectType::Transport>(*this);
            if (goInfo->transport.startOpen)
                SetGoState(GO_STATE_TRANSPORT_STOPPED);
            else
                SetGoState(GO_STATE_TRANSPORT_ACTIVE);

            SetGoAnimProgress(animProgress);
            setActive(true);
            break;
        }
        case GAMEOBJECT_TYPE_FISHINGNODE:
            SetLevel(1);
            SetGoAnimProgress(255);
            break;
        case GAMEOBJECT_TYPE_TRAP:
            if (GetGOInfo()->trap.stealthed)
            {
                m_stealth.AddFlag(STEALTH_TRAP);
                m_stealth.AddValue(STEALTH_TRAP, 70);
            }

            if (GetGOInfo()->trap.stealthAffected)
            {
                m_invisibility.AddFlag(INVISIBILITY_TRAP);
                m_invisibility.AddValue(INVISIBILITY_TRAP, 300);
            }
            break;
        case GAMEOBJECT_TYPE_PHASEABLE_MO:
            RemoveFlag(GameObjectFlags(0xF00));
            SetFlag(GameObjectFlags((m_goInfo->phaseableMO.AreaNameSet & 0xF) << 8));
            break;
        case GAMEOBJECT_TYPE_CAPTURE_POINT:
            SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::SpellVisualID), m_goInfo->capturePoint.SpellVisual1);
            m_goValue.CapturePoint.AssaultTimer = 0;
            m_goValue.CapturePoint.LastTeamCapture = TEAM_NEUTRAL;
            m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::Neutral;
            UpdateCapturePoint();
            break;
        default:
            SetGoAnimProgress(animProgress);
            break;
    }

    if (gameObjectAddon)
    {
        if (gameObjectAddon->InvisibilityValue)
        {
            m_invisibility.AddFlag(gameObjectAddon->invisibilityType);
            m_invisibility.AddValue(gameObjectAddon->invisibilityType, gameObjectAddon->InvisibilityValue);
        }

        if (gameObjectAddon->WorldEffectID)
        {
            m_updateFlag.GameObject = true;
            SetWorldEffectID(gameObjectAddon->WorldEffectID);
        }

        if (gameObjectAddon->AIAnimKitID)
            _animKitId = gameObjectAddon->AIAnimKitID;
    }

    LastUsedScriptID = GetGOInfo()->ScriptId;
    AIM_Initialize();

    // Initialize loot duplicate count depending on raid difficulty
    if (map->Is25ManRaid())
        loot.maxDuplicates = 3;

    if (spawnid)
        m_spawnId = spawnid;

    if (uint32 linkedEntry = GetGOInfo()->GetLinkedGameObjectEntry())
    {
        if (GameObject* linkedGo = GameObject::CreateGameObject(linkedEntry, map, pos, rotation, 255, GO_STATE_READY))
        {
            SetLinkedTrap(linkedGo);
            if (!map->AddToMap(linkedGo))
                delete linkedGo;
        }
    }

    // Check if GameObject is Infinite
    if (goInfo->IsInfiniteGameObject())
        SetVisibilityDistanceOverride(VisibilityDistanceType::Infinite);

    // Check if GameObject is Gigantic
    if (goInfo->IsGiganticGameObject())
        SetVisibilityDistanceOverride(VisibilityDistanceType::Gigantic);

    // Check if GameObject is Large
    if (goInfo->IsLargeGameObject())
        SetVisibilityDistanceOverride(VisibilityDistanceType::Large);

    return true;
}

GameObject* GameObject::CreateGameObject(uint32 entry, Map* map, Position const& pos, QuaternionData const& rotation, uint32 animProgress, GOState goState, uint32 artKit /*= 0*/)
{
    GameObjectTemplate const* goInfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!goInfo)
        return nullptr;

    GameObject* go = new GameObject();
    if (!go->Create(entry, map, pos, rotation, animProgress, goState, artKit, false, 0))
    {
        delete go;
        return nullptr;
    }

    return go;
}

GameObject* GameObject::CreateGameObjectFromDB(ObjectGuid::LowType spawnId, Map* map, bool addToMap /*= true*/)
{
    GameObject* go = new GameObject();
    if (!go->LoadFromDB(spawnId, map, addToMap))
    {
        delete go;
        return nullptr;
    }

    return go;
}

void GameObject::Update(uint32 diff)
{
    m_Events.Update(diff);

    if (AI())
        AI()->UpdateAI(diff);
    else if (!AIM_Initialize())
        TC_LOG_ERROR("misc", "Could not initialize GameObjectAI");

    if (m_despawnDelay)
    {
        if (m_despawnDelay > diff)
            m_despawnDelay -= diff;
        else
        {
            m_despawnDelay = 0;
            DespawnOrUnsummon(0ms, m_despawnRespawnTime);
        }
    }

    if (m_goTypeImpl)
        m_goTypeImpl->Update(diff);

    switch (m_lootState)
    {
        case GO_NOT_READY:
        {
            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_TRAP:
                {
                    // Arming Time for GAMEOBJECT_TYPE_TRAP (6)
                    GameObjectTemplate const* goInfo = GetGOInfo();
                    // Bombs
                    if (goInfo->trap.charges == 2)
                        // Hardcoded tooltip value
                        m_cooldownTime = GameTime::GetGameTimeMS() + 10 * IN_MILLISECONDS;
                    else if (Unit* owner = GetOwner())
                        if (owner->IsInCombat())
                            m_cooldownTime = GameTime::GetGameTimeMS() + goInfo->trap.startDelay * IN_MILLISECONDS;

                    SetLootState(GO_READY);
                    break;
                }
                case GAMEOBJECT_TYPE_FISHINGNODE:
                {
                    // fishing code (bobber ready)
                    if (GameTime::GetGameTime() > m_respawnTime - FISHING_BOBBER_READY_TIME)
                    {
                        // splash bobber (bobber ready now)
                        Unit* caster = GetOwner();
                        if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                        {
                            SetGoState(GO_STATE_ACTIVE);
                            ReplaceAllFlags(GO_FLAG_NODESPAWN);

                            UpdateData udata(caster->GetMapId());
                            WorldPacket packet;
                            BuildValuesUpdateBlockForPlayer(&udata, caster->ToPlayer());
                            udata.BuildPacket(&packet);
                            caster->ToPlayer()->SendDirectMessage(&packet);

                            SendCustomAnim(GetGoAnimProgress());
                        }

                        m_lootState = GO_READY;                 // can be successfully open with some chance
                    }
                    return;
                }
                case GAMEOBJECT_TYPE_CHEST:
                    if (m_restockTime > GameTime::GetGameTime())
                        return;
                    // If there is no restock timer, or if the restock timer passed, the chest becomes ready to loot
                    m_restockTime = 0;
                    m_lootState = GO_READY;
                    AddToObjectUpdateIfNeeded();
                    break;
                default:
                    m_lootState = GO_READY;                         // for other GOis same switched without delay to GO_READY
                    break;
            }
            [[fallthrough]];
        }
        case GO_READY:
        {
            if (m_respawnCompatibilityMode)
            {
                if (m_respawnTime > 0)                          // timer on
                {
                    time_t now = GameTime::GetGameTime();
                    if (m_respawnTime <= now)            // timer expired
                    {
                        ObjectGuid dbtableHighGuid = ObjectGuid::Create<HighGuid::GameObject>(GetMapId(), GetEntry(), m_spawnId);
                        time_t linkedRespawntime = GetMap()->GetLinkedRespawnTime(dbtableHighGuid);
                        if (linkedRespawntime)             // Can't respawn, the master is dead
                        {
                            ObjectGuid targetGuid = sObjectMgr->GetLinkedRespawnGuid(dbtableHighGuid);
                            if (targetGuid == dbtableHighGuid) // if linking self, never respawn
                                SetRespawnTime(WEEK);
                            else
                                m_respawnTime = (now > linkedRespawntime ? now : linkedRespawntime) + urand(5, MINUTE); // else copy time from master and add a little
                            SaveRespawnTime();
                            return;
                        }

                        m_respawnTime = 0;
                        m_SkillupList.clear();
                        m_usetimes = 0;

                        switch (GetGoType())
                        {
                            case GAMEOBJECT_TYPE_FISHINGNODE:   //  can't fish now
                            {
                                Unit* caster = GetOwner();
                                if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                                {
                                    caster->ToPlayer()->RemoveGameObject(this, false);

                                    caster->ToPlayer()->SendDirectMessage(WorldPackets::GameObject::FishEscaped().Write());
                                }
                                // can be delete
                                m_lootState = GO_JUST_DEACTIVATED;
                                return;
                            }
                            case GAMEOBJECT_TYPE_DOOR:
                            case GAMEOBJECT_TYPE_BUTTON:
                                // We need to open doors if they are closed (add there another condition if this code breaks some usage, but it need to be here for battlegrounds)
                                if (GetGoState() != GO_STATE_READY)
                                    ResetDoorOrButton();
                                break;
                            case GAMEOBJECT_TYPE_FISHINGHOLE:
                                // Initialize a new max fish count on respawn
                                m_goValue.FishingHole.MaxOpens = urand(GetGOInfo()->fishingHole.minRestock, GetGOInfo()->fishingHole.maxRestock);
                                break;
                            default:
                                break;
                        }

                        // Despawn timer
                        if (!m_spawnedByDefault)
                        {
                            // Can be despawned or destroyed
                            SetLootState(GO_JUST_DEACTIVATED);
                            return;
                        }

                        // Call AI Reset (required for example in SmartAI to clear one time events)
                        if (AI())
                            AI()->Reset();

                        // Respawn timer
                        uint32 poolid = GetSpawnId() ? sPoolMgr->IsPartOfAPool<GameObject>(GetSpawnId()) : 0;
                        if (poolid)
                            sPoolMgr->UpdatePool<GameObject>(poolid, GetSpawnId());
                        else
                            GetMap()->AddToMap(this);
                    }
                }
            }

            // Set respawn timer
            if (!m_respawnCompatibilityMode && m_respawnTime > 0)
                SaveRespawnTime();

            if (isSpawned())
            {
                GameObjectTemplate const* goInfo = GetGOInfo();
                if (goInfo->type == GAMEOBJECT_TYPE_TRAP)
                {
                    if (GameTime::GetGameTimeMS() < m_cooldownTime)
                        break;

                    // Type 2 (bomb) does not need to be triggered by a unit and despawns after casting its spell.
                    if (goInfo->trap.charges == 2)
                    {
                        SetLootState(GO_ACTIVATED);
                        break;
                    }

                    // Type 0 despawns after being triggered, type 1 does not.
                    /// @todo This is activation radius. Casting radius must be selected from spell data.
                    float radius;
                    if (!goInfo->trap.radius)
                    {
                        // Battleground traps: data2 == 0 && data5 == 3
                        if (goInfo->trap.cooldown != 3)
                            break;

                        radius = 3.f;
                    }
                    else
                        radius = goInfo->trap.radius / 2.f;

                    // Pointer to appropriate target if found any
                    Unit* target = nullptr;

                    /// @todo this hack with search required until GO casting not implemented
                    if (GetOwner())
                    {
                        // Hunter trap: Search units which are unfriendly to the trap's owner
                        Trinity::NearestAttackableNoTotemUnitInObjectRangeCheck checker(this, radius);
                        Trinity::UnitLastSearcher<Trinity::NearestAttackableNoTotemUnitInObjectRangeCheck> searcher(this, target, checker);
                        Cell::VisitAllObjects(this, searcher, radius);
                    }
                    else
                    {
                        // Environmental trap: Any player
                        Player* player = nullptr;
                        Trinity::AnyPlayerInObjectRangeCheck checker(this, radius);
                        Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(this, player, checker);
                        Cell::VisitWorldObjects(this, searcher, radius);
                        target = player;
                    }

                    if (target)
                        SetLootState(GO_ACTIVATED, target);

                }
                else if (goInfo->type == GAMEOBJECT_TYPE_CAPTURE_POINT)
                {
                    bool hordeCapturing = m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::ContestedHorde;
                    bool allianceCapturing = m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::ContestedAlliance;
                    if (hordeCapturing || allianceCapturing)
                    {
                        if (m_goValue.CapturePoint.AssaultTimer <= diff)
                        {
                            m_goValue.CapturePoint.State = hordeCapturing ? WorldPackets::Battleground::BattlegroundCapturePointState::HordeCaptured : WorldPackets::Battleground::BattlegroundCapturePointState::AllianceCaptured;
                            if (hordeCapturing)
                            {
                                m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::HordeCaptured;
                                if (BattlegroundMap* map = GetMap()->ToBattlegroundMap())
                                {
                                    if (Battleground* bg = map->GetBG())
                                    {
                                        if (goInfo->capturePoint.CaptureEventHorde)
                                            GameEvents::Trigger(goInfo->capturePoint.CaptureEventHorde, this, this);
                                        bg->SendBroadcastText(goInfo->capturePoint.CaptureBroadcastHorde, CHAT_MSG_BG_SYSTEM_HORDE);
                                    }
                                }
                            }
                            else
                            {
                                m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::AllianceCaptured;
                                if (BattlegroundMap* map = GetMap()->ToBattlegroundMap())
                                {
                                    if (Battleground* bg = map->GetBG())
                                    {
                                        if (goInfo->capturePoint.CaptureEventAlliance)
                                            GameEvents::Trigger(goInfo->capturePoint.CaptureEventAlliance, this, this);
                                        bg->SendBroadcastText(goInfo->capturePoint.CaptureBroadcastAlliance, CHAT_MSG_BG_SYSTEM_ALLIANCE);
                                    }
                                }
                            }

                            m_goValue.CapturePoint.LastTeamCapture = hordeCapturing ? TEAM_HORDE : TEAM_ALLIANCE;
                            UpdateCapturePoint();
                        }
                        else
                            m_goValue.CapturePoint.AssaultTimer -= diff;
                    }
                }
                else if (uint32 max_charges = goInfo->GetCharges())
                {
                    if (m_usetimes >= max_charges)
                    {
                        m_usetimes = 0;
                        SetLootState(GO_JUST_DEACTIVATED);      // can be despawned or destroyed
                    }
                }
            }

            break;
        }
        case GO_ACTIVATED:
        {
            switch (GetGoType())
            {
                case GAMEOBJECT_TYPE_DOOR:
                case GAMEOBJECT_TYPE_BUTTON:
                    if (m_cooldownTime && GameTime::GetGameTimeMS() >= m_cooldownTime)
                        ResetDoorOrButton();
                    break;
                case GAMEOBJECT_TYPE_GOOBER:
                    if (GameTime::GetGameTimeMS() >= m_cooldownTime)
                    {
                        RemoveFlag(GO_FLAG_IN_USE);

                        SetLootState(GO_JUST_DEACTIVATED);
                        m_cooldownTime = 0;
                    }
                    break;
                case GAMEOBJECT_TYPE_CHEST:
                    if (m_groupLootTimer)
                    {
                        if (m_groupLootTimer <= diff)
                        {
                            if (Group* group = sGroupMgr->GetGroupByGUID(lootingGroupLowGUID))
                                group->EndRoll(&loot, GetMap());

                            m_groupLootTimer = 0;
                            lootingGroupLowGUID.Clear();
                        }
                        else
                            m_groupLootTimer -= diff;
                    }

                    // Non-consumable chest was partially looted and restock time passed, restock all loot now
                    if (GetGOInfo()->chest.consumable == 0 && GameTime::GetGameTime() >= m_restockTime)
                    {
                        m_restockTime = 0;
                        m_lootState = GO_READY;
                        AddToObjectUpdateIfNeeded();
                    }
                    break;
                case GAMEOBJECT_TYPE_TRAP:
                {
                    GameObjectTemplate const* goInfo = GetGOInfo();
                    if (goInfo->trap.charges == 2 && goInfo->trap.spell)
                    {
                        /// @todo nullptr target won't work for target type 1
                        CastSpell(nullptr, goInfo->trap.spell);
                        SetLootState(GO_JUST_DEACTIVATED);
                    }
                    else if (Unit* target = ObjectAccessor::GetUnit(*this, m_lootStateUnitGUID))
                    {
                        // Some traps do not have a spell but should be triggered
                        CastSpellExtraArgs args;
                        args.SetOriginalCaster(GetOwnerGUID());
                        if (goInfo->trap.spell)
                            CastSpell(target, goInfo->trap.spell, args);

                        // Template value or 4 seconds
                        m_cooldownTime = GameTime::GetGameTimeMS() + (goInfo->trap.cooldown ? goInfo->trap.cooldown : uint32(4)) * IN_MILLISECONDS;

                        if (goInfo->trap.charges == 1)
                            SetLootState(GO_JUST_DEACTIVATED);
                        else if (!goInfo->trap.charges)
                            SetLootState(GO_READY);

                        // Battleground gameobjects have data2 == 0 && data5 == 3
                        if (!goInfo->trap.radius && goInfo->trap.cooldown == 3)
                            if (Player* player = target->ToPlayer())
                                if (Battleground* bg = player->GetBattleground())
                                    bg->HandleTriggerBuff(GetGUID());
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case GO_JUST_DEACTIVATED:
        {
            // If nearby linked trap exists, despawn it
            if (GameObject* linkedTrap = GetLinkedTrap())
                linkedTrap->DespawnOrUnsummon();

            //if Gameobject should cast spell, then this, but some GOs (type = 10) should be destroyed
            if (GetGoType() == GAMEOBJECT_TYPE_GOOBER)
            {
                uint32 spellId = GetGOInfo()->goober.spell;

                if (spellId)
                {
                    for (GuidSet::const_iterator it = m_unique_users.begin(); it != m_unique_users.end(); ++it)
                        // m_unique_users can contain only player GUIDs
                        if (Player* owner = ObjectAccessor::GetPlayer(*this, *it))
                            owner->CastSpell(owner, spellId, false);

                    m_unique_users.clear();
                    m_usetimes = 0;
                }

                // Only goobers with a lock id or a reset time may reset their go state
                if (GetGOInfo()->GetLockId() || GetGOInfo()->GetAutoCloseTime())
                    SetGoState(GO_STATE_READY);

                //any return here in case battleground traps
                if (GameObjectOverride const* goOverride = GetGameObjectOverride())
                    if (goOverride->Flags & GO_FLAG_NODESPAWN)
                        return;
            }

            loot.clear();

            // Do not delete chests or goobers that are not consumed on loot, while still allowing them to despawn when they expire if summoned
            bool isSummonedAndExpired = (GetOwner() || GetSpellId()) && m_respawnTime == 0;
            if ((GetGoType() == GAMEOBJECT_TYPE_CHEST || GetGoType() == GAMEOBJECT_TYPE_GOOBER) && !GetGOInfo()->IsDespawnAtAction() && !isSummonedAndExpired)
            {
                if (GetGoType() == GAMEOBJECT_TYPE_CHEST && GetGOInfo()->chest.chestRestockTime > 0)
                {
                    // Start restock timer when the chest is fully looted
                    m_restockTime = GameTime::GetGameTime() + GetGOInfo()->chest.chestRestockTime;
                    SetLootState(GO_NOT_READY);
                    AddToObjectUpdateIfNeeded();
                }
                else
                    SetLootState(GO_READY);
                UpdateObjectVisibility();
                return;
            }
            else if (!GetOwnerGUID().IsEmpty() || GetSpellId())
            {
                SetRespawnTime(0);
                Delete();
                return;
            }

            SetLootState(GO_NOT_READY);

            //burning flags in some battlegrounds, if you find better condition, just add it
            if (GetGOInfo()->IsDespawnAtAction() || GetGoAnimProgress() > 0)
            {
                SendGameObjectDespawn();
                //reset flags
                if (GameObjectOverride const* goOverride = GetGameObjectOverride())
                    ReplaceAllFlags(GameObjectFlags(goOverride->Flags));
            }

            if (!m_respawnDelayTime)
                return;

            if (!m_spawnedByDefault)
            {
                m_respawnTime = 0;

                if (m_spawnId)
                    UpdateObjectVisibilityOnDestroy();
                else
                    Delete();

                return;
            }

            uint32 respawnDelay = m_respawnDelayTime;
            if (uint32 scalingMode = sWorld->getIntConfig(CONFIG_RESPAWN_DYNAMICMODE))
                GetMap()->ApplyDynamicModeRespawnScaling(this, this->m_spawnId, respawnDelay, scalingMode);
            m_respawnTime = GameTime::GetGameTime() + respawnDelay;

            // if option not set then object will be saved at grid unload
            // Otherwise just save respawn time to map object memory
            SaveRespawnTime();

            if (m_respawnCompatibilityMode)
                UpdateObjectVisibilityOnDestroy();
            else
                AddObjectToRemoveList();

            break;
        }
    }
}

GameObjectOverride const* GameObject::GetGameObjectOverride() const
{
    if (m_spawnId)
    {
        if (GameObjectOverride const* goOverride = sObjectMgr->GetGameObjectOverride(m_spawnId))
            return goOverride;
    }

    return m_goTemplateAddon;
}

void GameObject::Refresh()
{
    // Do not refresh despawned GO from spellcast (GO's from spellcast are destroyed after despawn)
    if (m_respawnTime > 0 && m_spawnedByDefault)
        return;

    if (isSpawned())
        GetMap()->AddToMap(this);
}

void GameObject::AddUniqueUse(Player* player)
{
    AddUse();
    m_unique_users.insert(player->GetGUID());
}

void GameObject::DespawnOrUnsummon(Milliseconds delay, Seconds forceRespawnTime)
{
    if (delay > 0ms)
    {
        if (!m_despawnDelay || m_despawnDelay > delay.count())
        {
            m_despawnDelay = delay.count();
            m_despawnRespawnTime = forceRespawnTime;
        }
    }
    else
    {
        if (m_goData)
        {
            uint32 const respawnDelay = (forceRespawnTime > 0s) ? forceRespawnTime.count() : m_goData->spawntimesecs;
            SaveRespawnTime(respawnDelay);
        }
        Delete();
    }
}

void GameObject::Delete()
{
    SetLootState(GO_NOT_READY);
    RemoveFromOwner();

    if (m_goInfo->type == GAMEOBJECT_TYPE_CAPTURE_POINT)
    {
        WorldPackets::Battleground::CapturePointRemoved packet(GetGUID());
        SendMessageToSet(packet.Write(), true);
    }

    SendGameObjectDespawn();

    SetGoState(GO_STATE_READY);

    if (GameObjectOverride const* goOverride = GetGameObjectOverride())
        ReplaceAllFlags(GameObjectFlags(goOverride->Flags));

    uint32 poolid = GetSpawnId() ? sPoolMgr->IsPartOfAPool<GameObject>(GetSpawnId()) : 0;
    if (poolid)
        sPoolMgr->UpdatePool<GameObject>(poolid, GetSpawnId());
    else
        AddObjectToRemoveList();
}

void GameObject::SendGameObjectDespawn()
{
    WorldPackets::GameObject::GameObjectDespawn packet;
    packet.ObjectGUID = GetGUID();
    SendMessageToSet(packet.Write(), true);
}

void GameObject::getFishLoot(Loot* fishloot, Player* loot_owner)
{
    fishloot->clear();

    uint32 zone, subzone;
    uint32 defaultzone = 1;
    GetZoneAndAreaId(zone, subzone);

    // if subzone loot exist use it
    fishloot->FillLoot(subzone, LootTemplates_Fishing, loot_owner, true, true);
    if (fishloot->empty())  //use this becase if zone or subzone has set LOOT_MODE_JUNK_FISH,Even if no normal drop, fishloot->FillLoot return true. it wrong.
    {
        //subzone no result,use zone loot
        fishloot->FillLoot(zone, LootTemplates_Fishing, loot_owner, true, true);
        //use zone 1 as default, somewhere fishing got nothing,becase subzone and zone not set, like Off the coast of Storm Peaks.
        if (fishloot->empty())
            fishloot->FillLoot(defaultzone, LootTemplates_Fishing, loot_owner, true, true);
    }
}

void GameObject::getFishLootJunk(Loot* fishloot, Player* loot_owner)
{
    fishloot->clear();

    uint32 zone, subzone;
    uint32 defaultzone = 1;
    GetZoneAndAreaId(zone, subzone);

    // if subzone loot exist use it
    fishloot->FillLoot(subzone, LootTemplates_Fishing, loot_owner, true, true, LOOT_MODE_JUNK_FISH);
    if (fishloot->empty())  //use this becase if zone or subzone has normal mask drop, then fishloot->FillLoot return true.
    {
        //use zone loot
        fishloot->FillLoot(zone, LootTemplates_Fishing, loot_owner, true, true, LOOT_MODE_JUNK_FISH);
        if (fishloot->empty())
            //use zone 1 as default
            fishloot->FillLoot(defaultzone, LootTemplates_Fishing, loot_owner, true, true, LOOT_MODE_JUNK_FISH);
    }
}

void GameObject::SaveToDB()
{
    // this should only be used when the gameobject has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    GameObjectData const* data = sObjectMgr->GetGameObjectData(m_spawnId);
    if (!data)
    {
        TC_LOG_ERROR("misc", "GameObject::SaveToDB failed, cannot get gameobject data!");
        return;
    }

    uint32 mapId = GetMapId();
    if (TransportBase* transport = GetTransport())
        if (transport->GetMapIdForSpawning() >= 0)
            mapId = transport->GetMapIdForSpawning();

    SaveToDB(mapId, data->spawnDifficulties);
}

void GameObject::SaveToDB(uint32 mapid, std::vector<Difficulty> const& spawnDifficulties)
{
    GameObjectTemplate const* goI = GetGOInfo();
    if (!goI)
        return;

    if (!m_spawnId)
        m_spawnId = sObjectMgr->GenerateGameObjectSpawnId();

    // update in loaded data (changing data only in this place)
    GameObjectData& data = sObjectMgr->NewOrExistGameObjectData(m_spawnId);

    if (!data.spawnId)
        data.spawnId = m_spawnId;
    ASSERT(data.spawnId == m_spawnId);
    data.id = GetEntry();
    data.mapId = GetMapId();
    data.spawnPoint.Relocate(this);
    data.rotation = m_localRotation;
    data.spawntimesecs = m_spawnedByDefault ? m_respawnDelayTime : -(int32)m_respawnDelayTime;
    data.animprogress = GetGoAnimProgress();
    data.goState = GetGoState();
    data.spawnDifficulties = spawnDifficulties;
    data.artKit = GetGoArtKit();
    if (!data.spawnGroupData)
        data.spawnGroupData = sObjectMgr->GetDefaultSpawnGroup();

    data.phaseId = GetDBPhase() > 0 ? GetDBPhase() : data.phaseId;
    data.phaseGroup = GetDBPhase() < 0 ? -GetDBPhase() : data.phaseGroup;

    // Update in DB
    WorldDatabaseTransaction trans = WorldDatabase.BeginTransaction();

    uint8 index = 0;

    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAMEOBJECT);
    stmt->setUInt64(0, m_spawnId);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_GAMEOBJECT);
    stmt->setUInt64(index++, m_spawnId);
    stmt->setUInt32(index++, GetEntry());
    stmt->setUInt16(index++, uint16(mapid));
    stmt->setString(index++, [&data]() -> std::string
    {
        if (data.spawnDifficulties.empty())
            return "";

        std::ostringstream os;
        auto itr = data.spawnDifficulties.begin();
        os << int32(*itr++);

        for (; itr != data.spawnDifficulties.end(); ++itr)
            os << ',' << int32(*itr);

        return os.str();
    }());
    stmt->setUInt32(index++, data.phaseId);
    stmt->setUInt32(index++, data.phaseGroup);
    stmt->setFloat(index++, GetPositionX());
    stmt->setFloat(index++, GetPositionY());
    stmt->setFloat(index++, GetPositionZ());
    stmt->setFloat(index++, GetOrientation());
    stmt->setFloat(index++, m_localRotation.x);
    stmt->setFloat(index++, m_localRotation.y);
    stmt->setFloat(index++, m_localRotation.z);
    stmt->setFloat(index++, m_localRotation.w);
    stmt->setInt32(index++, int32(m_respawnDelayTime));
    stmt->setUInt8(index++, GetGoAnimProgress());
    stmt->setUInt8(index++, uint8(GetGoState()));
    trans->Append(stmt);

    WorldDatabase.CommitTransaction(trans);
}

bool GameObject::LoadFromDB(ObjectGuid::LowType spawnId, Map* map, bool addToMap, bool)
{
    GameObjectData const* data = sObjectMgr->GetGameObjectData(spawnId);
    if (!data)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject (GUID: " UI64FMTD ") not found in table `gameobject`, can't load. ", spawnId);
        return false;
    }

    uint32 entry = data->id;
    //uint32 map_id = data->mapid;                          // already used before call

    uint32 animprogress = data->animprogress;
    GOState go_state = data->goState;
    uint32 artKit = data->artKit;

    m_spawnId = spawnId;
    m_respawnCompatibilityMode = ((data->spawnGroupData->flags & SPAWNGROUP_FLAG_COMPATIBILITY_MODE) != 0);
    if (!Create(entry, map, data->spawnPoint, data->rotation, animprogress, go_state, artKit, !m_respawnCompatibilityMode, spawnId))
        return false;

    PhasingHandler::InitDbPhaseShift(GetPhaseShift(), data->phaseUseFlags, data->phaseId, data->phaseGroup);
    PhasingHandler::InitDbVisibleMapId(GetPhaseShift(), data->terrainSwapMap);

    if (data->spawntimesecs >= 0)
    {
        m_spawnedByDefault = true;

        if (!GetGOInfo()->GetDespawnPossibility() && !GetGOInfo()->IsDespawnAtAction())
        {
            SetFlag(GO_FLAG_NODESPAWN);
            m_respawnDelayTime = 0;
            m_respawnTime = 0;
        }
        else
        {
            m_respawnDelayTime = data->spawntimesecs;
            m_respawnTime = GetMap()->GetGORespawnTime(m_spawnId);

            // ready to respawn
            if (m_respawnTime && m_respawnTime <= GameTime::GetGameTime())
            {
                m_respawnTime = 0;
                GetMap()->RemoveRespawnTime(SPAWN_TYPE_GAMEOBJECT, m_spawnId);
            }
        }
    }
    else
    {
        if (!m_respawnCompatibilityMode)
        {
            TC_LOG_WARN("sql.sql", "GameObject %u (SpawnID " UI64FMTD ") is not spawned by default, but tries to use a non-hack spawn system. This will not work. Defaulting to compatibility mode.", entry, spawnId);
            m_respawnCompatibilityMode = true;
        }

        m_spawnedByDefault = false;
        m_respawnDelayTime = -data->spawntimesecs;
        m_respawnTime = 0;
    }

    m_goData = data;

    if (addToMap && !GetMap()->AddToMap(this))
        return false;

    return true;
}

/*static*/ bool GameObject::DeleteFromDB(ObjectGuid::LowType spawnId)
{
    GameObjectData const* data = sObjectMgr->GetGameObjectData(spawnId);
    if (!data)
        return false;

    CharacterDatabaseTransaction charTrans = CharacterDatabase.BeginTransaction();

    sMapMgr->DoForAllMapsWithMapId(data->mapId,
        [spawnId, charTrans](Map* map) -> void
        {
            // despawn all active objects, and remove their respawns
            std::vector<GameObject*> toUnload;
            for (auto const& pair : Trinity::Containers::MapEqualRange(map->GetGameObjectBySpawnIdStore(), spawnId))
                toUnload.push_back(pair.second);
            for (GameObject* obj : toUnload)
                map->AddObjectToRemoveList(obj);
            map->RemoveRespawnTime(SPAWN_TYPE_GAMEOBJECT, spawnId, charTrans);
        }
    );

    // delete data from memory
    sObjectMgr->DeleteGameObjectData(spawnId);

    CharacterDatabase.CommitTransaction(charTrans);

    WorldDatabaseTransaction trans = WorldDatabase.BeginTransaction();

    // ... and the database
    WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAMEOBJECT);
    stmt->setUInt64(0, spawnId);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_SPAWNGROUP_MEMBER);
    stmt->setUInt8(0, uint8(SPAWN_TYPE_GAMEOBJECT));
    stmt->setUInt64(1, spawnId);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_EVENT_GAMEOBJECT);
    stmt->setUInt64(0, spawnId);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_LINKED_RESPAWN);
    stmt->setUInt64(0, spawnId);
    stmt->setUInt32(1, LINKED_RESPAWN_GO_TO_GO);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_LINKED_RESPAWN);
    stmt->setUInt64(0, spawnId);
    stmt->setUInt32(1, LINKED_RESPAWN_GO_TO_CREATURE);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_LINKED_RESPAWN_MASTER);
    stmt->setUInt64(0, spawnId);
    stmt->setUInt32(1, LINKED_RESPAWN_GO_TO_GO);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_LINKED_RESPAWN_MASTER);
    stmt->setUInt64(0, spawnId);
    stmt->setUInt32(1, LINKED_RESPAWN_CREATURE_TO_GO);
    trans->Append(stmt);

    stmt = WorldDatabase.GetPreparedStatement(WORLD_DEL_GAMEOBJECT_ADDON);
    stmt->setUInt64(0, spawnId);
    trans->Append(stmt);

    WorldDatabase.CommitTransaction(trans);

    return true;
}

/*********************************************************/
/***                    QUEST SYSTEM                   ***/
/*********************************************************/
bool GameObject::hasQuest(uint32 quest_id) const
{
    return sObjectMgr->GetGOQuestRelations(GetEntry()).HasQuest(quest_id);
}

bool GameObject::hasInvolvedQuest(uint32 quest_id) const
{
    return sObjectMgr->GetGOQuestInvolvedRelations(GetEntry()).HasQuest(quest_id);
}

bool GameObject::IsTransport() const
{
    // If something is marked as a transport, don't transmit an out of range packet for it.
    GameObjectTemplate const* gInfo = GetGOInfo();
    if (!gInfo)
        return false;

    return gInfo->type == GAMEOBJECT_TYPE_TRANSPORT || gInfo->type == GAMEOBJECT_TYPE_MAP_OBJ_TRANSPORT;
}

// is Dynamic transport = non-stop Transport
bool GameObject::IsDynTransport() const
{
    // If something is marked as a transport, don't transmit an out of range packet for it.
    GameObjectTemplate const* gInfo = GetGOInfo();
    if (!gInfo)
        return false;

    return gInfo->type == GAMEOBJECT_TYPE_MAP_OBJ_TRANSPORT || gInfo->type == GAMEOBJECT_TYPE_TRANSPORT;
}

bool GameObject::IsDestructibleBuilding() const
{
    GameObjectTemplate const* gInfo = GetGOInfo();
    if (!gInfo)
        return false;

    return gInfo->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING;
}

void GameObject::SaveRespawnTime(uint32 forceDelay)
{
    if (m_goData && (forceDelay || m_respawnTime > GameTime::GetGameTime()) && m_spawnedByDefault)
    {
        if (m_respawnCompatibilityMode)
        {
            RespawnInfo ri;
            ri.type = SPAWN_TYPE_GAMEOBJECT;
            ri.spawnId = m_spawnId;
            ri.respawnTime = m_respawnTime;
            GetMap()->SaveRespawnInfoDB(ri);
            return;
        }

        uint32 thisRespawnTime = forceDelay ? GameTime::GetGameTime() + forceDelay : m_respawnTime;
        GetMap()->SaveRespawnTime(SPAWN_TYPE_GAMEOBJECT, m_spawnId, GetEntry(), thisRespawnTime, Trinity::ComputeGridCoord(GetPositionX(), GetPositionY()).GetId());
    }
}

bool GameObject::IsNeverVisibleFor(WorldObject const* seer) const
{
    if (WorldObject::IsNeverVisibleFor(seer))
        return true;

    if (GetGoType() == GAMEOBJECT_TYPE_SPELL_FOCUS && GetGOInfo()->spellFocus.serverOnly == 1)
        return true;

    if (!GetDisplayId())
        return true;

    return false;
}

bool GameObject::IsAlwaysVisibleFor(WorldObject const* seer) const
{
    if (WorldObject::IsAlwaysVisibleFor(seer))
        return true;

    if (IsTransport() || IsDestructibleBuilding())
        return true;

    if (!seer)
        return false;

    // Always seen by owner and friendly units
    if (!GetOwnerGUID().IsEmpty())
    {
        if (seer->GetGUID() == GetOwnerGUID())
            return true;

        Unit* owner = GetOwner();
        if (Unit const* unitSeer = seer->ToUnit())
            if (owner && owner->IsFriendlyTo(unitSeer))
                return true;
    }

    return false;
}

bool GameObject::IsInvisibleDueToDespawn() const
{
    if (WorldObject::IsInvisibleDueToDespawn())
        return true;

    // Despawned
    if (!isSpawned())
        return true;

    return false;
}

uint8 GameObject::GetLevelForTarget(WorldObject const* target) const
{
    if (Unit* owner = GetOwner())
        return owner->GetLevelForTarget(target);

    if (GetGoType() == GAMEOBJECT_TYPE_TRAP)
    {
        if (Player const* player = target->ToPlayer())
            if (Optional<ContentTuningLevels> userLevels = sDB2Manager.GetContentTuningData(GetGOInfo()->ContentTuningId, player->m_playerData->CtrOptions->ContentTuningConditionMask))
                return uint8(std::clamp<int16>(player->GetLevel(), userLevels->MinLevel, userLevels->MaxLevel));

        if (Unit const* targetUnit = target->ToUnit())
            return targetUnit->GetLevel();
    }

    return 1;
}

time_t GameObject::GetRespawnTimeEx() const
{
    time_t now = GameTime::GetGameTime();
    if (m_respawnTime > now)
        return m_respawnTime;
    else
        return now;
}

void GameObject::SetRespawnTime(int32 respawn)
{
    m_respawnTime = respawn > 0 ? GameTime::GetGameTime() + respawn : 0;
    m_respawnDelayTime = respawn > 0 ? respawn : 0;
    if (respawn && !m_spawnedByDefault)
        UpdateObjectVisibility(true);
}

void GameObject::Respawn()
{
    if (m_spawnedByDefault && m_respawnTime > 0)
    {
        m_respawnTime = GameTime::GetGameTime();
        GetMap()->Respawn(SPAWN_TYPE_GAMEOBJECT, m_spawnId);
    }
}

bool GameObject::ActivateToQuest(Player const* target) const
{
    if (target->HasQuestForGO(GetEntry()))
        return true;

    if (!sObjectMgr->IsGameObjectForQuests(GetEntry()))
        return false;

    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_QUESTGIVER:
        {
            GameObject* go = const_cast<GameObject*>(this);
            QuestGiverStatus questStatus = const_cast<Player*>(target)->GetQuestDialogStatus(go);
            if (questStatus != QuestGiverStatus::None && questStatus != QuestGiverStatus::Future)
                return true;
            break;
        }
        case GAMEOBJECT_TYPE_CHEST:
        {
            // Chests become inactive while not ready to be looted
            if (getLootState() == GO_NOT_READY)
                return false;

            // scan GO chest with loot including quest items
            if (target->GetQuestStatus(GetGOInfo()->chest.questID) == QUEST_STATUS_INCOMPLETE || LootTemplates_Gameobject.HaveQuestLootForPlayer(GetGOInfo()->GetLootId(), target))
            {
                if (Battleground const* bg = target->GetBattleground())
                    return bg->CanActivateGO(GetEntry(), bg->GetPlayerTeam(target->GetGUID()));
                return true;
            }
            break;
        }
        case GAMEOBJECT_TYPE_GENERIC:
        {
            if (target->GetQuestStatus(GetGOInfo()->generic.questID) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        case GAMEOBJECT_TYPE_GOOBER:
        {
            if (target->GetQuestStatus(GetGOInfo()->goober.questID) == QUEST_STATUS_INCOMPLETE)
                return true;
            break;
        }
        default:
            break;
    }

    return false;
}

void GameObject::TriggeringLinkedGameObject(uint32 trapEntry, Unit* target)
{
    GameObjectTemplate const* trapInfo = sObjectMgr->GetGameObjectTemplate(trapEntry);
    if (!trapInfo || trapInfo->type != GAMEOBJECT_TYPE_TRAP)
        return;

    SpellInfo const* trapSpell = sSpellMgr->GetSpellInfo(trapInfo->trap.spell, GetMap()->GetDifficultyID());
    if (!trapSpell)                                          // checked at load already
        return;

    if (GameObject* trapGO = GetLinkedTrap())
        trapGO->CastSpell(target, trapSpell->Id);
}

GameObject* GameObject::LookupFishingHoleAround(float range)
{
    GameObject* ok = nullptr;
    Trinity::NearestGameObjectFishingHole u_check(*this, range);
    Trinity::GameObjectSearcher<Trinity::NearestGameObjectFishingHole> checker(this, ok, u_check);
    Cell::VisitGridObjects(this, checker, range);
    return ok;
}

void GameObject::ResetDoorOrButton()
{
    if (m_lootState == GO_READY || m_lootState == GO_JUST_DEACTIVATED)
        return;

    RemoveFlag(GO_FLAG_IN_USE);
    SetGoState(m_prevGoState);

    SetLootState(GO_JUST_DEACTIVATED);
    m_cooldownTime = 0;
}

void GameObject::UseDoorOrButton(uint32 time_to_restore, bool alternative /* = false */, Unit* user /*=nullptr*/)
{
    if (m_lootState != GO_READY)
        return;

    if (!time_to_restore)
        time_to_restore = GetGOInfo()->GetAutoCloseTime();

    SwitchDoorOrButton(true, alternative);
    SetLootState(GO_ACTIVATED, user);

    m_cooldownTime = time_to_restore ? (GameTime::GetGameTimeMS() + time_to_restore) : 0;
}

void GameObject::ActivateObject(GameObjectActions action, int32 param, WorldObject* spellCaster /*= nullptr*/, uint32 spellId /*= 0*/, int32 effectIndex /*= -1*/)
{
    Unit* unitCaster = spellCaster ? spellCaster->ToUnit() : nullptr;

    switch (action)
    {
        case GameObjectActions::None:
            TC_LOG_FATAL("spell", "Spell %d has action type NONE in effect %d", spellId, effectIndex);
            break;
        case GameObjectActions::AnimateCustom0:
        case GameObjectActions::AnimateCustom1:
        case GameObjectActions::AnimateCustom2:
        case GameObjectActions::AnimateCustom3:
            SendCustomAnim(uint32(action) - uint32(GameObjectActions::AnimateCustom0));
            break;
        case GameObjectActions::Disturb: // What's the difference with Open?
            if (unitCaster)
                Use(unitCaster);
            break;
        case GameObjectActions::Unlock:
            RemoveFlag(GO_FLAG_LOCKED);
            break;
        case GameObjectActions::Lock:
            SetFlag(GO_FLAG_LOCKED);
            break;
        case GameObjectActions::Open:
            if (unitCaster)
                Use(unitCaster);
            break;
        case GameObjectActions::OpenAndUnlock:
            if (unitCaster)
                UseDoorOrButton(0, false, unitCaster);
            RemoveFlag(GO_FLAG_LOCKED);
            break;
        case GameObjectActions::Close:
            ResetDoorOrButton();
            break;
        case GameObjectActions::ToggleOpen:
            // No use cases, implementation unknown
            break;
        case GameObjectActions::Destroy:
            if (unitCaster)
                UseDoorOrButton(0, true, unitCaster);
            break;
        case GameObjectActions::Rebuild:
            ResetDoorOrButton();
            break;
        case GameObjectActions::Creation:
            // No use cases, implementation unknown
            break;
        case GameObjectActions::Despawn:
            DespawnOrUnsummon();
            break;
        case GameObjectActions::MakeInert:
            SetFlag(GO_FLAG_NOT_SELECTABLE);
            break;
        case GameObjectActions::MakeActive:
            RemoveFlag(GO_FLAG_NOT_SELECTABLE);
            break;
        case GameObjectActions::CloseAndLock:
            ResetDoorOrButton();
            SetFlag(GO_FLAG_LOCKED);
            break;
        case GameObjectActions::UseArtKit0:
        case GameObjectActions::UseArtKit1:
        case GameObjectActions::UseArtKit2:
        case GameObjectActions::UseArtKit3:
        case GameObjectActions::UseArtKit4:
        {
            GameObjectTemplateAddon const* templateAddon = GetTemplateAddon();

            uint32 artKitIndex = action != GameObjectActions::UseArtKit4 ? uint32(action) - uint32(GameObjectActions::UseArtKit0) : 4;

            uint32 artKitValue = 0;
            if (templateAddon != nullptr)
                artKitValue = templateAddon->ArtKits[artKitIndex];

            if (artKitValue == 0)
                TC_LOG_ERROR("sql.sql", "GameObject %d hit by spell %d needs `artkit%d` in `gameobject_template_addon`", GetEntry(), spellId, artKitIndex);
            else
                SetGoArtKit(artKitValue);

            break;
        }
        case GameObjectActions::GoTo1stFloor:
        case GameObjectActions::GoTo2ndFloor:
        case GameObjectActions::GoTo3rdFloor:
        case GameObjectActions::GoTo4thFloor:
        case GameObjectActions::GoTo5thFloor:
        case GameObjectActions::GoTo6thFloor:
        case GameObjectActions::GoTo7thFloor:
        case GameObjectActions::GoTo8thFloor:
        case GameObjectActions::GoTo9thFloor:
        case GameObjectActions::GoTo10thFloor:
            static_assert(int32(GO_STATE_TRANSPORT_ACTIVE) == int32(GameObjectActions::GoTo1stFloor));
            if (GetGoType() == GAMEOBJECT_TYPE_TRANSPORT)
                SetGoState(GOState(action));
            else
                TC_LOG_ERROR("spell", "Spell %d targeted non-transport gameobject for transport only action \"Go to Floor\" %d in effect %d", spellId, int32(action), effectIndex);
            break;
        case GameObjectActions::PlayAnimKit:
            SetAnimKitId(param, false);
            break;
        case GameObjectActions::OpenAndPlayAnimKit:
            if (unitCaster)
                UseDoorOrButton(0, false, unitCaster);
            SetAnimKitId(param, false);
            break;
        case GameObjectActions::CloseAndPlayAnimKit:
            ResetDoorOrButton();
            SetAnimKitId(param, false);
            break;
        case GameObjectActions::PlayOneShotAnimKit:
            SetAnimKitId(param, true);
            break;
        case GameObjectActions::StopAnimKit:
            SetAnimKitId(0, false);
            break;
        case GameObjectActions::OpenAndStopAnimKit:
            if (unitCaster)
                UseDoorOrButton(0, false, unitCaster);
            SetAnimKitId(0, false);
            break;
        case GameObjectActions::CloseAndStopAnimKit:
            ResetDoorOrButton();
            SetAnimKitId(0, false);
            break;
        case GameObjectActions::PlaySpellVisual:
            SetSpellVisualId(param, Object::GetGUID(spellCaster));
            break;
        case GameObjectActions::StopSpellVisual:
            SetSpellVisualId(0);
            break;
        default:
            TC_LOG_ERROR("spell", "Spell %d has unhandled action %d in effect %d", spellId, int32(action), effectIndex);
            break;
    }
}

void GameObject::SetGoArtKit(uint32 kit)
{
    SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::ArtKit), kit);
    GameObjectData* data = const_cast<GameObjectData*>(sObjectMgr->GetGameObjectData(m_spawnId));
    if (data)
        data->artKit = kit;
}

void GameObject::SetGoArtKit(uint32 artkit, GameObject* go, ObjectGuid::LowType lowguid)
{
    GameObjectData const* data = nullptr;
    if (go)
    {
        go->SetGoArtKit(artkit);
        data = go->GetGameObjectData();
    }
    else if (lowguid)
        data = sObjectMgr->GetGameObjectData(lowguid);

    if (data)
        const_cast<GameObjectData*>(data)->artKit = artkit;
}

void GameObject::SwitchDoorOrButton(bool activate, bool alternative /* = false */)
{
    if (activate)
        SetFlag(GO_FLAG_IN_USE);
    else
        RemoveFlag(GO_FLAG_IN_USE);

    if (GetGoState() == GO_STATE_READY)                      //if closed -> open
        SetGoState(alternative ? GO_STATE_DESTROYED : GO_STATE_ACTIVE);
    else                                                    //if open -> close
        SetGoState(GO_STATE_READY);
}

void GameObject::Use(Unit* user)
{
    // by default spell caster is user
    Unit* spellCaster = user;
    uint32 spellId = 0;
    bool triggered = false;

    if (Player* playerUser = user->ToPlayer())
    {
        if (m_goInfo->GetNoDamageImmune() && playerUser->HasUnitFlag(UNIT_FLAG_IMMUNE))
            return;

        if (!m_goInfo->IsUsableMounted())
            playerUser->RemoveAurasByType(SPELL_AURA_MOUNTED);

        playerUser->PlayerTalkClass->ClearMenus();
        if (AI()->OnGossipHello(playerUser))
            return;
    }

    // If cooldown data present in template
    if (uint32 cooldown = GetGOInfo()->GetCooldown())
    {
        if (m_cooldownTime > GameTime::GetGameTime())
            return;

        m_cooldownTime = GameTime::GetGameTimeMS() + cooldown * IN_MILLISECONDS;
    }

    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_DOOR:                          //0
        case GAMEOBJECT_TYPE_BUTTON:                        //1
            //doors/buttons never really despawn, only reset to default state/flags
            UseDoorOrButton(0, false, user);
            return;
        case GAMEOBJECT_TYPE_QUESTGIVER:                    //2
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            player->PrepareGossipMenu(this, GetGOInfo()->questgiver.gossipID, true);
            player->SendPreparedGossip(this);
            return;
        }
        case GAMEOBJECT_TYPE_TRAP:                          //6
        {
            GameObjectTemplate const* goInfo = GetGOInfo();
            if (goInfo->trap.spell)
                CastSpell(user, goInfo->trap.spell);

            m_cooldownTime = GameTime::GetGameTimeMS() + (goInfo->trap.cooldown ? goInfo->trap.cooldown :  uint32(4)) * IN_MILLISECONDS;   // template or 4 seconds

            if (goInfo->trap.charges == 1)         // Deactivate after trigger
                SetLootState(GO_JUST_DEACTIVATED);

            return;
        }
        //Sitting: Wooden bench, chairs enzz
        case GAMEOBJECT_TYPE_CHAIR:                         //7
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            if (ChairListSlots.empty())        // this is called once at first chair use to make list of available slots
            {
                if (info->chair.chairslots > 0)     // sometimes chairs in DB have error in fields and we dont know number of slots
                    for (uint32 i = 0; i < info->chair.chairslots; ++i)
                        ChairListSlots[i].Clear(); // Last user of current slot set to 0 (none sit here yet)
                else
                    ChairListSlots[0].Clear();     // error in DB, make one default slot
            }

            Player* player = user->ToPlayer();

            // a chair may have n slots. we have to calculate their positions and teleport the player to the nearest one

            float lowestDist = DEFAULT_VISIBILITY_DISTANCE;

            uint32 nearest_slot = 0;
            float x_lowest = GetPositionX();
            float y_lowest = GetPositionY();

            // the object orientation + 1/2 pi
            // every slot will be on that straight line
            float orthogonalOrientation = GetOrientation() + float(M_PI) * 0.5f;
            // find nearest slot
            bool found_free_slot = false;
            for (ChairSlotAndUser::iterator itr = ChairListSlots.begin(); itr != ChairListSlots.end(); ++itr)
            {
                // the distance between this slot and the center of the go - imagine a 1D space
                float relativeDistance = (info->size*itr->first) - (info->size*(info->chair.chairslots - 1) / 2.0f);

                float x_i = GetPositionX() + relativeDistance * std::cos(orthogonalOrientation);
                float y_i = GetPositionY() + relativeDistance * std::sin(orthogonalOrientation);

                if (!itr->second.IsEmpty())
                {
                    if (Player* ChairUser = ObjectAccessor::GetPlayer(*this, itr->second))
                    {
                        if (ChairUser->IsSitState() && ChairUser->GetStandState() != UNIT_STAND_STATE_SIT && ChairUser->GetExactDist2d(x_i, y_i) < 0.1f)
                            continue;        // This seat is already occupied by ChairUser. NOTE: Not sure if the ChairUser->GetStandState() != UNIT_STAND_STATE_SIT check is required.
                        else
                            itr->second.Clear(); // This seat is unoccupied.
                    }
                    else
                        itr->second.Clear();     // The seat may of had an occupant, but they're offline.
                }

                found_free_slot = true;

                // calculate the distance between the player and this slot
                float thisDistance = player->GetDistance2d(x_i, y_i);

                if (thisDistance <= lowestDist)
                {
                    nearest_slot = itr->first;
                    lowestDist = thisDistance;
                    x_lowest = x_i;
                    y_lowest = y_i;
                }
            }

            if (found_free_slot)
            {
                ChairSlotAndUser::iterator itr = ChairListSlots.find(nearest_slot);
                if (itr != ChairListSlots.end())
                {
                    itr->second = player->GetGUID(); //this slot in now used by player
                    player->TeleportTo(GetMapId(), x_lowest, y_lowest, GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
                    player->SetStandState(UnitStandStateType(UNIT_STAND_STATE_SIT_LOW_CHAIR + info->chair.chairheight));
                    if (info->chair.triggeredEvent)
                        GameEvents::Trigger(info->chair.triggeredEvent, player, this);
                    return;
                }
            }

            return;
        }
        //big gun, its a spell/aura
        case GAMEOBJECT_TYPE_GOOBER:                        //10
        {
            GameObjectTemplate const* info = GetGOInfo();

            if (Player* player = user->ToPlayer())
            {
                if (info->goober.pageID)                    // show page...
                {
                    WorldPackets::GameObject::PageText data;
                    data.GameObjectGUID = GetGUID();
                    player->SendDirectMessage(data.Write());
                }
                else if (info->goober.gossipID)
                {
                    player->PrepareGossipMenu(this, info->goober.gossipID);
                    player->SendPreparedGossip(this);
                }

                if (info->goober.eventID)
                {
                    TC_LOG_DEBUG("maps.script", "Goober ScriptStart id %u for GO entry %u (GUID " UI64FMTD ").", info->goober.eventID, GetEntry(), GetSpawnId());
                    GameEvents::Trigger(info->goober.eventID, player, this);
                }

                // possible quest objective for active quests
                if (info->goober.questID && sObjectMgr->GetQuestTemplate(info->goober.questID))
                {
                    //Quest require to be active for GO using
                    if (player->GetQuestStatus(info->goober.questID) != QUEST_STATUS_INCOMPLETE)
                        break;
                }

                if (Group* group = player->GetGroup())
                {
                    for (GroupReference const* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
                        if (Player* member = itr->GetSource())
                            if (member->IsAtGroupRewardDistance(this))
                                member->KillCreditGO(info->entry, GetGUID());
                }
                else
                    player->KillCreditGO(info->entry, GetGUID());
            }

            if (uint32 trapEntry = info->goober.linkedTrap)
                TriggeringLinkedGameObject(trapEntry, user);

            SetFlag(GO_FLAG_IN_USE);
            SetLootState(GO_ACTIVATED, user);

            // this appear to be ok, however others exist in addition to this that should have custom (ex: 190510, 188692, 187389)
            if (info->goober.customAnim)
                SendCustomAnim(GetGoAnimProgress());
            else
                SetGoState(GO_STATE_ACTIVE);

            m_cooldownTime = GameTime::GetGameTimeMS() + info->GetAutoCloseTime();

            // cast this spell later if provided
            spellId = info->goober.spell;
            spellCaster = nullptr;

            break;
        }
        case GAMEOBJECT_TYPE_CAMERA:                        //13
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            if (info->camera.camera)
                player->SendCinematicStart(info->camera.camera);

            if (info->camera.eventID)
                GameEvents::Trigger(info->camera.eventID, player, this);

            return;
        }
        //fishing bobber
        case GAMEOBJECT_TYPE_FISHINGNODE:                   //17
        {
            Player* player = user->ToPlayer();
            if (!player)
                return;

            if (player->GetGUID() != GetOwnerGUID())
                return;

            switch (getLootState())
            {
                case GO_READY:                              // ready for loot
                {
                    uint32 zone, subzone;
                    GetZoneAndAreaId(zone, subzone);

                    int32 zone_skill = sObjectMgr->GetFishingBaseSkillLevel(subzone);
                    if (!zone_skill)
                        zone_skill = sObjectMgr->GetFishingBaseSkillLevel(zone);

                    //provide error, no fishable zone or area should be 0
                    if (!zone_skill)
                        TC_LOG_ERROR("sql.sql", "Fishable areaId %u are not properly defined in `skill_fishing_base_level`.", subzone);

                    int32 skill = player->GetSkillValue(SKILL_FISHING);

                    int32 chance;
                    if (skill < zone_skill)
                    {
                        chance = int32(pow((double)skill/zone_skill, 2) * 100);
                        if (chance < 1)
                            chance = 1;
                    }
                    else
                        chance = 100;

                    int32 roll = irand(1, 100);

                    TC_LOG_DEBUG("misc", "Fishing check (skill: %i zone min skill: %i chance %i roll: %i", skill, zone_skill, chance, roll);

                    player->UpdateFishingSkill();

                    /// @todo find reasonable value for fishing hole search
                    GameObject* fishingPool = LookupFishingHoleAround(20.0f + CONTACT_DISTANCE);

                    // If fishing skill is high enough, or if fishing on a pool, send correct loot.
                    // Fishing pools have no skill requirement as of patch 3.3.0 (undocumented change).
                    if (chance >= roll || fishingPool)
                    {
                        /// @todo I do not understand this hack. Need some explanation.
                        // prevent removing GO at spell cancel
                        RemoveFromOwner();
                        SetOwnerGUID(player->GetGUID());
                        SetSpellId(0); // prevent removing unintended auras at Unit::RemoveGameObject

                        if (fishingPool)
                        {
                            fishingPool->Use(player);
                            SetLootState(GO_JUST_DEACTIVATED);
                        }
                        else
                            player->SendLoot(GetGUID(), LOOT_FISHING);
                    }
                    else // If fishing skill is too low, send junk loot.
                        player->SendLoot(GetGUID(), LOOT_FISHING_JUNK);
                    break;
                }
                case GO_JUST_DEACTIVATED:                   // nothing to do, will be deleted at next update
                    break;
                default:
                {
                    SetLootState(GO_JUST_DEACTIVATED);
                    player->SendDirectMessage(WorldPackets::GameObject::FishNotHooked().Write());
                    break;
                }
            }

            player->FinishSpell(CURRENT_CHANNELED_SPELL);
            return;
        }

        case GAMEOBJECT_TYPE_RITUAL:              //18
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            Unit* owner = GetOwner();

            GameObjectTemplate const* info = GetGOInfo();

            Player* m_ritualOwner = nullptr;
            if (!m_ritualOwnerGUID.IsEmpty())
                m_ritualOwner = ObjectAccessor::FindPlayer(m_ritualOwnerGUID);

            // ritual owner is set for GO's without owner (not summoned)
            if (!m_ritualOwner && !owner)
            {
                m_ritualOwnerGUID = player->GetGUID();
                m_ritualOwner = player;
            }

            if (owner)
            {
                if (owner->GetTypeId() != TYPEID_PLAYER)
                    return;

                // accept only use by player from same group as owner, excluding owner itself (unique use already added in spell effect)
                if (player == owner->ToPlayer() || (info->ritual.castersGrouped && !player->IsInSameRaidWith(owner->ToPlayer())))
                    return;

                // expect owner to already be channeling, so if not...
                if (!owner->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    return;

                // in case summoning ritual caster is GO creator
                spellCaster = owner;
            }
            else
            {
                if (player != m_ritualOwner && (info->ritual.castersGrouped && !player->IsInSameRaidWith(m_ritualOwner)))
                    return;

                spellCaster = player;
            }

            AddUniqueUse(player);

            if (info->ritual.animSpell)
            {
                player->CastSpell(player, info->ritual.animSpell, true);

                // for this case, summoningRitual.spellId is always triggered
                triggered = true;
            }

            // full amount unique participants including original summoner
            if (GetUniqueUseCount() == info->ritual.casters)
            {
                if (m_ritualOwner)
                    spellCaster = m_ritualOwner;

                spellId = info->ritual.spell;

                if (spellId == 62330)                       // GO store nonexistent spell, replace by expected
                {
                    // spell have reagent and mana cost but it not expected use its
                    // it triggered spell in fact cast at currently channeled GO
                    spellId = 61993;
                    triggered = true;
                }

                // Cast casterTargetSpell at a random GO user
                // on the current DB there is only one gameobject that uses this (Ritual of Doom)
                // and its required target number is 1 (outter for loop will run once)
                if (info->ritual.casterTargetSpell && info->ritual.casterTargetSpell != 1) // No idea why this field is a bool in some cases
                    for (uint32 i = 0; i < info->ritual.casterTargetSpellTargets; i++)
                        // m_unique_users can contain only player GUIDs
                        if (Player* target = ObjectAccessor::GetPlayer(*this, Trinity::Containers::SelectRandomContainerElement(m_unique_users)))
                            spellCaster->CastSpell(target, info->ritual.casterTargetSpell, true);

                // finish owners spell
                if (owner)
                    owner->FinishSpell(CURRENT_CHANNELED_SPELL);

                // can be deleted now, if
                if (!info->ritual.ritualPersistent)
                    SetLootState(GO_JUST_DEACTIVATED);
                else
                {
                    // reset ritual for this GO
                    m_ritualOwnerGUID.Clear();
                    m_unique_users.clear();
                    m_usetimes = 0;
                }
            }
            else
                return;

            // go to end function to spell casting
            break;
        }
        case GAMEOBJECT_TYPE_SPELLCASTER:                   //22
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (info->spellCaster.partyOnly)
            {
                Unit* caster = GetOwner();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (user->GetTypeId() != TYPEID_PLAYER || !user->ToPlayer()->IsInSameRaidWith(caster->ToPlayer()))
                    return;
            }

            user->RemoveAurasByType(SPELL_AURA_MOUNTED);
            spellId = info->spellCaster.spell;

            AddUse();
            break;
        }
        case GAMEOBJECT_TYPE_MEETINGSTONE:                  //23
        {
            GameObjectTemplate const* info = GetGOInfo();

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            Player* targetPlayer = ObjectAccessor::FindPlayer(player->GetTarget());

            // accept only use by player from same raid as caster, except caster itself
            if (!targetPlayer || targetPlayer == player || !targetPlayer->IsInSameRaidWith(player))
                return;

            //required lvl checks!
            if (Optional<ContentTuningLevels> userLevels = sDB2Manager.GetContentTuningData(info->ContentTuningId, player->m_playerData->CtrOptions->ContentTuningConditionMask))
                if (player->GetLevel() < userLevels->MaxLevel)
                    return;

            if (Optional<ContentTuningLevels> targetLevels = sDB2Manager.GetContentTuningData(info->ContentTuningId, targetPlayer->m_playerData->CtrOptions->ContentTuningConditionMask))
                if (targetPlayer->GetLevel() < targetLevels->MaxLevel)
                    return;

            if (info->entry == 194097)
                spellId = 61994;                            // Ritual of Summoning
            else
                spellId = 59782;                            // Summoning Stone Effect

            break;
        }

        case GAMEOBJECT_TYPE_FLAGSTAND:                     // 24
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            if (player->CanUseBattlegroundObject(this))
            {
                // in battleground check
                Battleground* bg = player->GetBattleground();
                if (!bg)
                    return;

                if (player->GetVehicle())
                    return;

                player->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                player->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
                // BG flag click
                // AB:
                // 15001
                // 15002
                // 15003
                // 15004
                // 15005
                bg->EventPlayerClickedOnFlag(player, this);
                return;                                     //we don;t need to delete flag ... it is despawned!
            }
            break;
        }

        case GAMEOBJECT_TYPE_FISHINGHOLE:                   // 25
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            player->SendLoot(GetGUID(), LOOT_FISHINGHOLE);
            player->UpdateCriteria(CriteriaType::CatchFishInFishingHole, GetGOInfo()->entry);
            return;
        }

        case GAMEOBJECT_TYPE_FLAGDROP:                      // 26
        {
            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            if (player->CanUseBattlegroundObject(this))
            {
                // in battleground check
                Battleground* bg = player->GetBattleground();
                if (!bg)
                    return;

                if (player->GetVehicle())
                    return;

                player->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                player->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
                // BG flag dropped
                // WS:
                // 179785 - Silverwing Flag
                // 179786 - Warsong Flag
                // EotS:
                // 184142 - Netherstorm Flag
                GameObjectTemplate const* info = GetGOInfo();
                if (info)
                {
                    switch (info->entry)
                    {
                        case 179785:                        // Silverwing Flag
                        case 179786:                        // Warsong Flag
                            if (bg->GetTypeID(true) == BATTLEGROUND_WS)
                                bg->EventPlayerClickedOnFlag(player, this);
                            break;
                        case 184142:                        // Netherstorm Flag
                            if (bg->GetTypeID(true) == BATTLEGROUND_EY)
                                bg->EventPlayerClickedOnFlag(player, this);
                            break;
                    }

                    if (info->flagDrop.eventID)
                        GameEvents::Trigger(info->flagDrop.eventID, player, this);
                }
                //this cause to call return, all flags must be deleted here!!
                spellId = 0;
                Delete();
            }
            break;
        }
        case GAMEOBJECT_TYPE_BARBER_CHAIR:                  //32
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();

            WorldPackets::Misc::EnableBarberShop packet;
            player->SendDirectMessage(packet.Write());

            // fallback, will always work
            player->TeleportTo(GetMapId(), GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);

            player->SetStandState(UnitStandStateType(UNIT_STAND_STATE_SIT_LOW_CHAIR + info->barberChair.chairheight), info->barberChair.SitAnimKit);
            return;
        }
        case GAMEOBJECT_TYPE_NEW_FLAG:
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            spellId = info->newflag.pickupSpell;
            break;
        }
        case GAMEOBJECT_TYPE_ITEM_FORGE:
        {
            GameObjectTemplate const* info = GetGOInfo();
            if (!info)
                return;

            if (user->GetTypeId() != TYPEID_PLAYER)
                return;

            Player* player = user->ToPlayer();
            if (PlayerConditionEntry const* playerCondition = sPlayerConditionStore.LookupEntry(info->itemForge.conditionID1))
                if (!sConditionMgr->IsPlayerMeetingCondition(player, playerCondition))
                    return;

            switch (info->itemForge.ForgeType)
            {
                case 0: // Artifact Forge
                case 1: // Relic Forge
                {
                    Aura const* artifactAura = player->GetAura(ARTIFACTS_ALL_WEAPONS_GENERAL_WEAPON_EQUIPPED_PASSIVE);
                    Item const* item = artifactAura ? player->GetItemByGuid(artifactAura->GetCastItemGUID()) : nullptr;
                    if (!item)
                    {
                        player->SendDirectMessage(WorldPackets::Misc::DisplayGameError(GameError::ERR_MUST_EQUIP_ARTIFACT).Write());
                        return;
                    }

                    WorldPackets::Artifact::OpenArtifactForge openArtifactForge;
                    openArtifactForge.ArtifactGUID = item->GetGUID();
                    openArtifactForge.ForgeGUID = GetGUID();
                    player->SendDirectMessage(openArtifactForge.Write());
                    break;
                }
                case 2: // Heart Forge
                {
                    Item const* item = player->GetItemByEntry(ITEM_ID_HEART_OF_AZEROTH, ItemSearchLocation::Everywhere);
                    if (!item)
                        return;

                    WorldPackets::Azerite::OpenHeartForge openHeartForge;
                    openHeartForge.ForgeGUID = GetGUID();
                    player->SendDirectMessage(openHeartForge.Write());
                    break;
                }
                default:
                    break;
            }
            return;
        }
        case GAMEOBJECT_TYPE_UI_LINK:
        {
            Player* player = user->ToPlayer();
            if (!player)
                return;

            WorldPackets::GameObject::GameObjectUILink gameObjectUILink;
            gameObjectUILink.ObjectGUID = GetGUID();
            gameObjectUILink.UILink = GetGOInfo()->UILink.UILinkType;
            player->SendDirectMessage(gameObjectUILink.Write());
            return;
        }
        default:
            if (GetGoType() >= MAX_GAMEOBJECT_TYPE)
                TC_LOG_ERROR("misc", "GameObject::Use(): unit (%s, name: %s) tries to use object (%s, name: %s) of unknown type (%u)",
                    user->GetGUID().ToString().c_str(), user->GetName().c_str(), GetGUID().ToString().c_str(), GetGOInfo()->name.c_str(), GetGoType());
            break;
    }

    if (!spellId)
        return;

    if (!sSpellMgr->GetSpellInfo(spellId, GetMap()->GetDifficultyID()))
    {
        if (user->GetTypeId() != TYPEID_PLAYER || !sOutdoorPvPMgr->HandleCustomSpell(user->ToPlayer(), spellId, this))
            TC_LOG_ERROR("misc", "WORLD: unknown spell id %u at use action for gameobject (Entry: %u GoType: %u)", spellId, GetEntry(), GetGoType());
        else
            TC_LOG_DEBUG("outdoorpvp", "WORLD: %u non-dbc spell was handled by OutdoorPvP", spellId);
        return;
    }

    if (Player* player = user->ToPlayer())
        sOutdoorPvPMgr->HandleCustomSpell(player, spellId, this);

    if (spellCaster)
        spellCaster->CastSpell(user, spellId, triggered);
    else
        CastSpell(user, spellId);
}

void GameObject::SendCustomAnim(uint32 anim)
{
    WorldPackets::GameObject::GameObjectCustomAnim customAnim;
    customAnim.ObjectGUID = GetGUID();
    customAnim.CustomAnim = anim;
    SendMessageToSet(customAnim.Write(), true);
}

bool GameObject::IsInRange(float x, float y, float z, float radius) const
{
    GameObjectDisplayInfoEntry const* info = sGameObjectDisplayInfoStore.LookupEntry(m_goInfo->displayId);
    if (!info)
        return IsWithinDist3d(x, y, z, radius);

    float sinA = std::sin(GetOrientation());
    float cosA = std::cos(GetOrientation());
    float dx = x - GetPositionX();
    float dy = y - GetPositionY();
    float dz = z - GetPositionZ();
    float dist = std::sqrt(dx*dx + dy*dy);
    //! Check if the distance between the 2 objects is 0, can happen if both objects are on the same position.
    //! The code below this check wont crash if dist is 0 because 0/0 in float operations is valid, and returns infinite
    if (G3D::fuzzyEq(dist, 0.0f))
        return true;

    float sinB = dx / dist;
    float cosB = dy / dist;
    dx = dist * (cosA * cosB + sinA * sinB);
    dy = dist * (cosA * sinB - sinA * cosB);
    return dx < info->GeoBoxMax.X + radius && dx > info->GeoBoxMin.X - radius
        && dy < info->GeoBoxMax.Y + radius && dy > info->GeoBoxMin.Y - radius
        && dz < info->GeoBoxMax.Z + radius && dz > info->GeoBoxMin.Z - radius;
}

uint32 GameObject::GetScriptId() const
{
    if (GameObjectData const* gameObjectData = GetGameObjectData())
        if (uint32 scriptId = gameObjectData->scriptId)
            return scriptId;

    return GetGOInfo()->ScriptId;
}

// overwrite WorldObject function for proper name localization
std::string GameObject::GetNameForLocaleIdx(LocaleConstant locale) const
{
    if (locale != DEFAULT_LOCALE)
        if (GameObjectLocale const* cl = sObjectMgr->GetGameObjectLocale(GetEntry()))
            if (cl->Name.size() > locale && !cl->Name[locale].empty())
                return cl->Name[locale];

    return GetName();
}

void GameObject::UpdatePackedRotation()
{
    static const int32 PACK_YZ = 1 << 20;
    static const int32 PACK_X = PACK_YZ << 1;

    static const int32 PACK_YZ_MASK = (PACK_YZ << 1) - 1;
    static const int32 PACK_X_MASK = (PACK_X << 1) - 1;

    int8 w_sign = (m_localRotation.w >= 0.f ? 1 : -1);
    int64 x = int32(m_localRotation.x * PACK_X)  * w_sign & PACK_X_MASK;
    int64 y = int32(m_localRotation.y * PACK_YZ) * w_sign & PACK_YZ_MASK;
    int64 z = int32(m_localRotation.z * PACK_YZ) * w_sign & PACK_YZ_MASK;
    m_packedRotation = z | (y << 21) | (x << 42);
}

void GameObject::SetLocalRotation(float qx, float qy, float qz, float qw)
{
    G3D::Quat rotation(qx, qy, qz, qw);
    rotation.unitize();
    m_localRotation.x = rotation.x;
    m_localRotation.y = rotation.y;
    m_localRotation.z = rotation.z;
    m_localRotation.w = rotation.w;
    UpdatePackedRotation();
}

void GameObject::SetParentRotation(QuaternionData const& rotation)
{
    SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::ParentRotation), rotation);
}

void GameObject::SetLocalRotationAngles(float z_rot, float y_rot, float x_rot)
{
    G3D::Quat quat(G3D::Matrix3::fromEulerAnglesZYX(z_rot, y_rot, x_rot));
    SetLocalRotation(quat.x, quat.y, quat.z, quat.w);
}

QuaternionData GameObject::GetWorldRotation() const
{
    QuaternionData localRotation = GetLocalRotation();
    if (Transport* transport = dynamic_cast<Transport*>(GetTransport()))
    {
        QuaternionData worldRotation = transport->GetWorldRotation();

        G3D::Quat worldRotationQuat(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w);
        G3D::Quat localRotationQuat(localRotation.x, localRotation.y, localRotation.z, localRotation.w);

        G3D::Quat resultRotation = localRotationQuat * worldRotationQuat;

        return QuaternionData(resultRotation.x, resultRotation.y, resultRotation.z, resultRotation.w);
    }
    return localRotation;
}

void GameObject::ModifyHealth(int32 change, WorldObject* attackerOrHealer /*= nullptr*/, uint32 spellId /*= 0*/)
{
    if (!m_goValue.Building.MaxHealth || !change)
        return;

    // prevent double destructions of the same object
    if (change < 0 && !m_goValue.Building.Health)
        return;

    if (int32(m_goValue.Building.Health) + change <= 0)
        m_goValue.Building.Health = 0;
    else if (int32(m_goValue.Building.Health) + change >= int32(m_goValue.Building.MaxHealth))
        m_goValue.Building.Health = m_goValue.Building.MaxHealth;
    else
        m_goValue.Building.Health += change;

    // Set the health bar, value = 255 * healthPct;
    SetGoAnimProgress(m_goValue.Building.Health * 255 / m_goValue.Building.MaxHealth);

    // dealing damage, send packet
    if (Player* player = attackerOrHealer ? attackerOrHealer->GetCharmerOrOwnerPlayerOrPlayerItself() : nullptr)
    {
        WorldPackets::GameObject::DestructibleBuildingDamage packet;
        packet.Caster = attackerOrHealer->GetGUID(); // todo: this can be a GameObject
        packet.Target = GetGUID();
        packet.Damage = -change;
        packet.Owner = player->GetGUID();
        packet.SpellID = spellId;
        player->SendDirectMessage(packet.Write());
    }

    if (change < 0 && GetGOInfo()->destructibleBuilding.DamageEvent)
        GameEvents::Trigger(GetGOInfo()->destructibleBuilding.DamageEvent, attackerOrHealer, this);

    GameObjectDestructibleState newState = GetDestructibleState();

    if (!m_goValue.Building.Health)
        newState = GO_DESTRUCTIBLE_DESTROYED;
    else if (m_goValue.Building.Health <= 10000/*GetGOInfo()->destructibleBuilding.damagedNumHits*/) // TODO: Get health somewhere
        newState = GO_DESTRUCTIBLE_DAMAGED;
    else if (m_goValue.Building.Health == m_goValue.Building.MaxHealth)
        newState = GO_DESTRUCTIBLE_INTACT;

    if (newState == GetDestructibleState())
        return;

    SetDestructibleState(newState, attackerOrHealer, false);
}

void GameObject::SetDestructibleState(GameObjectDestructibleState state, WorldObject* attackerOrHealer /*= nullptr*/, bool setHealth /*= false*/)
{
    // the user calling this must know he is already operating on destructible gameobject
    ASSERT(GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING);

    switch (state)
    {
        case GO_DESTRUCTIBLE_INTACT:
            RemoveFlag(GO_FLAG_DAMAGED | GO_FLAG_DESTROYED);
            SetDisplayId(m_goInfo->displayId);
            if (setHealth)
            {
                m_goValue.Building.Health = m_goValue.Building.MaxHealth;
                SetGoAnimProgress(255);
            }
            EnableCollision(true);
            break;
        case GO_DESTRUCTIBLE_DAMAGED:
        {
            if (GetGOInfo()->destructibleBuilding.DamagedEvent)
                GameEvents::Trigger(GetGOInfo()->destructibleBuilding.DamagedEvent, attackerOrHealer, this);
            AI()->Damaged(attackerOrHealer, m_goInfo->destructibleBuilding.DamagedEvent);

            RemoveFlag(GO_FLAG_DESTROYED);
            SetFlag(GO_FLAG_DAMAGED);

            uint32 modelId = m_goInfo->displayId;
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->destructibleBuilding.DestructibleModelRec))
                if (modelData->State1Wmo)
                    modelId = modelData->State1Wmo;
            SetDisplayId(modelId);

            if (setHealth)
            {
                m_goValue.Building.Health = 10000/*m_goInfo->destructibleBuilding.damagedNumHits*/;
                uint32 maxHealth = m_goValue.Building.MaxHealth;
                // in this case current health is 0 anyway so just prevent crashing here
                if (!maxHealth)
                    maxHealth = 1;
                SetGoAnimProgress(m_goValue.Building.Health * 255 / maxHealth);
            }
            break;
        }
        case GO_DESTRUCTIBLE_DESTROYED:
        {
            if (GetGOInfo()->destructibleBuilding.DestroyedEvent)
                GameEvents::Trigger(GetGOInfo()->destructibleBuilding.DestroyedEvent, attackerOrHealer, this);
            AI()->Destroyed(attackerOrHealer, m_goInfo->destructibleBuilding.DestroyedEvent);

            if (Player* player = attackerOrHealer ? attackerOrHealer->GetCharmerOrOwnerPlayerOrPlayerItself() : nullptr)
                if (Battleground* bg = player->GetBattleground())
                    bg->DestroyGate(player, this);

            RemoveFlag(GO_FLAG_DAMAGED);
            SetFlag(GO_FLAG_DESTROYED);

            uint32 modelId = m_goInfo->displayId;
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->destructibleBuilding.DestructibleModelRec))
                if (modelData->State2Wmo)
                    modelId = modelData->State2Wmo;
            SetDisplayId(modelId);

            if (setHealth)
            {
                m_goValue.Building.Health = 0;
                SetGoAnimProgress(0);
            }
            EnableCollision(false);
            break;
        }
        case GO_DESTRUCTIBLE_REBUILDING:
        {
            if (GetGOInfo()->destructibleBuilding.RebuildingEvent)
                GameEvents::Trigger(GetGOInfo()->destructibleBuilding.RebuildingEvent, attackerOrHealer, this);
            RemoveFlag(GO_FLAG_DAMAGED | GO_FLAG_DESTROYED);

            uint32 modelId = m_goInfo->displayId;
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->destructibleBuilding.DestructibleModelRec))
                if (modelData->State3Wmo)
                    modelId = modelData->State3Wmo;
            SetDisplayId(modelId);

            // restores to full health
            if (setHealth)
            {
                m_goValue.Building.Health = m_goValue.Building.MaxHealth;
                SetGoAnimProgress(255);
            }
            EnableCollision(true);
            break;
        }
    }
}

void GameObject::SetLootState(LootState state, Unit* unit)
{
    m_lootState = state;
    if (unit)
        m_lootStateUnitGUID = unit->GetGUID();
    else
        m_lootStateUnitGUID.Clear();

    AI()->OnLootStateChanged(state, unit);

    // Start restock timer if the chest is partially looted or not looted at all
    if (GetGoType() == GAMEOBJECT_TYPE_CHEST && state == GO_ACTIVATED && GetGOInfo()->chest.chestRestockTime > 0 && m_restockTime == 0)
        m_restockTime = GameTime::GetGameTime() + GetGOInfo()->chest.chestRestockTime;

    if (GetGoType() == GAMEOBJECT_TYPE_DOOR) // only set collision for doors on SetGoState
        return;

    if (m_model)
    {
        bool collision = false;
        // Use the current go state
        if ((GetGoState() != GO_STATE_READY && (state == GO_ACTIVATED || state == GO_JUST_DEACTIVATED)) || state == GO_READY)
            collision = !collision;

        EnableCollision(collision);
    }
}

void GameObject::SetLootGenerationTime()
{
    m_lootGenerationTime = GameTime::GetGameTime();
}

void GameObject::SetGoState(GOState state)
{
    GOState oldState = GetGoState();
    SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::State), state);
    if (AI())
        AI()->OnStateChanged(state);

    if (m_goTypeImpl)
        m_goTypeImpl->OnStateChanged(oldState, state);

    if (m_model && !IsTransport())
    {
        if (!IsInWorld())
            return;

        // startOpen determines whether we are going to add or remove the LoS on activation
        bool collision = false;
        if (state == GO_STATE_READY)
            collision = !collision;

        EnableCollision(collision);
    }
}

void GameObject::SetDisplayId(uint32 displayid)
{
    SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::DisplayID), displayid);
    UpdateModel();
}

uint8 GameObject::GetNameSetId() const
{
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
            if (DestructibleModelDataEntry const* modelData = sDestructibleModelDataStore.LookupEntry(m_goInfo->destructibleBuilding.DestructibleModelRec))
            {
                switch (GetDestructibleState())
                {
                    case GO_DESTRUCTIBLE_INTACT:
                        return modelData->State0NameSet;
                    case GO_DESTRUCTIBLE_DAMAGED:
                        return modelData->State1NameSet;
                    case GO_DESTRUCTIBLE_DESTROYED:
                        return modelData->State2NameSet;
                    case GO_DESTRUCTIBLE_REBUILDING:
                        return modelData->State3NameSet;
                    default:
                        break;
                }
            }
            break;
        case GAMEOBJECT_TYPE_GARRISON_BUILDING:
        case GAMEOBJECT_TYPE_GARRISON_PLOT:
        case GAMEOBJECT_TYPE_PHASEABLE_MO:
            return ((*m_gameObjectData->Flags) >> 8) & 0xF;
        default:
            break;
    }

    return 0;
}

void GameObject::EnableCollision(bool enable)
{
    if (!m_model)
        return;

    /*if (enable && !GetMap()->ContainsGameObjectModel(*m_model))
        GetMap()->InsertGameObjectModel(*m_model);*/

    m_model->enableCollision(enable);
}

void GameObject::UpdateModel()
{
    if (!IsInWorld())
        return;
    if (m_model)
        if (GetMap()->ContainsGameObjectModel(*m_model))
            GetMap()->RemoveGameObjectModel(*m_model);
    RemoveFlag(GO_FLAG_MAP_OBJECT);
    delete m_model;
    m_model = nullptr;
    CreateModel();
    if (m_model)
        GetMap()->InsertGameObjectModel(*m_model);
}

Player* GameObject::GetLootRecipient() const
{
    if (!m_lootRecipient)
        return nullptr;
    return ObjectAccessor::FindConnectedPlayer(m_lootRecipient);
}

Group* GameObject::GetLootRecipientGroup() const
{
    if (!m_lootRecipientGroup)
        return nullptr;
    return sGroupMgr->GetGroupByGUID(m_lootRecipientGroup);
}

void GameObject::SetLootRecipient(Unit* unit, Group* group)
{
    // set the player whose group should receive the right
    // to loot the creature after it dies
    // should be set to nullptr after the loot disappears

    if (!unit)
    {
        m_lootRecipient.Clear();
        m_lootRecipientGroup = group ? group->GetGUID() : ObjectGuid::Empty;
        return;
    }

    if (unit->GetTypeId() != TYPEID_PLAYER && !unit->IsVehicle())
        return;

    Player* player = unit->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!player)                                             // normal creature, no player involved
        return;

    m_lootRecipient = player->GetGUID();

    // either get the group from the passed parameter or from unit's one
    if (group)
        m_lootRecipientGroup = group->GetGUID();
    else if (Group* unitGroup = player->GetGroup())
        m_lootRecipientGroup = unitGroup->GetGUID();
}

bool GameObject::IsLootAllowedFor(Player const* player) const
{
    if (!m_lootRecipient && !m_lootRecipientGroup)
        return true;

    if (player->GetGUID() == m_lootRecipient)
        return true;

    Group const* playerGroup = player->GetGroup();
    if (!playerGroup || playerGroup != GetLootRecipientGroup()) // if we dont have a group we arent the recipient
        return false;                                           // if go doesnt have group bound it means it was solo killed by someone else

    return true;
}

GameObject* GameObject::GetLinkedTrap()
{
    return ObjectAccessor::GetGameObject(*this, m_linkedTrap);
}

void GameObject::BuildValuesCreate(ByteBuffer* data, Player const* target) const
{
    UF::UpdateFieldFlag flags = GetUpdateFieldFlagsFor(target);
    std::size_t sizePos = data->wpos();
    *data << uint32(0);
    *data << uint8(flags);
    m_objectData->WriteCreate(*data, flags, this, target);
    m_gameObjectData->WriteCreate(*data, flags, this, target);
    data->put<uint32>(sizePos, data->wpos() - sizePos - 4);
}

void GameObject::BuildValuesUpdate(ByteBuffer* data, Player const* target) const
{
    UF::UpdateFieldFlag flags = GetUpdateFieldFlagsFor(target);
    std::size_t sizePos = data->wpos();
    *data << uint32(0);
    *data << uint32(m_values.GetChangedObjectTypeMask());

    if (m_values.HasChanged(TYPEID_OBJECT))
        m_objectData->WriteUpdate(*data, flags, this, target);

    if (m_values.HasChanged(TYPEID_GAMEOBJECT))
        m_gameObjectData->WriteUpdate(*data, flags, this, target);

    data->put<uint32>(sizePos, data->wpos() - sizePos - 4);
}

void GameObject::BuildValuesUpdateForPlayerWithMask(UpdateData* data, UF::ObjectData::Mask const& requestedObjectMask,
    UF::GameObjectData::Mask const& requestedGameObjectMask, Player const* target) const
{
    UpdateMask<NUM_CLIENT_OBJECT_TYPES> valuesMask;
    if (requestedObjectMask.IsAnySet())
        valuesMask.Set(TYPEID_OBJECT);

    if (requestedGameObjectMask.IsAnySet())
        valuesMask.Set(TYPEID_GAMEOBJECT);

    ByteBuffer buffer = PrepareValuesUpdateBuffer();
    std::size_t sizePos = buffer.wpos();
    buffer << uint32(0);
    buffer << uint32(valuesMask.GetBlock(0));

    if (valuesMask[TYPEID_OBJECT])
        m_objectData->WriteUpdate(buffer, requestedObjectMask, true, this, target);

    if (valuesMask[TYPEID_GAMEOBJECT])
        m_gameObjectData->WriteUpdate(buffer, requestedGameObjectMask, true, this, target);

    buffer.put<uint32>(sizePos, buffer.wpos() - sizePos - 4);

    data->AddUpdateBlock(buffer);
}

void GameObject::ValuesUpdateForPlayerWithMaskSender::operator()(Player const* player) const
{
    UpdateData udata(Owner->GetMapId());
    WorldPacket packet;

    Owner->BuildValuesUpdateForPlayerWithMask(&udata, ObjectMask.GetChangesMask(), GameObjectMask.GetChangesMask(), player);

    udata.BuildPacket(&packet);
    player->SendDirectMessage(&packet);
}

void GameObject::ClearUpdateMask(bool remove)
{
    m_values.ClearChangesMask(&GameObject::m_gameObjectData);
    Object::ClearUpdateMask(remove);
}

std::vector<uint32> const* GameObject::GetPauseTimes() const
{
    if (GameObjectType::Transport const* transport = dynamic_cast<GameObjectType::Transport const*>(m_goTypeImpl.get()))
        return transport->GetPauseTimes();

    return nullptr;
}

void GameObject::SetPathProgressForClient(float progress)
{
    DoWithSuppressingObjectUpdates([&]()
    {
        UF::ObjectData::Base dynflagMask;
        dynflagMask.MarkChanged(&UF::ObjectData::DynamicFlags);
        bool marked = (m_objectData->GetChangesMask() & dynflagMask.GetChangesMask()).IsAnySet();

        uint32 dynamicFlags = GetDynamicFlags();
        dynamicFlags &= 0xFFFF; // remove high bits
        dynamicFlags |= uint32(progress * 65535.0f) << 16;
        ReplaceAllDynamicFlags(dynamicFlags);

        if (!marked)
            const_cast<UF::ObjectData&>(*m_objectData).ClearChanged(&UF::ObjectData::DynamicFlags);
    });
}

void GameObject::GetRespawnPosition(float &x, float &y, float &z, float* ori /* = nullptr*/) const
{
    if (m_goData)
    {
        if (ori)
            m_goData->spawnPoint.GetPosition(x, y, z, *ori);
        else
            m_goData->spawnPoint.GetPosition(x, y, z);
    }
    else
    {
        if (ori)
            GetPosition(x, y, z, *ori);
        else
            GetPosition(x, y, z);
    }
}

TransportBase const* GameObject::ToTransportBase() const
{
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_TRANSPORT:
            return static_cast<GameObjectType::Transport const*>(m_goTypeImpl.get());
        case GAMEOBJECT_TYPE_MAP_OBJ_TRANSPORT:
            return static_cast<Transport const*>(this);
        default:
            break;
    }

    return nullptr;
}

void GameObject::AfterRelocation()
{
    UpdateModelPosition();
    UpdatePositionData();
    if (m_goTypeImpl)
        m_goTypeImpl->OnRelocated();

    UpdateObjectVisibility(false);
}

float GameObject::GetInteractionDistance() const
{
    switch (GetGoType())
    {
        case GAMEOBJECT_TYPE_AREADAMAGE:
            return 0.0f;
        case GAMEOBJECT_TYPE_QUESTGIVER:
        case GAMEOBJECT_TYPE_TEXT:
        case GAMEOBJECT_TYPE_FLAGSTAND:
        case GAMEOBJECT_TYPE_FLAGDROP:
        case GAMEOBJECT_TYPE_MINI_GAME:
            return 5.5555553f;
        case GAMEOBJECT_TYPE_BINDER:
            return 10.0f;
        case GAMEOBJECT_TYPE_CHAIR:
        case GAMEOBJECT_TYPE_BARBER_CHAIR:
            return 3.0f;
        case GAMEOBJECT_TYPE_FISHINGNODE:
            return 100.0f;
        case GAMEOBJECT_TYPE_FISHINGHOLE:
            return 20.0f + CONTACT_DISTANCE; // max spell range
        case GAMEOBJECT_TYPE_CAMERA:
        case GAMEOBJECT_TYPE_MAP_OBJECT:
        case GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY:
        case GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING:
        case GAMEOBJECT_TYPE_DOOR:
            return 5.0f;
        // Following values are not blizzlike
        case GAMEOBJECT_TYPE_GUILD_BANK:
        case GAMEOBJECT_TYPE_MAILBOX:
            // Successful mailbox interaction is rather critical to the client, failing it will start a minute-long cooldown until the next mail query may be executed.
            // And since movement info update is not sent with mailbox interaction query, server may find the player outside of interaction range. Thus we increase it.
            return 10.0f; // 5.0f is blizzlike
        default:
            return INTERACTION_DISTANCE;
    }
}

void GameObject::UpdateModelPosition()
{
    if (!m_model)
        return;

    if (GetMap()->ContainsGameObjectModel(*m_model))
    {
        GetMap()->RemoveGameObjectModel(*m_model);
        m_model->UpdatePosition();
        GetMap()->InsertGameObjectModel(*m_model);
    }
}

void GameObject::SetAnimKitId(uint16 animKitId, bool oneshot)
{
    if (_animKitId == animKitId)
        return;

    if (animKitId && !sAnimKitStore.LookupEntry(animKitId))
        return;

    if (!oneshot)
        _animKitId = animKitId;
    else
        _animKitId = 0;

    WorldPackets::GameObject::GameObjectActivateAnimKit activateAnimKit;
    activateAnimKit.ObjectGUID = GetGUID();
    activateAnimKit.AnimKitID = animKitId;
    activateAnimKit.Maintain = !oneshot;
    SendMessageToSet(activateAnimKit.Write(), true);
}

void GameObject::SetSpellVisualId(int32 spellVisualId, ObjectGuid activatorGuid)
{
    SetUpdateFieldValue(m_values.ModifyValue(&GameObject::m_gameObjectData).ModifyValue(&UF::GameObjectData::SpellVisualID), spellVisualId);

    WorldPackets::GameObject::GameObjectPlaySpellVisual packet;
    packet.ObjectGUID = GetGUID();
    packet.ActivatorGUID = activatorGuid;
    packet.SpellVisualID = spellVisualId;
    SendMessageToSet(packet.Write(), true);
}

void GameObject::AssaultCapturePoint(Player* player)
{
    if (!CanInteractWithCapturePoint(player))
        return;

    if (GameObjectAI* ai = AI())
        if (ai->OnCapturePointAssaulted(player))
            return;

    // only supported in battlegrounds
    Battleground* battleground = nullptr;
    if (BattlegroundMap* map = GetMap()->ToBattlegroundMap())
        if (Battleground* bg = map->GetBG())
            battleground = bg;

    if (!battleground)
        return;

    // Cancel current timer
    m_goValue.CapturePoint.AssaultTimer = 0;

    if (player->GetBGTeam() == HORDE)
    {
        if (m_goValue.CapturePoint.LastTeamCapture == TEAM_HORDE)
        {
            // defended. capture instantly.
            m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::HordeCaptured;
            battleground->SendBroadcastText(GetGOInfo()->capturePoint.DefendedBroadcastHorde, CHAT_MSG_BG_SYSTEM_HORDE, player);
            UpdateCapturePoint();
            if (GetGOInfo()->capturePoint.DefendedEventHorde)
                GameEvents::Trigger(GetGOInfo()->capturePoint.DefendedEventHorde, player, this);
            return;
        }

        switch (m_goValue.CapturePoint.State)
        {
            case WorldPackets::Battleground::BattlegroundCapturePointState::Neutral:
            case WorldPackets::Battleground::BattlegroundCapturePointState::AllianceCaptured:
            case WorldPackets::Battleground::BattlegroundCapturePointState::ContestedAlliance:
                m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::ContestedHorde;
                battleground->SendBroadcastText(GetGOInfo()->capturePoint.AssaultBroadcastHorde, CHAT_MSG_BG_SYSTEM_HORDE, player);
                UpdateCapturePoint();
                if (GetGOInfo()->capturePoint.ContestedEventHorde)
                    GameEvents::Trigger(GetGOInfo()->capturePoint.ContestedEventHorde, player, this);
                m_goValue.CapturePoint.AssaultTimer = GetGOInfo()->capturePoint.CaptureTime;
                break;
            default:
                break;
        }
    }
    else
    {
        if (m_goValue.CapturePoint.LastTeamCapture == TEAM_ALLIANCE)
        {
            // defended. capture instantly.
            m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::AllianceCaptured;
            battleground->SendBroadcastText(GetGOInfo()->capturePoint.DefendedBroadcastAlliance, CHAT_MSG_BG_SYSTEM_ALLIANCE, player);
            UpdateCapturePoint();
            if (GetGOInfo()->capturePoint.DefendedEventAlliance)
                GameEvents::Trigger(GetGOInfo()->capturePoint.DefendedEventAlliance, player, this);
            return;
        }

        switch (m_goValue.CapturePoint.State)
        {
            case WorldPackets::Battleground::BattlegroundCapturePointState::Neutral:
            case WorldPackets::Battleground::BattlegroundCapturePointState::HordeCaptured:
            case WorldPackets::Battleground::BattlegroundCapturePointState::ContestedHorde:
                m_goValue.CapturePoint.State = WorldPackets::Battleground::BattlegroundCapturePointState::ContestedAlliance;
                battleground->SendBroadcastText(GetGOInfo()->capturePoint.AssaultBroadcastAlliance, CHAT_MSG_BG_SYSTEM_ALLIANCE, player);
                UpdateCapturePoint();
                if (GetGOInfo()->capturePoint.ContestedEventAlliance)
                    GameEvents::Trigger(GetGOInfo()->capturePoint.ContestedEventAlliance, player, this);
                m_goValue.CapturePoint.AssaultTimer = GetGOInfo()->capturePoint.CaptureTime;
                break;
            default:
                break;
        }
    }
}

void GameObject::UpdateCapturePoint()
{
    if (GetGoType() != GAMEOBJECT_TYPE_CAPTURE_POINT)
        return;

    if (GameObjectAI* ai = AI())
        if (ai->OnCapturePointUpdated(m_goValue.CapturePoint.State))
            return;

    uint32 spellVisualId = 0;
    uint32 customAnim = 0;

    switch (m_goValue.CapturePoint.State)
    {
        case WorldPackets::Battleground::BattlegroundCapturePointState::Neutral:
            spellVisualId = GetGOInfo()->capturePoint.SpellVisual1;
            break;
        case WorldPackets::Battleground::BattlegroundCapturePointState::ContestedHorde:
            customAnim = 1;
            spellVisualId = GetGOInfo()->capturePoint.SpellVisual2;
            break;
        case WorldPackets::Battleground::BattlegroundCapturePointState::ContestedAlliance:
            customAnim = 2;
            spellVisualId = GetGOInfo()->capturePoint.SpellVisual3;
            break;
        case WorldPackets::Battleground::BattlegroundCapturePointState::HordeCaptured:
            customAnim = 3;
            spellVisualId = GetGOInfo()->capturePoint.SpellVisual4;
            break;
        case WorldPackets::Battleground::BattlegroundCapturePointState::AllianceCaptured:
            customAnim = 4;
            spellVisualId = GetGOInfo()->capturePoint.SpellVisual5;
            break;
        default:
            break;
    }

    if (customAnim != 0)
        SendCustomAnim(customAnim);

    SetSpellVisualId(spellVisualId);
    UpdateDynamicFlagsForNearbyPlayers();

    if (BattlegroundMap* map = GetMap()->ToBattlegroundMap())
    {
        if (Battleground* bg = map->GetBG())
        {
            WorldPackets::Battleground::UpdateCapturePoint packet;
            packet.CapturePointInfo.State = m_goValue.CapturePoint.State;
            packet.CapturePointInfo.Pos = GetPosition();
            packet.CapturePointInfo.Guid = GetGUID();
            packet.CapturePointInfo.CaptureTotalDuration = Milliseconds(GetGOInfo()->capturePoint.CaptureTime);
            packet.CapturePointInfo.CaptureTime = m_goValue.CapturePoint.AssaultTimer;
            bg->SendPacketToAll(packet.Write());
            bg->UpdateWorldState(GetGOInfo()->capturePoint.worldState1, AsUnderlyingType(m_goValue.CapturePoint.State));
        }
    }
}

bool GameObject::CanInteractWithCapturePoint(Player const* target) const
{
    if (m_goInfo->type != GAMEOBJECT_TYPE_CAPTURE_POINT)
        return false;

    if (m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::Neutral)
        return true;

    if (target->GetBGTeam() == HORDE)
    {
        return m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::ContestedAlliance
            || m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::AllianceCaptured;
    }

    // For Alliance players
    return m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::ContestedHorde
        || m_goValue.CapturePoint.State == WorldPackets::Battleground::BattlegroundCapturePointState::HordeCaptured;
}

class GameObjectModelOwnerImpl : public GameObjectModelOwnerBase
{
public:
    explicit GameObjectModelOwnerImpl(GameObject* owner) : _owner(owner) { }
    virtual ~GameObjectModelOwnerImpl() = default;

    bool IsSpawned() const override { return _owner->isSpawned(); }
    uint32 GetDisplayId() const override { return _owner->GetDisplayId(); }
    uint8 GetNameSetId() const override { return _owner->GetNameSetId(); }
    bool IsInPhase(PhaseShift const& phaseShift) const override { return _owner->GetPhaseShift().CanSee(phaseShift); }
    G3D::Vector3 GetPosition() const override { return G3D::Vector3(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ()); }
    G3D::Quat GetRotation() const override { return G3D::Quat(_owner->GetLocalRotation().x, _owner->GetLocalRotation().y, _owner->GetLocalRotation().z, _owner->GetLocalRotation().w); }
    float GetScale() const override { return _owner->GetObjectScale(); }
    void DebugVisualizeCorner(G3D::Vector3 const& corner) const override { _owner->SummonCreature(1, corner.x, corner.y, corner.z, 0, TEMPSUMMON_MANUAL_DESPAWN); }

private:
    GameObject* _owner;
};

void GameObject::UpdateDynamicFlagsForNearbyPlayers() const
{
    ValuesUpdateForPlayerWithMaskSender sender(this);
    sender.ObjectMask.MarkChanged(&UF::ObjectData::DynamicFlags);
    Trinity::MessageDistDeliverer<ValuesUpdateForPlayerWithMaskSender> deliverer(this, sender, GetVisibilityRange());
    Cell::VisitWorldObjects(this, deliverer, GetVisibilityRange());
}

void GameObject::HandleCustomTypeCommand(GameObjectTypeBase::CustomCommand const& command) const
{
    if (m_goTypeImpl)
        command.Execute(*m_goTypeImpl);
}

void GameObject::CreateModel()
{
    m_model = GameObjectModel::Create(std::make_unique<GameObjectModelOwnerImpl>(this), sWorld->GetDataPath());
    if (m_model && m_model->isMapObject())
        SetFlag(GO_FLAG_MAP_OBJECT);
}

std::string GameObject::GetDebugInfo() const
{
    std::stringstream sstr;
    sstr << WorldObject::GetDebugInfo() << "\n"
        << "SpawnId: " << GetSpawnId() << " GoState: " << std::to_string(GetGoState()) << " ScriptId: " << GetScriptId() << " AIName: " << GetAIName();
    return sstr.str();
}

bool GameObject::IsAtInteractDistance(Player const* player, SpellInfo const* spell) const
{
    if (spell || (spell = GetSpellForLock(player)))
    {
        float maxRange = spell->GetMaxRange(spell->IsPositive());

        if (GetGoType() == GAMEOBJECT_TYPE_SPELL_FOCUS)
            return maxRange * maxRange >= GetExactDistSq(player);

        if (sGameObjectDisplayInfoStore.LookupEntry(GetGOInfo()->displayId))
            return IsAtInteractDistance(*player, maxRange);
    }

    return IsAtInteractDistance(*player, GetInteractionDistance());
}

bool GameObject::IsAtInteractDistance(Position const& pos, float radius) const
{
    if (GameObjectDisplayInfoEntry const* displayInfo = sGameObjectDisplayInfoStore.LookupEntry(GetGOInfo()->displayId))
    {
        float scale = GetObjectScale();

        float minX = displayInfo->GeoBoxMin.X * scale - radius;
        float minY = displayInfo->GeoBoxMin.Y * scale - radius;
        float minZ = displayInfo->GeoBoxMin.Z * scale - radius;
        float maxX = displayInfo->GeoBoxMax.X * scale + radius;
        float maxY = displayInfo->GeoBoxMax.Y * scale + radius;
        float maxZ = displayInfo->GeoBoxMax.Z * scale + radius;

        QuaternionData worldRotation = GetWorldRotation();
        G3D::Quat worldRotationQuat(worldRotation.x, worldRotation.y, worldRotation.z, worldRotation.w);

        return G3D::CoordinateFrame { { worldRotationQuat }, { GetPositionX(), GetPositionY(), GetPositionZ() } }
                .toWorldSpace(G3D::Box { { minX, minY, minZ }, { maxX, maxY, maxZ } })
                .contains({ pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() });
    }

    return GetExactDist(&pos) <= radius;
}

bool GameObject::IsWithinDistInMap(Player const* player) const
{
    return IsInMap(player) && IsInPhase(player) && IsAtInteractDistance(player);
}

SpellInfo const* GameObject::GetSpellForLock(Player const* player) const
{
    if (!player)
        return nullptr;

    uint32 lockId = GetGOInfo()->GetLockId();
    if (!lockId)
        return nullptr;

    LockEntry const* lock = sLockStore.LookupEntry(lockId);
    if (!lock)
        return nullptr;

    for (uint8 i = 0; i < MAX_LOCK_CASE; ++i)
    {
        if (!lock->Type[i])
            continue;

        if (lock->Type[i] == LOCK_KEY_SPELL)
            if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(lock->Index[i], GetMap()->GetDifficultyID()))
                return spell;

        if (lock->Type[i] != LOCK_KEY_SKILL)
            break;

        for (auto&& playerSpell : player->GetSpellMap())
            if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(playerSpell.first, GetMap()->GetDifficultyID()))
                for (auto&& effect : spell->GetEffects())
                    if (effect.Effect == SPELL_EFFECT_OPEN_LOCK && effect.MiscValue == lock->Index[i])
                        if (effect.CalcValue(player) >= int32(lock->Skill[i]))
                            return spell;
    }

    return nullptr;
}
