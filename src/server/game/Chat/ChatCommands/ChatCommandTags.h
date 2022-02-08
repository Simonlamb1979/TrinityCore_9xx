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

#ifndef TRINITY_CHATCOMMANDTAGS_H
#define TRINITY_CHATCOMMANDTAGS_H

#include "advstd.h"
#include "ChatCommandHelpers.h"
#include "Hyperlinks.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Util.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

class ChatHandler;
class Player;

namespace Trinity::Impl::ChatCommands
{
    struct ContainerTag {};
    template <typename T>
    struct tag_base<T, std::enable_if_t<std::is_base_of_v<ContainerTag, T>>>
    {
        using type = typename T::value_type;
    };
}

namespace Trinity::ChatCommands
{
    /************************** CONTAINER TAGS **********************************************\
    |* Simple holder classes to differentiate between extraction methods                    *|
    |* Must inherit from Trinity::Impl::ChatCommands::ContainerTag                          *|
    |* Must implement the following:                                                        *|
    |* - TryConsume: std::string_view -> Optional<std::string_view>                         *|
    |*   returns nullopt if no match, otherwise the tail of the provided argument string    *|
    |* - typedef value_type of type that is contained within the tag                        *|
    |* - cast operator to value_type                                                        *|
    |*                                                                                      *|
    \****************************************************************************************/

    template <char c1, char... chars>
    struct ExactSequence : Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = void;

        static constexpr size_t N = (sizeof...(chars) + 1);

        static bool Match(char const* pos)
        {
            if (std::toupper(*(pos++)) != std::toupper(c1))
                return false;
            else if constexpr (sizeof...(chars) > 0)
                return ExactSequence<chars...>::Match(pos);
            else
                return true;
        }

        Optional<std::string_view> TryConsume(std::string_view args) const
        {
            if ((N <= args.length()) && ExactSequence::Match(args.data()))
            {
                auto [remainingToken, tail] = Trinity::Impl::ChatCommands::tokenize(args.substr(N));
                if (remainingToken.empty()) // if this is not empty, then we did not consume the full token
                    return tail;
            }
            return std::nullopt;
        }
    };

    struct Tail : std::string_view, Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = std::string_view;

        using std::string_view::operator=;

        Optional<std::string_view> TryConsume(std::string_view args)
        {
            std::string_view::operator=(args);
            return std::string_view();
        }
    };

    struct WTail : std::wstring, Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = std::wstring;

        using std::wstring::operator=;

        Optional<std::string_view> TryConsume(std::string_view args)
        {
            if (Utf8toWStr(args, *this))
                return std::string_view();
            else
                return std::nullopt;
        }
    };

    struct QuotedString : std::string, Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = std::string;

        TC_GAME_API Optional<std::string_view> TryConsume(std::string_view args);
    };

    struct TC_GAME_API AccountIdentifier : Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = uint32;

        operator uint32() const { return _id; }
        operator std::string const& () const { return _name; }
        operator std::string_view() const { return { _name }; }

        uint32 GetID() const { return _id; }
        std::string const& GetName() const { return _name; }

        Optional<std::string_view> TryConsume(std::string_view args);

        private:
            uint32 _id;
            std::string _name;
    };

    struct TC_GAME_API PlayerIdentifier : Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = Player*;

        PlayerIdentifier() : _name(), _guid(), _player(nullptr) {}
        PlayerIdentifier(Player& player);

        operator ObjectGuid() const { return _guid; }
        operator std::string const&() const { return _name; }
        operator std::string_view() const { return _name; }

        std::string const& GetName() const { return _name; }
        ObjectGuid GetGUID() const { return _guid; }
        bool IsConnected() const { return (_player != nullptr); }
        Player* GetConnectedPlayer() const { return _player; }

        Optional<std::string_view> TryConsume(std::string_view args);

        static Optional<PlayerIdentifier> FromTarget(ChatHandler* handler);
        static Optional<PlayerIdentifier> FromSelf(ChatHandler* handler);
        static Optional<PlayerIdentifier> FromTargetOrSelf(ChatHandler* handler)
        {
            if (Optional<PlayerIdentifier> fromTarget = FromTarget(handler))
                return fromTarget;
            else
                return FromSelf(handler);
        }

        private:
            std::string _name;
            ObjectGuid _guid;
            Player* _player;
    };

    template <typename linktag>
    struct Hyperlink : Trinity::Impl::ChatCommands::ContainerTag
    {
        using value_type = typename linktag::value_type;
        using storage_type = advstd::remove_cvref_t<value_type>;

        operator value_type() const { return val; }
        value_type operator*() const { return val; }
        storage_type const* operator->() const { return &val; }

        Optional<std::string_view> TryConsume(std::string_view args)
        {
            Trinity::Hyperlinks::HyperlinkInfo info = Trinity::Hyperlinks::ParseSingleHyperlink(args);
            // invalid hyperlinks cannot be consumed
            if (!info)
                return std::nullopt;

            // check if we got the right tag
            if (info.tag != linktag::tag())
                return std::nullopt;

            // store value
            if (!linktag::StoreTo(val, info.data))
                return std::nullopt;

            // finally, skip any potential delimiters
            auto [token, next] = Trinity::Impl::ChatCommands::tokenize(info.tail);
            if (token.empty()) /* empty token = first character is delimiter, skip past it */
                return next;
            else
                return info.tail;
        }

        private:
            storage_type val;
    };

    // pull in link tags for user convenience
    using namespace ::Trinity::Hyperlinks::LinkTags;
}

