// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_RecipeEntry.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_RecipeEntry : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UImage* m_pIcon;
	UPROPERTY()
	class UTextBlock* m_pNameText;
	
	class UWidget_CraftingMenu* m_pCraftingMenu;
	TSubclassOf<class AItem_Base> m_ItemClass;
	int m_RequiredAmount;
	int m_CurrentAmount  = 0;

public :
	void InitializeWidget(class UWidget_CraftingMenu* _pWidget, TSubclassOf<class AItem_Base> _Class, int _RequiredAmount);
	void Update();

	/* Get */
	FORCEINLINE class UImage*     const& GetIcon()     const { return m_pIcon; }
	FORCEINLINE class UTextBlock* const& GetNameText() const { return m_pNameText; }

	FORCEINLINE const int& GetRequiredAmount() const { return m_RequiredAmount; }
	FORCEINLINE const int& GetCurrentAmount()  const { return m_CurrentAmount; }
	
};
