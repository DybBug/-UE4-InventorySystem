// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_OfferedItem.h"
#include "InventorySystemCharacter.h"
#include "Components/Component_Inventory.h"
#include "ItemClasses/Item_Base.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/Slider.h>
#include <Components/Button.h>

void UWidget_OfferedItem::InitializeWidget(TSubclassOf<class AItem_Base> _AssignedItemClass, class UComponent_Inventory* _pInventory)
{
	m_AssignedItemClass = _AssignedItemClass;
	m_pInventory = _pInventory;

	m_ItemInfo = m_AssignedItemClass.GetDefaultObject()->GetItemInfo();

	m_pNameText->SetText(FText::FromName(m_ItemInfo.Name));
	m_pIcon->SetBrushFromTexture(m_ItemInfo.pIcon);

	UpdatePrice();

	if (m_ItemInfo.bCanBeStacked)
	{
		m_pAmountText->SetText(FText::AsNumber(m_CurrentAmount));
		m_pAmountSlider->SetVisibility(ESlateVisibility::Visible);
		m_pAmountText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		m_CurrentAmount = 1;
		m_pAmountSlider->SetVisibility(ESlateVisibility::Hidden);
		m_pAmountText->SetVisibility(ESlateVisibility::Hidden);
	}

	m_pAmountSlider->OnValueChanged.AddDynamic(this, &UWidget_OfferedItem::_OnValueChanged);
	m_pBuyButton->OnClicked.AddDynamic(this, &UWidget_OfferedItem::_OnBuyButtonClicked);
}

void UWidget_OfferedItem::UpdatePrice()
{
	int PlayerCurrentGold = m_pInventory->GetPlayer()->GetCurrentGold();
	int TotalPrice = m_CurrentAmount * m_ItemInfo.Value;

	// #. 플레이어의 돈이 충분함.
	if (PlayerCurrentGold >= TotalPrice)
	{
		m_pPriceText->SetText(FText::AsNumber(TotalPrice));
		m_pPriceText->SetColorAndOpacity(FLinearColor(FColor(0xFFFFB535)));
		m_pBuyButton->SetIsEnabled(true);
	}
	// #. 플레이어의 돈이 부족함.
	else
	{
		m_pPriceText->SetText(FText::AsNumber(TotalPrice));
		m_pPriceText->SetColorAndOpacity(FLinearColor(FColor(0xFFFF5B35)));
		m_pBuyButton->SetIsEnabled(false);
	}
}

void UWidget_OfferedItem::_OnValueChanged(float _Value)
{
	m_CurrentAmount = FMath::RoundToInt(FMath::Lerp<float, float>(1, m_MaxAmountToBuy, _Value));
	m_pAmountText->SetText(FText::AsNumber(m_CurrentAmount));

	UpdatePrice();
}

void UWidget_OfferedItem::_OnBuyButtonClicked()
{
	int Rest;
	bool bSuccess = m_pInventory->AddItem(m_AssignedItemClass, m_CurrentAmount, true, &Rest);

	if (bSuccess)
	{		
		m_pInventory->GetPlayer()->DecreaseGold((m_CurrentAmount - Rest) * m_ItemInfo.Value);
		UpdatePrice();
	}
}
