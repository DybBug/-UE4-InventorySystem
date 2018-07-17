// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_HealthPotion.h"
#include "Components/Component_Inventory.h"
#include "InventorySystemCharacter.h"


void AItem_HealthPotion::EventOnUsed()
{
	int CurrentHealth = m_pInventory->GetPlayer()->GetCurrentHealth();
	int MaxHealth = m_pInventory->GetPlayer()->GetMaxHealth();

	if (CurrentHealth < MaxHealth)
	{
		m_pInventory->GetPlayer()->IncreaseHealth(10);

		Super::EventOnUsed();
	}
}


