#pragma once


#include "config.h"
#include "MenuChecker.h"
#include "lsl\lsl_cpp.h"


namespace RealVirtualMagic
{
	const std::string MOD_VERSION = "0.4.1";

	void CreateSystem();
	void InitializeIXRStream();
	void CreateEventStream();
	double GetFocusValue();
	void WriteEventMarker(const std::string& eventType);
}
