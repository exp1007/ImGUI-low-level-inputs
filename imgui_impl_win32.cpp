[...]

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandlerMod(HWND hwnd, WPARAM wParam, LPARAM lParam, bool mouse) //if it's not a mouse callback it's a keyboard one
{
    if (ImGui::GetCurrentContext() == NULL)
        return 0;

    ImGui_ImplWin32_Data* bd = ImGui_ImplWin32_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    if (mouse) {
        switch (wParam)
        {

        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        {
            // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events

            POINT mouse_pos = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->pt;
            POINT temp;
            //if (wParam == WM_NCMOUSEMOVE) // WM_NCMOUSEMOVE are provided in absolute coordinates.
            {
                    //RECT borderedWindow;
                    //GetClientRect(hwnd, &borderedWindow);
                    //
                    //mouse_pos.x -= borderedWindow.left;
                    //mouse_pos.y -= borderedWindow.top; 
            }

            io.AddMousePosEvent((float)mouse_pos.x, (float)mouse_pos.y);

            //wprintf(L"mouse_pos: %ld, %ld\n", mouse_pos.x, mouse_pos.y);

            break;
        }
        case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
        {
            int button = 0;
            if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONDBLCLK) { button = 0; }
            if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONDBLCLK) { button = 1; }
            if (wParam == WM_MBUTTONDOWN || wParam == WM_MBUTTONDBLCLK) { button = 2; }
            if (wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->mouseData) == XBUTTON1) ? 3 : 4; }
            //if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
            //    ::SetCapture(hwnd);

            bd->MouseButtonsDown |= 1 << button;
            io.AddMouseButtonEvent(button, true);
            //wprintf(L"mouse_down: %d\n", button);

            return 0;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            int button = 0;
            if (wParam == WM_LBUTTONUP) { button = 0; }
            if (wParam == WM_RBUTTONUP) { button = 1; }
            if (wParam == WM_MBUTTONUP) { button = 2; }
            if (wParam == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(lParam) == XBUTTON1) ? 3 : 4; }
            bd->MouseButtonsDown |= 1 << button;
            io.AddMouseButtonEvent(button, false);

            //wprintf(L"mouse_up: %d\n", button);

            //if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
            //    ::ReleaseCapture();
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            MSLLHOOKSTRUCT* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(info->mouseData) / (float)WHEEL_DELTA;
            return 0;
        }
        case WM_MOUSEHWHEEL:
        {
            MSLLHOOKSTRUCT* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(info->mouseData) / (float)WHEEL_DELTA;
            return 0;
        }
        }
    }
    else {
        KBDLLHOOKSTRUCT* keyboardInfo = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        switch (wParam) {
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
            const bool is_key_down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

            if (keyboardInfo->vkCode < 512) {


                ImGui_ImplWin32_UpdateKeyModifiers();


                const ImGuiKey key = ImGui_ImplWin32_VirtualKeyToImGuiKey(keyboardInfo->vkCode);
                const int scancode = (int)LOBYTE(HIWORD(lParam));

                // Special behavior for VK_SNAPSHOT / ImGuiKey_PrintScreen as Windows doesn't emit the key down event.
                if (key == ImGuiKey_PrintScreen && !is_key_down)
                    ImGui_ImplWin32_AddKeyEvent(key, true, keyboardInfo->vkCode, scancode);

                // Submit key event
                if (key != ImGuiKey_None)
                    ImGui_ImplWin32_AddKeyEvent(key, is_key_down, keyboardInfo->vkCode, scancode);

                // Submit individual left/right modifier events
                if (keyboardInfo->vkCode == VK_SHIFT)
                {
                    // Important: Shift keys tend to get stuck when pressed together, missing key-up events are corrected in ImGui_ImplWin32_ProcessKeyEventsWorkarounds()
                    if (IsVkDown(VK_LSHIFT) == is_key_down) { ImGui_ImplWin32_AddKeyEvent(ImGuiKey_LeftShift, is_key_down, VK_LSHIFT, scancode); }
                    if (IsVkDown(VK_RSHIFT) == is_key_down) { ImGui_ImplWin32_AddKeyEvent(ImGuiKey_RightShift, is_key_down, VK_RSHIFT, scancode); }
                }
                else if (keyboardInfo->vkCode == VK_CONTROL)
                {
                    if (IsVkDown(VK_LCONTROL) == is_key_down) { ImGui_ImplWin32_AddKeyEvent(ImGuiKey_LeftCtrl, is_key_down, VK_LCONTROL, scancode); }
                    if (IsVkDown(VK_RCONTROL) == is_key_down) { ImGui_ImplWin32_AddKeyEvent(ImGuiKey_RightCtrl, is_key_down, VK_RCONTROL, scancode); }
                }
                else if (keyboardInfo->vkCode == VK_MENU)
                {
                    if (IsVkDown(VK_LMENU) == is_key_down) { ImGui_ImplWin32_AddKeyEvent(ImGuiKey_LeftAlt, is_key_down, VK_LMENU, scancode); }
                    if (IsVkDown(VK_RMENU) == is_key_down) { ImGui_ImplWin32_AddKeyEvent(ImGuiKey_RightAlt, is_key_down, VK_RMENU, scancode); }
                }

                if (is_key_down) {
                    BYTE keys[256];
                    if (GetKeyboardState(keys)) {
                        WCHAR keyPressed[10];
                        HKL keyboardLayout = GetKeyboardLayout(NULL);
                        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
                        if (ToUnicodeEx(keyboardInfo->vkCode, keyboardInfo->scanCode, (BYTE*)keys, keyPressed, 10, 2, keyboardLayout))
                        {
                            //wprintf(L"key: %ws scanCode: %d vkCode: %d\n lShift: %x rShift: %x\n", keyPressed, keyboardInfo->scanCode, keyboardInfo->vkCode, (BYTE)io.KeysDown[VK_LSHIFT], (BYTE)io.KeysDown[VK_RSHIFT]);
                            for (int i = 0; i < 10; i++)
                            {
                                if (keyPressed[i] > 0 && keyPressed[i] < 0x10000)
                                    io.AddInputCharacterUTF16((unsigned short)keyPressed[i]);
                            }
                        }
                    }
                }

            }
            break;
        }
        return 0;
    }
    return 0;
}

[...]
