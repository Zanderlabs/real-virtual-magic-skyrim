#pragma once


#include "config.h"
#include "MenuChecker.h"
#include "lsl\lsl_cpp.h"


namespace RealVirtualMagic
{
	const std::string MOD_VERSION = "0.3.0";

	void CreateSystem();
	void InitializeIXRStream();
	void CreateEventStream();
	double GetFocusValue();
	void WriteEventMarker(float eventType);
}
