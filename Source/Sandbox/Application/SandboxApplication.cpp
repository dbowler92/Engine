#include "SandboxApplication.h"

SandboxApplication::SandboxApplication()
{}

HICON SandboxApplication::GetIcon()
{
	//TODO: return LoadIcon(hInst, MAKEINTRESOURCE(101));
	return NULL;
}  

bool SandboxApplication::InitApplication()
{
	PrintApplicationTitleToConsole();
	std::cout << ": InitApplication()\n";

	//Set active scene

	return true;
}  