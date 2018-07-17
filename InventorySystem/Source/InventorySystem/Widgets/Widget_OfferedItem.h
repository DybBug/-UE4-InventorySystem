// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_OfferedItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_OfferedItem : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UImage* m_pIcon;
	UPROPERTY()
	class UTextBlock* m_pNameText;
	UPROPERTY()
	class UTextBlock* m_pPriceText;
	UPROPERTY()
	class UTextBlock* m_pAmountText;
	UPROPERTY()
	class USlider* m_pAmountSlider;
	UPROPERTY()
	class UButton* m_pBuyButton;

	TSubclassOf<class AItem_Base> m_AssignedItemClass;
	class UComponent_Inventory* m_pInventory;
	FItemInfo m_ItemInfo;
	int m_CurrentAmount = 1;
	int m_MaxAmountToBuy = 99;

	
public :
	void InitializeWidget(TSubclassOf<class AItem_Base> _AssignedItemClass, class UComponent_Inventory* _pInventory);
	void UpdatePrice();

	/* Get */
	FORCEINLINE class UImage*     const& GetIcon()         const { return m_pIcon; }
	FORCEINLINE class UTextBlock* const& GetNameText()     const { return m_pNameText; }
	FORCEINLINE class UTextBlock* const& GetPriceText()    const { return m_pPriceText; }
	FORCEINLINE class UTextBlock* const& GetAmountText()   const { return m_pAmountText; }
	FORCEINLINE class USlider*    const& GetAmountSlider() const { return m_pAmountSlider; }
	FORCEINLINE class UButton*    const& GetBuyButton()    const { return m_pBuyButton; }

protected :
	UFUNCTION()
	void _OnValueChanged(float _Value);

	UFUNCTION()
	void _OnBuyButtonClicked();

	
};
