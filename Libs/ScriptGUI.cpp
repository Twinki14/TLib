#include "ScriptGUI.hpp"
#include "imgui/fonts/RobotoRegular.hpp"

#include <Core/Script.hpp>
#include <SDL2/SDL_syswm.h>
#include <imgui_impl_sdl.h>
#include <imgui_sw.hpp>
#include <aclapi.h>
#include <vector>
#include <cmath>

bool ScriptGUI::Running                     = false;
bool ScriptGUI::Result                      = false;
std::string ScriptGUI::Title;
std::uint32_t ScriptGUI::Width              = 0;
std::uint32_t ScriptGUI::Height             = 0;
SDL_Window   *ScriptGUI::Window          = nullptr;
SDL_Surface  *ScriptGUI::Surface         = nullptr;
SDL_Renderer *ScriptGUI::Renderer        = nullptr;
SDL_Texture  *ScriptGUI::ImGuiTexture    = nullptr;

typedef struct HWNDListPID_T
{
    DWORD PID;
    std::vector<HWND> Handles;
} HWNDListPID;

static BOOL CALLBACK GetHWNDListPID(HWND WindowHandle, LPARAM lParam)
{
    HWNDListPID* Entry = reinterpret_cast<HWNDListPID*>(lParam);
    DWORD HWNDPID;
    GetWindowThreadProcessId(WindowHandle, &HWNDPID);
    if (Entry->PID == HWNDPID)
        Entry->Handles.emplace_back(WindowHandle);
    return TRUE;
}

void ScriptGUI::Run(GUIData* Data)
{
    if (ScriptGUI::Running)
        return;

    if (!Data)
        return;

    if (!Data->DrawGUIFrameMethod)
        return;

    if (Data->Title.empty())
    {
        ScriptInfo Info;
        GetScriptInfo(Info);
        Data->Title = Info.Name + "-GUI";
    }

    ScriptGUI::Running = true;
    ScriptGUI::Result = false;
    ScriptGUI::Title = Data->Title;

    HWND RShWnd = ScriptGUI::GetRSWindowHandle();
    RECT WinRect;
    GetWindowRect(RShWnd, &WinRect);

    ScriptGUI::Width = Data->Width;
    ScriptGUI::Height = Data->Height;

	int Width_Pixels;
	int Height_Pixels;

	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
	ScriptGUI::Window = SDL_CreateWindow(ScriptGUI::Title.c_str(), Data->X, Data->Y, ScriptGUI::Width, ScriptGUI::Height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

	HWND WinHhWnd = GetHandle();
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SetWindowLongPtr(WinHhWnd, GWLP_HWNDPARENT, (LONG_PTR) RShWnd);
	SDL_GL_GetDrawableSize(Window, &Width_Pixels, &Height_Pixels);

	ScriptGUI::Surface = SDL_GetWindowSurface(Window);
	ScriptGUI::Renderer = SDL_CreateSoftwareRenderer(Surface);
	ScriptGUI::ImGuiTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, Width, Height);
    std::vector<uint32_t> PixelBuffer(Width_Pixels * Height_Pixels, 0x323232);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

	if (Data->LoadFonts)
    {
        ImFontConfig RobotoConfig;
        strcpy(RobotoConfig.Name, "RobotoRegular.ttf 18px");
        RobotoConfig.FontDataOwnedByAtlas = false;
        RobotoConfig.MergeMode = false;
        io.Fonts->AddFontFromMemoryCompressedBase85TTF(RobotoRegular_Base85, 18.0f, &RobotoConfig);
    }

	if (Data->LoadAlpacaClientStyle)
    {
	    Data->ClearColorHex = 0x323232;
	    ScriptGUI::LoadAlpacaClientStyle();
    }

	if (Data->OnGUIStartMethod) Data->OnGUIStartMethod();

    imgui_sw::bind_imgui_painting();
    imgui_sw::SwOptions SW_Options;
    SW_Options.optimize_text = false;
    SW_Options.optimize_rectangles = true;

    ImGui_ImplSDL2_Init(ScriptGUI::Window);

	while (Running)
    {
        std::uint32_t FrameTime = SDL_GetTicks();

        if (Terminate)
            Running = false;

        // OnGUIFrameStart - No ImGui code here

		SDL_Event Event;
		while (SDL_PollEvent(&Event))
        {
            ImGui_ImplSDL2_ProcessEvent(&Event);
			if (Event.type == SDL_QUIT)
                Running = false;
			//if (Event.type == SDL_KEYDOWN && Event.key.keysym.scancode == SDL_SCANCODE_KP_ENTER)
			    //ImGui::GetIO().KeysDown[SDL_SCANCODE_RETURN] = Event.type == SDL_KEYDOWN;
		}

        ImGui_ImplSDL2_NewFrame(Window);
        ImGui::NewFrame();

        Data->DrawGUIFrameMethod();

        ImGui::Render();

        std::fill_n(PixelBuffer.data(), PixelBuffer.size(), Data->ClearColorHex); // Resets pixels - 0x323232
        paint_imgui(PixelBuffer.data(), Width_Pixels, Height_Pixels, SW_Options);

		SDL_UpdateTexture(ImGuiTexture, nullptr, PixelBuffer.data(), Width_Pixels * sizeof(Uint32));
        SDL_RenderCopy(Renderer, ImGuiTexture, nullptr, nullptr);
        SDL_RenderPresent(Renderer);
		SDL_UpdateWindowSurface(Window);

        // OnGUIFrameEnd - No ImGui code here

        FrameTime = SDL_GetTicks() - FrameTime;
        if (Data->FPSLimit > 0 && FrameTime < (1000 / Data->FPSLimit))
            SDL_Delay((1000 / Data->FPSLimit) - FrameTime);
	}

	ScriptGUI::Shutdown();
    CloseHandle(WinHhWnd);
    CloseHandle(RShWnd);

    if (Data->OnGUIEndMethod) Data->OnGUIEndMethod();
}

