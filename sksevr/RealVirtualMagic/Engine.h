#include "RealVirtualMagic.h"
#include "Helper.h"

#include "skse64\GameMenus.h"
#include <thread>
#include <atomic>
#include "skse64/GameVR.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64\PapyrusEvents.h"
#include "skse64\PapyrusSpell.h"
#include "skse64\PapyrusForm.h"

namespace RealVirtualMagic 
{
	extern bool IXRInitialized;

	extern double latestBrainPower;

	
	void GameLoad();
	void StartFunction();
	void WaitForDebugger(bool should_break);
	void LeftHandedModeChange();
	void CheckMagickCasting();
	bool IsCasting();
	void HookAnimations();
	void StartCast();
	void EndCast();

	bool RegisterFuncs(VMClassRegistry* registry);
}