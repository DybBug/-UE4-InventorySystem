// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_CraftableItem.h"
#include "Widget_CraftingMenu.h"
#include "ItemClasses/Item_Base.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>

void UWidget_CraftableItem::InitializeWidget(TSubclassOf<class AItem_Base> _ItemClass, UWidget_CraftingMenu * _pCraftingMenu)
{
	m_ItemClass     = _ItemClass;
	m_pCraftingMenu = _pCraftingMenu;

	FName ItemName = m_ItemClass.GetDefaultObject()->GetItemInfo().Name;
	m_pNameText->SetText(FText::FromName(ItemName));

	m_pSelectButton->OnClicked.AddDynamic(this, &UWidget_CraftableItem::_OnSelectButtonClicked);
}

void UWidget_CraftableItem::_OnSelectButtonClicked()
{
	m_pCraftingMenu->OnCraftableClicked(this);
}
