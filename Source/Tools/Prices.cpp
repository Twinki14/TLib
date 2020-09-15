#include "../../Include/Tools/Prices.hpp"
#include "../../Include/Tools/json11.hpp"
#include "../../Include/TScript.hpp"
#include <spdlog/fmt/bundled/chrono.h>
#include "../../Include/Tools/WinnHttpComm.hpp"

Timer Prices::LastUpdated = Timer(false);
std::map<std::int32_t, Prices::ItemPriceInfo> Prices::PriceCache;

void Prices::Update()
{
    std::vector<std::int32_t> ItemIDs;
    for (auto& I : Prices::PriceCache)
        ItemIDs.emplace_back(I.first);

    Prices::Cache(ItemIDs);

    auto Time = std::time(nullptr);//std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    DebugLog("Updated {} items at {}", Prices::PriceCache.size(), fmt::format("{%H:%M:%S}", *std::localtime(&Time)).c_str());

    LastUpdated.Restart();
    LastUpdated.Resume();
}

Prices::ItemPriceInfo Prices::Get(std::int32_t ItemID, bool CheckCache, bool UpdateCache)
{
    if (ItemID < 0) return Prices::ItemPriceInfo();

    if (CheckCache)
    {
        if (!UpdateCache && Prices::PriceCache.count(ItemID))
        {
            //DebugLog << "Found " << ItemID << " [" << Prices::PriceCache[ItemID].Name << "]" << " in cache with overall average " << Prices::PriceCache[ItemID].OverallAverage << std::endl;
            return Prices::PriceCache[ItemID];
        }

        Prices::Cache(ItemID);
        if (Prices::PriceCache.count(ItemID))
            return Prices::PriceCache[ItemID];
    } else
        return GrabItem(ItemID);
    return Prices::ItemPriceInfo();
}

Prices::ItemPriceInfo Prices::Get(const std::string& Name, bool CheckCache, bool UpdateCache)
{
    if (CheckCache)
    {
        std::int32_t ItemID = -1;
        for (const auto& I : Prices::PriceCache)
        {
            if (I.second.Name == Name)
            {
                ItemID = I.second.ID;
                break;
            }
        }

        if (ItemID > 0)
        {
            if (!UpdateCache && Prices::PriceCache.count(ItemID))
            {
                //DebugLog << "Found " << ItemID << " [" << Prices::PriceCache[ItemID].Name << "]" << " in cache with overall average " << Prices::PriceCache[ItemID].OverallAverage << std::endl;
                return Prices::PriceCache[ItemID];
            }

            Prices::Cache(ItemID);
            if (Prices::PriceCache.count(ItemID))
                return Prices::PriceCache[ItemID];
        } else
        {
            Prices::Cache(Name);
            for (const auto& I : Prices::PriceCache)
            {
                if (I.second.Name == Name)
                {
                    ItemID = I.second.ID;
                    break;
                }
            }
            if (Prices::PriceCache.count(ItemID))
                return Prices::PriceCache[ItemID];
        }
    } else
        return Prices::GrabItem(Name);
    return Prices::ItemPriceInfo();
}

bool Prices::InCache(std::int32_t ItemID)
{
    return Prices::PriceCache.count(ItemID) > 0;
}

bool Prices::InCache(const std::string& Name)
{
    for (const auto& I : Prices::PriceCache)
        if (I.second.Name == Name)
            return true;
    return false;
}

