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

/* ScriptData
Name: tele_commandscript
%Complete: 100
Comment: All tele related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "Group.h"
#include "Language.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "PhasingHandler.h"
#include "Player.h"
#include "RBAC.h"
#include "WorldSession.h"

using namespace Trinity::ChatCommands;

class tele_commandscript : public CommandScript
{
public:
    tele_commandscript() : CommandScript("tele_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> teleCommandTable =
        {
            { "add",   rbac::RBAC_PERM_COMMAND_TELE_ADD,   false, &HandleTeleAddCommand,   "" },
            { "del",   rbac::RBAC_PERM_COMMAND_TELE_DEL,    true, &HandleTeleDelCommand,   "" },
            { "name",  rbac::RBAC_PERM_COMMAND_TELE_NAME,   true, &HandleTeleNameCommand,  "" },
            { "group", rbac::RBAC_PERM_COMMAND_TELE_GROUP, false, &HandleTeleGroupCommand, "" },
            { "",      rbac::RBAC_PERM_COMMAND_TELE,       false, &HandleTeleCommand,      "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "tele", rbac::RBAC_PERM_COMMAND_TELE, false, nullptr, "", teleCommandTable },
        };
        return commandTable;
    }

    static bool HandleTeleAddCommand(ChatHandler* handler, std::string const& name)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (sObjectMgr->GetGameTeleExactName(name))
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ALREADYEXIST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        GameTele tele;
        tele.position_x  = player->GetPositionX();
        tele.position_y  = player->GetPositionY();
        tele.position_z  = player->GetPositionZ();
        tele.orientation = player->GetOrientation();
        tele.mapId       = player->GetMapId();
        tele.name        = name;

        if (sObjectMgr->AddGameTele(tele))
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ADDED);
        }
        else
        {
            handler->SendSysMessage(LANG_COMMAND_TP_ADDEDERR);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleTeleDelCommand(ChatHandler* handler, GameTele const* tele)
    {
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }
        std::string name = tele->name;
        sObjectMgr->DeleteGameTele(name);
        handler->SendSysMessage(LANG_COMMAND_TP_DELETED);
        return true;
    }

    // teleport player to given game_tele.entry
    static bool HandleTeleNameCommand(ChatHandler* handler, Optional<PlayerIdentifier> player, Variant<GameTele const*, EXACT_SEQUENCE("$home")> where)
    {
        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);
        if (!player)
            return false;

        if (where.index() == 1)    // References target's homebind
        {
            if (Player* target = player->GetConnectedPlayer())
                target->TeleportTo(target->m_homebind);
            else
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_HOMEBIND);
                stmt->setUInt64(0, player->GetGUID().GetCounter());
                PreparedQueryResult resultDB = CharacterDatabase.Query(stmt);

                if (resultDB)
                {
                    Field* fieldsDB = resultDB->Fetch();
                    WorldLocation loc(fieldsDB[0].GetUInt16(), fieldsDB[2].GetFloat(), fieldsDB[3].GetFloat(), fieldsDB[4].GetFloat(), 0.0f);
                    uint32 zoneId = fieldsDB[1].GetUInt16();

                    CharacterDatabaseTransaction dummy;
                    Player::SavePositionInDB(loc, zoneId, player->GetGUID(), dummy);
                }
            }

            return true;
        }

        // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = where.get<GameTele const*>();
        if (Player* target = player->GetConnectedPlayer())
        {
            // check online security
            if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
                return false;

            std::string chrNameLink = handler->playerLink(target->GetName());

            if (target->IsBeingTeleported() == true)
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, chrNameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            handler->PSendSysMessage(LANG_TELEPORTING_TO, chrNameLink.c_str(), "", tele->name.c_str());
            if (handler->needReportToTarget(target))
                ChatHandler(target->GetSession()).PSendSysMessage(LANG_TELEPORTED_TO_BY, handler->GetNameLink().c_str());

            // stop flight if need
            if (target->IsInFlight())
                target->FinishTaxiFlight();
            else
                target->SaveRecallPosition(); // save only in non-flight case

            target->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(nullptr, player->GetGUID()))
                return false;

            std::string nameLink = handler->playerLink(player->GetName());

            handler->PSendSysMessage(LANG_TELEPORTING_TO, nameLink.c_str(), handler->GetTrinityString(LANG_OFFLINE), tele->name.c_str());

            CharacterDatabaseTransaction dummy;
            Player::SavePositionInDB(WorldLocation(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation),
                sMapMgr->GetZoneId(PhasingHandler::GetEmptyPhaseShift(), tele->mapId, tele->position_x, tele->position_y, tele->position_z), player->GetGUID(), dummy);
        }

        return true;
    }

    //Teleport group to given game_tele.entry
    static bool HandleTeleGroupCommand(ChatHandler* handler, GameTele const* tele)
    {
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, ObjectGuid::Empty))
            return false;

        MapEntry const* map = sMapStore.LookupEntry(tele->mapId);
        if (!map || map->IsBattlegroundOrArena())
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string nameLink = handler->GetNameLink(target);

        Group* grp = target->GetGroup();
        if (!grp)
        {
            handler->PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* player = itr->GetSource();

            if (!player || !player->GetSession())
                continue;

            // check online security
            if (handler->HasLowerSecurity(player, ObjectGuid::Empty))
                return false;

            std::string plNameLink = handler->GetNameLink(player);

            if (player->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
                continue;
            }

            handler->PSendSysMessage(LANG_TELEPORTING_TO, plNameLink.c_str(), "", tele->name.c_str());
            if (handler->needReportToTarget(player))
                ChatHandler(player->GetSession()).PSendSysMessage(LANG_TELEPORTED_TO_BY, nameLink.c_str());

            // stop flight if need
            if (player->IsInFlight())
                player->FinishTaxiFlight();
            else
                player->SaveRecallPosition(); // save only in non-flight case

            player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        }

        return true;
    }

    static bool HandleTeleCommand(ChatHandler* handler, GameTele const* tele)
    {
        if (!tele)
        {
            handler->SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (player->IsInCombat() && !handler->GetSession()->HasPermission(rbac::RBAC_PERM_COMMAND_TELE_NAME))
        {
            handler->SendSysMessage(LANG_YOU_IN_COMBAT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        MapEntry const* map = sMapStore.LookupEntry(tele->mapId);
        if (!map || (map->IsBattlegroundOrArena() && (player->GetMapId() != tele->mapId || !player->IsGameMaster())))
        {
            handler->SendSysMessage(LANG_CANNOT_TELE_TO_BG);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (player->IsInFlight())
            player->FinishTaxiFlight();
        else
            player->SaveRecallPosition(); // save only in non-flight case

        player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);
        return true;
    }
};

void AddSC_tele_commandscript()
{
    new tele_commandscript();
}
