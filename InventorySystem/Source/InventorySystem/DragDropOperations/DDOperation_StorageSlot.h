// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DDOperation_StorageSlot.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UDDOperation_StorageSlot : public UDragDropOperation
{
	GENERATED_BODY()
	
protected :
	class UWidget_StorageSlot* m_pStorageSlotWidget;

public :
	FORCEINLINE class UWidget_StorageSlot* const& GetStorageSlotWidget() const { return m_pStorageSlotWidget; }

	void SetStorageSlotWidget(class UWidget_StorageSlot* _pWidget) { m_pStorageSlotWidget = _pWidget;}
	
};
