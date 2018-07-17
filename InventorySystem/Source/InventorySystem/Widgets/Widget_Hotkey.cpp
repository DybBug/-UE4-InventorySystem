// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Hotkey.h"
#include "Widget_HotkeySlot.h"
#include "DragDropOperations/DDOperation_Item.h"

#include <Components/TextBlock.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_Hotkey::InitializeWidget(const FKey& _Key, class UComponent_Inventory* _pInventory)
{
	m_Key = _Key;
	m_pInventory = _pInventory;

	m_pHotkeySlotWidget->InitializeWidget(_pInventory);

	FText Format = FText::Format(LOCTEXT("Format", "[{0}]"), m_Key.GetDisplayName());
	m_pKeyText->SetText(Format);
}



bool UWidget_Hotkey::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{	
	// #. 드랍 효과 없음...
	return false;
}

#undef LOCTEXT_NAMESPACE
