#ifndef SCRIPTGUI_HPP_INCLUDED
#define SCRIPTGUI_HPP_INCLUDED

#include <windows.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui/imgui_internal.h>
#include <cstdint>
#include <string>

class ScriptGUI
{
    public:
        static bool Running; // Settings to false while Running is true, will exit out of the GUI and shutdown
        static bool Result; // Can be used to determine if the script can start

        typedef struct GUIData
        {
            std::uint32_t X             = 0;            // X position of where the window will be spawned
            std::uint32_t Y             = 0;            // Y position of where the window will be spawned
            std::uint32_t Width         = 0;            // Window Width for creation
            std::uint32_t Height        = 0;            // Window Height for creation
            std::uint32_t FPSLimit      = 25;
            COLORREF ClearColorHex      = 0x323232;     // Color in hex that's used to clear the pixel buffer, this is the "background"
            std::string Title           = "";           // (Optional) Window Title - Defaults to Script Name
            bool LoadAlpacaClientStyle  = true;         // (Optional) Loads the same style used in the AlpacaClient GUI, if false the default imgui style is used
            bool LoadFonts              = true;         // (Optional) Loads the Robot Regular text font and sets it as the default for ImGui to use
            void (*OnGUIStartMethod)()      = nullptr;      // (Optional) Runs this method after ImGui creates the context and loads fonts (if passed), directly before running the frame loop
            void (*DrawGUIFrameMethod)()    = nullptr;      // (Required) The method to run that contains your ImGui logic/code
            void (*OnGUIEndMethod)()        = nullptr;      // (Optional) Runs this method after the GUI shutdowns and cleans up
        } GUIData;

        static void Run(GUIData* Data);
        static void Shutdown();

        static void CenterWindowOnCanvas(); // Must be running
        static SDL_Window* GetSDLWindow();

        static std::string GetTitle();
        static std::uint32_t GetWidth();
        static std::uint32_t GetHeight();

        static void LoadAlpacaClientStyle();

        static HWND GetCanvasHandle();
        static HWND GetRSWindowHandle();
        static HWND GetHandle();


    private:
        static std::string Title;
        static std::uint32_t Width;
        static std::uint32_t Height;
        static SDL_Window   *Window;
        static SDL_Surface  *Surface;
        static SDL_Renderer *Renderer;
        static SDL_Texture  *ImGuiTexture;
};

#endif // SCRIPTGUI_HPP_INCLUDED
