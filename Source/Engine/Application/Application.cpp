#include "Application.h"

//Init global reference to the app. 
EngineAPI::Application *g_App = NULL;

EngineAPI::Application::Application()
{
	OutputDebugStringW(L"Application::Application()\n");
}