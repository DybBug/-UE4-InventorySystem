// Fill out your copyright notice in the Description page of Project Settings.

#include "DDOperation_Item.h"
#include "Components/Component_Inventory.h"
#include "Widgets/Widget_InventorySlot.h"
#include "Widgets/Widget_Main.h"


void UDDOperation_Item::Drop_Implementation(const FPointerEvent& PointerEvent)
{	
	m_pDraggedSlot->GetInventory()->GetMainWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDDOperation_Item::DragCancelled_Implementation(const FPointerEvent & PointerEvent)
{
	m_pDraggedSlot->GetInventory()->GetMainWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDDOperation_Item::Dragged_Implementation(const FPointerEvent & PointerEvent)
{
	if (m_pDraggedSlot->GetInventory()->GetMainWidget()->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		m_pDraggedSlot->GetInventory()->GetMainWidget()->SetVisibility(ESlateVisibility::Visible);
	}
}

