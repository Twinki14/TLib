#include "../../Include/TScript.hpp"
#include "../../Include/Utilities/Bank.hpp"
#include <Core/Internal.hpp>
#include <Game/Tools/Settings.hpp>
#include <Game/Tools/Widgets.hpp>

bool SmartClick(const Interactable::Item& Item, std::int32_t Amount, std::int32_t StopAt, bool Deposit, bool OpenRespectiveTab = false)
{
    std::uint32_t Clicked = 0;
    Countdown Failsafe = Countdown(10000);
    while (Failsafe)
    {
        if (Clicked == StopAt)
            return true;

        if (Clicked > StopAt)
            return false;

        if (!Item.Exists())
            return true;

        if (Deposit)
        {
            if (Bank::Deposit(Item, Amount))
                Clicked++;
        } else
        {
            if (Bank::Withdraw(Item, Amount, OpenRespectiveTab))
                Clicked++;
        }
    }
    return false;
}

namespace Bank
{
    bool SmartWithdraw(const Interactable::Item& Item, std::int32_t Amount, bool OpenRespectiveTab)
    {
        if (Amount == 1 || Amount == 5 || Amount == 10 || Amount == Bank::GetDefaultWithdrawQuantity())
        {
            return Bank::Withdraw(Item, Amount, OpenRespectiveTab);
        } else
        {
            if (Amount < 5) return SmartClick(Item, 1, Amount, false, OpenRespectiveTab);

            double Fives = (double) Amount / 5;
            double Tens = (double) Amount / 10;

            std::uint32_t FivesCount = std::floor(Fives);
            std::uint32_t TensCount = std::floor(Tens);

            bool DoFives = FivesCount > 0 && Fives < 3.0;
            bool DoTens = TensCount > 0 && Tens < 3.0;

            if (DoTens)
            {
                std::uint32_t OnesCount =  std::floor((Tens * 10) - (TensCount * 10));
                if (OnesCount >= 5)
                {
                    if (OnesCount == 5)
                        return SmartClick(Item, 10, TensCount, false, OpenRespectiveTab) && SmartClick(Item, 5, OnesCount, false, OpenRespectiveTab);
                    return SmartClick(Item, 10, TensCount, false, OpenRespectiveTab) && SmartClick(Item, 5, 1, false, OpenRespectiveTab) && SmartClick(Item, 1, OnesCount - 5, false, OpenRespectiveTab);
                }
                return SmartClick(Item, 10, TensCount, false, OpenRespectiveTab) && SmartClick(Item, 1, OnesCount, false, OpenRespectiveTab);
            } else if (DoFives)
            {
                std::uint32_t OnesCount =  std::floor((Fives * 5) - (FivesCount * 5));
                return SmartClick(Item, 5, FivesCount, false, OpenRespectiveTab) && SmartClick(Item, 1, OnesCount, false, OpenRespectiveTab);
            }
        }
        return false;
    }

    bool SmartDeposit(const Interactable::Item& LastItem, std::int32_t Amount)
    {
        if (Amount == 1 || Amount == 5 || Amount == 10 || Amount == Bank::GetDefaultWithdrawQuantity())
        {
            return Bank::Deposit(LastItem, Amount);
        } else
        {
            //if (Amount < 5) return SmartClick(LastItem, 1, Amount, true);

            double Fives = (double) Amount / 5;
            double Tens = (double) Amount / 10;

            std::uint32_t FivesCount = std::floor(Fives);
            std::uint32_t TensCount = std::floor(Tens);

            bool DoFives = FivesCount > 0 && Fives < 3.0;
            bool DoTens = TensCount > 0 && Tens < 3.0;

            if (Amount < 5)
            {
                if (!DoFives) DoFives = UniformRandom() <= 0.65;
                if (!DoTens) DoTens = UniformRandom() <= 0.65;
            }

            if (DoTens)
            {
                std::uint32_t OnesCount =  std::floor((Tens * 10) - (TensCount * 10));
                if (OnesCount >= 5)
                {
                    if (OnesCount == 5)
                        return SmartClick(LastItem, 10, TensCount, true) && SmartClick(LastItem, 5, OnesCount, true);
                    return SmartClick(LastItem, 10, TensCount, true) && SmartClick(LastItem, 5, 1, true) && SmartClick(LastItem, 1, OnesCount - 5, true);
                }
                return SmartClick(LastItem, 10, TensCount, true) && SmartClick(LastItem, 1, OnesCount, true);
            } else if (DoFives)
            {
                std::uint32_t OnesCount =  std::floor((Fives * 5) - (FivesCount * 5));
                return SmartClick(LastItem, 5, FivesCount, true) && SmartClick(LastItem, 1, OnesCount, true);
            }
        }
        return false;
    }

