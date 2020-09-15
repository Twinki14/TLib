#ifndef IMGUI_DISABLEWIDGET_H_
#define IMGUI_DISABLEWIDGET_H_

#ifndef IMGUI_API
#include <imgui.h>
#include <imgui_internal.h>
#endif //IMGUI_API

namespace ImGui
{
    IMGUI_API void PushDisabled(const bool& Disabled = true);
    IMGUI_API void PopDisabled(const bool& Disabled = true, int Num = 1);
}

#endif //IMGUI_DISABLEWIDGET_H_

