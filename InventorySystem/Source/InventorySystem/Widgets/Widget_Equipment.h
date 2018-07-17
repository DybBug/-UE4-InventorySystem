// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_Equipment.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Equipment : public UUserWidget
{
	GENERATED_BODY()

protected:
	//
	// #. À§Á¬ ÆÈ·¹Æ®
	//
	UPROPERTY()
	class UButton* m_pCloseButton;
	UPROPERTY()
	class UUniformGridPanel* m_pGridPanel;
	UPROPERTY()
	class UWidget_WeaponSlot* m_pWeaponSlotWidget;
	UPROPERTY()
	class UTextBlock* m_pNameText;
	UPROPERTY()
	class UTextBlock* m_pDescriptionText;

	//
	// #. º¯¼ö.
	//
	class UComponent_Inventory* m_pInventory;
	FItemInfo m_CurrentItemInfo;

public:
	void InitializeWidget(class UComponent_Inventory* _pInventory);
	void UpdateEquipment();

	/* Get */
	FORCEINLINE class UButton*           const& GetCloseButton()     const { return m_pCloseButton; }
	FORCEINLINE class UUniformGridPanel* const& GetGridPanel()       const { return m_pGridPanel; }
	FORCEINLINE class UTextBlock*        const& GetNameText()        const { return m_pNameText; }
	FORCEINLINE class UTextBlock*        const& GetDescriptionText() const { return m_pDescriptionText; }

protected:
	UFUNCTION()
	void _OnCloseButtonClicked();
	
	
};