    bool ProfileDepositAll(const std::vector<Interactable::Item>& Items, std::int32_t TotalItemCount)
    {
        //DebugLog << "Items: " << Items.size() << " | Amount: " << Amount << std::endl;
        if (Items.empty() || TotalItemCount <= 0)
            return false;

        const auto DefaultWithdrawQuant = Bank::GetDefaultWithdrawQuantity();
        const auto WithdrawXQuant = Bank::GetWithdrawXQuantity();

        if (DefaultWithdrawQuant == Bank::ALL || DefaultWithdrawQuant >= TotalItemCount)
        {
            //DebugLog << "Depositing with default withdraw" << std::endl;
            auto Item = &Items[0];
            if (Items.size() > 1) Item = &Items[UniformRandom(0, Items.size() - 1)];
            return Bank::Deposit(*Item, DefaultWithdrawQuant);
        }

        bool DepositWithFive = false;
        bool DepositWithTen = false;
        bool DepositWithAll = false;
        bool DepositWithSpamClick = false;
        bool DepositWithWithdrawX = false;

        if (TotalItemCount <= 5)
        {
            //DebugLog << "'<= 5'" << std::endl;

            DepositWithFive = UniformRandom() <= 0.55;
            DepositWithTen = UniformRandom() <= 0.25;
            DepositWithAll = UniformRandom() <= 0.10;
            DepositWithSpamClick = UniformRandom() <= 0.15;

            if (TotalItemCount <= 3)
            {
                DepositWithFive = UniformRandom() <= 0.55;
                DepositWithTen = UniformRandom() <= 0.15;
                DepositWithAll = UniformRandom() <= 0.05;
                DepositWithSpamClick = UniformRandom() <= 0.15;
            }

            if (TotalItemCount == 1)
            {
                DepositWithFive = UniformRandom() <= 0.15;
                DepositWithTen = UniformRandom() <= 0.05;
                DepositWithAll = UniformRandom() <= 0.05;
                if (!DepositWithFive && !DepositWithTen && !DepositWithAll)
                    DepositWithSpamClick = true;
            }

            if (!DepositWithFive && !DepositWithTen && !DepositWithAll && !DepositWithSpamClick)
                DepositWithFive = true;

        } else if (TotalItemCount > 5 && TotalItemCount <= 10)
        {
           // DebugLog << "'> 5' && '<= 10'" << std::endl;

            DepositWithFive = UniformRandom() <= 0.25;
            DepositWithTen = UniformRandom() <= 0.45;
            DepositWithAll = UniformRandom() <= 0.10;
            DepositWithWithdrawX = ((WithdrawXQuant >= TotalItemCount) && (UniformRandom() <= 0.10));
            if (!DepositWithFive && !DepositWithTen && !DepositWithAll && !DepositWithWithdrawX)
                DepositWithTen = true;

        } else if (TotalItemCount <= 15)
        {
            //DebugLog << "'<= 15'" << std::endl;
            DepositWithFive = UniformRandom() <= 0.15;
            DepositWithTen = UniformRandom() <= 0.35;
            DepositWithAll = UniformRandom() <= 0.45;
            DepositWithWithdrawX = ((WithdrawXQuant >= TotalItemCount) && (UniformRandom() <= 0.35));
            if (!DepositWithFive && !DepositWithTen && !DepositWithAll && !DepositWithWithdrawX)
                DepositWithAll = true;
        } else
        {
            //DebugLog << "'else'" << std::endl;
            DepositWithWithdrawX = ((WithdrawXQuant >= TotalItemCount) && (UniformRandom() <= 0.85));
            if (!DepositWithWithdrawX) DepositWithAll = true;
        }

        //DebugLog << "DepositWithFive: " << ((DepositWithFive) ? "true" : "false") << std::endl;
        //DebugLog << "DepositWithTen: " << ((DepositWithTen) ? "true" : "false") << std::endl;
        //DebugLog << "DepositWithAll: " << ((DepositWithAll) ? "true" : "false") << std::endl;
        //DebugLog << "DepositWithSpamClick: " << ((DepositWithSpamClick) ? "true" : "false") << std::endl;
        //DebugLog << "DepositWithWithdrawX: " << ((DepositWithWithdrawX) ? "true" : "false") << std::endl;

        std::int32_t DepositAmount = 5;
        if (DepositWithFive) DepositAmount = 5;
        if (DepositWithTen) DepositAmount = 10;
        if (DepositWithAll) DepositAmount = Bank::ALL;
        if (DepositWithWithdrawX) DepositAmount = WithdrawXQuant;

        //DebugLog << "DepositAmount: " << DepositAmount << std::endl;

        if (DepositWithSpamClick)
        {
            return SmartClick(Items.back(), 1, UniformRandom(Items.size(), Items.size() + 4), true);
        } else if (TotalItemCount <= 5)
        {
            auto Item = &Items[0];
            if (Items.size() > 1) Item = &Items[UniformRandom(0, Items.size() - 1)];
            return Bank::Deposit(*Item, DepositAmount);
        } else if (TotalItemCount > 5 && TotalItemCount <= 10)
        {
            if (DepositWithFive)
            {
                auto ToClick = static_cast<uint32_t>(std::ceil(Items.size() / 5));
                return SmartClick(Items.back(), 1, UniformRandom(ToClick, ToClick + 3), true);
            } else
            {
                auto Item = &Items[0];
                if (Items.size() > 1) Item = &Items[UniformRandom(0, Items.size() - 1)];
                return Bank::Deposit(*Item, DepositAmount);
            }
        } else if (TotalItemCount <= 15)
        {
            if (DepositWithFive || DepositWithTen)
            {
                std::uint32_t ToClick = 1;
                if (DepositWithFive) ToClick = static_cast<uint32_t>(std::ceil(Items.size() / 5));
                if (DepositWithTen) ToClick = static_cast<uint32_t>(std::ceil(Items.size() / 10));
                return SmartClick(Items.back(), 5, UniformRandom(ToClick, ToClick + 2), true);
            } else
            {
                auto Item = &Items[0];
                if (Items.size() > 1) Item = &Items[UniformRandom(0, Items.size() - 1)];
                return Bank::Deposit(*Item, DepositAmount);
            }
        } else
        {
            if (DepositAmount != Bank::ALL && DepositAmount != WithdrawXQuant)
            {
                DepositAmount = Bank::ALL;
                //DebugLog << "Adjusted DepositAmount: " << DepositAmount << std::endl;
            }
            auto Item = &Items[0];
            if (Items.size() > 1) Item = &Items[UniformRandom(0, Items.size() - 1)];
            return Bank::Deposit(*Item, DepositAmount);
        }
    }

