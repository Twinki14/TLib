#ifndef TLIB_PRICES_HPP_INCLUDED
#define TLIB_PRICES_HPP_INCLUDED

#include <Core/Types/Timer.hpp>
#include <map>
#include <vector>

class Prices
{
public:

    typedef struct ItemPriceInfo
    {
        std::int32_t ID = -1;
        std::string Name;
        std::uint32_t AverageBuyOffer = 0;
        std::uint32_t AverageSellOffer = 0;
        std::uint32_t AveragePrice = 0;
        std::uint32_t StorePrice = 0;
        std::uint32_t HighAlchemyValue = 0;
    } ItemPriceInfo;

    static void Update();
    static ItemPriceInfo Get(std::int32_t ItemID, bool CheckCache = true, bool UpdateCache = false);
    static ItemPriceInfo Get(const std::string& Name, bool CheckCache = true, bool UpdateCache = false);

    static bool InCache(std::int32_t ItemID);
    static bool InCache(const std::string& Name);

    // Will only cache the items
    static void Cache(std::int32_t ItemID);
    static void Cache(const std::string& Name);
    static void Cache(const std::vector<std::int32_t>& ItemIDs);
    static void Cache(const std::vector<std::string>& Names);

    static void ForceEmplaceCache(std::int32_t ID, ItemPriceInfo Item);

    static std::uint32_t GetHighAlchemyCost(std::int32_t FireRuneID, bool HasFireStaff = false);

    static Timer GetLastUpdated();
    static std::uint32_t GetCacheSize();
    static std::map<std::int32_t, ItemPriceInfo> PriceCache;
private:
    static Timer LastUpdated;
    //static std::map<std::int32_t, ItemPriceInfo> PriceCache;

    static ItemPriceInfo GrabItem(std::int32_t ID);
    static ItemPriceInfo GrabItem(const std::string& Name);
    static std::vector<ItemPriceInfo> GrabItems(const std::vector<std::int32_t>& IDs);
    static std::vector<ItemPriceInfo> GrabItems(const std::vector<std::string>& Names);

    static void GrabLatestOSBuddy(Prices::ItemPriceInfo* Item);
};

#endif // TLIB_PRICES_HPP_INCLUDED