#include "Engine.h"


namespace RealVirtualMagic
{
	bool runOnce = false; 

	double latestBrainPower = 0.0;
	bool isInCombat = false;

	std::atomic<bool> LeftHandSpellCastingOn;
	std::atomic<bool> RightHandSpellCastingOn;
	std::atomic<bool> DualSpellCastingOn;
	std::atomic<bool> DualAimedStarted;
	std::atomic<bool> LeftAimedStarted;
	std::atomic<bool> RightAimedStarted;

	// Animation graph hook for the player that allows specific animations to be blocked from playing
	typedef uint64_t(*IAnimationGraphManagerHolder_NotifyAnimationGraph_VFunc)(uintptr_t* iagmh, BSFixedString* animName);
	IAnimationGraphManagerHolder_NotifyAnimationGraph_VFunc g_originalNotifyAnimationGraph = nullptr; // Normally a JMP to 0x00501530
	static RelocPtr<IAnimationGraphManagerHolder_NotifyAnimationGraph_VFunc> IAnimationGraphManagerHolder_NotifyAnimationGraph_vtbl(0x016E2BF8);
	
	uint64_t Hooked_IAnimationGraphManagerHolder_NotifyAnimationGraph(uintptr_t* iAnimationGraphManagerHolder, BSFixedString* animationName)
	{
		//_MESSAGE("Called %s", animationName->data);

		if (strcmp(animationName->data, "MRh_Equipped_Event") == 0)
		{
			RightHandSpellCastingOn.store(false);
			DualSpellCastingOn.store(false);
		}
		else if (strcmp(animationName->data, "MLh_Equipped_Event") == 0)
		{
			LeftHandSpellCastingOn.store(false);
			DualSpellCastingOn.store(false);
		}
		else if (strcmp(animationName->data, "DualMagic_SpellAimedConcentrationStart") == 0 || strcmp(animationName->data, "DualMagic_SpellSelfConcentrationStart") == 0 || strcmp(animationName->data, "RitualSpellAimConcentrationStart") == 0)
		{
			DualSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MRh_SpellAimedConcentrationStart") == 0 || strcmp(animationName->data, "MRh_SpellSelfConcentrationStart") == 0 || strcmp(animationName->data, "MRh_SpellTelekinesisStart") == 0)
		{
			RightHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MLh_SpellAimedConcentrationStart") == 0 || strcmp(animationName->data, "MLh_SpellSelfConcentrationStart") == 0 || strcmp(animationName->data, "MLh_SpellTelekinesisStart") == 0)
		{
			LeftHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MLh_SpellAimedStart") == 0 || strcmp(animationName->data, "MLh_SpellSelfStart") == 0)
		{
			LeftAimedStarted.store(true);
			LeftHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "MRh_SpellAimedStart") == 0 || strcmp(animationName->data, "MRh_SpellSelfStart") == 0)
		{
			RightAimedStarted.store(true);
			RightHandSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "DualMagic_SpellAimedStart") == 0 || strcmp(animationName->data, "DualMagic_SpellSelfStart") == 0 || strcmp(animationName->data, "RitualSpellStart") == 0)
		{
			DualAimedStarted.store(true);
			DualSpellCastingOn.store(true);
			StartCast();
			std::thread t2(CheckMagickCasting);
			t2.detach();
		}
		else if (strcmp(animationName->data, "RitualSpellOut") == 0)
		{
			if (DualAimedStarted.load() || DualSpellCastingOn.load())
			{
				DualAimedStarted.store(false);
				DualSpellCastingOn.store(false);
				EndCast();
			}
		}
		else if (strcmp(animationName->data, "MRh_SpellRelease_Event") == 0)
		{
			if (RightAimedStarted.load())
			{
				RightAimedStarted.store(false);
				RightHandSpellCastingOn.store(false);
				EndCast();
			}
			if (DualAimedStarted.load())
			{
				DualAimedStarted.store(false);
				DualSpellCastingOn.store(false);
				EndCast();
			}
		}
		else if (strcmp(animationName->data, "MLH_SpellRelease_event") == 0)
		{
			if (LeftAimedStarted.load())
			{
				LeftAimedStarted.store(false);
				LeftHandSpellCastingOn.store(false);
				EndCast();
			}
			if (DualAimedStarted.load())
			{
				DualAimedStarted.store(false);
				DualSpellCastingOn.store(false);
				EndCast();
			}
		}
		// return 0 to block the animation from playing
		// Otherwise return...
		return   g_originalNotifyAnimationGraph(iAnimationGraphManagerHolder, animationName);
	}

	void GameLoad()
	{
		if ((bool)useDebugger)
		{
			WaitForDebugger(true);
		}

		if (runOnce == false)
		{
			runOnce = true;

			//This stuff runs only once after game loaded
			LoadGlobalValues();

		}
	}

	void HandHapticFeedbackEffect(bool left)
	{
		if (*g_openVR)
		{
			for (int i = 0; i < hapticStrength; i++)
			{
				if (leftHandedMode == true)
				{
					left = !left;
				}
				LOG_INFO("Triggering haptic pulse for %g on %s...", (float)hapticFrequency, left ? "left hand" : "right hand");
				(*g_openVR)->TriggerHapticPulse(left ? BSVRInterface::kControllerHand_Left : BSVRInterface::kControllerHand_Right, (float)hapticFrequency / 3999.0f);
			}
		}
		else
		{
			LOG("g_openVR is false...");
		}
	}

	void LeftHandedModeChange()
	{
		const int value = vlibGetSetting("bLeftHandedMode:VRInput");
		if (value != leftHandedMode)
		{
			leftHandedMode = value;
			LOG_ERR("Left Handed Mode is %s.", leftHandedMode ? "ON" : "OFF");
		}
	}

	void CheckMagickCasting()
	{
		while (IsCasting())
		{
			Sleep(22); // 1 frame with reprojection in most HMDs
		}
		EndCast();

	}

	bool IsCasting()
	{
		return (DualSpellCastingOn.load() || LeftHandSpellCastingOn.load() || RightHandSpellCastingOn.load());
	}

	void GameLogic()
	{
		// set up the LSL stream in the beginning
		CreateSystem();

		// game logic runs forever here
		while (true)
		{
			if (ActorInCombat(*g_thePlayer) && !isInCombat)
			{
				WriteEventMarker("start:combat");
				isInCombat = true;
			}
			else if (!ActorInCombat(*g_thePlayer) && isInCombat)
			{
				WriteEventMarker("stop:combat");
				isInCombat = false;
			}

			// this will be true only when the LSL stream is there
			if (IXRInitialized)
			{
				float newBrainPower = GetFocusValue();

				// only do actual game stuff if the brain power has changed
				if (newBrainPower != latestBrainPower)
				{
					latestBrainPower = newBrainPower;

					LOG("------------------");
					LOG("final focus value: %f", latestBrainPower);

					ApplyFocusValue(latestBrainPower);

					Sleep(50); // we expect new values every roughly 50ms
				}
			}
			else
			{
				LOG_ERR("IXR not initialized. Attempting to create LSL system.");
				LOG_ERR("Setting focus to 0.5 to allow playing");
				ApplyFocusValue(0.5f);
				// set up the LSL stream
				CreateSystem();
			}
		}
	}

	void StartFunction()
	{
		
		LeftHandedModeChange();

		//You can create a thread here like this and run all your game logic in that
		std::thread t1(GameLogic);
		t1.detach();

	}

	void WaitForDebugger(bool should_break = false) {
		while (!IsDebuggerPresent())
			Sleep(100);
		if (should_break)
			DebugBreak();
	}

	void HookAnimations()
	{
		g_originalNotifyAnimationGraph = *IAnimationGraphManagerHolder_NotifyAnimationGraph_vtbl;
		SafeWrite64(IAnimationGraphManagerHolder_NotifyAnimationGraph_vtbl.GetUIntPtr(), uintptr_t(Hooked_IAnimationGraphManagerHolder_NotifyAnimationGraph));
	}

	float GetBrainPower(StaticFunctionTag* base)
	{
		return latestBrainPower;
	}

	bool RegisterFuncs(VMClassRegistry* registry)
	{
		registry->RegisterFunction(new NativeFunction0<StaticFunctionTag, float>("GetBrainPower", "RealVirtualMagic_PluginScript", GetBrainPower, registry));

		LOG("RealVirtualMagic registerFunction");
		return true;
	}

	void StartCast()
	{
		WriteEventMarker("start:magicCast");

		if (IXRInitialized && latestBrainPower < unstableMagicThreshold)
		{
			float maxhealth = GetMaxHealth();
			float healthdamage = maxhealth * unstableMagicDamage / 100;
			LOG("damaging health by %f percent, resulting in a damage of %f health", unstableMagicDamage, healthdamage);
			LOG("current health: %f", GetCurrentHealth());
			std::thread t3(ChangeCurrentHealth, -healthdamage);
			t3.detach();
			//ChangeCurrentHealth(-healthdamage);
			LOG("new current health after changing: %f", GetCurrentHealth());
		}
	}


	void EndCast()
	{
		WriteEventMarker("stop:magicCast");
	}

	void ApplyFocusValue(float newFocus)
	{
		LOG("applying focus value of %f", newFocus);
		if ((*g_thePlayer) != nullptr && (*g_thePlayer)->loadedState != nullptr && !isGameStopped()) //Player is alive and Menu is not open
		{
			/*
			// applying magicka regen
			BSFixedString magickaRegenRate("fCombatMagickaRegenRateMult");

			SettingCollectionMap* settings = *g_gameSettingCollection;
			if (settings)
			{
				Setting* dsetting = settings->Get(magickaRegenRate.data);
				if (dsetting)
				{
					//Getting:
					//double jumpDist = 0.0;
					//dsetting->GetDouble(&jumpDist);

					//Setting:
					dsetting->SetDouble(1.0);
				}
			}
			*/

			// applying magic power
			float currentdestruction = GetDestruction();
			float newdestruction = minSpellpower + newFocus * (maxSpellpower - minSpellpower);
			LOG("destruction: %f", currentdestruction);
			LOG("setting magic power: %f", newdestruction);
			SetAllMagickPower(newdestruction);

			// applying magicka regen
			LOG("magicka rate: %f", GetMagickaRate());
			float newmagickarate = minMagickaRate + newFocus * (maxMagickaRate - minMagickaRate);
			if (newmagickarate > 0)
			{
				LOG("setting magicka rate: %f", newmagickarate);
				SetMagickaRate(newmagickarate);
			}
			else
			{
				// manual damage to magicka
				float maxmagicka = GetMaxMagicka();
				float magickadamage = (maxmagicka - (maxmagicka * (100 + newmagickarate) / 100)) / 20; // we expect a new focus value every 50ms so this should damage 1/20 of the desired damage, TODO make this dependent on the passed time
				LOG("new magicka rate negative, damaging magicka by %f percent, resulting in a damage of %f magicka", newmagickarate, magickadamage);
				LOG("setting magicka rate: %f", 0);
				SetMagickaRate(0);
				LOG("current magicka: %f", GetCurrentMagicka());
				if (GetCurrentMagicka() > 10)
				{
					ChangeCurrentMagicka(-magickadamage);
					LOG("new current magicka after changing: %f", GetCurrentMagicka());
				}
				else
				{
					LOG("magicka was below 10, did not damage to prevent crashes");
				}
			}

			// scale runes
			LOG("before scaling");
			if (strangeRunesModIndex != 9999)
			{
				LOG("in scaling");

				BSFixedString runeNodeName("RotNode");
				BSFixedString subRuneNodeName("Rune");
				BSFixedString leftHandNodeName("NPC L Hand [LHnd]");
				BSFixedString rightHandNodeName("NPC R Hand [RHnd]");

				float newScale = 0.5f + newFocus;

				NiNode* rootNodeTP = (*g_thePlayer)->GetNiRootNode(0);

				NiNode* rootNodeFP = (*g_thePlayer)->GetNiRootNode(2);

				NiNode* mostInterestingRoot = (rootNodeFP != nullptr) ? rootNodeFP : rootNodeTP;

				if (mostInterestingRoot)
				{
					LOG("In mostInterestingRoot");
					NiAVObject* lefthand_node = mostInterestingRoot->GetObjectByName(&leftHandNodeName.data);
					NiAVObject* righthand_node = mostInterestingRoot->GetObjectByName(&rightHandNodeName.data);
					if (lefthand_node != nullptr)
					{
						LOG("In left hand");
						NiAVObject* leftRuneNode = lefthand_node->GetObjectByName(&runeNodeName.data);
						if (leftRuneNode != nullptr)
						{
							LOG("in left RotNode");
							NiAVObject* leftRuneNodeSubRune = leftRuneNode->GetObjectByName(&subRuneNodeName.data);
							if (leftRuneNodeSubRune != nullptr)
							{
								LOG("Scaling left rune");
								leftRuneNodeSubRune->m_localTransform.scale = newScale;
							}
						}
					}
					if (righthand_node != nullptr)
					{
						LOG("In right hand");
						NiAVObject* rightRuneNode = righthand_node->GetObjectByName(&runeNodeName.data);
						if (rightRuneNode != nullptr)
						{
							LOG("in right RotNode");
							NiAVObject* rightRuneNodeSubRune = rightRuneNode->GetObjectByName(&subRuneNodeName.data);
							if (rightRuneNodeSubRune != nullptr)
							{
								LOG("Scaling right rune");
								rightRuneNodeSubRune->m_localTransform.scale = newScale;
							}
						}
					}
				}
			}


			// wait for a bit to make sure the new settings are applied before logging
			// Sleep(50);

			LOG("new destruction: %f", GetDestruction());
			LOG("new magicka rate: %f", GetMagickaRate());

		}
	}
}
