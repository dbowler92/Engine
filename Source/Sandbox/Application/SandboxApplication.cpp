#include "SandboxApplication.h"

#include <iostream> //printf()

SandboxApplication::SandboxApplication()
{}

bool SandboxApplication::InitApplication()
{
	printf("SandboxApplication::InitApplication()\n");
	return true;
}

bool SandboxApplication::ShutdownApplication()
{
	printf("SandboxApplication::ShutdownApplication()\n");
	return true;
}