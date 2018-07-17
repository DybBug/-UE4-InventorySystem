// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DDOperation_Weapon.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UDDOperation_Weapon : public UDragDropOperation
{
	GENERATED_BODY()
	
protected :
	class UWidget_WeaponSlot* m_pDraggedSlot;
	
	
public:
	/* Get */
	FORCEINLINE class UWidget_WeaponSlot* const& GetDraggedSlot() const { return m_pDraggedSlot; }

	/* Set */
	FORCEINLINE void SetDraggedSlot(class UWidget_WeaponSlot* _pSlot) { m_pDraggedSlot = _pSlot; }
};
