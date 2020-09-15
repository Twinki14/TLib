#ifndef IMGUI_ALPACAWIDGETS_H_INCLUDED
#define IMGUI_ALPACAWIDGETS_H_INCLUDED

#ifndef IMGUI_API
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#endif //IMGUI_API

enum ImGuiTitleBarButton_
{
    ImGuiTitleBarButton_ButtonWidth,            // Default 46, Width of the actual buttons, height is forced to 0
    ImGuiTitleBarButton_Minimize_LineLength,    // Default 10, Length of the single-pixel line
    ImGuiTitleBarButton_Restore_EdgeLength,     // Default 8, Length of the edges, of the two squares
    ImGuiTitleBarButton_Restore_Offset,         // Default 2, Right side offset of the second square
    ImGuiTitleBarButton_Maximize_EdgeLength,    // Default 10, Length of the Maximize square edges
    ImGuiTitleBarButton_Close_XRadius,          // Default 9, Radius of the X, setting to -1 will use a radius based on the current font size
};

extern float ImGuiTitleBarButton_Settings[6];

namespace ImGui
{
    IMGUI_API bool MenuBasicItem(const char* Label, const char* Tooltip);
    IMGUI_API bool IconToggleMenuItem(const char* On, const char* Off, const char* Label, bool* Selected);
    IMGUI_API void IconToggleText(const char* On, const char* Off, bool* Toggle);
    IMGUI_API bool IconMenuItem(const char* Icon, const char* Label);
    IMGUI_API bool HelpIcon(const char* TooltipText, const char* Text = "(?)");
    IMGUI_API bool BeginToolbar(const char* StrID, const ImVec2& Pos);
    IMGUI_API void EndToolbar();
    IMGUI_API void VerticalSeparatorEx(int Offset);

    IMGUI_API bool BufferingBar(const char* label, float value,  const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
    IMGUI_API bool Spinner(const char* label, float radius, int thickness, const ImU32& color);

    IMGUI_API void SetTitleBarButtonSettings();
    IMGUI_API bool TitleBarMinimizeButton(bool ThinCursorPosAtEnd = true); // ThinCursorPosAtEnd true = Will move the cursor back to the very end of the button, allowing easy MainButton chaining
    IMGUI_API bool TitleBarRestoreButton(bool ThinCursorPosAtEnd = true);
    IMGUI_API bool TitleBarMaximizeButton(bool ThinCursorPosAtEnd = true);
    IMGUI_API bool TitleBarCloseButton(bool ThinCursorPosAtEnd = false);
}

#endif //IMGUI_ALPACAWIDGETS_H_INCLUDED

