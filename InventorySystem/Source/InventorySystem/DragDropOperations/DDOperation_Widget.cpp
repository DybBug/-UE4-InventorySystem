// Fill out your copyright notice in the Description page of Project Settings.

#include "DDOperation_Widget.h"
#include "Widgets/Widget_Main.h"

void UDDOperation_Widget::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	m_pMainWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDDOperation_Widget::DragCancelled_Implementation(const FPointerEvent & PointerEvent)
{
	m_pMainWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDDOperation_Widget::Dragged_Implementation(const FPointerEvent & PointerEvent)
{
	if (m_pMainWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		m_pMainWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
