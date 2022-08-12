#include <iostream>
#include <Windows.h>
#include "Resizer.h"

#define MOD_KEY VK_LCONTROL

LRESULT KbHookHandler(int code, WPARAM wParam, LPARAM lParam);
LRESULT MouseHookHandler(int nCode, WPARAM wParam, LPARAM lParam);

void Unhook(HHOOK hHook);

HMODULE hModule;

HHOOK hKbHook;
HHOOK hMouseHook;

bool resizeMode = false;
bool resizing = false;

POINT initialMousePos;
POINT currentMousePos;

HWND hResizingWindow;
RECT initialWndRect;

int main()
{
    std::cout << "Hello World!\n";

    hModule = GetModuleHandle(L"WinApiTest2.exe");
    
    hKbHook = SetWindowsHookEx(WH_KEYBOARD_LL, KbHookHandler, hModule, 0);
    std::cout << "Added " << hKbHook << " hook for WH_KEYBOARD_LL\n";
    
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    std::cout << hModule << "\nBYE World!\n";
}


LRESULT KbHookHandler(int code, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT* eventData = (KBDLLHOOKSTRUCT*)lParam;
    
    if (eventData->vkCode == MOD_KEY && wParam == WM_KEYDOWN)
    {
        if (!resizeMode)
        {
            std::cout << "Registered click: " << eventData->vkCode << " " << std::endl;
            std::cout << "Resize mode ON" << std::endl;
            hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookHandler, hModule, 0);
            resizeMode = true;
        }
    }
    else if (eventData->vkCode == MOD_KEY && wParam == WM_KEYUP)
    {
        Unhook(hMouseHook);
        std::cout << "Resize mode OFF" << std::endl;
        resizeMode = false;
        return NULL;
    }
    else if (eventData->vkCode == VK_ESCAPE)
    {
        Unhook(hKbHook);
        Unhook(hMouseHook);
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT MouseHookHandler(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* eventData = (MSLLHOOKSTRUCT*)lParam;

    if (wParam == WM_RBUTTONDOWN && resizeMode)
    {
        std::cout << "Detected input: WM_RBUTTONDOWN" << std::endl;
        hResizingWindow = GetForegroundWindow();

        GetCursorPos(&initialMousePos);
        GetWindowRect(hResizingWindow, &initialWndRect);

        std::cout << "Starting resize mode" << std::endl;
        resizing = true;
    }
    else if (wParam == WM_MOUSEMOVE && resizeMode && resizing)
    {
        GetCursorPos(&currentMousePos);

        LONG diffX = currentMousePos.x - initialMousePos.x;
        LONG diffY = currentMousePos.y - initialMousePos.y;
        LONG newX = (initialWndRect.right - initialWndRect.left) + diffX;
        LONG newY = (initialWndRect.bottom - initialWndRect.top) + diffY;

        SetWindowPos(hResizingWindow, NULL, (int)initialWndRect.left, (int)initialWndRect.top, newX, newY, SWP_NOMOVE | SWP_DRAWFRAME | SWP_FRAMECHANGED);
        
    }
    else if (wParam == WM_RBUTTONUP)
    {
        std::cout << "Stopping resize mode" << std::endl;
        resizing = false;
    }

    return CallNextHookEx(NULL, 0, wParam, lParam);
}

void Unhook(HHOOK hHook)
{
    UnhookWindowsHookEx(hHook);
    std::cout << "Unhooked " << hHook << std::endl;
}
