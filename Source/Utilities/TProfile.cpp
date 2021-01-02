#include "../../Include/Utilities/TProfile.hpp"
#include <Game/Tools/Profile.hpp>
#include <stack>
#include <Core/Math.hpp>

namespace Profile
{
    union AType
    {
        bool B;
        double D;
        std::int32_t I;
    };

    std::stack<std::pair<Profile::ProfileVar, AType>> Stack;

    bool UseHotkeys_Gametabs = false;
    double UseHotkeys_Gametabs_Chance = 0.00;

    bool UseHotkeys_EscCloseInterface = false;
    double UseHotkeys_EscCloseInterface_Chance = 0.00;

    std::int32_t AFK_Frequency = FREQUENCY_REGULARLY;
    std::int32_t TabOut_Tendency = TENDENCY_NORMAL;
    std::int32_t Camera_Tendency = TENDENCY_NORMAL;
    std::int32_t Passivity = PASSIVITY_MILD;

    void PushToStack(Profile::ProfileVar V, AType New)
    {
        AType Old;
        switch (V)
        {
            // bool
            case Var_UseHotkeys_Gametabs:                       Old.B = UseHotkeys_Gametabs;            UseHotkeys_Gametabs = New.B; break;
            case Var_UseHotkeys_EscCloseInterface:              Old.B = UseHotkeys_EscCloseInterface;   UseHotkeys_EscCloseInterface = New.B; break;

            // double
            case Var_UseHotkeys_Gametabs_Chance:                    Old.D = UseHotkeys_Gametabs_Chance; UseHotkeys_Gametabs_Chance = New.D; break;
            case Var_UseHotkeys_EscCloseInterface_Chance:           Old.D = UseHotkeys_EscCloseInterface_Chance; UseHotkeys_EscCloseInterface_Chance = New.D; break;

            case Var_RawInteractableDeviation:  Old.D = Profile::GetInteractableDeviation();    Profile::SetRawInteractableDeviation(New.D); break;
            case Var_RawMoveDeviation:          Old.D = Profile::GetRawMoveDeviation();         Profile::SetRawMoveDeviation(New.D); break;
            case Var_RawMouseDownDeviation:     Old.D = Profile::GetRawMouseDownDeviation();    Profile::SetRawMouseDownDeviation(New.D); break;
            case Var_RawMouseUpDeviation:       Old.D = Profile::GetRawMouseUpDeviation();      Profile::SetRawMouseUpDeviation(New.D); break;
            case Var_RawKeyDownDeviation:       Old.D = Profile::GetRawKeyDownDeviation();      Profile::SetRawKeyDownDeviation(New.D); break;
            case Var_RawKeyUpDeviation:         Old.D = Profile::GetRawKeyUpDeviation();        Profile::SetRawKeyUpDeviation(New.D); break;

            // int
            case Var_RawInteractableMean:       Old.I = Profile::GetRawInteractableMean();      Profile::SetRawInteractableMean(New.I); break;
            case Var_RawMoveMean:               Old.I = Profile::GetRawMoveMean();              Profile::SetRawMoveMean(New.I); break;
            case Var_RawMouseDownMean:          Old.I = Profile::GetRawMouseDownMean();         Profile::SetRawMouseDownMean(New.I); break;
            case Var_RawMouseUpMean:            Old.I = Profile::GetRawMouseUpMean();           Profile::SetRawMouseUpMean(New.I); break;
            case Var_RawKeyDownMean:            Old.I = Profile::GetRawKeyDownMean();           Profile::SetRawKeyDownMean(New.I); break;
            case Var_RawKeyUpMean:              Old.I = Profile::GetRawKeyUpMean();             Profile::SetRawKeyUpMean(New.I); break;
            case Var_AFK_Frequency:             Old.I = AFK_Frequency;                          AFK_Frequency = New.I; break;
            case Var_TabOut_Tendency:           Old.I = TabOut_Tendency;                        TabOut_Tendency = New.I; break;
            case Var_Camera_Tendency:           Old.I = Camera_Tendency;                        Camera_Tendency = New.I; break;
            case Var_Passivity:                 Old.I = Passivity;                              Passivity = New.I; break;

            default: return;
        }
        Stack.push(std::make_pair(V, Old));
    }

