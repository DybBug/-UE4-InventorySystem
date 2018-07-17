// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CraftableItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_CraftableItem : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UButton* m_pSelectButton;
	UPROPERTY()
	class UTextBlock* m_pNameText;

	TSubclassOf<class AItem_Base> m_ItemClass;

	class UWidget_CraftingMenu* m_pCraftingMenu;
	
public :
	void InitializeWidget(TSubclassOf<class AItem_Base> _ItemClass, class UWidget_CraftingMenu* _pCraftingMenu);

	/* Get */
	FORCEINLINE class UButton*    const& GetSelectButton() const { return m_pSelectButton; }
	FORCEINLINE class UTextBlock* const& GetNameText()     const { return m_pNameText; }

	FORCEINLINE  const TSubclassOf<class AItem_Base>& GetItemClass() const { return m_ItemClass; }


protected :
	UFUNCTION()
	void _OnSelectButtonClicked();
};
