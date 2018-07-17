// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_HotkeySlot.h"
#include "Widget_InventorySlot.h"
#include "Components/Component_Inventory.h"
#include "DragDropOperations/DDOperation_Item.h"   

#include <Components/Border.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_HotkeySlot::InitializeWidget(class UComponent_Inventory* _pInventory)
{
	m_pInventory = _pInventory;
}

void UWidget_HotkeySlot::Update()
{
	if (m_bEmpty)
	{
		_Empty();
	}
	else
	{
		bool bEmpty;
		FItemInfo ItemInfo;
		int Amount;
		m_pInventory->GetItemInfoAtIndex(m_InventorySlotIndex, &bEmpty, &ItemInfo, &Amount);
		if (bEmpty)
		{
			_Empty();
		}
		else
		{
			FText Format = FText::Format(LOCTEXT("Format", "x{0}"), Amount);
			m_pAmountText->SetText(Format);
			m_pAmountText->SetVisibility(ESlateVisibility::Visible);

			m_pIcon->SetBrushFromTexture(ItemInfo.pIcon);
			m_pIcon->SetVisibility(ESlateVisibility::Visible);
		}

	}
}

bool UWidget_HotkeySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
	{
		m_bDraggedOver = true;
		m_pSlotBG->SetBrushColor(m_DragOverTint);
		return true;
	}

	return true;
}

bool UWidget_HotkeySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
	{
		UWidget_InventorySlot* pDraggedInventorySlot = pItemOper->GetDraggedSlot();
		int DraggedInventorySlotIndex = pDraggedInventorySlot->GetSlotIndex();
		FItemInfo DraggedItemInfo = pDraggedInventorySlot->GetItemInfo();

		if (DraggedItemInfo.bCanBeUsed)
		{
			m_InventorySlotIndex = DraggedInventorySlotIndex;
			m_bEmpty = false;
			Update();
			m_bDraggedOver = false;
			m_pSlotBG->SetBrushColor(m_DefaultTint);			
		}		
		else
		{
			m_bDraggedOver = false;
			m_pSlotBG->SetBrushColor(m_DefaultTint);
		}
		return true;
	}	
	return false;
}

void UWidget_HotkeySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
	{
		m_bDraggedOver = false;
		m_pSlotBG->SetBrushColor(m_DefaultTint);
	}
}

FReply UWidget_HotkeySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && !m_bEmpty)
	{
		m_bEmpty = true;
		Update();
	}
	return FEventReply().NativeReply;
}

void UWidget_HotkeySlot::_Empty()
{
	m_pIcon->SetVisibility(ESlateVisibility::Hidden);
	m_pAmountText->SetVisibility(ESlateVisibility::Hidden);
	m_bDraggedOver = false;

	m_pSlotBG->SetBrushColor(m_DefaultTint);
}

#undef LOCTEXT_NAMESPACE
