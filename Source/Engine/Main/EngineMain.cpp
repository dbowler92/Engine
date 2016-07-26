//EngineMain.cpp
//Created 21/07/16
//Created By Daniel Bowler
//
//Entry point for the engine

#pragma once

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "EngineStd.h"
#include "../Application/Application.h"

//Libs
#pragma comment (lib, "DXUT.lib")
#pragma comment (lib, "DXUTOpt.lib")
#pragma comment (lib, "comctl32.lib")

void ShowConsole()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);

	*stdout = *hf_out;
	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
}
  
int APIENTRY EngineMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance,
	LPWSTR     lpCmdLine,
	int       nCmdShow, 
	EngineAPI::Base::Application* gameAppInstance)
{    
	using namespace EngineAPI::Base;   

	//Set up checks for memory leaks.  
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Enable console for debugging purposes
	ShowConsole();   

	//Ensure an app exists.  
	if (gameAppInstance == NULL)
		printf("EngineMain Error: gameAppInstance == NULL. Make sure to create a project specific Application instance.\n");
	 
	//Set DXUT callbacks
	DXUTSetCallbackMsgProc(Application::MsgProc); 
	DXUTSetCallbackDeviceChanging(Application::ModifyDeviceSettings);
	DXUTSetCallbackD3D11DeviceAcceptable(Application::IsD3D11DeviceAcceptable);  
	DXUTSetCallbackD3D11DeviceCreated(Application::OnD3D11CreateDevice); 
	DXUTSetCallbackD3D11SwapChainResized(Application::OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11SwapChainReleasing(Application::OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(Application::OnD3D11DestroyDevice);
	DXUTSetCallbackFrameMove(Application::OnUpdateGame);
	DXUTSetCallbackD3D11FrameRender(Application::OnD3D11FrameRender);

	//Set app global pointer & init the application. 
	g_App = (EngineAPI::Base::Application*)gameAppInstance;
	if (!g_App->Init(hInstance, lpCmdLine, NULL, 960, 540))
		return -1;

	//Enter game loop
	DXUTMainLoop();

	//Once finished, shutdown the game
	DXUTShutdown();
	return 0;
}