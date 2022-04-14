#include "skse64_common/skse_version.h"
#include <shlobj.h>
#include <intrin.h>
#include <string>

#include "skse64/PluginAPI.h"	
#include "Engine.h"
#include "rvmpluginapi.h"

static SKSEMessagingInterface		* g_messaging = NULL;
static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;
static SKSEObjectInterface         * g_object = NULL;

#pragma comment(lib, "Ws2_32.lib")

void SetupReceptors()
{
	_MESSAGE("Building Event Sinks...");

	MenuManager * menuManager = MenuManager::GetSingleton();
	if (menuManager)
		menuManager->MenuOpenCloseEventDispatcher()->AddEventSink(&RealVirtualMagic::menuEvent);
}


extern "C" {

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\RealVirtualMagic.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
		//gLog.SetLogLevel(IDebugLog::kLevel_FatalError);

		std::string logMsg("RealVirtualMagic: ");
		logMsg.append(RealVirtualMagic::MOD_VERSION);
		_MESSAGE(logMsg.c_str());

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "RealVirtualMagic";
		info->version = 00000100; 

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();
		
		std::string skseVers = "SKSE Version: ";
		skseVers += std::to_string(skse->runtimeVersion);
		_MESSAGE(skseVers.c_str());

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion < CURRENT_RELEASE_RUNTIME)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}
		
		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}


	//Listener for SKSE Messages
	void OnSKSEMessage(SKSEMessagingInterface::Message* msg)
	{
		if (msg)
		{
			if (msg->type == SKSEMessagingInterface::kMessage_InputLoaded)
				SetupReceptors();
			else if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
			{				
				RealVirtualMagic::loadConfig();
				RealVirtualMagic::GameLoad();	
				RealVirtualMagic::StartFunction();
			}
			else if (msg->type == SKSEMessagingInterface::kMessage_PostLoad)
			{
				// Register our own mod api listener
				g_messaging->RegisterListener(g_pluginHandle, nullptr, RVMPluginApi::modMessageHandler);
			}
		}
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse) {	// Called by SKSE to load this plugin
		
		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

		g_messaging = (SKSEMessagingInterface*)skse->QueryInterface(kInterface_Messaging);
		g_messaging->RegisterListener(g_pluginHandle, "SKSE", OnSKSEMessage);

		//Check if the function registration was a success...
		bool bSuccess = g_papyrus->Register(RealVirtualMagic::RegisterFuncs);

		if (bSuccess) {
			_MESSAGE("Register Succeeded");
		}

		RealVirtualMagic::HookAnimations();
				
		return true;
	}
};