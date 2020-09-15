#include "imgui_disablewidgets.h"

namespace ImGui
{
    void PushDisabled(const bool& Disabled)
    {
        if (Disabled)
        {
            PushItemFlag(ImGuiItemFlags_Disabled, true);
            PushStyleVar(ImGuiStyleVar_Alpha,GetStyle().Alpha * 0.5f);
        }
    }

    void PopDisabled(const bool& Disabled, int Num)
    {
        if (Disabled)
        {
            while (Num --)
            {
                PopItemFlag();
                PopStyleVar();
            }
        }
    }
}


