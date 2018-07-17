// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_DraggedItem.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_DraggedItem::InitializeWidget(FItemInfo _ItemInfo, int _Amount)
{
	m_ItemInfo = _ItemInfo;
	m_Amount = _Amount;

	m_pIcon->SetBrushFromTexture(m_ItemInfo.pIcon);
	if (m_ItemInfo.bCanBeStacked)
	{
		FText Format = FText::Format(LOCTEXT("Format", "x{0}"), m_Amount);
		m_pAmountText->SetText(Format);
	}
	else
	{
		m_pAmountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

#undef LOCTEXT_NAMESPACE

