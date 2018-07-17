// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_CraftingMenu.h"
#include "Widget_CraftableItem.h"
#include "Widget_RecipeEntry.h"
#include "Components/Component_Inventory.h"
#include "ItemClasses/Item_Base.h"

#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/ScrollBox.h>
#include <Components/VerticalBox.h>

void UWidget_CraftingMenu::InitializeWidget(UComponent_Inventory* _pInventory)
{
	m_pInventory = _pInventory;

	m_pCloseButton->OnClicked.AddDynamic(this, &UWidget_CraftingMenu::_OnCloseButtonClicked);
	m_pCraftButton->OnClicked.AddDynamic(this, &UWidget_CraftingMenu::_OnCraftButtonClicked);

	_GennerateCraftableList();
	UpdateDetailWindow(nullptr);
}

void UWidget_CraftingMenu::UpdateDetailWindow(TSubclassOf<AItem_Base> _NewItemClass)
{
	if (_NewItemClass)
	{
		if (m_CurrentItemClass == _NewItemClass)
		{
			for (auto& Iter : m_RecipeEntries)
			{
				Iter->Update();
			}
		}
		else
		{
			m_CurrentItemClass = _NewItemClass;
			m_CurrentItemInfo = m_CurrentItemClass.GetDefaultObject()->GetItemInfo();

			m_pItemIcon->SetBrushFromTexture(m_CurrentItemInfo.pIcon);
			m_pNameText->SetText(FText::FromName(m_CurrentItemInfo.Name));
			m_pDescription->SetText(m_CurrentItemInfo.Description);

			_GenerateRecipeEntries();			
		}

		bool bCaanbeCrafted = _CanBeCrafted();
		m_pCraftButton->SetIsEnabled(bCaanbeCrafted);

		m_pItemIcon->SetVisibility(ESlateVisibility::Visible);
		m_pDivision->SetVisibility(ESlateVisibility::Visible);
		m_pRecipeText->SetVisibility(ESlateVisibility::Visible);
		m_pCraftButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		m_CurrentItemClass = nullptr;
		m_pNameText->SetText(FText::FromString(TEXT("")));
		m_pDescription->SetText(FText::FromString(TEXT("")));

		m_pItemIcon->SetVisibility(ESlateVisibility::Hidden);
		m_pDivision->SetVisibility(ESlateVisibility::Hidden);
		m_pRecipeText->SetVisibility(ESlateVisibility::Hidden);
		m_pCraftButton->SetVisibility(ESlateVisibility::Hidden);

		m_pRecipeEntriesList->ClearChildren();
	}
}

void UWidget_CraftingMenu::OnCraftableClicked(UWidget_CraftableItem* _pClicked)
{
	if (m_CurrentCraftable)
	{
		m_CurrentCraftable->GetSelectButton()->SetIsEnabled(true);	
	}

	m_CurrentCraftable = _pClicked;

	m_CurrentCraftable->GetSelectButton()->SetIsEnabled(false);
	UpdateDetailWindow(m_CurrentCraftable->GetItemClass());
}

void UWidget_CraftingMenu::_GennerateCraftableList()
{
	m_pCraftableItemScrollBox->ClearChildren();

	UClass* WB_CraftableItem = LoadClass<UWidget_CraftableItem>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_CraftableItem.WB_CraftableItem_C'"));

	TArray<TSubclassOf<AItem_Base>> UnlockedCraftableItems = m_pInventory->GetUnlockedCraftableItems();
	for (int i = 0; i < UnlockedCraftableItems.Num(); ++i)
	{
		UWidget_CraftableItem* pCraftableItemWidget = CreateWidget<UWidget_CraftableItem>(GetWorld(), WB_CraftableItem);
		pCraftableItemWidget->InitializeWidget(UnlockedCraftableItems[i], this);

		m_pCraftableItemScrollBox->AddChild(pCraftableItemWidget);
	}
}

void UWidget_CraftingMenu::_GenerateRecipeEntries()
{
	m_RecipeEntries.Empty();
	m_pRecipeEntriesList->ClearChildren();

	UClass* WB_RecipeEntry = LoadClass<UWidget_RecipeEntry>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_RecipeEntry.WB_RecipeEntry_C'"));


	for (int i = 0; i < m_CurrentItemInfo.Recipe.Num(); ++i)
	{
		UWidget_RecipeEntry* pRecipeEntryWidget = CreateWidget<UWidget_RecipeEntry>(GetWorld(), WB_RecipeEntry);
		pRecipeEntryWidget->InitializeWidget(this, m_CurrentItemInfo.Recipe[i].ItemClass,  m_CurrentItemInfo.Recipe[i].Amount);
		pRecipeEntryWidget->Update();

		m_RecipeEntries.Add(pRecipeEntryWidget);

		m_pRecipeEntriesList->AddChildToVerticalBox(pRecipeEntryWidget);
	}
}

bool UWidget_CraftingMenu::_CanBeCrafted()
{
	for (int i = 0; i < m_RecipeEntries.Num(); ++i)
	{
		int RequiredAmount = m_RecipeEntries[i]->GetRequiredAmount();
		int CurrentAmount = m_RecipeEntries[i]->GetCurrentAmount();

		if (CurrentAmount < RequiredAmount)
		{
			return false;
		}
	}

	return true;
}

void UWidget_CraftingMenu::_OnCloseButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_CraftingMenu::_OnCraftButtonClicked()
{
	//m_pCraftButton->SetIsEnabled(false);
	int Reset;
	bool bSuccess = m_pInventory->AddItem(m_CurrentItemClass, 1, false, &Reset);

	if (bSuccess)
	{
		for(FInventorySlot& Iter : m_CurrentItemInfo.Recipe)
		{
			m_pInventory->RemoveItem(Iter.ItemClass, Iter.Amount);
		}
	}

	bool bCanBeCrafted = _CanBeCrafted();
	m_pCraftButton->SetIsEnabled(bCanBeCrafted);	
}
