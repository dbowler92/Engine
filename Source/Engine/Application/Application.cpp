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

	//Setup W32 application & DXUT
	DXUTInit(true, true, lpCmdLine, true);
	if (hWnd == NULL)
		DXUTCreateWindow(VGetGameTitle(), hInstance, VGetIcon());
	else
		DXUTSetWindow(hWnd, hWnd, hWnd);

	if (!DXUTGetHWND())
		return false;

	SetWindowText(DXUTGetHWND(), VGetGameTitle());

	DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, appWidth, appHeight);

	return true;
}

//
//DXUT Callbacks
//
bool CALLBACK Application::IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo, DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}

HRESULT CALLBACK Application::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}

HRESULT CALLBACK Application::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}

void CALLBACK Application::OnD3D11ReleasingSwapChain(void* pUserContext)
{

}

void CALLBACK Application::OnD3D11DestroyDevice(void* pUserContext)
{

}

bool CALLBACK Application::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}

LRESULT CALLBACK Application::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void *pUserContext)
{
	return 0;
}

void CALLBACK Application::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime, float fElapsedTime, void* pUserContext)
{

}

void CALLBACK Application::OnUpdateGame(double fTime, float fElapsedTime, void *pUserContext)
{

}
