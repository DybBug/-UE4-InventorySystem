// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Sell.h"
#include "Widget_Shop.h"
#include "Widget_Inventory.h"
#include "Components/Component_Inventory.h"
#include "InventorySystemCharacter.h"

#include <Components/TextBlock.h>
#include <Components/Image.h>
#include <Components/UniformGridPanel.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_Sell::InitializeWidget(UWidget_Shop* _pShopWidget, UComponent_Inventory* _pInventory)
{
	UWidget_Throw::InitializeWidget(_pInventory);

	m_pShopWidget = _pShopWidget;
}

void UWidget_Sell::Update(int _NewIndex)
{
	m_SlotIndex = _NewIndex;

	bool bEmpty;
	FItemInfo Info;
	int Amount;
	m_pInventory->GetItemInfoAtIndex(m_SlotIndex, &bEmpty, &Info, &Amount);

	// #. 빈 슬롯이 아니면...
	if (!bEmpty)
	{
		m_ItemInfo = Info;

		// #. 선택한 아이템의 이름과 현재 갯수 설정.
		FText Fromat = FText::Format(LOCTEXT("Format", "{0} x{1}"), FText::FromName(Info.Name), Amount);
		m_pNameText->SetText(Fromat);

		// #. 헌택한 아이템 최대 수량 설정.
		m_MaxAmount = Amount;

		// #. 선택한 아이템 아이콘 설정.
		m_pIcon->SetBrushFromTexture(Info.pIcon);

		// #. 버릴 아이템 갯수 설정(기본값 : 1개)
		m_ThrowAmount = 1;
		m_pAmountText->SetText(FText::AsNumber(m_ThrowAmount));

		UpdatePrice();
	}
}

void UWidget_Sell::UpdatePrice()
{
	m_pPriceText->SetText(FText::AsNumber(m_ThrowAmount * m_ItemInfo.Value));
}

void UWidget_Sell::_IncreaseAmount()
{
	UWidget_Throw::_IncreaseAmount();

	UpdatePrice();
}

void UWidget_Sell::_DecreaseAmount()
{
	UWidget_Throw::_DecreaseAmount();

	UpdatePrice();
}

void UWidget_Sell::_OnMinusButtonClicked()
{
	UWidget_Throw::_OnMinusButtonClicked();

	UpdatePrice();
}

void UWidget_Sell::_OnPlusButtonClicked()
{
	UWidget_Throw::_OnPlusButtonClicked();

	UpdatePrice();
}

void UWidget_Sell::_OnConfirmButtonClicked()
{
	bool bSuccess = m_pInventory->RemoveItemAtIndex(m_SlotIndex, m_ThrowAmount);

	if (bSuccess)
	{
		m_pInventory->GetPlayer()->IncreaseGold(m_ThrowAmount * m_ItemInfo.Value);
		m_pShopWidget->GetGridPanel()->SetIsEnabled(true);
		m_pInventory->GetInventoryWidget()->GetSlotPanel()->SetIsEnabled(true);
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWidget_Sell::_OnCancelButtonClicked()
{
	m_pShopWidget->GetGridPanel()->SetIsEnabled(true);
	UWidget_Throw::_OnCancelButtonClicked();
}


#undef LOCTEXT_NAMESPACE
