#pragma once
#include "rvminterface001.h"
#include "skse64/PluginAPI.h"
#include "Engine.h"

// RVM build numbers are made up as follows: V0.1.2
#define RVM_BUILD_NUMBER 012

namespace RVMPluginApi {
	// Handles skse mod messages requesting to fetch API functions from RVM
	void modMessageHandler(SKSEMessagingInterface::Message * message);

	// This object provides access to RVM's mod support API version 1
	class RVMInterface001 : IRVMInterface001
	{
	public:
		// Gets the build number
		virtual unsigned int getBuildNumber();

		//Returns latest BrainPower value
		virtual double GetBrainPower();
	};
}
