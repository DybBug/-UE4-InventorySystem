// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Main.h"
#include "Widget_Inventory.h"
#include "Widget_Throw.h"
#include "Widget_InventorySlot.h"
#include "Widget_ObtainedItem.h"
#include "Widget_CraftingMenu.h"
#include "Widget_Storage.h"
#include "Widget_Equipment.h"
#include "Widget_Hotkey.h"
#include "Components/Component_Inventory.h"
#include "ItemClasses/Item_Base.h"
#include "DragDropOperations/DDOperation_Widget.h"
#include "DragDropOperations/DDOperation_Item.h"

#include <Components/UniformGridPanel.h>
#include <Components/Border.h>
#include <Components/HorizontalBox.h>


void UWidget_Main::InitializeWidget(UComponent_Inventory * _pInventory, int _SlotsPerRow,  float _VisibleDuration)
{
	m_pInventory = _pInventory;
	m_SlotsPerRow = _SlotsPerRow;
	m_VisibleDuration = _VisibleDuration;

	m_ObtainedItemWidgetClass = LoadClass<UWidget_ObtainedItem>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_ObtainedItem.WB_ObtainedItem_C'"));

	m_pInventoryWidget->InitializeWidget(m_pInventory, m_SlotsPerRow);
	m_pThrowWidget->InitializeWidget(m_pInventory);
	m_pCraftingMenuWidget->InitializeWidget(m_pInventory);
	m_pStorageWidget->InitializeWidget(m_pInventory);
	m_pEquipmentWidget->InitializeWidget(m_pInventory);

	if (m_bShowHotkeys)
	{
		_GenerateHotkeys();
		m_pInventory->LoadHotkeys();
	}
	else
	{
		m_pHotkeyBar->RemoveFromParent();
	}
}

void UWidget_Main::AddItemToObtainedQueue(TSubclassOf<class AItem_Base> _ItemClass, int _Amount)
{
	if (m_ObtainedItemsQueue.Num() > 0)
	{
		FInventorySlot InventorySlot;
		InventorySlot.ItemClass = _ItemClass;
		InventorySlot.Amount = _Amount;
		m_ObtainedItemsQueue.Emplace(InventorySlot);
	}
	else
	{
		FInventorySlot InventorySlot;
		InventorySlot.ItemClass = _ItemClass;
		InventorySlot.Amount = _Amount;
		m_ObtainedItemsQueue.Emplace(InventorySlot);
		UWidget_ObtainedItem* pObtainedItemWidget = CreateWidget<UWidget_ObtainedItem>(GetWorld(), m_ObtainedItemWidgetClass);
		pObtainedItemWidget->InitializeWidget(_ItemClass, _Amount, m_VisibleDuration, this);

		m_pObtainedItemBorder->AddChild(pObtainedItemWidget);
	}
}

void UWidget_Main::OnObtainMessageEnd()
{
	m_ObtainedItemsQueue.RemoveAt(0);
	m_pObtainedItemBorder->ClearChildren();

	if (m_ObtainedItemsQueue.Num() > 0)
	{
		UWidget_ObtainedItem* pObtainedItemWidget = CreateWidget<UWidget_ObtainedItem>(GetWorld(), m_ObtainedItemWidgetClass);
		pObtainedItemWidget->InitializeWidget(m_ObtainedItemsQueue[0].ItemClass, m_ObtainedItemsQueue[0].Amount, m_VisibleDuration, this);

		m_pObtainedItemBorder->AddChild(pObtainedItemWidget);
	}
}

bool UWidget_Main::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UDDOperation_Widget* pDDOper = Cast<UDDOperation_Widget>(InOperation);
	if (pDDOper)
	{
		// #. 스크린 공간의 마우스 위치.
		FVector2D MousePos = InDragDropEvent.GetScreenSpacePosition();

		// #. 절대 좌표(마우스 위치)를 로컬 좌표로 변환.
		FVector2D LocalPos = InGeometry.AbsoluteToLocal(MousePos);

		FVector2D Pos = LocalPos - pDDOper->GetMouseOffset();	
		pDDOper->GetWidgetToDrag()->AddToViewport();
		pDDOper->GetWidgetToDrag()->SetPositionInViewport(Pos, false);

		return true;
	}
	else
	{
		if (UDDOperation_Item* pDDOper = Cast<UDDOperation_Item>(InOperation))
		{
			UWidget_InventorySlot* pInventorySlotWidget = pDDOper->GetDraggedSlot();
			if (pInventorySlotWidget->GetItemInfo().Category != EItemCategories::QuestItems)
			{
				if (pInventorySlotWidget->GetItemInfo().bCanBeStacked && (pInventorySlotWidget->GetAmount() > 1))
				{
					m_pThrowWidget->Update(pInventorySlotWidget->GetSlotIndex());
					m_pThrowWidget->SetVisibility(ESlateVisibility::Visible);

					m_pInventoryWidget->GetSlotPanel()->SetIsEnabled(false);
					return true;
				}
				else
				{
					m_pInventory->RemoveItemAtIndex(pInventorySlotWidget->GetSlotIndex(), 1);
					return true;
				}
			}		
		}	
	}
	return false;
}

void UWidget_Main::_GenerateHotkeys()
{
	m_HotkeyWidgets.Empty();
	m_pHotkeyBar->ClearChildren();

	UClass* pWB_Hotkey = LoadClass<UWidget_Hotkey>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_Hotkey.WB_Hotkey_C'"));
	for (FKey& Iter : m_Hotkeys)
	{
		UWidget_Hotkey* pHotkeyWidget = CreateWidget<UWidget_Hotkey>(GetWorld(), pWB_Hotkey);
		pHotkeyWidget->InitializeWidget(Iter, m_pInventory);
		m_HotkeyWidgets.Add(pHotkeyWidget);

		m_pHotkeyBar->AddChildToHorizontalBox(pHotkeyWidget);
	}
}

