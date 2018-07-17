// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Shop.h"
#include "Widget_Sell.h"
#include "Widget_OfferedItem.h"
#include "Widget_InventorySlot.h"
#include "Widget_Inventory.h"
#include "NPCs/NPC_Merchant.h"
#include "InventorySystemCharacter.h"
#include "Components/Component_Inventory.h"
#include "DragDropOperations/DDOperation_Item.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/UniformGridPanel.h>
#include <Components/UniformGridSlot.h>

void UWidget_Shop::InitializeWidget(ANPC_Merchant* _pMerchant, AInventorySystemCharacter* _pPlayer)
{
	m_pMerchant = _pMerchant;
	m_pPlayer = _pPlayer;

	UpdateGold();
	_GenerateItemList();

	m_pSellWidget->InitializeWidget(this, m_pPlayer->GetInventory());

	m_pCloseButton->OnClicked.AddDynamic(this, &UWidget_Shop::_OnCloseButtonClicked);
}

void UWidget_Shop::UpdateGold()
{
	m_pGoldText->SetText(FText::AsNumber(m_pPlayer->GetCurrentGold()));
}

bool UWidget_Shop::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UDDOperation_Item* pOper = Cast<UDDOperation_Item>(InOperation);
	if (pOper)
	{
		FItemInfo ItemInfo = pOper->GetDraggedSlot()->GetItemInfo();
		if (ItemInfo.Category != EItemCategories::QuestItems)
		{
			if (ItemInfo.bCanBeStacked && (pOper->GetDraggedSlot()->GetAmount() > 1))
			{
				m_pSellWidget->Update(pOper->GetDraggedSlot()->GetSlotIndex());
				m_pSellWidget->SetVisibility(ESlateVisibility::Visible);

				m_pPlayer->GetInventory()->GetInventoryWidget()->GetSlotPanel()->SetIsEnabled(false);
				m_pGridPanel->SetIsEnabled(false);
			}
			else
			{
				bool bSuccess = m_pPlayer->GetInventory()->RemoveItemAtIndex(pOper->GetDraggedSlot()->GetSlotIndex(), 1);
				if (bSuccess)
				{
					m_pPlayer->IncreaseGold(ItemInfo.Value);
				}
			}
		}
	}
	return false;
}

void UWidget_Shop::_GenerateItemList()
{
	UClass* WB_OfferdItem = LoadClass<UWidget_OfferedItem>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_OfferedItem.WB_OfferedItem_C'"));

	int i = 0;
	for (auto& Iter : m_pMerchant->GetOfferItemClasses())
	{
		UWidget_OfferedItem* pOfferedItemWidget = CreateWidget<UWidget_OfferedItem>(GetWorld(), WB_OfferdItem);
		pOfferedItemWidget->InitializeWidget(Iter, m_pPlayer->GetInventory());

		m_OfferedItemWidgets.Add(pOfferedItemWidget);
		UUniformGridSlot* pSlot = m_pGridPanel->AddChildToUniformGrid(pOfferedItemWidget);
		pSlot->SetRow(i/2);
		pSlot->SetColumn(i%2);

		i++;
	}
}

void UWidget_Shop::_OnCloseButtonClicked()
{
	m_pMerchant->OnShopClose();
}
