// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DDOperation_Item.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UDDOperation_Item : public UDragDropOperation
{
	GENERATED_BODY()
	
protected :
	class UWidget_InventorySlot* m_pDraggedSlot;
	bool m_bWasShiftDown;

public :
	void Drop_Implementation(const FPointerEvent& PointerEvent);
	void DragCancelled_Implementation(const FPointerEvent& PointerEvent);
	void Dragged_Implementation(const FPointerEvent& PointerEvent);

	/* Get */
	FORCEINLINE class UWidget_InventorySlot* const& GetDraggedSlot() const { return m_pDraggedSlot; }

	FORCEINLINE const bool& GetWasShiftDown() const { return m_bWasShiftDown; }


	/* Set */
	FORCEINLINE void SetDraggedSlot(class UWidget_InventorySlot* const& _pDraggedSlot) { m_pDraggedSlot = _pDraggedSlot; }

	FORCEINLINE void SetWasShiftDown(const bool& _bDown) { m_bWasShiftDown = _bDown; }
};