namespace Trinity::Impl
{
    template <typename T>
    struct CastToVisitor
    {
        template <typename U>
        T operator()(U const& v) const { return v; }
    };
}

namespace Trinity::ChatCommands
{
    template <typename T1, typename... Ts>
    struct Variant : public std::variant<T1, Ts...>
    {
        using base = std::variant<T1, Ts...>;

        using first_type = Trinity::Impl::ChatCommands::tag_base_t<T1>;
        static constexpr bool have_operators = Trinity::Impl::ChatCommands::are_all_assignable<first_type, Trinity::Impl::ChatCommands::tag_base_t<Ts>...>::value;

        template <bool C = have_operators>
        std::enable_if_t<C, first_type> operator*() const
        {
            return visit(Trinity::Impl::CastToVisitor<first_type>());
        }

        template <bool C = have_operators>
        operator std::enable_if_t<C, first_type>() const
        {
            return operator*();
        }

        template<bool C = have_operators>
        operator std::enable_if_t<C && !std::is_same_v<first_type, size_t> && std::is_convertible_v<first_type, size_t>, size_t>() const
        {
            return operator*();
        }

        template <bool C = have_operators>
        std::enable_if_t<C, bool> operator!() const { return !**this; }

        template <typename T>
        Variant& operator=(T&& arg) { base::operator=(std::forward<T>(arg)); return *this; }

        template <size_t index>
        constexpr decltype(auto) get() { return std::get<index>(static_cast<base&>(*this)); }
        template <size_t index>
        constexpr decltype(auto) get() const { return std::get<index>(static_cast<base const&>(*this)); }
        template <typename type>
        constexpr decltype(auto) get() { return std::get<type>(static_cast<base&>(*this)); }
        template <typename type>
        constexpr decltype(auto) get() const { return std::get<type>(static_cast<base const&>(*this)); }

        template <typename T>
        constexpr decltype(auto) visit(T&& arg) { return std::visit(std::forward<T>(arg), static_cast<base&>(*this)); }
        template <typename T>
        constexpr decltype(auto) visit(T&& arg) const { return std::visit(std::forward<T>(arg), static_cast<base const&>(*this)); }

        template <typename T>
        constexpr bool holds_alternative() const { return std::holds_alternative<T>(static_cast<base const&>(*this)); }

        template <bool C = have_operators>
        friend std::enable_if_t<C, std::ostream&> operator<<(std::ostream& os, Trinity::ChatCommands::Variant<T1, Ts...> const& v)
        {
            return (os << *v);
        }
    };
}

#endif
