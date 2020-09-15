#ifndef TLIB_TPROFILE_HPP
#define TLIB_TPROFILE_HPP

#include <cstdint>
#include <string_view>

namespace Profile
{
    enum FREQUENCY
    {
        FREQUENCY_NEVER,
        FREQUENCY_RARELY,
        FREQUENCY_REGULARLY,
        FREQUENCY_FREQUENTLY,
        FREQUENCY_VERY_FREQUENTLY
    };

    enum TENDENCY
    {
        TENDENCY_VERY_LOW,
        TENDENCY_LOW,
        TENDENCY_NORMAL,
        TENDENCY_HIGH,
        TENDENCY_VERY_HIGH
    };

    enum PASSIVITY
    {
        PASSIVITY_EXHILARATED,
        PASSIVITY_HYPER,
        PASSIVITY_MILD,
        PASSIVITY_MELLOW,
        PASSIVITY_DISINTERESTED,
    };

    typedef enum ProfileVar
    {
        Var_UseHotkeys_Gametabs,
        Var_UseHotkeys_Gametabs_Chance,

        Var_UseHotkeys_EscCloseInterface,
        Var_UseHotkeys_EscCloseInterface_Chance,

        Var_AFK_Frequency,
        Var_TabOut_Tendency,
        Var_Camera_Tendency,
        Var_Passivity,

        // End custom
        Var_RawInteractableMean,
        Var_RawInteractableDeviation,

        Var_RawMoveMean,
        Var_RawMoveDeviation,

        Var_RawMouseDownMean,
        Var_RawMouseDownDeviation,
        Var_RawMouseUpMean,
        Var_RawMouseUpDeviation,

        Var_RawKeyDownMean,
        Var_RawKeyDownDeviation,
        Var_RawKeyUpMean,
        Var_RawKeyUpDeviation

    } ProfileVar;

    void Push(ProfileVar V, bool B);
    void Push(ProfileVar V, std::int32_t I);
    void Push(ProfileVar V, double D);

    void Pop(std::int32_t Count = 1);

    std::int32_t GetInt(ProfileVar V);
    double GetDouble(ProfileVar V);
    bool GetBool(ProfileVar V);

    void Set(ProfileVar V, std::int32_t New);
    void Set(ProfileVar V, double New);
    void Set(ProfileVar V, bool New);

    bool RollUseGametabHotKey();
    bool RollUseEscHotkey();

    bool GeneratePlayerUnique(std::string_view PlayerName, std::string_view UniqueKey);
    std::int32_t GeneratePlayerUnique(std::string_view PlayerName, std::string_view UniqueKey, std::int32_t Min, std::int32_t Max);
}


#endif //TLIB_TPROFILE_HPP
