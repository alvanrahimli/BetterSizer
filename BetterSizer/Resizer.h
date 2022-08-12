//#pragma once
//#include <Windows.h>
//#include <iostream>
//
//#ifndef UNICODE
//#define UNICODE
//#endif
//#include <functional>
//
//class Resizer
//{
//	Resizer(int modKey)
//	{
//		MOD_KEY = modKey;
//	}
//	
//	~Resizer()
//	{
//		Unhook(m_hKbHook);
//		Unhook(m_hMsHook);
//	}
//
//private:
//	int MOD_KEY;
//	
//	HWND m_hResizingWindow;
//	HHOOK m_hKbHook;
//	HHOOK m_hMsHook;
//	HMODULE m_hModule;
//
//	AppMode m_CurrentMode;
//
//	void Unhook(HHOOK hHook)
//	{
//		UnhookWindowsHookEx(hHook);
//		std::cout << "Unhooked " << hHook << std::endl;
//	}
//	
//	void MessageLoop()
//	{
//		MSG msg;
//		while (GetMessage(&msg, NULL, 0, 0) != 0)
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//	
//	void SetHook(int idHook, HOOKPROC proc)
//	{
//		m_hKbHook = SetWindowsHookEx(idHook, proc, m_hModule, 0);
//		std::cout << "Keyboard hook set\n";
//	}
//
//	
//
//
//public:
//	void Run()
//	{
//		m_CurrentMode = Idle;
//
//		std::function<LRESULT(int code, WPARAM wParam, LPARAM lParam)> keyProc =
//			[this](int code, WPARAM wParam, LPARAM lParam) -> LRESULT
//			{
//				KBDLLHOOKSTRUCT* eventData = (KBDLLHOOKSTRUCT*)lParam;
//
//				if (eventData->vkCode == MOD_KEY && wParam == WM_KEYDOWN)
//				{
//					if (!resizeMode)
//					{
//						std::cout << "Registered click: " << eventData->vkCode << " " << std::endl;
//						std::cout << "Resize mode ON" << std::endl;
//						hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookHandler, hModule, 0);
//						resizeMode = true;
//					}
//				}
//				else if (eventData->vkCode == MOD_KEY && wParam == WM_KEYUP)
//				{
//					Unhook(hMouseHook);
//					std::cout << "Resize mode OFF" << std::endl;
//					resizeMode = false;
//					return NULL;
//				}
//				else if (eventData->vkCode == VK_ESCAPE)
//				{
//					Unhook(hKbHook);
//					Unhook(hMouseHook);
//				}
//
//				return CallNextHookEx(NULL, code, wParam, lParam);
//			};
//
//		SetHook(WH_KEYBOARD_LL, keyProc);
//
//		MessageLoop();
//	}
//	
//	//LRESULT KbHookHandler
//};
//
//enum AppMode
//{
//	Idle, 
//	Resizing
//};