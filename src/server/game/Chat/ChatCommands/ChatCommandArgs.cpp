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

#include "ChatCommandArgs.h"
#include "ChatCommand.h"
#include "DB2Stores.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Util.h"

using namespace Trinity::ChatCommands;

struct AchievementVisitor
{
    using value_type = AchievementEntry const*;
    value_type operator()(Hyperlink<achievement> achData) const { return achData->Achievement; }
    value_type operator()(uint32 achId) const { return sAchievementStore.LookupEntry(achId); }
};
Optional<std::string_view> Trinity::Impl::ChatCommands::ArgInfo<AchievementEntry const*>::TryConsume(AchievementEntry const*& data, std::string_view args)
{
    Variant<Hyperlink<achievement>, uint32> val;
    Optional<std::string_view> next = SingleConsumer<decltype(val)>::TryConsumeTo(val, args);
    if (next)
        data = val.visit(AchievementVisitor());
    return next;
}

struct CurrencyTypesVisitor
{
    using value_type = CurrencyTypesEntry const*;
    value_type operator()(Hyperlink<currency> currency) const { return currency->Currency; }
    value_type operator()(uint32 currencyId) const { return sCurrencyTypesStore.LookupEntry(currencyId); }
};
Optional<std::string_view> Trinity::Impl::ChatCommands::ArgInfo<CurrencyTypesEntry const*>::TryConsume(CurrencyTypesEntry const*& data, std::string_view args)
{
    Variant<Hyperlink<currency>, uint32> val;
    Optional<std::string_view> next = SingleConsumer<decltype(val)>::TryConsumeTo(val, args);
    if (next)
        data = val.visit(CurrencyTypesVisitor());
    return args;
}

struct GameTeleVisitor
{
    using value_type = GameTele const*;
    value_type operator()(Hyperlink<tele> tele) const { return sObjectMgr->GetGameTele(tele); }
    value_type operator()(std::string const& tele) const { return sObjectMgr->GetGameTele(tele); }
};
Optional<std::string_view> Trinity::Impl::ChatCommands::ArgInfo<GameTele const*>::TryConsume(GameTele const*& data, std::string_view args)
{
    Variant<Hyperlink<tele>, std::string> val;
    Optional<std::string_view> next = SingleConsumer<decltype(val)>::TryConsumeTo(val, args);
    if (next)
        data = val.visit(GameTeleVisitor());
    return next;
}

struct SpellInfoVisitor
{
    using value_type = SpellInfo const*;

    value_type operator()(Hyperlink<apower> artifactPower) const { return operator()(artifactPower->ArtifactPower->SpellID); }
    value_type operator()(Hyperlink<conduit> soulbindConduit) const { return operator()((*soulbindConduit)->SpellID); }
    value_type operator()(Hyperlink<enchant> enchant) const { return enchant; }
    value_type operator()(Hyperlink<mawpower> mawPower) const { return operator()((*mawPower)->SpellID); }
    value_type operator()(Hyperlink<pvptal> pvpTalent) const { return operator()((*pvpTalent)->SpellID); }
    value_type operator()(Hyperlink<spell> spell) const { return spell->Spell; }
    value_type operator()(Hyperlink<talent> talent) const { return operator()((*talent)->SpellID); }
    value_type operator()(Hyperlink<trade> trade) const { return trade->Spell; }

    value_type operator()(uint32 spellId) const { return sSpellMgr->GetSpellInfo(spellId, DIFFICULTY_NONE); }
};
Optional<std::string_view> Trinity::Impl::ChatCommands::ArgInfo<SpellInfo const*>::TryConsume(SpellInfo const*& data, std::string_view args)
{
    Variant<Hyperlink<apower>, Hyperlink<conduit>, Hyperlink<enchant>, Hyperlink<mawpower>, Hyperlink<pvptal>, Hyperlink<spell>, Hyperlink<talent>, Hyperlink<trade>, uint32> val;
    Optional<std::string_view> next = SingleConsumer<decltype(val)>::TryConsumeTo(val, args);
    if (next)
        data = val.visit(SpellInfoVisitor());
    return next;
}
