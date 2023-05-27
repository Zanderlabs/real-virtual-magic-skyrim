#include "Helper.h"

namespace RealVirtualMagic
{

	RelocAddr<_DamageActorValue> DamageActorValue(0x09848B0);
	RelocAddr<_RestoreActorValue> RestoreActorValue(0x0986480);

	
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

}