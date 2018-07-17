// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_WeaponSlot.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_WeaponSlot : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UButton* m_pSlotButton;
	UPROPERTY()
	class UImage* m_pItemIcon;
	UPROPERTY()
	TSubclassOf<class UWidget_DraggedItem> m_DraggedItemClass;
	TSubclassOf<class UWidget_Detail> m_DetailWidgetClass;

	TSubclassOf<class  UAItem_Base> m_ItemClass;

	class UComponent_Inventory* m_pInventory;
	FItemInfo m_ItemInfo;
	int m_ClickCount = 0;

	bool m_bDraggedOver;

	FButtonStyle m_DefaultButtonStyle;
	FButtonStyle m_DraggedButtonOverStyle;

public:
	void InitializeWidget(class UComponent_Inventory* _pInvnetory);
	void UpdateWeapon();

	/* Get */
	FORCEINLINE class UButton*    const& GetSlotButton() const { return m_pSlotButton; }
	FORCEINLINE class UImage*     const& GetItemIcon()   const { return m_pItemIcon; }

	FORCEINLINE class UComponent_Inventory* const& GetInventory() const { return m_pInventory; }

	FORCEINLINE const FItemInfo& GetItemInfo()  const { return m_ItemInfo; }

protected:
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual void NativeOnDragDetected( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation ) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION()
	void _ResetStyle();
	
	
};