void ScriptGUI::Shutdown()
{
    if (!ScriptGUI::Running)
    {
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyTexture(ScriptGUI::ImGuiTexture);
        SDL_FreeSurface(ScriptGUI::Surface);
        SDL_DestroyRenderer(ScriptGUI::Renderer);
        SDL_DestroyWindow(ScriptGUI::Window);
        SDL_Quit();
    }
}

SDL_Window* ScriptGUI::GetSDLWindow()
{
    return ScriptGUI::Window;
}

std::string ScriptGUI::GetTitle()
{
    return ScriptGUI::Title;
}

std::uint32_t ScriptGUI::GetWidth()
{
    return ScriptGUI::Width;
}

std::uint32_t ScriptGUI::GetHeight()
{
    return ScriptGUI::Height;
}

HWND ScriptGUI::GetCanvasHandle()
{
    HWND CanvasHandle = 0;
    HWNDListPID Entry;
    Entry.PID = GetCurrentProcessId();
    EnumWindows(GetHWNDListPID, reinterpret_cast<LPARAM>(&Entry));
    if (!Entry.Handles.empty())
    {
        for (auto const& W : Entry.Handles)
        {
            char ClassName[128];
            GetClassName(W, ClassName, sizeof(ClassName));
            std::string WindowClassName(ClassName);
            if (WindowClassName == "SunAwtFrame")
            {
                if (IsIconic(W))
                    ShowWindowAsync(W, SW_SHOWNOACTIVATE);
                CanvasHandle = W;
                break;
            }
        }
        HWND Temp = CanvasHandle;
        CanvasHandle = nullptr;
        while (Temp)
        {
            Temp = GetWindow(Temp, GW_CHILD);
            if (Temp)
            {
                char ClassName[128];
                GetClassName(Temp, ClassName, sizeof(ClassName));
                std::string ChildClassName(ClassName);
                if (ChildClassName == "SunAwtCanvas")
                    CanvasHandle = Temp;
            }
        }
    }
    return CanvasHandle;
}

HWND ScriptGUI::GetRSWindowHandle()
{
    return GetParent(ScriptGUI::GetCanvasHandle());
}

HWND ScriptGUI::GetHandle()
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(Window, &wmInfo);
    return wmInfo.info.win.window;
}

void ScriptGUI::LoadAlpacaClientStyle()
{
    // Style and Colors
    ImGuiStyle& Style = ImGui::GetStyle();
    Style.ScrollbarSize = 12;
    Style.ScrollbarRounding = 0.0f;
    Style.WindowRounding = 0.0f;
    Style.FrameBorderSize = 1.0f;
    Style.PopupBorderSize = Style.FrameBorderSize;
    Style.PopupRounding = Style.WindowRounding;
    Style.TabRounding = Style.WindowRounding;

    ImVec4* Colors = Style.Colors;
    Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 0.67f);
    Colors[ImGuiCol_WindowBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.92f);
    Colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    Colors[ImGuiCol_Border]                 = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.61f);
    Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.14f, 0.07f, 0.07f, 0.61f);
    Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    Colors[ImGuiCol_TitleBg]                = ImVec4(0.35f, 0.11f, 0.11f, 1.00f);
    Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.43f, 0.14f, 0.14f, 1.00f);
    Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.08f, 0.08f, 0.08f, 0.92f);
    Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.17f);
    Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.93f, 0.42f, 0.42f, 1.00f);
    Colors[ImGuiCol_CheckMark]              = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_SliderGrab]             = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.93f, 0.42f, 0.42f, 1.00f);
    Colors[ImGuiCol_Button]                 = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.55f, 0.17f, 0.17f, 1.00f);
    Colors[ImGuiCol_ButtonActive]           = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_Header]                 = ImVec4(0.43f, 0.14f, 0.14f, 1.00f);
    Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.55f, 0.17f, 0.17f, 1.00f);
    Colors[ImGuiCol_HeaderActive]           = ImVec4(0.55f, 0.17f, 0.17f, 1.00f);
    Colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.67f);
    Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.91f, 0.40f, 0.40f, 1.00f);
    Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.84f, 0.29f, 0.29f, 1.00f);
    Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.91f, 0.40f, 0.40f, 1.00f);
    Colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    Colors[ImGuiCol_PlotHistogram]          = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.91f, 0.40f, 0.40f, 1.00f);
    Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.43f, 0.14f, 0.14f, 1.00f);
    Colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
    Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.75f, 0.28f, 0.28f, 1.00f);
    Colors[ImGuiCol_Tab]                    = ImVec4(0.43f, 0.14f, 0.14f, 1.00f);
    Colors[ImGuiCol_TabHovered]             = ImVec4(0.55f, 0.17f, 0.17f, 1.00f);
    Colors[ImGuiCol_TabActive]              = ImVec4(0.55f, 0.17f, 0.17f, 1.00f);
    Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
}
