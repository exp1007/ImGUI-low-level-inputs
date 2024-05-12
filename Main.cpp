[...]

// Forward declare message handler from imgui_impl_win32.cpp
// We we'll only need the modded implementation
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandlerMod(HWND hwnd, WPARAM wParam, LPARAM lParam, bool mouse); //if it's not a mouse callback it's a keyboard one

// Hooking functions
LRESULT __stdcall KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && requires_input)
    {
        ImGui_ImplWin32_WndProcHandlerMod(globalHWND, wParam, lParam, false);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT __stdcall MouseHookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && requires_input)
    {
        ImGui_ImplWin32_WndProcHandlerMod(globalHWND, wParam, lParam, true);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// This is the thread used to create the hooks
void hookWrapper() {
    bool hooked = false;
    HHOOK _keyhook = NULL;
    HHOOK _mousehook = NULL;

        if (requires_input) {
            if (!hooked) {
                _mousehook = SetWindowsHookExA(WH_MOUSE_LL, MouseHookCallback, NULL, 0);
                _keyhook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardHookCallback, NULL, 0);
                hooked = true;
            }
        }
        else
        {
            UnhookWindowsHookEx(_keyhook);
            UnhookWindowsHookEx(_mousehook);
            hooked = false;
        }

        GetMessage(nullptr, nullptr, 0, 0); //make the thread not close
}

// Replace 
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    //if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    //    return true;


    static bool hooked = false;
  
    if (!hooked) {
        std::thread hookWrapperThread(hookWrapper);
        hookWrapperThread.detach();
        hooked = true;
    }

    [...]

}
