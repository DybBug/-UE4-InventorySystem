// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_HotkeySlot.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_HotkeySlot : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	//
	// #. ÆÈ·¹Æ®.
	//
	UPROPERTY()
	class UBorder* m_pSlotBG;
	UPROPERTY()
	class UImage* m_pIcon;
	UPROPERTY()
	class UTextBlock* m_pAmountText;

	//
	// #. º¯¼ö.
	//
	bool m_bEmpty = true;
	bool m_bDraggedOver = false;
	int m_InventorySlotIndex;
	class UComponent_Inventory* m_pInventory;
	FLinearColor m_DefaultTint = FColor(0xFF535555);
	FLinearColor m_DragOverTint = FColor(0xFFE99300);


public :
	void InitializeWidget(class UComponent_Inventory* _pInventory);
	void Update();

	/* Get */
	FORCEINLINE class UBorder*    const& GetSlotBG()     const { return m_pSlotBG; }
	FORCEINLINE class UImage*     const& GetIcon()       const { return m_pIcon; }
	FORCEINLINE class UTextBlock* const& GetAmountText() const { return m_pAmountText; }

	FORCEINLINE const bool& GetEmpty()              const { return m_bEmpty; }
	FORCEINLINE const int&  GetInventorySlotIndex() const { return m_InventorySlotIndex; }

	/* Set */
	FORCEINLINE void SetEmpty(bool _bEmpty) { m_bEmpty = _bEmpty; }
	FORCEINLINE void SetInventorySlotIndex(int _Index) { m_InventorySlotIndex = _Index; }

protected :
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	void _Empty();
		
};
