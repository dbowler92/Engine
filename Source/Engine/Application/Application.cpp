#include "Application.h"

//Init global reference to the app. 
EngineAPI::Base::Application *g_App = NULL;

using namespace EngineAPI::Base;

Application::Application()
{}

bool Application::Init(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd,
	int screenWidth, int screenHeight)
{   
	//   
	//TODO: Plenty more init work that needs to be completed once I get to the relivent
	//chapters in the book. Eg: Registering all engine events
	// 
	        
	//Store data             
	hInst = hInstance;   
	appWidth = screenWidth;       
	appHeight = screenHeight;        
	      
	//TODO: Setup W32 application  

	//SetWindowText(GetHWND(), VGetGameTitle());

	//TODO: Setup D3D11

	return true;
}

void Application::EnterGameLoop()
{

}

void Application::Shutdown()
{}