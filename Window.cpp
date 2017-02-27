
#include "Window.h"
#include <iostream>

bool WindowInitialize(HWND &windowHandle) {

	// HINSTANCE is a handle to an instance. This is the base address of the module in memory and handles the instance of the module to be associated with the window.
	HINSTANCE applicationHandle = GetModuleHandle(NULL);

	if (applicationHandle == NULL) {

		// If GetModuleHandle fails, there is no application handle for us to use. The WindowInitialize function returns false and 
		// sends an error message to the console as a string
		cout << "Window Handle Error: Application handle could not be retrieved due to the following error\n"
			<< "Error Code: " << GetLastError() << endl;

	}

	WNDCLASSEX windowClass = { 0 };	// We start by registering the window class to be used to specify both the behavior and appearence of the window

	windowClass.style = CS_HREDRAW | CS_VREDRAW;	// We require this to specify this option if height or width is ever changed by the user
	windowClass.lpfnWndProc = WindowProcedure;	// Pointer to the callback function defined in the "Windows Function Forward Declarations" section
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hInstance = applicationHandle;	// Handle to the instance holding the window procedure
	windowClass.hIcon = LoadIcon(0, IDI_APPLICATION);	// Load the default window icon
	windowClass.hCursor = LoadCursor(0, IDC_ARROW);		// Load the default arrow curser
	windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));	// HBRUSH can be used to set the background color of the window
	windowClass.lpszMenuName = NULL;	// The window has no menu 
	windowClass.lpszClassName = L"WindowClass";	// Name for the window class

	if (RegisterClassEx(&windowClass) == 0) {

		// If registration of the window class fails, then the window can't be created. The WindowInitialize function returns false and 
		// sends an error message to the console as a string
		cout << "Window Class Registration Error: Window Class could not be registered due to the following error\n"
			<< "Error Code: " << GetLastError() << endl;
	}

	RECT wRC = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&wRC, WS_OVERLAPPEDWINDOW, FALSE);

	windowHandle = CreateWindow(

		L"WindowClass",			// The name of the previously defined window class (Wide string, 16 bit)
		L"TA15 Krigare",		// Text to display at the window title bar (Wide string, 16 bit)
		WS_OVERLAPPEDWINDOW,	//The window style is an overlapped window. An overlapped window has a title bar and a border.
		CW_USEDEFAULT,			// The window's x position in pixels from the monitor's upper left corner
		CW_USEDEFAULT,			// The window's y position in pixels from the monitor's upper left corner
		wRC.right - wRC.left,	// Window width in pixels defined in "Global Window Variables"
		wRC.bottom - wRC.top,	// Windiw height in pixels defined in "Global Window Variables"
		NULL,					// Here we would normally set the parent, but we only have one window
		NULL,					// Since the window class isn't using a menu, we don¨t have a handle to it
		applicationHandle,		// Handle to the application containing the window 
		NULL					// Information that we can send to the WM_CREATE message through its lParam member
	);

	if (windowHandle == NULL) {

		cout << "Window Handle Error: Window Handle was not valid and window couldn't be created due to the following error\n"
			<< "Error Code: " << GetLastError() << endl;
	}

	// The window doesn't show by default, so we must call for it and update it by using the following functions

	SetCapture(windowHandle);
	ShowWindow(windowHandle, SW_SHOWDEFAULT);
	UpdateWindow(windowHandle);
	return true;
}

void showFPS(HWND windowHandle, float deltaTime) {

	static int interval;
	
	int fpsCounter = 1.0f / deltaTime;
	
	stringstream text_FPS;
	
	text_FPS << "FPS: " << fpsCounter;

	interval++;

	if (interval == 200) {

		SetWindowTextA(windowHandle, text_FPS.str().c_str());

		interval = 0;
	}
}

LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {

	case WM_DESTROY:	// This case only happens when the user presses the window's close button

		PostQuitMessage(0);	// We post a WM_QUIT message with the exit code 0
		break;

	case WM_KEYDOWN:

		if (wParam == VK_ESCAPE) {

			PostQuitMessage(0);
			break;	
		}

	default:

		// If a message has not been handled, meaning the window is still open, we sent it to our default window procedure for handling
		return DefWindowProc(windowHandle, message, wParam, lParam);
	}
	return 0;
}