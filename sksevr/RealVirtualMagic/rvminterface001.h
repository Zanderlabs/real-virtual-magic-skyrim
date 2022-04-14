#pragma once
#include "skse64/PluginAPI.h"

namespace RVMPluginApi {
	// Returns an IRVMInterface001 object compatible with the API shown below
	// This should only be called after SKSE sends kMessage_PostLoad to your plugin
	class IRVMInterface001;
	IRVMInterface001* getRVMInterface001(const PluginHandle & pluginHandle, SKSEMessagingInterface * messagingInterface);

	// This object provides access to RVM's mod support API
	class IRVMInterface001
	{
	public:
		// Gets the build number
		virtual unsigned int getBuildNumber() = 0;

		//Returns latest BrainPower value
		virtual double GetBrainPower() = 0;
	};
}

extern RVMPluginApi::IRVMInterface001* g_rvmInterface;
