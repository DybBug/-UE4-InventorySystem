// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include <SlateTypes.h>
#include "Widget_StorageSlot.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_StorageSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	class UButton* m_pSlotButton;
	UPROPERTY()
	class UImage* m_pItemIcon;
	UPROPERTY()
	class UTextBlock* m_pAmountText;

	TSubclassOf<class UWidget_DraggedItem> m_DraggedItemClass;
	TSubclassOf<class UWidget_Detail> m_DetailWidgetClass;

	int m_SlotIndex = 0;
	class UComponent_Storage* m_pStorage;
	FItemInfo m_ItemInfo;
	int m_Amount = 0;


	bool m_bDraggedOver;

	FButtonStyle m_DefaultButtonStyle;
	FButtonStyle m_DraggedButtonOverStyle;

	class UWidget_Detail* m_pDetailWidget;

public:
	void InitializeWidget(int _Index, class UComponent_Storage* _pStorage);
	void UpdateSlot();
	void ResetStyle();

	/* Get */
	FORCEINLINE class UButton*    const& GetSlotButton() const { return m_pSlotButton; }
	FORCEINLINE class UImage*     const& GetItemIcon()   const { return m_pItemIcon; }
	FORCEINLINE class UTextBlock* const& GetAmountText() const { return m_pAmountText; }

	FORCEINLINE class UComponent_Storage* const& GetStorage() const { return m_pStorage; }

	FORCEINLINE const int&       GetSlotIndex() const { return m_SlotIndex; }
	FORCEINLINE const int&       GetAmount()    const { return m_Amount; }
	FORCEINLINE const FItemInfo& GetItemInfo()  const { return m_ItemInfo; }

protected:
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;


	UFUNCTION()
	void _OnClicked();


	
	
};
