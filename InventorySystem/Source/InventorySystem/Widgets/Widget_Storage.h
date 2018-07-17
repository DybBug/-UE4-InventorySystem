// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Storage.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Storage : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class UButton* m_pCloseButton;
	UPROPERTY()
	class UUniformGridPanel* m_pSlotPanel;
	UPROPERTY()
	class UScrollBox* m_pStorageScrollBox;

	class UComponent_Storage* m_pStorage;
	class UComponent_Inventory* m_pInventory;

	bool m_bReversedSort;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Widgets")
	TArray<class UWidget_StorageSlot*> m_SlotWidgets;

	int m_AmountToRemove;

public:
	void InitializeWidget(class UComponent_Inventory* m_pInventory);
	void GenerateSlotWidgets(class UComponent_Storage* _pStorage);
	void OnSlotClicked(class UWidget_StorageSlot* _pSlotWidget);

	/* Get */
	FORCEINLINE class UButton*            const& GetCloseButton() const { return m_pCloseButton; }
	FORCEINLINE class UUniformGridPanel*  const& GetSlotPanel()   const { return m_pSlotPanel; }


	FORCEINLINE const TArray<class UWidget_StorageSlot*>& GetSlotWidgets() const { return m_SlotWidgets; }

	/* Set */
	FORCEINLINE void SetInventory(class UComponent_Storage* _pInventory) { m_pStorage = m_pStorage; }

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


	UFUNCTION()
		void _OnCloseButtonClicked();
	
	
};