void Prices::Cache(std::int32_t ItemID)
{
    if (ItemID < 0) return;


    auto Item = Prices::GrabItem(ItemID);
    if (Item.ID < 0) return;

    //DebugLog << ((Prices::PriceCache.count(ItemID) > 0) ? ("Updated [") : ("Cached [")) << Item.Name << "] [" << Item.ID << "] into cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
    DebugLog("{} [{}] [{}] into cache [{}] [{}]", ((Prices::PriceCache.count(ItemID) > 0) ? ("Updated") : ("Cached")), Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
    Prices::PriceCache[ItemID] = std::move(Item);
}

void Prices::Cache(const std::string& Name)
{
    std::int32_t ItemID = -1;
    for (const auto& I : Prices::PriceCache)
    {
        if (I.second.Name == Name)
        {
            ItemID = I.second.ID;
            break;
        }
    }

    if (ItemID > 0)
        Prices::PriceCache[ItemID] = Prices::GrabItem(ItemID);
    else
    {
        auto Item = Prices::GrabItem(Name);
        if (Item.ID < 0) return;
        //DebugLog << ((ItemID > 0) ? ("Updated [") : ("Cached [")) << Item.Name << "] [" << Item.ID << "] into cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
        DebugLog("{} [{}] [{}] into cache [{}] [{}]", ((ItemID > 0) ? ("Updated") : ("Cached")), Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
        Prices::PriceCache[Item.ID] = std::move(Item);
    }
}

void Prices::Cache(const std::vector<std::int32_t>& ItemIDs)
{
    auto Items = Prices::GrabItems(ItemIDs);
    for (auto& I : Items)
        Prices::PriceCache[I.ID] = std::move(I);
}

void Prices::Cache(const std::vector<std::string>& Names)
{
    auto Items = Prices::GrabItems(Names);
    for (auto& I : Items)
        Prices::PriceCache[I.ID] = std::move(I);
}

void Prices::ForceEmplaceCache(std::int32_t ID, Prices::ItemPriceInfo Item)
{
    if (ID > -1)
    {
        if (!Prices::PriceCache.count(ID))
            DebugLog("Forced [{}] [{}] into cache [{}] [{}]", Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
            //DebugLog << "Forced [" << Item.Name << "] [" << Item.ID << "] into cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
        else
            DebugLog("Forced updated [{}] [{}] into cache [{}] [{}]", Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
            //DebugLog << "Force updated [" << Item.Name << "] [" << Item.ID << "] in cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
        Prices::PriceCache[ID] = std::move(Item);
    }
}

std::uint32_t Prices::GetHighAlchemyCost(std::int32_t FireRuneID, bool HasFireStaff)
{
    auto NatureRuneCost = Prices::Get(561, true).AveragePrice;
    std::uint32_t FireRuneCost = HasFireStaff ? 0 : Prices::Get(FireRuneID, true).AveragePrice * 5;
    return NatureRuneCost + FireRuneCost;
}

Timer Prices::GetLastUpdated()
{
    return Prices::LastUpdated;
}

std::uint32_t Prices::GetCacheSize()
{
    return Prices::PriceCache.size();
}

Prices::ItemPriceInfo Prices::GrabItem(std::int32_t ID)
{
    if (ID == 995)
        return Prices::ItemPriceInfo { 995, "Coins", 1, 1, 1, 1, 0 };

    const auto Prices = Prices::GrabItems(std::vector<std::int32_t> { ID });
    if (!Prices.empty())
        return Prices[0];
    return Prices::ItemPriceInfo { -1, "" };
}

Prices::ItemPriceInfo Prices::GrabItem(const std::string& Name)
{
    if (Name == "Coins")
        return Prices::ItemPriceInfo { 995, "Coins", 1, 1, 1, 1, 0 };

    const auto Prices = Prices::GrabItems(std::vector<std::string> { Name });
    if (!Prices.empty())
        return Prices[0];
    return Prices::ItemPriceInfo { -1, "", 0, 0 };
}

std::vector<Prices::ItemPriceInfo> Prices::GrabItems(const std::vector<std::int32_t>& IDs)
{
    std::vector<Prices::ItemPriceInfo> Result;
    if (IDs.empty()) return Result;

    Timer T;
    WinnHttpComm::Request Req;
    Req.Host = "rsbuddy.com";
    Req.Method = "GET";
    Req.URI = "/exchange/summary.json";
    const auto Response = WinnHttpComm::Handle(Req);
    std::string Content = Response.Content;

    std::string Err;
    json11::Json Json = json11::Json::parse(Content, Err);
    if (!Json.is_null())
    {
        for (const auto& ID : IDs)
        {
            if (ID < 0) continue;

            bool UsedGraphs = false;
            Prices::ItemPriceInfo Item;

            json11::Json ItemJson = Json[std::to_string(ID)];
            if (!ItemJson.is_null())
            {
                Item.ID                 = ItemJson["id"].int_value();
                Item.Name               = ItemJson["name"].string_value();
                Item.AverageBuyOffer    = (std::uint32_t) ItemJson["buy_average"].int_value();
                Item.AverageSellOffer   = (std::uint32_t) ItemJson["sell_average"].int_value();
                Item.AveragePrice       = (std::uint32_t) ItemJson["overall_average"].int_value();
                Item.StorePrice         = (std::uint32_t) ItemJson["sp"].int_value();
                Item.HighAlchemyValue   = Item.StorePrice * 0.6f;
                if (Item.AveragePrice == 0)
                {
                    UsedGraphs = true;
                    Prices::GrabLatestOSBuddy(&Item);
                } else
                {
                    if (Item.AverageBuyOffer == 0) Item.AverageBuyOffer = Item.AveragePrice;
                    if (Item.AverageSellOffer == 0) Item.AverageSellOffer = Item.AveragePrice;
                }
            } else
            {
                if (ID == 995)
                {
                    Item.ID = ID;
                    Item.Name = "Coins";
                    Item.AverageBuyOffer    = 1;
                    Item.AverageSellOffer   = 1;
                    Item.AveragePrice       = 1;
                    Item.StorePrice         = 1;
                    Item.HighAlchemyValue   = 0;
                }
            }

            DebugLog("Grabbed {}[{}] [{}] into cache [{}] [{}]", ((UsedGraphs) ? ("[Graphs] > ") : ""), Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
            //DebugLog << "Grabbed " <<  ((UsedGraphs) ? ("[Graphs] > [") : "[") << Item.Name << "] [" << Item.ID << "] into cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
            Result.emplace_back(std::move(Item));
        }
    }

    DebugLog("Grabbed {} items - Took {}ms", Result.size(), T.GetTimeElapsed());
    //DebugLog << "Grabbed " << Result.size() << " items - Took " << T.GetTimeElapsed() << "ms" << std::endl;
    return Result;
}

std::vector<Prices::ItemPriceInfo> Prices::GrabItems(const std::vector<std::string>& Names)
{
    std::vector<Prices::ItemPriceInfo> Result;
    if (Names.empty()) return Result;

    Timer T;
    WinnHttpComm::Request Req;
    Req.Host = "rsbuddy.com";
    Req.Method = "GET";
    Req.URI = "/exchange/summary.json";
    const auto Response = WinnHttpComm::Handle(Req);
    std::string Content = Response.Content;

    bool Coins = false;
    std::string Err;
    json11::Json Json = json11::Json::parse(Content, Err);
    if (!Json.is_null())
    {
        for (const auto& Object : Json.object_items())
        {
            if (!Object.second.is_null())
            {
                bool Found = false;
                bool UsedGraphs = false;
                Prices::ItemPriceInfo Item;

                for (const auto& Name : Names)
                {
                    if (Name.empty())
                        continue;

                    if (Name == "Coins")
                    {
                        Coins = true;
                        continue;
                    }

                    if (Object.second["name"].string_value() == Name)
                    {
                        Item.ID                 = Object.second["id"].int_value();
                        Item.Name               = Object.second["name"].string_value();
                        Item.AverageBuyOffer    = (std::uint32_t) Object.second["buy_average"].int_value();
                        Item.AverageSellOffer   = (std::uint32_t) Object.second["sell_average"].int_value();
                        Item.AveragePrice       = (std::uint32_t) Object.second["overall_average"].int_value();
                        Item.StorePrice         = (std::uint32_t) Object.second["sp"].int_value();
                        Item.HighAlchemyValue   = Item.StorePrice * 0.6f;
                        if (Item.AveragePrice == 0)
                        {
                            UsedGraphs = true;
                            Prices::GrabLatestOSBuddy(&Item);
                        }

                        Found = true;
                        break;
                    }
                }

                if (!Found)
                    continue;

                DebugLog("Grabbed {}[{}] [{}] into cache [{}] [{}]", ((UsedGraphs) ? ("[Graphs] > ") : ""), Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
                //DebugLog << "Grabbed " <<  ((UsedGraphs) ? ("[Graphs] > [") : "[") << Item.Name << "] [" << Item.ID << "] into cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
                Result.emplace_back(std::move(Item));
            }
        }
    }

    if (Coins)
    {
        Prices::ItemPriceInfo Item;
        Item.ID = 995;
        Item.Name = "Coins";
        Item.AverageBuyOffer    = 1;
        Item.AverageSellOffer   = 1;
        Item.AveragePrice       = 1;
        Item.StorePrice         = 1;
        Item.HighAlchemyValue   = 0;
        DebugLog("Grabbed [{}] [{}] into cache [{}] [{}]", Item.Name, Item.ID, Item.AveragePrice, Item.HighAlchemyValue);
        //DebugLog << "Grabbed [" << Item.Name << "] [" << Item.ID << "] into cache [" << Item.AveragePrice << "] [" << Item.HighAlchemyValue << "]" << std::endl;
        Result.emplace_back(std::move(Item));
    }

    DebugLog("Grabbed {} items - Took {}ms", Result.size(), T.GetTimeElapsed());
    //DebugLog << "Grabbed " << Result.size() << " items - Took " << T.GetTimeElapsed() << "ms" << std::endl;
    return Result;
}

void Prices::GrabLatestOSBuddy(Prices::ItemPriceInfo* Item)
{
    if (!Item) return;
    if (Item->ID == 995 || Item->Name == "Coins")
    {
        Item->ID = 995;
        Item->Name = "Coins";
        Item->AverageBuyOffer    = 1;
        Item->AverageSellOffer   = 1;
        Item->AveragePrice       = 1;
        Item->StorePrice         = 1;
        Item->HighAlchemyValue   = 0;
        return;
    }

    WinnHttpComm::Request Req;
    Req.Host = "rsbuddy.com";
    Req.Method = "GET";
    Req.URI = "/exchange/graphs/1440/";
    Req.URI += std::to_string(Item->ID) + ".json";
    const auto Response = WinnHttpComm::Handle(Req);
    std::string Content = Response.Content;

    std::string Err;
    json11::Json Json = json11::Json::parse(Content, Err);

    if (!Json.is_null())
    {
        const auto& ArrayItems = Json.array_items();
        for (auto I = ArrayItems.rbegin(); I != ArrayItems.rend(); I++)
        {
            const auto& ObjectItems = I->object_items();
            if (!ObjectItems.empty())
            {
                bool HasBuying = ObjectItems.count("buyingPrice") > 0;
                bool HasOverall = ObjectItems.count("overallPrice") > 0;
                bool HasSelling = ObjectItems.count("sellingPrice") > 0;

                Item->AverageBuyOffer = (HasBuying) ? (ObjectItems.at("buyingPrice").int_value()) : (0);
                Item->AveragePrice = (HasOverall) ? (ObjectItems.at("overallPrice").int_value()) : (0);
                Item->AverageSellOffer = (HasSelling) ? (ObjectItems.at("sellingPrice").int_value()) : (0);

                if (Item->AveragePrice == 0)
                    Item->AveragePrice = (Item->AverageBuyOffer + Item->AverageSellOffer) / 2;

                if (Item->AveragePrice > 0)
                {
                    if (Item->AverageBuyOffer == 0) Item->AverageBuyOffer = Item->AveragePrice;
                    if (Item->AverageSellOffer == 0) Item->AverageSellOffer = Item->AveragePrice;
                    return;
                }
            }
        }
    }
}
