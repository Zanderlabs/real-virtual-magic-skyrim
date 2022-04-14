#pragma once


#include "config.h"
#include "MenuChecker.h"
#include "brainflow\board_shim.h"
#include "brainflow\data_filter.h"


namespace RealVirtualMagic
{
	const std::string MOD_VERSION = "0.2.0";

	void CreateSystem();
	double GetBrainData();
	void WriteEventMarker(float eventType);
}