    void PopStack()
    {
        if (Stack.empty()) return;
        std::pair<Profile::ProfileVar, AType> Top = Stack.top();
        Stack.pop();

        ProfileVar V = Top.first;
        AType New = Top.second;
        switch (V)
        {
            // bool
            case Var_UseHotkeys_Gametabs:                       UseHotkeys_Gametabs = New.B; break;
            case Var_UseHotkeys_EscCloseInterface:              UseHotkeys_EscCloseInterface = New.B; break;

            // double
            case Var_UseHotkeys_Gametabs_Chance: UseHotkeys_Gametabs_Chance = New.D; break;
            case Var_UseHotkeys_EscCloseInterface_Chance: UseHotkeys_EscCloseInterface_Chance = New.D; break;

            case Var_RawInteractableDeviation:  Profile::SetRawInteractableDeviation(New.D); break;
            case Var_RawMoveDeviation:          Profile::SetRawMoveDeviation(New.D); break;
            case Var_RawMouseDownDeviation:     Profile::SetRawMouseDownDeviation(New.D); break;
            case Var_RawMouseUpDeviation:       Profile::SetRawMouseUpDeviation(New.D); break;
            case Var_RawKeyDownDeviation:       Profile::SetRawKeyDownDeviation(New.D); break;
            case Var_RawKeyUpDeviation:         Profile::SetRawKeyUpDeviation(New.D); break;

                // int
            case Var_RawInteractableMean:       Profile::SetRawInteractableMean(New.I); break;
            case Var_RawMoveMean:               Profile::SetRawMoveMean(New.I); break;
            case Var_RawMouseDownMean:          Profile::SetRawMouseDownMean(New.I); break;
            case Var_RawMouseUpMean:            Profile::SetRawMouseUpMean(New.I); break;
            case Var_RawKeyDownMean:            Profile::SetRawKeyDownMean(New.I); break;
            case Var_RawKeyUpMean:              Profile::SetRawKeyUpMean(New.I); break;
            case Var_AFK_Frequency:             AFK_Frequency = New.I; break;
            case Var_TabOut_Tendency:           TabOut_Tendency = New.I; break;
            case Var_Camera_Tendency:           Camera_Tendency = New.I; break;
            case Var_Passivity:                 Passivity = New.I; break;

            default: return;
        }
    }

    std::int32_t GetInt(ProfileVar V)
    {
        switch (V)
        {
            case Var_RawInteractableMean:   return Profile::GetRawInteractableMean();
            case Var_RawMoveMean:           return Profile::GetRawMoveMean();
            case Var_RawMouseDownMean:      return Profile::GetRawMouseDownMean();
            case Var_RawMouseUpMean:        return Profile::GetRawMouseUpMean();
            case Var_RawKeyDownMean:        return Profile::GetRawKeyDownMean();
            case Var_RawKeyUpMean:          return Profile::GetRawKeyUpMean();
            case Var_Passivity:             return Passivity;
            case Var_AFK_Frequency:         return AFK_Frequency;
            case Var_TabOut_Tendency:       return TabOut_Tendency;
            case Var_Camera_Tendency:       return Camera_Tendency;
            default: return 0;
        }
    }

    double GetDouble(ProfileVar V)
    {
        switch (V)
        {
            case Var_UseHotkeys_Gametabs_Chance:                    return UseHotkeys_Gametabs_Chance;
            case Var_UseHotkeys_EscCloseInterface_Chance:           return UseHotkeys_EscCloseInterface_Chance;
            case Var_RawInteractableDeviation:  return Profile::GetRawInteractableDeviation();
            case Var_RawMoveDeviation:          return Profile::GetRawMoveDeviation();
            case Var_RawMouseDownDeviation:     return Profile::GetRawMouseDownDeviation();
            case Var_RawMouseUpDeviation:       return Profile::GetRawMouseUpDeviation();
            case Var_RawKeyDownDeviation:       return Profile::GetRawKeyDownDeviation();
            case Var_RawKeyUpDeviation:         return Profile::GetRawKeyUpDeviation();
            default: return 0.00;
        }
    }

    bool GetBool(ProfileVar V)
    {
        switch (V)
        {
            case Var_UseHotkeys_Gametabs:                   return UseHotkeys_Gametabs;
            case Var_UseHotkeys_EscCloseInterface:          return UseHotkeys_EscCloseInterface;
            default: return false;
        }
    }

