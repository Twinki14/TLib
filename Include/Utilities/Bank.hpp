#ifndef TLIB_BANK_HPP_INCLUDED
#define TLIB_BANK_HPP_INCLUDED

#include <Game/Interfaces/Bank.hpp>
#include <Game/Interactable/Widget.hpp>

#ifndef T_CONTAINER_ITEM_STRUCT
#define T_CONTAINER_ITEM_STRUCT
typedef struct CONTAINER_ITEM
{
    std::int32_t ID;
    std::string Name;
    std::int32_t Amount;
    std::uint32_t Index;
} CONTAINER_ITEM;
#endif

namespace Bank
{
    bool SmartWithdraw(const Interactable::Item& Item, std::int32_t Amount, bool OpenRespectiveTab = true);
    bool SmartDeposit(const Interactable::Item& LastItem, std::int32_t Amount);

    bool ProfileDepositAll(const std::vector<Interactable::Item>& Items, std::int32_t TotalItemCount);

    std::int32_t GetWithdrawXQuantity();

    std::vector<CONTAINER_ITEM> GetContainerItems(bool IncludeEmpty = true);

    std::int32_t Count(std::int32_t ID, const std::vector<CONTAINER_ITEM>& ContainerItems);
    std::int32_t Count(const std::string& Name, const std::vector<CONTAINER_ITEM>& ContainerItems);
    std::int32_t Count(const Interactable::Item& Item, const std::vector<CONTAINER_ITEM>& ContainerItems);

    std::vector<std::int32_t> CountIndividual(const std::vector<std::int32_t>& IDs, const std::vector<CONTAINER_ITEM>& ContainerItems);
    std::vector<std::int32_t> CountIndividual(const std::vector<std::string>& Names, const std::vector<CONTAINER_ITEM>& ContainerItems);
    std::vector<std::int32_t> CountIndividual(const std::vector<Interactable::Item>& Items, const std::vector<CONTAINER_ITEM>& ContainerItems);

    Interactable::Widget GetFirstItemWidget();
}

#endif // TLIB_BANK_HPP_INCLUDED