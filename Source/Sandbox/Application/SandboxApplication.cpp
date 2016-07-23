#include "SandboxApplication.h"

SandboxApplication::SandboxApplication()
{
	OutputDebugStringW(L"SandboxApplication::SandboxApplication()\n");
}

HICON SandboxApplication::VGetIcon()
{
	return LoadIcon(hInstance, MAKEINTRESOURCE(101));
}