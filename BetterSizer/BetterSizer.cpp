#include <iostream>
#include <thread>
#include <Windows.h>

#define MOD_KEY VK_LCONTROL

#define RectW(r) (r.right - r.left)
#define RectH(r) (r.bottom - r.top)
#define LOG(m) std::cout << m << std::endl

LRESULT KbHookHandler(int code, WPARAM wParam, LPARAM lParam);
LRESULT MouseHookHandler(int nCode, WPARAM wParam, LPARAM lParam);

void Unhook(HHOOK hHook);
void UpdaterTask();

HMODULE hModule;

HHOOK hKbHook;
HHOOK hMouseHook;

enum class Mode {
	None,
	Resizing,
	Moving
};

bool resizeMode = false;

bool mouseMoved = false;
Mode currentMode = Mode::None;

POINT initialMousePos;

HWND hResizingWindow;
RECT initialWndRect;

void UpdaterTask()
{
	while (true)
	{
		if (mouseMoved)
		{
			mouseMoved = false;

			POINT currentMousePos;
			GetCursorPos(&currentMousePos);

			LONG diffX = currentMousePos.x - initialMousePos.x;
			LONG diffY = currentMousePos.y - initialMousePos.y;

			switch (currentMode)
			{
			case Mode::Resizing:
			{
				LONG cx = (initialWndRect.right - initialWndRect.left) + diffX;
				LONG cy = (initialWndRect.bottom - initialWndRect.top) + diffY;

				SetWindowPos(hResizingWindow, NULL, (int)initialWndRect.left, (int)initialWndRect.top, cx, cy, SWP_NOMOVE);
				UpdateWindow(hResizingWindow);
				break;
			}

			case Mode::Moving:
			{
				SetWindowPos(hResizingWindow, NULL, initialWndRect.left + diffX, initialWndRect.top + diffY, RectW(initialWndRect), RectH(initialWndRect), 0);
				break;
			}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
}

LRESULT KbHookHandler(int code, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT* eventData = (KBDLLHOOKSTRUCT*)lParam;

	if (eventData->vkCode == MOD_KEY && wParam == WM_KEYDOWN)
	{
		if (!resizeMode)
		{
			LOG("Resize mode ON");
			hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseHookHandler, hModule, 0);
			resizeMode = true;
		}
	}
	else if (eventData->vkCode == MOD_KEY && wParam == WM_KEYUP)
	{
		Unhook(hMouseHook);
		resizeMode = false;
		currentMode = Mode::None;
		LOG("Resize mode OFF");
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

	if (resizeMode)
	{
		switch (wParam)
		{
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
			LOG("Detected mouse click: " << wParam);
			hResizingWindow = GetForegroundWindow();

			GetCursorPos(&initialMousePos);
			GetWindowRect(hResizingWindow, &initialWndRect);

			LOG("Starting resize mode");
			if (wParam == WM_RBUTTONDOWN) currentMode = Mode::Resizing;
			else if (wParam == WM_LBUTTONDOWN) currentMode = Mode::Moving;
			break;

		case WM_MOUSEMOVE:
			if (currentMode != Mode::None) mouseMoved = true;
			break;

		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			LOG("Stopping resize mode");
			currentMode = Mode::None;
			return 0;
		}
	}

	return CallNextHookEx(NULL, 0, wParam, lParam);
}

void Unhook(HHOOK hHook)
{
	UnhookWindowsHookEx(hHook);
	LOG("Unhooked " << hHook);
}

int main()
{
	hModule = GetModuleHandle(NULL);
	hKbHook = SetWindowsHookEx(WH_KEYBOARD_LL, KbHookHandler, hModule, 0);

	LOG("Added " << hKbHook << " hook for WH_KEYBOARD_LL");

	std::thread updaterTask(UpdaterTask);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	updaterTask.detach();
}