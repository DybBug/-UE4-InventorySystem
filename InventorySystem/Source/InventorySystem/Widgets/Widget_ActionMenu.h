// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_ActionMenu.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_ActionMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UButton* m_pUseButton;
	UPROPERTY()
	class UButton* m_pThrowSellButton;
	UPROPERTY()
	class UButton* m_pSplitButton;
	UPROPERTY()
	class UButton* m_pCancelButton;
	UPROPERTY()
	class UTextBlock* m_pUseText;
	UPROPERTY()
	class UTextBlock* m_pThrowSellText;

	class UComponent_Inventory* m_pInventory;
	int m_CurrentIndex;
	int m_Amount;
	FItemInfo m_ItemInfo;


public :
	void InitializeWidget(class UComponent_Inventory* _pInventory);
	void Update(int _NewIndex);

	/* Get */
	FORCEINLINE class UButton* const& GetUseButton()        const { return m_pUseButton; }
	FORCEINLINE class UButton* const& GetThrowSellButton()  const { return m_pThrowSellButton; }
	FORCEINLINE class UButton* const& GetSplitButton()      const { return m_pSplitButton; }
	FORCEINLINE class UButton* const& GetCancelButton()     const { return m_pCancelButton; }
	
protected :
	UFUNCTION()
	void _OnUseButtonClicked();
	UFUNCTION()
	void _OnThrowSellButtonClicked();
	UFUNCTION()
	void _OnSplitButtonClicked();
	UFUNCTION()
	void _OnCancelButtonClicked();
};
