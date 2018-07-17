// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Inventory : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UButton* m_pCloseButton;
	UPROPERTY()
	class UUniformGridPanel* m_pSlotPanel;
	UPROPERTY()
	class UWidget_ActionMenu* m_pActionMenu;
	UPROPERTY()
	class UScrollBox* m_pInventoryScrollBox;
	UPROPERTY()
	class UComboBoxString* m_pSortTypeBox;
	UPROPERTY()
	class UButton* m_pSortButton;
	UPROPERTY()
	class UTextBlock* m_pWeightText;
	UPROPERTY()
	class UTextBlock* m_pGoldText;
	
	class UComponent_Inventory* m_pInventory;
	int m_SlotsPerRow;

	bool m_bReversedSort;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Widgets")
	TArray<class UWidget_InventorySlot*> m_SlotWidgets;

public :
	void InitializeWidget(UComponent_Inventory* _pInventory, int _SlotsPerRow);
	void GenerateSlotWidgets();
	void OnSlotClicked(class UWidget_InventorySlot* _pSlotWidget);

	/* Get */
	FORCEINLINE class UButton*              const& GetCloseButton() const { return m_pCloseButton; }
	FORCEINLINE class UUniformGridPanel*    const& GetSlotPanel()   const { return m_pSlotPanel; }
	FORCEINLINE class UWidget_ActionMenu*   const& GetActionMenu()  const { return m_pActionMenu; }
	FORCEINLINE class UComboBoxString*      const& GetSortTypeBox() const { return m_pSortTypeBox; }
	FORCEINLINE class UButton*              const& GetSortButton()  const { return m_pSortButton; }
	FORCEINLINE class UTextBlock*           const& GetWeightText()  const { return m_pWeightText; }
	FORCEINLINE class UTextBlock*           const& GetGoldText()    const { return m_pGoldText; }
	FORCEINLINE class UComponent_Inventory* const& GetInventory()   const { return m_pInventory; }


	FORCEINLINE const TArray<class UWidget_InventorySlot*>& GetSlotWidgets() const { return m_SlotWidgets; }

	/* Set */
	FORCEINLINE void SetInventory(class UComponent_Inventory* _pInventory) { m_pInventory = _pInventory; }
	FORCEINLINE void SetSlotsPerRow(int _Num)                              { m_SlotsPerRow = _Num;}

protected :
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual void NativeOnDragDetected( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation ) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override { return true; }


	UFUNCTION()
	void _OnCloseButtonClicked();
	UFUNCTION()
	void _OnSortButtonClicked();
};