    std::int32_t GetWithdrawXQuantity()
    {
        return (std::int32_t)(((std::uint32_t) Settings::GetSetting(Globals::SETTING_BANK_DEFAULT_QUANTITY.Index)) >> 1);
    }

    std::vector<CONTAINER_ITEM> GetContainerItems(bool IncludeEmpty)
    {
        std::vector<CONTAINER_ITEM> Result;

        if (!Bank::IsOpen())
            return Result;

        const auto ItemIDs = Bank::GetItemIDs();
        const auto ItemAmounts = Bank::GetItemAmounts();

        if (ItemIDs.size() != ItemAmounts.size())
            return Result;

        for (std::uint32_t I = 0; I < ItemIDs.size(); I++)
        {
            if (ItemIDs[I] > -1)
            {
                std::string Name;
                const auto Info = Internal::GetItemInfo(ItemIDs[I]);
                if (Info)
                    Name = Info.GetName();
                else
                    Name = Internal::GetItemName(ItemIDs[I]);

                Result.emplace_back( CONTAINER_ITEM { ItemIDs[I], std::move(Name), ItemAmounts[I], I } );
            } else if (IncludeEmpty)
            {
                Result.emplace_back( CONTAINER_ITEM { ItemIDs[I], "", ItemAmounts[I], I } );
            }
        }

        return Result;
    }

    std::int32_t Count(std::int32_t ID, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::int32_t Count = 0;
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
            if (ItemID == ID)
                Count += ItemAmount;
        return Count;
    }

    std::int32_t Count(const std::string& Name, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::int32_t Count = 0;
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
            if (ItemName == Name)
                Count += ItemAmount;
        return Count;
    }

    std::int32_t Count(const Interactable::Item& Item, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        if (Item.GetSource() != Interactable::Item::BANK)
            return 0;

        std::int32_t Count = 0;
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
            if (ItemID == Item.GetID())
                Count += ItemAmount;

        return Count;
    }

    std::vector<std::int32_t> CountIndividual(const std::vector<std::int32_t>& IDs, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::vector<std::int32_t> Result = std::vector<std::int32_t>(IDs.size(), 0);
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
        {
            if (ItemID < 0) continue;
            for (std::uint32_t I = 0; I < IDs.size(); I++)
                if (ItemID == IDs[I])
                    Result[I] += ItemAmount;
        }
        return Result;
    }

    std::vector<std::int32_t> CountIndividual(const std::vector<std::string>& Names, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::vector<std::int32_t> Result = std::vector<std::int32_t>(Names.size(), 0);
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
        {
            if (ItemID < 0) continue;
            for (std::uint32_t I = 0; I < Names.size(); I++)
                if (ItemName == Names[I])
                    Result[I] += ItemAmount;
        }
        return Result;
    }

    std::vector<std::int32_t> CountIndividual(const std::vector<Interactable::Item>& Items, const std::vector<CONTAINER_ITEM>& ContainerItems)
    {
        std::vector<std::int32_t> Result = std::vector<std::int32_t>( Items.size(), 0);
        for (const auto& [ItemID, ItemName, ItemAmount, Index] : ContainerItems)
        {
            if (ItemID < 0) continue;
            for (std::uint32_t I = 0; I < Items.size(); I++)
            {
                if (Items[I].GetSource() != Interactable::Item::BANK)
                    continue;
                if (ItemID == Items[I].GetID())
                    Result[I] += ItemAmount;
            }
        }
        return Result;
    }

    Interactable::Widget GetFirstItemWidget()
    {
        const auto Items = Widgets::Get(Globals::BANK_ITEM_CONTAINER_WIDGET.Parent, Globals::BANK_ITEM_CONTAINER_WIDGET.Child).GetInteractableChildren();
        auto First = std::find_if(Items.begin(), Items.end(), [](const Interactable::Widget& W) -> bool { return W.IsVisible(); });
        return First != Items.end() ?  *First : Interactable::Widget();
    }

}; // namespace Bank