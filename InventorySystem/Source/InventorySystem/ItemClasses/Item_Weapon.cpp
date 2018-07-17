// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_Weapon.h"
#include "Components/Component_Inventory.h"
#include "InventorySystemCharacter.h"


void AItem_Weapon::EventOnUsed()
{
	bool bSuccess = m_pInventory->GetPlayer()->EquipItem(this);
	if (bSuccess)
	{

	}
	else
	{
		Destroy();
	}

}
