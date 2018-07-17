// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Hotkey.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Hotkey : public UUserWidget
{
	GENERATED_BODY()

protected :
	//
	// #. ÆÈ·¹Æ®.
	//
	UPROPERTY()
	class UTextBlock* m_pKeyText;
	UPROPERTY()
	class UWidget_HotkeySlot* m_pHotkeySlotWidget;

	//
	// #. º¯¼ö.
	//
	FKey m_Key;
	class UComponent_Inventory* m_pInventory;

public :
	void InitializeWidget(const FKey& _Key, class UComponent_Inventory* _pInventory);

	/* Get */
	FORCEINLINE class UTextBlock*         const& GetKeyText() const { return m_pKeyText; }
	FORCEINLINE class UWidget_HotkeySlot* const& GetHotkeySlotWidget() const { return m_pHotkeySlotWidget; }
	
	FORCEINLINE const FKey& GetKey() const { return m_Key; }

	FORCEINLINE class UComponent_Inventory* const& GetInventory() const { return m_pInventory; }

protected :
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
