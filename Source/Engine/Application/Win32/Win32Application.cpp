#include "Win32Application.h"
#include <sstream>

//Init global reference to the app. 
EngineAPI::Core::Platform::Win32Application *g_App = NULL;

using namespace EngineAPI::Core::Platform;

LRESULT WINAPI GlobalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid
	// This pointer will have been set before the WNDCLASS struct has been given
	// its message processing function pointer. 
	if (g_App)
		return g_App->WndProc(hwnd, msg, wParam, lParam);
	else
	{
		MessageBox(0, L"Unable to process Windows Message.", 0, 0);
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

Win32Application::Win32Application()
	: appPaused(false), minimized(false), maximized(false), resizing(false)
{}

LRESULT Win32Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			appPaused = true;
			mainGameLoopTimer.Stop();
		}
		else
		{
			appPaused = false;
			mainGameLoopTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
	{
		//Update size of window
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{
			appPaused = true;
			minimized = true;
			maximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			appPaused = false;
			minimized = false;
			maximized = true;
			assert(OnResize() == true);
		}
		else if (wParam == SIZE_RESTORED)
		{

			// Restoring from minimized state?
			if (minimized)
			{
				appPaused = false;
				minimized = false;
				assert(OnResize() == true);
			}

			// Restoring from maximized state?
			else if (maximized)
			{
				appPaused = false;
				maximized = false;
				assert(OnResize() == true);
			}
			else if (resizing)
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				assert(OnResize() == true);
			}
		}
		return 0;
	}
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		appPaused = true;
		resizing = true;
		mainGameLoopTimer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		appPaused = false;
		resizing = false;
		mainGameLoopTimer.Start();
		assert(OnResize() == true);
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_CHAR:
		//Quit the application using the Escape Key - Will be taken
		//out in release build. 
		if (wParam == VK_ESCAPE)
		{
			//Post the quit message. 
			PostQuitMessage(0);
			return 0;
		}
	}//End switch(msg) {...}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Win32Application::InitEngine(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd, 
	int appVersionMajor, int appVersionMinor, int appVersionPatch,
	int screenWidth, int screenHeight)
{
	//   
	//TODO: Plenty more init work that needs to be completed once I get to the relivent
	//chapters in the book. Eg: Registering all engine events
	// 
	EngineAPI::Debug::DebugLog::PrintInfoMessage("Win32Application::InitEngine()\n");

	//Store data             
	osWindow.SetWin32AppInstance(hInstance);
	osWindow.UpdateWindowWidth((unsigned)screenWidth);
	osWindow.UpdateWindowHeight((unsigned)screenHeight);
	this->appVersionMajor = appVersionMajor;
	this->appVersionMinor = appVersionMinor;
	this->appVersionPatch = appVersionPatch;

	//Setup window first.
	bool success = InitWin32App();

	//Init engine subsystems - eg: Graphics
	if (success)
		return InitEngineSubsystems();
	else
		return success; //False
}

bool Win32Application::ShutdownEngine()
{
	EngineAPI::Debug::DebugLog::PrintInfoMessage("Win32Application::ShutdownEngine()\n");

	//Shutdown subsystems - reverse order to creation
	if (graphicsSubsystem)
	{
		graphicsSubsystem->ShutdownSubsystem();
		delete graphicsSubsystem;
		graphicsSubsystem = nullptr;
	}

	return true;
}

bool Win32Application::InitWin32App()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = GlobalWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = osWindow.GetAppInstanceHandle();
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"WndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	// Will be the same values as the initial values specified in the two int consts
	// in the header file. 
	RECT R = { 0, 0, (LONG)osWindow.GetWindowWidth(), (LONG)osWindow.GetWindowHeight()};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	//Windows app name from char to TCHAR
	std::string str(GetGameTitle());
	TCHAR *windowsAppTitle = GE_NEW TCHAR[str.size() + 1];
	windowsAppTitle[str.size()] = 0;
	for (int i = 0; i < (int)str.size(); ++i)
		windowsAppTitle[i] = str[i];

	//Create the window.
	HWND mainWnd = CreateWindow(L"WndClassName", windowsAppTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, 0, 0, osWindow.GetAppInstanceHandle(), 0);

	if (!mainWnd)
	{
		delete[] windowsAppTitle;
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	//Update with new handle. 
	osWindow.SetWin32AppMainWindowHandle(mainWnd);

	//Display window.
	ShowWindow(mainWnd, SW_SHOW);
	UpdateWindow(mainWnd);

	//Cleanup
	delete[] windowsAppTitle;

	//Done
	return true;
}

bool Win32Application::InitEngineSubsystems()
{
	//Init graphics
	//graphicsSubsystem = GE_NEW EngineAPI::Graphics::GraphicsManager();
	graphicsSubsystem = EngineAPI::Graphics::GraphicsManager::GetInstance();
	if (!graphicsSubsystem->InitSubsystem(&osWindow, 
		GetGameTitle(), appVersionMajor, appVersionMinor, appVersionPatch, 
		osWindow.GetWindowWidth(), osWindow.GetWindowHeight()))
		return false;

	//Done
	return true;
}

bool Win32Application::OnResize()
{
	/*
	std::stringstream ss;
	std::stringstream ss2;
	ss << windowWidth;
	ss2 << windowHeight;

	EngineAPI::Debug::DebugLog::PrintInfoMessage("OnResize()\n");
	EngineAPI::Debug::DebugLog::PrintInfoMessage(ss.str().c_str());
	EngineAPI::Debug::DebugLog::PrintMessage(", ");
	EngineAPI::Debug::DebugLog::PrintMessage(ss2.str().c_str());
	EngineAPI::Debug::DebugLog::PrintMessage("\n");
	*/

	//Is the current window size the same as the new one? If so, skip resizing 
	//since its not needed (this occurs if the user drags the screen around, for example)
	if (osWindow.GetWindowWidth() == windowWidth && osWindow.GetWindowHeight() == windowHeight)
	{
		return true; //Didn't need to resize anything. However, this isnt an error. 
	}

	//Update OS window size with new info
	osWindow.UpdateWindowWidth((unsigned)windowWidth);
	osWindow.UpdateWindowHeight((unsigned)windowHeight);

	//Resize render targets, etc
	if (!graphicsSubsystem->OnResize(&osWindow))
	{
		EngineAPI::Debug::DebugLog::PrintErrorMessage("Win32Application::OnResize() Erorr: Graphics subsystem failed on resize event\n");
		return false;
	}

	//Done
	return true;
}

void Win32Application::EnterGameLoop()
{
	MSG msg = { 0 };
	mainGameLoopTimer.Reset();
	while (msg.message != WM_QUIT)
	{
		//If there windows messages, then process them
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//Otherwise, proceed with the game loop - which is a very simple
		//example of a loop. 
		else
		{
			mainGameLoopTimer.Tick();
			if (!appPaused)
			{
				CalculateFrameRateStats();

				//Update and render (this calls the users Application subclasses 
				//update and render functions). 
				//
				//In terms of the rendering, it is the applications responsibility
				//to ensure they bind the swapchain at the correct time && to call
				//present when they are finished (for now, anyway). 
				UpdateScene(mainGameLoopTimer.DeltaTime());
				RenderScene();
			}
			else
				Sleep(100);
		}
	}// while (msg.message != WM_QUIT)
}

void Win32Application::CalculateFrameRateStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mainGameLoopTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(8);
		outs << (GetGameTitle()) << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(osWindow.GetAppMainWindowHandle(), outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}