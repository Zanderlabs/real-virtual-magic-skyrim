#include "rvmpluginapi.h"

using namespace RVMPluginApi;


// A message used to fetch RVM's interface
struct RVMMessage
{
	enum { kMessage_GetInterface = 0x70e770bf	}; // Randomly chosen by the universe
	void* (*GetApiFunction)(unsigned int revisionNumber) = nullptr;
};

// Interface classes are stored statically
RVMInterface001 g_interface001;

// Constructs and returns an API of the revision number requested
void* getApi(unsigned int revisionNumber)
{
	return &g_interface001;
}

// Handles skse mod messages requesting to fetch API functions from RVM
void RVMPluginApi::modMessageHandler(SKSEMessagingInterface::Message* message)
{
	if (message != nullptr)
	{
		if (message->type == RVMMessage::kMessage_GetInterface)
		{
			RVMMessage* rvmMessage = (RVMMessage*)message->data;
			rvmMessage->GetApiFunction = getApi;
			_MESSAGE("...Provided RVM plugin interface to %s", message->sender);
		}
	}
}

double RVMInterface001::GetBrainPower()
{
	return RealVirtualMagic::latestBrainPower;
}

// Fetches the version number
unsigned int RVMInterface001::getBuildNumber()
{
	return RVM_BUILD_NUMBER;
}
