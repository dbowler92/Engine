#include "SandboxApplication.h"

SandboxApplication::SandboxApplication()
{}

HICON SandboxApplication::VGetIcon()
{
	return LoadIcon(hInst, MAKEINTRESOURCE(101));
}