    void Set(ProfileVar V, std::int32_t New)
    {
        switch (V)
        {
            case Var_RawInteractableMean:   Profile::SetRawInteractableMean(New); return;
            case Var_RawMoveMean:           Profile::SetRawMoveMean(New); return;
            case Var_RawMouseDownMean:      Profile::SetRawMouseDownMean(New); return;
            case Var_RawMouseUpMean:        Profile::SetRawMouseUpMean(New); return;
            case Var_RawKeyDownMean:        Profile::SetRawKeyDownMean(New); return;
            case Var_RawKeyUpMean:          Profile::SetRawKeyUpMean(New); return;
            case Var_AFK_Frequency:         AFK_Frequency = New; return;
            case Var_TabOut_Tendency:       TabOut_Tendency = New; return;
            case Var_Camera_Tendency:       Camera_Tendency = New; return;
            case Var_Passivity:             Passivity = New; return;
            default: return;
        }
    }

    void Set(ProfileVar V, double New)
    {
        switch (V)
        {
            case Var_UseHotkeys_Gametabs_Chance: UseHotkeys_Gametabs_Chance = New;
            case Var_UseHotkeys_EscCloseInterface_Chance: UseHotkeys_EscCloseInterface_Chance = New;
            case Var_RawInteractableDeviation:  Profile::SetRawInteractableDeviation(New); return;
            case Var_RawMoveDeviation:          Profile::SetRawMoveDeviation(New); return;
            case Var_RawMouseDownDeviation:     Profile::SetRawMouseDownDeviation(New); return;
            case Var_RawMouseUpDeviation:       Profile::SetRawMouseUpDeviation(New); return;
            case Var_RawKeyDownDeviation:       Profile::SetRawKeyDownDeviation(New); return;
            case Var_RawKeyUpDeviation:         Profile::SetRawKeyUpDeviation(New); return;
            default: return;
        }
    }

    void Set(ProfileVar V, bool New)
    {
        switch (V)
        {
            case Var_UseHotkeys_Gametabs:                       UseHotkeys_Gametabs = New; return;
            case Var_UseHotkeys_EscCloseInterface:              UseHotkeys_EscCloseInterface = New; return;
            default: return;
        }
    }

    void Push(ProfileVar V, bool B)             { AType T; T.B = B; PushToStack(V, T); }
    void Push(ProfileVar V, double D)           { AType T; T.D = D; PushToStack(V, T); }
    void Push(ProfileVar V, std::int32_t I)     { AType T; T.I = I; PushToStack(V, T); }

    void Pop(std::int32_t Count)
    {
        for (std::int32_t I = 0; I < Count; I++)
            PopStack();
    }

    bool RollUseGametabHotKey()
    {
        return Profile::GetBool(Profile::Var_UseHotkeys_Gametabs) && UniformRandom() <= Profile::GetDouble(Profile::Var_UseHotkeys_Gametabs_Chance);
    }

    bool RollUseEscHotkey()
    {
        return Profile::GetBool(Profile::Var_UseHotkeys_EscCloseInterface) && UniformRandom() <= Profile::GetDouble(Profile::Var_UseHotkeys_EscCloseInterface_Chance);
    }

    bool GeneratePlayerUnique(std::string_view PlayerName, std::string_view UniqueKey)
    {
        std::size_t PlayerHash = std::hash<std::string_view>{}(PlayerName);
        std::size_t UniqueHash = std::hash<std::string_view>{}(UniqueKey);
        std::size_t CombinedHash = PlayerHash;
        CombinedHash ^= UniqueHash + 0x9e3779b9 + (CombinedHash << 6) + (CombinedHash >> 2);

        std::mt19937 Seed(CombinedHash);
        std::uniform_int_distribution<std::int32_t> Rand(0, 1);
        return Rand(Seed);
    }

    std::int32_t GeneratePlayerUnique(std::string_view PlayerName, std::string_view UniqueKey, std::int32_t Min, std::int32_t Max)
    {
        std::size_t PlayerHash = std::hash<std::string_view>{}(PlayerName);
        std::size_t UniqueHash = std::hash<std::string_view>{}(UniqueKey);
        std::size_t CombinedHash = PlayerHash;
        CombinedHash ^= UniqueHash + 0x9e3779b9 + (CombinedHash << 6) + (CombinedHash >> 2);

        std::mt19937 Seed(CombinedHash);
        std::uniform_int_distribution<std::int32_t> Rand(Min, Max);
        return Rand(Seed);
    }

} // Profile