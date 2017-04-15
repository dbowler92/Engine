//CommonApplication.h
//Created 15/14/17
//Created By Daniel Bowler
//
//Interface class which all platform specific application classes should
//implement. 

#pragma once

#include "../../Config/EngineConfig.h" //Engine settings

class CommonApplication
{
public:
	//Game title
	virtual char *GetGameTitle() = 0;

	//Inits your game - called after engine and subsystems have been 
	//inited. Here, you would create your game specific subsystems and
	//assets etc. 
	virtual bool InitApplication() = 0;

	//Called when the engine is shutdown before closing the actual engine. 
	virtual bool ShutdownApplication() = 0;
};