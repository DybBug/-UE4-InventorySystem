// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_CraftingMenu.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_CraftingMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UButton* m_pCloseButton;
	UPROPERTY()
	class UButton* m_pCraftButton;
	UPROPERTY()
	class UImage* m_pItemIcon;
	UPROPERTY()
	class UImage* m_pDivision;
	UPROPERTY()
	class UTextBlock* m_pNameText;
	UPROPERTY()
	class UTextBlock* m_pDescription;
	UPROPERTY()
	class UTextBlock* m_pRecipeText;
	UPROPERTY()
	class UScrollBox* m_pCraftableItemScrollBox;
	UPROPERTY()
	class UVerticalBox* m_pRecipeEntriesList;

	class UComponent_Inventory* m_pInventory;
	TSubclassOf<class AItem_Base> m_CurrentItemClass;
	FItemInfo m_CurrentItemInfo;
	TArray<class UWidget_RecipeEntry*> m_RecipeEntries;
	class UWidget_CraftableItem* m_CurrentCraftable;

public :
	void InitializeWidget(class UComponent_Inventory* _pInventory);
	void UpdateDetailWindow(TSubclassOf<AItem_Base> _NewItemClass);
	void OnCraftableClicked(class UWidget_CraftableItem* _pClicked);

	/* Get */
	FORCEINLINE class UButton*      const& GetCloseButton()            const { return m_pCloseButton; }
	FORCEINLINE class UButton*      const& GetCraftButton()            const { return m_pCraftButton; }
	FORCEINLINE class UImage*       const& GetItemIcon()               const { return m_pItemIcon; }
	FORCEINLINE class UImage*       const& GetDivision()               const { return m_pDivision; }
	FORCEINLINE class UTextBlock*   const& GetNameText()               const { return m_pNameText; }
	FORCEINLINE class UTextBlock*   const& GetDescription()            const { return m_pDescription; }
	FORCEINLINE class UTextBlock*   const& GetRecipeText()             const { return m_pRecipeText; }
	FORCEINLINE class UScrollBox*   const& GetCraftableItemScrollBox() const { return m_pCraftableItemScrollBox; }
	FORCEINLINE class UVerticalBox* const& GetRecipeEntriesList()      const { return m_pRecipeEntriesList; }

	FORCEINLINE class UComponent_Inventory* const& GetInventory() const { return m_pInventory; }

	FORCEINLINE const TSubclassOf<class AItem_Base>& GetCurrentItemClass() const { return m_CurrentItemClass; }


protected :
	void _GennerateCraftableList();
	void _GenerateRecipeEntries();
	bool _CanBeCrafted();	

	UFUNCTION()
	void _OnCloseButtonClicked();

	UFUNCTION()
	void _OnCraftButtonClicked();
};
