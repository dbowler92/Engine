//Application.h
//Created 23/07/16
//Created by Daniel Bowler
//
//Application layer of the engine. Each game that uses this engine
//should define a game specific class that inherits from this
//base application class & implement the virtual functions.

#pragma once

#include <Windows.h>
#include "../Main/EngineStd.h"
#include "../3rdParty/DXUT/Core/DXUT.h"

namespace EngineAPI 
{
	namespace Base
	{
		class Application
		{
			HIDE_COPY_ASSIGNMENT(Application)
		public:
			Application();

			//
			//Virtual functions. Game specific instance of this class should implement these
			//
			virtual TCHAR *VGetGameTitle() = 0;
			virtual TCHAR *VGetGameAppDirectory() = 0;
			virtual HICON VGetIcon() = 0;

		public:
			//
			//Win32
			//
			//Init app. Will bring up a W32 window - Optionally, can provide additional init code. 
			virtual bool Init(HINSTANCE hInstance, LPWSTR lpCmdLine, HWND hWnd = NULL,
				int screenWidth = 960, int screenHeight = 540);

		public:
			//
			//DXUT callbacks
			// 
			static bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
			static HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
			static HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
			static void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext); 
			static void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
			static bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);

			//DXUT handles our W32 message pump
			static LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void *pUserContext);

			//DXUT Handles our main loop
			static void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext);
			static void CALLBACK OnUpdateGame(double fTime, float fElapsedTime, void *pUserContext);

		protected:
			HINSTANCE hInst;			  //App instance handle
			unsigned appWidth, appHeight; //Dimentions of application window
		};
	};
};

//Define a global pointer to the singleton app instance. Override Application
//and provide a project specific version
extern EngineAPI::Base::Application* g_App;