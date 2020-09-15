#include "imgui_alpacawidgets.h"
#include <iostream>

float ImGuiTitleBarButton_Settings[6] = { 46, 10, 8, 2, 10, 9 };

namespace ImGui
{
    bool HelpIcon(const char* TooltipText, const char* Text)
    {
        ImGui::TextDisabled(Text);
        bool IsHovered = IsItemHovered();
        if (IsHovered)
            ImGui::SetTooltip(TooltipText);
        return IsHovered;
    }

	bool BeginToolbar(const char* StrID, const ImVec2& Pos)
    {
        ImGuiContext& g = *GImGui;
        g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));
        SetNextWindowPos(Pos);
        SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y));
        PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0,0));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_MenuBar;
        bool is_open = Begin(StrID, NULL, window_flags) && BeginMenuBar();
        PopStyleVar(2);
        g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.0f, 0.0f);
        if (!is_open)
        {
            End();
            return false;
        }
        return true;
    }

    void EndToolbar()
    {
        ImGui::EndMainMenuBar();
    }

    void VerticalSeparatorEx(int Offset)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;
        ImGuiContext& g = *GImGui;

        float y1 = window->DC.CursorPos.y + Offset;
        float y2 = (window->DC.CursorPos.y + window->DC.CurrLineSize.y) - Offset;
        const ImRect bb(ImVec2(window->DC.CursorPos.x, y1), ImVec2(window->DC.CursorPos.x + 1.0f, y2));
        ItemSize(ImVec2(bb.GetWidth(), 0.0f));
        if (!ItemAdd(bb, 0))
            return;

        window->DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x, bb.Max.y), GetColorU32(ImGuiCol_Separator));
        if (g.LogEnabled)
            LogText(" |");
    }

    bool BufferingBar(const char* label, float value,  const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        const float circleStart = size.x * 0.7f;
        const float circleEnd = size.x;
        const float circleWidth = circleEnd - circleStart;

        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart*value, bb.Max.y), fg_col);

        const float t = g.Time;
        const float r = size.y / 2;
        const float speed = 1.5f;

        const float a = speed*0;
        const float b = speed*0.333f;
        const float c = speed*0.666f;

        const float o1 = (circleWidth+r) * (t+a - speed * (int)((t+a) / speed)) / speed;
        const float o2 = (circleWidth+r) * (t+b - speed * (int)((t+b) / speed)) / speed;
        const float o3 = (circleWidth+r) * (t+c - speed * (int)((t+c) / speed)) / speed;

        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
        window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
        return true;
    }

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius )*2, (radius + style.FramePadding.y)*2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();

        int num_segments = 30;
        int start = abs(ImSin(g.Time*1.8f)*(num_segments-5));

        const float a_min = IM_PI*2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI*2.0f * ((float)num_segments-3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x+radius, pos.y+radius+style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time*8) * radius,
                                                centre.y + ImSin(a+g.Time*8) * radius));
        }

        window->DrawList->PathStroke(color, false, thickness);
        return true;
    }

    void SetTitleBarButtonSettings()
    {
        ImGuiContext& G = *GImGui;
        ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_ButtonWidth]           = 46;
        ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Minimize_LineLength]   = 10;
        ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Restore_EdgeLength]    = 8;
        ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Restore_Offset]        = 2;
        ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Maximize_EdgeLength]   = 10;
        ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Close_XRadius]         = G.FontSize * 0.5f;
    }

    bool TitleBarMinimizeButton(bool ThinCursorPosAtEnd)
    {
        ImGuiWindow* Window = GetCurrentWindow();
        if (Window->SkipItems)
            return false;

        ImVec4 ImGuiColButton = GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 ImGuiColButtonActive = ImVec4(ImGuiColButton.x / 1.5f, ImGuiColButton.y / 1.5f, ImGuiColButton.z / 1.5f, ImGuiColButton.w);
        PushStyleColor(ImGuiCol_ButtonHovered, ImGuiColButton);
        PushStyleColor(ImGuiCol_ButtonActive, ImGuiColButtonActive);
        PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        bool Clicked = Button("###TitleBarMinimizeButton", ImVec2(ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_ButtonWidth], 0));
        PopStyleColor(4);

        const ImRect ButtonRect = Window->DC.LastItemRect;
        const ImVec2 Center = ButtonRect.GetCenter();
        const float Length = ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Minimize_LineLength];

        ImVec2 A = ImVec2(Center.x - (Length / 2), Center.y);
        ImVec2 B = ImVec2(A.x + Length, A.y);
        Window->DrawList->AddLine(A, B, GetColorU32(ImGuiCol_Text), 1.0f);

        if (ThinCursorPosAtEnd)
        {
            ImGuiContext& G = *GImGui;
            const ImGuiStyle& Style = G.Style;
            ImVec2 EndCursorPos = GetCursorPos();
            SetCursorPos(ImVec2(EndCursorPos.x - Style.ItemSpacing.x, EndCursorPos.y));
        }
        return Clicked;
    }

    bool TitleBarRestoreButton(bool ThinCursorPosAtEnd)
    {
        ImVec2 CursorPos = GetCursorScreenPos();
        ImGuiWindow* Window = GetCurrentWindow();
        if (Window->SkipItems)
            return false;

        ImVec4 ImGuiColButton = GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 ImGuiColButtonActive = ImVec4(ImGuiColButton.x / 1.5f, ImGuiColButton.y / 1.5f, ImGuiColButton.z / 1.5f, ImGuiColButton.w);
        PushStyleColor(ImGuiCol_ButtonHovered, ImGuiColButton);
        PushStyleColor(ImGuiCol_ButtonActive, ImGuiColButtonActive);
        PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        bool Clicked = Button("###TitleBarRestoreButton", ImVec2(ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_ButtonWidth], 0));
        PopStyleColor(4);

        const ImVec2 ItemRect = GetItemRectSize();
        const ImU32 ShapeCol = GetColorU32(ImGuiCol_Text);
        float Length = ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Restore_EdgeLength];
        float Offset = ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Restore_Offset];

        ImVec2 UpperLeft = ImVec2(CursorPos.x + ((ItemRect.x - Length)  / 2), CursorPos.y + ((ItemRect.y / 2) - (Length / 2)));
        ImVec2 BottomRight = ImVec2(UpperLeft.x + Length, UpperLeft.y + Length);
        Window->DrawList->AddRect(UpperLeft, BottomRight, ShapeCol);

        ImVec2 LineStart = ImVec2(UpperLeft.x + Offset, UpperLeft.y);
        ImVec2 LineEnd = ImVec2(LineStart.x, LineStart.y - Offset);
        Window->DrawList->AddLine(LineStart, LineEnd, ShapeCol);

        LineStart = LineEnd;
        LineEnd = ImVec2(LineStart.x + Length, LineStart.y);
        Window->DrawList->AddLine(LineStart, LineEnd, ShapeCol);

        LineStart = LineEnd;
        LineEnd = ImVec2(LineStart.x, LineStart.y + Length);
        Window->DrawList->AddLine(LineStart, LineEnd, ShapeCol);

        LineStart = LineEnd;
        LineEnd = ImVec2(LineStart.x - Offset, LineStart.y);
        Window->DrawList->AddLine(LineStart, LineEnd, ShapeCol);

        if (ThinCursorPosAtEnd)
        {
            ImGuiContext& G = *GImGui;
            const ImGuiStyle& Style = G.Style;
            ImVec2 EndCursorPos = GetCursorPos();
            SetCursorPos(ImVec2(EndCursorPos.x - Style.ItemSpacing.x, EndCursorPos.y));
        }
        return Clicked;
    }

    bool TitleBarMaximizeButton(bool ThinCursorPosAtEnd)
    {
        ImVec2 CursorPos = GetCursorScreenPos();
        ImGuiWindow* Window = GetCurrentWindow();
        if (Window->SkipItems)
            return false;

        ImVec4 ImGuiColButton = GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 ImGuiColButtonActive = ImVec4(ImGuiColButton.x / 1.5f, ImGuiColButton.y / 1.5f, ImGuiColButton.z / 1.5f, ImGuiColButton.w);
        PushStyleColor(ImGuiCol_ButtonHovered, ImGuiColButton);
        PushStyleColor(ImGuiCol_ButtonActive, ImGuiColButtonActive);
        PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        bool Clicked = Button("###TitleBarMaximizeButton", ImVec2(ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_ButtonWidth], 0));
        PopStyleColor(4);

        const ImVec2 ItemRect = GetItemRectSize();
        const float Length = ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Maximize_EdgeLength];

        ImVec2 UpperLeft = ImVec2(CursorPos.x + ((ItemRect.x - Length)  / 2), CursorPos.y + ((ItemRect.y / 2) - (Length / 2)));
        ImVec2 BottomRight = ImVec2(UpperLeft.x + Length, UpperLeft.y + Length);
        Window->DrawList->AddRect(UpperLeft, BottomRight, GetColorU32(ImGuiCol_Text));

        if (ThinCursorPosAtEnd)
        {
            ImGuiContext& G = *GImGui;
            const ImGuiStyle& Style = G.Style;
            ImVec2 EndCursorPos = GetCursorPos();
            SetCursorPos(ImVec2(EndCursorPos.x - Style.ItemSpacing.x, EndCursorPos.y));
        }
        return Clicked;
    }

    bool TitleBarCloseButton(bool ThinCursorPosAtEnd)
    {
        ImGuiWindow* Window = GetCurrentWindow();
        if (Window->SkipItems)
            return false;

        PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
        bool Clicked = Button("###TitleBarCloseButton", ImVec2(ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_ButtonWidth], 0));
        PopStyleColor(2);

        float ButtonRadius = ImGuiTitleBarButton_Settings[ImGuiTitleBarButton_Close_XRadius];
        if (ButtonRadius < 0)
        {
            ImGuiContext& G = *GImGui;
            ButtonRadius = G.FontSize * 0.5f;
        }

        const ImVec2 ButtonCenter = Window->DC.LastItemRect.GetCenter();
        const ImRect Rect(ButtonCenter - ImVec2(ButtonRadius, ButtonRadius), ButtonCenter + ImVec2(ButtonRadius, ButtonRadius));
        ImVec2 Center = Rect.GetCenter();

        float CrossExtent = (ButtonRadius * 0.7071f) - 1.0f;
        ImU32 CrossCol = GetColorU32(ImGuiCol_Text);
        Center -= ImVec2(0.5f, 0.5f);

        Window->DrawList->AddLine(Center + ImVec2(+CrossExtent, +CrossExtent), Center + ImVec2(-CrossExtent, -CrossExtent), CrossCol, 1.0f);
        Window->DrawList->AddLine(Center + ImVec2(+CrossExtent, -CrossExtent), Center + ImVec2(-CrossExtent, +CrossExtent), CrossCol, 1.0f);

        if (ThinCursorPosAtEnd)
        {
            ImGuiContext& G = *GImGui;
            const ImGuiStyle& Style = G.Style;
            ImVec2 EndCursorPos = GetCursorPos();
            SetCursorPos(ImVec2(EndCursorPos.x - Style.ItemSpacing.x, EndCursorPos.y));
        }
        return Clicked;
    }
}


