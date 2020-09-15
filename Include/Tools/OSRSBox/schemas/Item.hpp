#ifndef OSRSBOX_ITEM_HPP_INCLUDED
#define OSRSBOX_ITEM_HPP_INCLUDED

#include "../../json.hpp"

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann
{
    template<typename T>
    struct adl_serializer<std::shared_ptr<T>>
    {
        static void to_json(json& j, const std::shared_ptr<T>& opt)
        {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json& j)
        {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace OSRSBox::Items
{
    using nlohmann::json;

    inline json get_untyped(const json& j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json& j, std::string property)
    {
        return get_untyped(j, property.data());
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json& j, const char *property)
    {
        if (j.find(property) != j.end())
        {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template<typename T>
    inline std::shared_ptr<T> get_optional(const json& j, std::string property)
    {
        return get_optional<T>(j, property.data());
    }

    /**
     * The equipment slot associated with the item (e.g., head).
     */
    enum class Slot : int
    {
        Ammo, Body, Cape, Feet, Hands, Head, Legs, Neck, Ring, Shield, TwoHanded, Weapon
    };

    struct Equipment
    {
        int32_t attackCrush    = 0; /// The attack crush bonus of the item.
        int32_t attackMagic    = 0; /// The attack magic bonus of the item.
        int32_t attackRanged   = 0; /// The attack ranged bonus of the item.
        int32_t attackSlash    = 0; /// The attack slash bonus of the item.
        int32_t attackStab     = 0; /// The attack stab bonus of the item.
        int32_t defenceCrush   = 0; /// The defence crush bonus of the item.
        int32_t defenceMagic   = 0; /// The defence magic bonus of the item.
        int32_t defenceRanged  = 0; /// The defence ranged bonus of the item.
        int32_t defenceSlash   = 0; /// The defence slash bonus of the item.
        int32_t defenceStab    = 0; /// The defence stab bonus of the item.
        int32_t magicDamage    = 0; /// The magic damage bonus of the item.
        int32_t meleeStrength  = 0; /// The melee strength bonus of the item.
        int32_t prayer         = 0; /// The prayer bonus of the item.
        int32_t rangedStrength = 0; /// The ranged strength bonus of the item.
        OSRSBox::Items::Slot slot; /// The equipment slot associated with the item (e.g., head).
    };

    struct Weapon
    {
        int32_t attackSpeed = 0; /// The attack speed of a weapon (in game ticks).
    };

    struct Item
    {
        std::shared_ptr<int32_t> buyLimit;                    /// The Grand Exchange buy limit of the item.
        int32_t cost = 0;                                     /// The store price of an item.
        bool duplicate = false;                               /// If the item is a duplicate.
        bool equipable = false;                               /// If the item is equipable (based on right-click menu entry).
        bool equipableByPlayer = false;                       /// If the item is equipable in-game by a player.
        bool equipableWeapon = false;                         /// If the item is an equipable weapon.
        std::shared_ptr<OSRSBox::Items::Equipment> equipment; /// The equipment bonuses of equipable armour/weapons.
        std::shared_ptr<std::string> examine;                 /// The examine text for the item.
        std::shared_ptr<int32_t> highalch;                    /// The high alchemy value of the item (cost * 0.6).
        std::string icon;                                     /// The item icon (in base64 encoding).
        int32_t id = -1;                                      /// Unique OSRS item ID number.
        bool incomplete = false;                              /// If the item has incomplete wiki data.
        std::shared_ptr<int32_t> linkedIdItem;                /// The linked ID of the actual item (if noted/placeholder).
        std::shared_ptr<int32_t> linkedIdNoted;               /// The linked ID of an item in noted form.
        std::shared_ptr<int32_t> linkedIdPlaceholder;         /// The linked ID of an item in placeholder form.
        std::shared_ptr<int32_t> lowalch;                     /// The low alchemy value of the item (cost * 0.4).
        bool members = false;                                 /// If the item is a members-only.
        std::string name;                                     /// The name of the item.
        bool noteable = false;                                /// If the item is noteable.
        bool noted = false;                                   /// If the item is noted.
        bool placeholder = false;                             /// If the item is a placeholder.
        bool questItem = false;                               /// If the item is associated with a quest.
        std::shared_ptr<std::string> releaseDate;             /// Date the item was released (in ISO8601 format).
        bool stackable = false;                               /// If the item is stackable (in inventory).
        std::shared_ptr<int32_t> stacked;                     /// If the item is stacked, indicated by the stack count.
        bool tradeable = false;                               /// If the item is tradeable (between players and on the GE).
        bool tradeableOnGe = false;                           /// If the item is tradeable (only on GE).
        std::shared_ptr<OSRSBox::Items::Weapon> weapon;       /// The weapon bonuses including attack speed, type and stance.
        std::shared_ptr<double> weight;                       /// The weight (in kilograms) of the item.
        std::shared_ptr<std::string> wikiExchange;            /// The OSRS Wiki Exchange URL.
        std::shared_ptr<std::string> wikiName;                /// The OSRS Wiki name for the item.
        std::shared_ptr<std::string> wikiUrl;                 /// The OSRS Wiki URL (possibly including anchor link).

    };

    void from_json(const json& j, OSRSBox::Items::Equipment& x);
    void to_json(json& j, const OSRSBox::Items::Equipment& x);

    void from_json(const json& j, OSRSBox::Items::Weapon& x);
    void to_json(json& j, const OSRSBox::Items::Weapon& x);

    void from_json(const json& j, OSRSBox::Items::Item& x);
    void to_json(json& j, const OSRSBox::Items::Item& x);

    void from_json(const json& j, OSRSBox::Items::Slot& x);
    void to_json(json& j, const OSRSBox::Items::Slot& x);

    inline void from_json(const json& j, OSRSBox::Items::Equipment& x)
    {
        x.attackCrush    = j.at("attack_crush").get<int32_t>();
        x.attackMagic    = j.at("attack_magic").get<int32_t>();
        x.attackRanged   = j.at("attack_ranged").get<int32_t>();
        x.attackSlash    = j.at("attack_slash").get<int32_t>();
        x.attackStab     = j.at("attack_stab").get<int32_t>();
        x.defenceCrush   = j.at("defence_crush").get<int32_t>();
        x.defenceMagic   = j.at("defence_magic").get<int32_t>();
        x.defenceRanged  = j.at("defence_ranged").get<int32_t>();
        x.defenceSlash   = j.at("defence_slash").get<int32_t>();
        x.defenceStab    = j.at("defence_stab").get<int32_t>();
        x.magicDamage    = j.at("magic_damage").get<int32_t>();
        x.meleeStrength  = j.at("melee_strength").get<int32_t>();
        x.prayer         = j.at("prayer").get<int32_t>();
        x.rangedStrength = j.at("ranged_strength").get<int32_t>();
        x.slot           = j.at("slot").get<OSRSBox::Items::Slot>();
    }

    inline void to_json(json& j, const OSRSBox::Items::Equipment& x)
    {
        j = json::object();
        j["attack_crush"]    = x.attackCrush;
        j["attack_magic"]    = x.attackMagic;
        j["attack_ranged"]   = x.attackRanged;
        j["attack_slash"]    = x.attackSlash;
        j["attack_stab"]     = x.attackStab;
        j["defence_crush"]   = x.defenceCrush;
        j["defence_magic"]   = x.defenceMagic;
        j["defence_ranged"]  = x.defenceRanged;
        j["defence_slash"]   = x.defenceSlash;
        j["defence_stab"]    = x.defenceStab;
        j["magic_damage"]    = x.magicDamage;
        j["melee_strength"]  = x.meleeStrength;
        j["prayer"]          = x.prayer;
        j["ranged_strength"] = x.rangedStrength;
        j["slot"]            = x.slot;
    }

    inline void from_json(const json& j, OSRSBox::Items::Weapon& x)
    {
        x.attackSpeed = j.at("attack_speed").get<int32_t>();
    }

    inline void to_json(json& j, const OSRSBox::Items::Weapon& x)
    {
        j = json::object();
        j["attack_speed"] = x.attackSpeed;
    }

    inline void from_json(const json& j, OSRSBox::Items::Item& x)
    {
        x.buyLimit            = OSRSBox::Items::get_optional<int32_t>(j, "buy_limit");
        x.cost                = j.at("cost").get<int32_t>();
        x.duplicate           = j.at("duplicate").get<bool>();
        x.equipable           = j.at("equipable").get<bool>();
        x.equipableByPlayer   = j.at("equipable_by_player").get<bool>();
        x.equipableWeapon     = j.at("equipable_weapon").get<bool>();
        x.equipment           = OSRSBox::Items::get_optional<OSRSBox::Items::Equipment>(j, "equipment");
        x.examine             = OSRSBox::Items::get_optional<std::string>(j, "examine");
        x.highalch            = OSRSBox::Items::get_optional<int32_t>(j, "highalch");
        x.icon                = j.at("icon").get<std::string>();
        x.id                  = j.at("id").get<int32_t>();
        x.incomplete          = j.at("incomplete").get<bool>();
        x.linkedIdItem        = OSRSBox::Items::get_optional<int32_t>(j, "linked_id_item");
        x.linkedIdNoted       = OSRSBox::Items::get_optional<int32_t>(j, "linked_id_noted");
        x.linkedIdPlaceholder = OSRSBox::Items::get_optional<int32_t>(j, "linked_id_placeholder");
        x.lowalch             = OSRSBox::Items::get_optional<int32_t>(j, "lowalch");
        x.members             = j.at("members").get<bool>();
        x.name                = j.at("name").get<std::string>();
        x.noteable            = j.at("noteable").get<bool>();
        x.noted               = j.at("noted").get<bool>();
        x.placeholder         = j.at("placeholder").get<bool>();
        x.questItem           = j.at("quest_item").get<bool>();
        x.releaseDate         = OSRSBox::Items::get_optional<std::string>(j, "release_date");
        x.stackable           = j.at("stackable").get<bool>();
        x.stacked             = OSRSBox::Items::get_optional<int32_t>(j, "stacked");
        x.tradeable           = j.at("tradeable").get<bool>();
        x.tradeableOnGe       = j.at("tradeable_on_ge").get<bool>();
        x.weapon              = OSRSBox::Items::get_optional<OSRSBox::Items::Weapon>(j, "weapon");
        x.weight              = OSRSBox::Items::get_optional<double>(j, "weight");
        x.wikiExchange        = OSRSBox::Items::get_optional<std::string>(j, "wiki_exchange");
        x.wikiName            = OSRSBox::Items::get_optional<std::string>(j, "wiki_name");
        x.wikiUrl             = OSRSBox::Items::get_optional<std::string>(j, "wiki_url");
    }

    inline void to_json(json& j, const OSRSBox::Items::Item& x)
    {
        j = json::object();
        j["buy_limit"]             = x.buyLimit;
        j["cost"]                  = x.cost;
        j["duplicate"]             = x.duplicate;
        j["equipable"]             = x.equipable;
        j["equipable_by_player"]   = x.equipableByPlayer;
        j["equipable_weapon"]      = x.equipableWeapon;
        j["equipment"]             = x.equipment;
        j["examine"]               = x.examine;
        j["highalch"]              = x.highalch;
        j["icon"]                  = x.icon;
        j["id"]                    = x.id;
        j["incomplete"]            = x.incomplete;
        j["linked_id_item"]        = x.linkedIdItem;
        j["linked_id_noted"]       = x.linkedIdNoted;
        j["linked_id_placeholder"] = x.linkedIdPlaceholder;
        j["lowalch"]               = x.lowalch;
        j["members"]               = x.members;
        j["name"]                  = x.name;
        j["noteable"]              = x.noteable;
        j["noted"]                 = x.noted;
        j["placeholder"]           = x.placeholder;
        j["quest_item"]            = x.questItem;
        j["release_date"]          = x.releaseDate;
        j["stackable"]             = x.stackable;
        j["stacked"]               = x.stacked;
        j["tradeable"]             = x.tradeable;
        j["tradeable_on_ge"]       = x.tradeableOnGe;
        j["weapon"]                = x.weapon;
        j["weight"]                = x.weight;
        j["wiki_exchange"]         = x.wikiExchange;
        j["wiki_name"]             = x.wikiName;
        j["wiki_url"]              = x.wikiUrl;

    }

    inline void from_json(const json& j, OSRSBox::Items::Slot& x)
    {
        if (j == "ammo") x = OSRSBox::Items::Slot::Ammo;
        else if (j == "body") x = OSRSBox::Items::Slot::Body;
        else if (j == "cape") x = OSRSBox::Items::Slot::Cape;
        else if (j == "feet") x = OSRSBox::Items::Slot::Feet;
        else if (j == "hands") x = OSRSBox::Items::Slot::Hands;
        else if (j == "head") x = OSRSBox::Items::Slot::Head;
        else if (j == "legs") x = OSRSBox::Items::Slot::Legs;
        else if (j == "neck") x = OSRSBox::Items::Slot::Neck;
        else if (j == "ring") x = OSRSBox::Items::Slot::Ring;
        else if (j == "shield") x = OSRSBox::Items::Slot::Shield;
        else if (j == "2h") x = OSRSBox::Items::Slot::TwoHanded;
        else if (j == "weapon") x = OSRSBox::Items::Slot::Weapon;
        else throw "Input JSON does not conform to schema";
    }

    inline void to_json(json& j, const OSRSBox::Items::Slot& x)
    {
        switch (x)
        {
            case OSRSBox::Items::Slot::Ammo: j = "ammo"; break;
            case OSRSBox::Items::Slot::Body: j = "body"; break;
            case OSRSBox::Items::Slot::Cape: j = "cape"; break;
            case OSRSBox::Items::Slot::Feet: j = "feet"; break;
            case OSRSBox::Items::Slot::Hands: j = "hands"; break;
            case OSRSBox::Items::Slot::Head: j = "head"; break;
            case OSRSBox::Items::Slot::Legs: j = "legs"; break;
            case OSRSBox::Items::Slot::Neck: j = "neck"; break;
            case OSRSBox::Items::Slot::Ring: j = "ring"; break;
            case OSRSBox::Items::Slot::Shield: j = "shield"; break;
            case OSRSBox::Items::Slot::TwoHanded: j = "2h"; break;
            case OSRSBox::Items::Slot::Weapon: j = "weapon"; break;
            default: throw "This should not happen";
        }
    }
}

#endif // TEST_HPP_INCLUDED