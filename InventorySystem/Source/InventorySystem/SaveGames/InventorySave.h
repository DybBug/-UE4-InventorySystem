// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InventorySystemStructure.h"
#include "InventorySave.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UInventorySave : public USaveGame
{
	GENERATED_BODY()
	
public :
	UPROPERTY()
	float m_SavedWeight;

	UPROPERTY()
	TArray<FInventorySlot> m_SavedSlots;

	UPROPERTY()
	TArray<FSavedPickup> m_SavedPickups;

	UPROPERTY()
	int m_SavedAmountOfSlots;

	UPROPERTY()
	TArray<FSavedHotkey> m_SavedHotkeys;
	
};
