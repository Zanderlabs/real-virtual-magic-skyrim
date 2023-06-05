#include "Helper.h"

#include "skse64/PapyrusFormList.cpp"
#include <skse64/PapyrusActor.cpp>

namespace RealVirtualMagic
{

	RelocAddr<_DamageActorValue> DamageActorValue(0x09848B0);
	RelocAddr<_RestoreActorValue> RestoreActorValue(0x0986480);
	UInt32 strangeRunesModIndex = 9999;
	std::string strangeRunesName = "StrangeRunes.esp";
	RelocPtr<AIProcessManager*> g_AIProcessManager(0x1F831B0);

	bool ActorInCombat(Actor* actor)
	{
		UInt64* vtbl = *((UInt64**)actor);
		const bool combat = ((_IsInCombatNative)(vtbl[0xE5]))(actor);
		return combat;
	}
	
	float GetCurrentMagicka()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(25);
	}

	float GetMaxMagicka()
	{
		return (*g_thePlayer)->actorValueOwner.GetMaximum(25);
	}

	void SetMaxMagicka(float new_current_magicka)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(25, new_current_magicka);
	}

	void ChangeCurrentMagicka(float amount)
	{
		LOG("Changing Magicka by %g", amount);
		//(*g_thePlayer)->actorValueOwner.RestoreActorValue((amount < 0) ? 2 : 4, 26, abs(amount));

		std::thread t3(ChangeCurrentMagickaFunc, amount);
		t3.detach();
		//ChangeCurrentMagickaFunc(amount);
	}

	void ChangeCurrentMagickaFunc(float amount)
	{
		if (amount < 0)
		{
			DamageActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", abs(amount));
			Sleep(1500);
			const float currentMagicka = GetCurrentMagicka();
			if (currentMagicka < 0)
			{
				RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", 10 - currentMagicka);
			}
		}
		else
		{
			//Sleep(500);
			RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", abs(amount));
		}
	}

	float GetCurrentHealth()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(24);
	}

	float GetMaxHealth()
	{
		return (*g_thePlayer)->actorValueOwner.GetMaximum(24);
	}

	void SetMaxHealth(float new_current_magicka)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(24, new_current_magicka);
	}

	void ChangeCurrentHealth(float amount)
	{
		LOG("Changing Health by %g", amount);
		//(*g_thePlayer)->actorValueOwner.RestoreActorValue((amount < 0) ? 2 : 4, 26, abs(amount));

		//std::thread t3(ChangeCurrentMagickaFunc, amount);
		//t3.detach();
		ChangeCurrentHealthFunc(amount);
	}

	void ChangeCurrentHealthFunc(float amount)
	{
		if (amount < 0)
		{
			DamageActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Health", abs(amount));
			Sleep(1500);
			const float currentHealth = GetCurrentHealth();
			if (currentHealth < 0)
			{
				RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Health", 10 - currentHealth);
			}
		}
		else
		{
			//Sleep(500);
			RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Health", abs(amount));
		}
	}

	float GetMagickaRate()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(28);
	}
	
	float GetMagickaRateMult()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(156);
	}

	void SetMagickaRate(float new_current_magicka_rate)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(28, new_current_magicka_rate);
	}

	// ------------------- destruction
	float GetDestruction()
	{
		return (*g_thePlayer)->actorValueOwner.GetCurrent(149);
	}

	void SetAllMagickPower(float new_power)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(147, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(148, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(149, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(150, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(151, new_power);
		/*
			kAlterationPowerModifier = 147,
			kConjurationPowerModifier = 148,
			kDestructionPowerModifier = 149,
			kIllusionPowerModifier = 150,
			kRestorationPowerModifier = 151,
		*/
	}


	void SetAllDamageResist(float new_power)
	{
		(*g_thePlayer)->actorValueOwner.SetCurrent(39, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(40, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(41, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(42, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(43, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(44, new_power);
		(*g_thePlayer)->actorValueOwner.SetCurrent(45, new_power);
		/*
			kDamageResist = 39,
			kPoisonResist = 40,
			kResistFire = 41,
			kResistShock = 42,
			kResistFrost = 43,
			kResistMagic = 44,
			kResistDisease = 45,
		*/
	}

	

	void ChangeCurrentDestruction(float amount)
	{
		LOG("Changing Magicka by %g", amount);
		//(*g_thePlayer)->actorValueOwner.RestoreActorValue((amount < 0) ? 2 : 4, 26, abs(amount));

		//std::thread t3(ChangeCurrentMagickaFunc, amount);
		//t3.detach();
		ChangeCurrentDestructionFunc(amount);
	}

	void ChangeCurrentDestructionFunc(float amount)
	{
		if (amount < 0)
		{
			DamageActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", abs(amount));
			Sleep(1500);
			const float currentMagicka = GetCurrentMagicka();
			if (currentMagicka < 0)
			{
				RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", 10 - currentMagicka);
			}
		}
		else
		{
			//Sleep(500);
			RestoreActorValue((*g_skyrimVM)->GetClassRegistry(), 0, (*g_thePlayer), "Magicka", abs(amount));
		}
	}

	void LoadGlobalValues()
	{
		DataHandler* dataHandler = DataHandler::GetSingleton();

		if (dataHandler)
		{
			const ModInfo* strangeRunesModInfo = dataHandler->LookupLoadedModByName(strangeRunesName.c_str());

			if (strangeRunesModInfo)
			{
				if (IsValidModIndex(strangeRunesModInfo->modIndex))
				{
					strangeRunesModIndex = strangeRunesModInfo->modIndex;
					LOG_ERR("Strange Runes esp found. Mod Index: %x", strangeRunesModIndex);
				}
				else
				{
					LOG_ERR("Strange runes mod not found!");
				}
			}
		}
	}

	// Fetches information about a node
	std::string getNodeDesc(NiAVObject* node)
	{
		// Use periods to indicate the depth in the scene graph
		if (!node) {
			return "Missing node";
		}

		// Include the node's type followed by its name
		std::string text;
		if (node->GetRTTI() && node->GetRTTI()->name)
			text = text + node->GetRTTI()->name + " ";
		else
			text = text + "UnknownType ";
		if (node->m_name)
			text = text + node->m_name;
		else
			text = text + "Unnamed";

		// Include any extra data
		if (node->m_extraDataLen > 0)
			text = text + " {";
		for (int i = 0; i < node->m_extraDataLen; ++i) {
			// Fetch extra data, writing "NULL" for missing elements
			NiExtraData* extraData = node->m_extraData[i];
			if (i > 0)
				text = text + ", ";
			if (!extraData) {
				text = text + "Missing Data";
				continue;
			}
			else if (!extraData->m_pcName) {
				text = text + "(null)=";
			}
			else {
				text = text + extraData->m_pcName + "=";
			}

			// Cast the extra data to all supported types
			NiStringExtraData* stringData = DYNAMIC_CAST(extraData, NiExtraData, NiStringExtraData);
			NiStringsExtraData* stringsData = DYNAMIC_CAST(extraData, NiExtraData, NiStringsExtraData);
			NiBooleanExtraData* boolData = DYNAMIC_CAST(extraData, NiExtraData, NiBooleanExtraData);
			NiIntegerExtraData* intData = DYNAMIC_CAST(extraData, NiExtraData, NiIntegerExtraData);
			NiIntegersExtraData* intsData = DYNAMIC_CAST(extraData, NiExtraData, NiIntegersExtraData);
			NiFloatExtraData* floatData = DYNAMIC_CAST(extraData, NiExtraData, NiFloatExtraData);
			NiFloatsExtraData* floatsData = DYNAMIC_CAST(extraData, NiExtraData, NiFloatsExtraData);
			NiVectorExtraData* vectorData = DYNAMIC_CAST(extraData, NiExtraData, NiVectorExtraData);

			// Include NiStringExtraData
			if (stringData) {
				text = text + "\"" + stringData->m_pString + "\"";

				// Include NiStringsExtraData as an array
			}
			else if (stringsData) {
				text = text + "[";
				if (!stringsData->m_data) {
					text = text + "(null string data)";
				}
				else {
					for (int j = 0; j < stringsData->m_size; ++j) {
						if (j > 0)
							text = text + ", ";
						text = text + "\"" + stringsData->m_data[j] + "\"";
					}
				}
				text = text + "]";

				// Include NiBooleanExtraData
			}
			else if (boolData) {
				text = text + (boolData->m_data ? "True" : "False");

				// Include NiIntegerExtraData
			}
			else if (intData) {
				text = text + std::to_string(intData->m_data);

				// Include NiIntegersExtraData as an array
			}
			else if (intsData) {
				text = text + "[";
				if (!intsData->m_data) {
					text = text + "(null int data)";
				}
				else {
					for (int j = 0; j < intsData->m_size; ++j) {
						if (j > 0)
							text = text + ", ";
						text = text + std::to_string(intsData->m_data[j]);
					}
				}
				text = text + "]";

				// Include NiFloatExtraData
			}
			else if (floatData) {
				text = text + std::to_string(floatData->m_data);

				// Include NiFloatsExtraData as an array
			}
			else if (floatsData) {
				text = text + "[";
				if (!floatsData->m_data) {
					text = text + "(null float data)";
				}
				else {
					for (int j = 0; j < floatsData->m_size; ++j) {
						if (j > 0)
							text = text + ", ";
						text = text + std::to_string(floatsData->m_data[j]);
					}
				}
				text = text + "]";

				// Include NiVectorExtraData
			}
			else if (vectorData) {
				text = text + "<" + std::to_string(vectorData->m_vector[0]) +
					std::to_string(vectorData->m_vector[1]) +
					std::to_string(vectorData->m_vector[2]) +
					std::to_string(vectorData->m_vector[3]) + +">";

				// Include BSBehaviorGraphExtraData
			}
			else {
				text = text + extraData->GetRTTI()->name;
			}

		}
		if (node->m_extraDataLen > 0)
			text = text + "}";
		return text;
	}

	// Writes information about a node to the log file
	void logNode(int depth, NiAVObject* node)
	{
		auto text = std::string(depth, '.') + getNodeDesc(node);
		_MESSAGE("%d: %s", depth, text.c_str());
	}

	// Lists all parents of a bone to the log file
	void logParents(NiAVObject* bone)
	{
		NiNode* node = bone ? bone->GetAsNiNode() : 0;
		int depth = 1;
		while (node) {
			//auto blanks = std::string(depth, '.');
			//log("%sNode name = %s, RTTI = %s\n", blanks.c_str(), node->m_name, node->GetRTTI()->name);
			logNode(depth, node);
			node = node->m_parent;
			++depth;
		}
	}

	// Lists all children of a bone to the log file, filtering by RTTI type name
	void logChildren(NiAVObject* bone, int depth, int maxDepth, const char* filter)
	{
		if (!bone)
			return;

		if (filter != 0 && filter[0])
		{
			if (strcmp(bone->GetRTTI()->name, filter) != 0)
			{
				return;
			}
		}
		//auto blanks = std::string(depth, '.');
		//log("%sNode name = %s, RTTI = %s\n", blanks.c_str(), bone->m_name, bone->GetRTTI()->name);
		logNode(depth, bone);
		NiNode* node = bone ? bone->GetAsNiNode() : 0;
		if (!node)
			return;

		auto children = NINODE_CHILDREN(node);
		if (children)
		{
			//log("Children has %d %d %d %d\n", children->m_arrayBufLen, children->m_emptyRunStart, children->m_size, children->m_growSize);
			for (int i = 0; i < children->m_emptyRunStart; i++)
			{
				if (depth < maxDepth || maxDepth < 0)
				{
					logChildren(children->m_data[i], depth + 1, maxDepth, filter);
				}
			}
		}
	}

}