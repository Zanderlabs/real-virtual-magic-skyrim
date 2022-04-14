#include "rvminterface001.h"

// A message used to fetch RVM's interface
struct RVMMessage
{
	enum {	kMessage_GetInterface = 0x70e770bf	}; // Randomly chosen by the universe
	void* (*GetApiFunction)(unsigned int revisionNumber) = nullptr;
};

// Stores the API after it has already been fetched
static RVMPluginApi::IRVMInterface001 * g_rvmInterface = nullptr;

// Fetches the interface to use from RVM
RVMPluginApi::IRVMInterface001 * RVMPluginApi::getRVMInterface001(const PluginHandle & pluginHandle, SKSEMessagingInterface * messagingInterface)
{
	// If the interface has already been fetched, return the same object
	if (g_rvmInterface) {
		return g_rvmInterface;
	}

	// Dispatch a message to get the plugin interface from RVM
	RVMMessage rvmMessage;
	if(messagingInterface->Dispatch(pluginHandle, RVMMessage::kMessage_GetInterface, (void*)&rvmMessage, sizeof(RVMMessage*), "RealVirtualMagic"))
	{
		_MESSAGE("RVM dispatch message returned true...");
	}
	else
	{
		_MESSAGE("RVM dispatch message returned false...");
	}
	
	if (!rvmMessage.GetApiFunction)
	{
		return nullptr;
	}

	// Fetch the API for this version of the RVM interface
	g_rvmInterface = static_cast<IRVMInterface001*>(rvmMessage.GetApiFunction(1));
	return g_rvmInterface;
}
