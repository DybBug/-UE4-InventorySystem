// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_ActionMenu.h"
#include "Widget_Main.h"
#include "Widget_Throw.h"
#include "Widget_Inventory.h"
#include "Widget_Shop.h"
#include "Widget_Sell.h"
#include "InventorySystemCharacter.h"
#include "NPCs/NPC_Merchant.h"
#include "Components/Component_Inventory.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/UniformGridPanel.h>

void UWidget_ActionMenu::InitializeWidget(UComponent_Inventory* _pInventory)
{
	m_pInventory = _pInventory;

	m_pUseButton->OnClicked.AddDynamic(this, &UWidget_ActionMenu::_OnUseButtonClicked);
	m_pThrowSellButton->OnClicked.AddDynamic(this, &UWidget_ActionMenu::_OnThrowSellButtonClicked);
	m_pSplitButton->OnClicked.AddDynamic(this, &UWidget_ActionMenu::_OnSplitButtonClicked);
	m_pCancelButton->OnClicked.AddDynamic(this, &UWidget_ActionMenu::_OnCancelButtonClicked);
}

void UWidget_ActionMenu::Update(int _NewIndex)
{
	if (m_pInventory->GetPlayer()->GetIsShopOpen())
	{
		m_pThrowSellText->SetText(FText::FromString(TEXT("팔기")));
	}
	else
	{
		m_pThrowSellText->SetText(FText::FromString(TEXT("버리기")));
	}

	m_CurrentIndex = _NewIndex;

	m_pInventory->GetItemInfoAtIndex(m_CurrentIndex, nullptr, &m_ItemInfo, &m_Amount);

	// #. 사용 가능한가? ('사용 버튼' 업데이트)
	if (m_ItemInfo.bCanBeUsed)
	{
		m_pUseText->SetText(m_ItemInfo.UseText);
		m_pUseButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		m_pUseButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	// #. 퀘스트 아이템인가? ('버리기 버튼' 업테이트)
	if (m_ItemInfo.Category == EItemCategories::QuestItems)
	{
		m_pThrowSellButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		m_pThrowSellButton->SetVisibility(ESlateVisibility::Visible);
	}

	// #. 나눌수 있는가? ('나누기 버튼' 업데이트)
	if (m_ItemInfo.bCanBeStacked && m_Amount > 1)
	{
		m_pSplitButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		m_pSplitButton->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

void UWidget_ActionMenu::_OnUseButtonClicked()
{
	m_pInventory->UseItemAtIndex(m_CurrentIndex);

	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_ActionMenu::_OnThrowSellButtonClicked()
{
	if (m_pInventory->GetPlayer()->GetIsShopOpen())
	{
		if (m_ItemInfo.bCanBeStacked &&  m_Amount > 1)
		{
			m_pInventory->GetPlayer()->GetNearbyMerchantActors()[0]->GetShopWidget()->GetSellWidget()->Update(m_CurrentIndex);
			m_pInventory->GetPlayer()->GetNearbyMerchantActors()[0]->GetShopWidget()->GetSellWidget()->SetVisibility(ESlateVisibility::Visible);
			m_pInventory->GetPlayer()->GetNearbyMerchantActors()[0]->GetShopWidget()->GetGridPanel()->SetIsEnabled(false);
			m_pInventory->GetInventoryWidget()->GetSlotPanel()->SetIsEnabled(false);
			SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			bool bSuccess = m_pInventory->RemoveItemAtIndex(m_CurrentIndex, 1);
			if (bSuccess)
			{
				m_pInventory->GetPlayer()->IncreaseGold(m_ItemInfo.Value);
				SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else
	{
		if (m_ItemInfo.bCanBeStacked && (m_Amount > 1))
		{
			m_pInventory->GetMainWidget()->GetThrowWidget()->Update(m_CurrentIndex);
			m_pInventory->GetMainWidget()->GetThrowWidget()->SetVisibility(ESlateVisibility::Visible);

			m_pInventory->GetInventoryWidget()->GetSlotPanel()->SetIsEnabled(false);
			SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			m_pInventory->RemoveItemAtIndex(m_CurrentIndex, 1);

			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UWidget_ActionMenu::_OnSplitButtonClicked()
{
	m_pInventory->SplitStack(m_CurrentIndex, m_Amount / 2);

	SetVisibility(ESlateVisibility::Hidden);
}

void UWidget_ActionMenu::_OnCancelButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
}
