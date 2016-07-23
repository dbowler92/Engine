#include "Application.h"

//Init global reference to the app. 
EngineAPI::Base::Application *g_App = NULL;

using namespace EngineAPI::Base;

Application::Application()
{
	OutputDebugStringW(L"Application::Application()\n");
}

bool Application::Init(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd,
	int screenWidth, int screenHeight)
{
	OutputDebugStringW(L"Application::Init()\n");
	return true;
}