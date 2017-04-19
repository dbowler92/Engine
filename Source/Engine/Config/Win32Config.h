//Win32.h
//Created 18/04/17
//Created By Daniel Bowler
//
//Some config stuff for Win32 - removes it from EngineConfig.h which, it seems, may
//get too big for its own good!

#pragma once

//Win32 lean and mean!
#define WIN32_LEAN_AND_MEAN

//Macro which will select the correct subclass to inherit from for OS abstraction classes (eg:
//OSWindow will inherit from Win32OSWindow for Win32 (this) builds
#define OS_PLATFORM_IMPLEMENTATION(TYPE) Platform::##Win32##TYPE