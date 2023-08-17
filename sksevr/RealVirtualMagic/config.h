#pragma once
#include <iostream>
#include <string>
#include <fstream>

#include "skse64\GameSettings.h"
#include "Utility.hpp"


namespace RealVirtualMagic 
{
	//function declarations
	void loadConfig();
		

	//Config parameters
	extern int logging;
	extern int useDebugger;
	extern int useBCI;


	// game parameters
	extern float minMagickaRate;
	extern float maxMagickaRate;
	extern float minSpellpower;
	extern float maxSpellpower;
	extern float unstableMagicThreshold;
	extern float unstableMagicDamage;

	extern unsigned short hapticFrequency;
	extern int hapticStrength;


	//Other parameters
	extern bool leftHandedMode;


	void Log(const int msgLogLevel, const char* fmt, ...);
	enum eLogLevels
	{
		LOGLEVEL_ERR = 0,
		LOGLEVEL_WARN,
		LOGLEVEL_INFO,
	};

#define LOG(fmt, ...) Log(LOGLEVEL_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) Log(LOGLEVEL_ERR, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Log(LOGLEVEL_INFO, fmt, ##__VA_ARGS__)

}