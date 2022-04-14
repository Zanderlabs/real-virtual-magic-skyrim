#pragma once
#include "skse64\NiNodes.h"
#include "skse64\NiTypes.h"
#include "skse64/GameObjects.h"
#include "skse64/NiExtraData.h"
#include "skse64/GameRTTI.h"
#include "skse64/InternalTasks.h"
#include "skse64/PluginAPI.h"
#include "skse64/PapyrusActor.h"
#include "skse64/PapyrusGame.h"
#include "skse64_common/SafeWrite.h"
#include "skse64/GameExtraData.h"
#include "RealVirtualMagic.h"

#include "MenuChecker.h"
#include "Engine.h"

namespace RealVirtualMagic
{
	typedef void(*_DamageActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor* thisActor, BSFixedString const& dmgValueName, float dmg);
	extern RelocAddr<_DamageActorValue> DamageActorValue; 
	typedef void(*_RestoreActorValue)(VMClassRegistry* VMinternal, UInt32 stackId, Actor* thisActor, BSFixedString const& dmgValueName, float amount);
	extern RelocAddr<_RestoreActorValue> RestoreActorValue;



	float GetCurrentMagicka();
	float GetMaxMagicka();
	void SetMaxMagicka(float amount);
	void ChangeCurrentMagicka(float amount);
	void ChangeCurrentMagickaFunc(float amount);

	float GetCurrentHealth();
	float GetMaxHealth();
	void SetMaxHealth(float amount);
	void ChangeCurrentHealth(float amount);
	void ChangeCurrentHealthFunc(float amount);

	float GetMagickaRate();
	float GetMagickaRateMult();
	void SetMagickaRate(float amount);

	float GetDestruction();
	void SetAllMagickPower(float amount);
	void ChangeCurrentDestruction(float amount);
	void ChangeCurrentDestructionFunc(float amount);
	void SetAllDamageResist(float amount);
}
