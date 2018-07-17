// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_RecipeEntry.h"
#include "Widget_CraftingMenu.h"
#include "Components/Component_Inventory.h"
#include "ItemClasses/Item_Base.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_RecipeEntry::InitializeWidget(UWidget_CraftingMenu * _pWidget, TSubclassOf<class AItem_Base> _Class, int _RequiredAmount)
{
	m_pCraftingMenu = _pWidget;
	m_ItemClass = _Class;
	m_RequiredAmount = _RequiredAmount;

}

void UWidget_RecipeEntry::Update()
{
	// #. 아이콘 설정.
	UTexture2D* pIcon = m_ItemClass.GetDefaultObject()->GetItemInfo().pIcon;
	m_pIcon->SetBrushFromTexture(pIcon);

	// #. 이름 x x수량 설정.
	TArray<int> SlotIndices;
	m_pCraftingMenu->GetInventory()->GetTotalAmountOfItem(m_ItemClass, m_CurrentAmount, SlotIndices);

	FName Name = m_ItemClass.GetDefaultObject()->GetItemInfo().Name;
	FText Format = FText::Format(LOCTEXT("Format", "{0} ({1}/{2})"), FText::FromName(Name), m_CurrentAmount, m_RequiredAmount);
	m_pNameText->SetText(Format);

	// #. 현재 갯수에 따라 이름 색상 설정.
	FLinearColor Color = (m_CurrentAmount < m_RequiredAmount) ? FColor(0xFFFF6200) : FColor(0xFFFFFFFF);
	m_pNameText->SetColorAndOpacity(Color);
}

#undef LOCTEXT_NAMESPACE

