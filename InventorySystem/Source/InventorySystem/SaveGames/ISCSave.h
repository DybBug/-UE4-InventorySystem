// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ItemClasses/Item_Weapon.h"
#include "ISCSave.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UISCSave : public USaveGame
{
	GENERATED_BODY()
	
public :
	UPROPERTY()
	int m_SavedGold;

	UPROPERTY()
	int m_SavedHP;

	UPROPERTY()
	TSubclassOf<class AItem_Weapon> m_SavedEquippedWeapon;
	
	
};
