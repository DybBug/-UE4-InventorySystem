// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Detail.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Kismet/KismetTextLibrary.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_Detail::InitializeWidget(const FItemInfo & _ItemInfo, const int & _Amount)
{
	m_ItemInfo = _ItemInfo;
	m_Amount = _Amount;
}

void UWidget_Detail::Update()
{
	// #. ������.
	m_pIcon->SetBrushFromTexture(m_ItemInfo.pIcon);

	// #. �׸�
	FString CategoryStr =  CONVERT_DISPLAYNAME_TO_STRING(L"EItemCategories", m_ItemInfo.Category);
	m_pCategory->SetText(FText::FromString(CategoryStr));

	// #. ����.
	m_pDescription->SetText(m_ItemInfo.Description);

	// #. �̸� �� ����.
	if (m_ItemInfo.bCanBeStacked)
	{
		FText Format = FText::Format(LOCTEXT("Format", "{0} x{1}"), FText::FromName(m_ItemInfo.Name), m_Amount);
		m_pName->SetText(Format);
	}
	else
	{
		m_pName->SetText(FText::FromName(m_ItemInfo.Name));
	}

	// #. ����.
	FNumberFormattingOptions Options;
	Options.RoundingMode = ERoundingMode::HalfToZero;
	Options.MinimumIntegralDigits = 1;   // #. ���� �κ� �ּ� �ڸ���.
	Options.MaximumIntegralDigits = 324; // #. ���� �κ� �ִ� �ڸ���.
	Options.MinimumFractionalDigits = 1; // #. �Ҽ��� �κ� �ּ� �ڸ���.
	Options.MaximumFractionalDigits = 1; // #. �Ҽ��� �κ� �ִ� �ڸ���.

	if (m_ItemInfo.bCanBeStacked && (m_Amount > 1))
	{
		FText SingleWeightText = FText::AsNumber(m_ItemInfo.Weight, &Options);
		FText TotalWeightText = FText::AsNumber(m_ItemInfo.Weight * m_Amount, &Options);
		
		FText Format = FText::Format(LOCTEXT("Format", "{0}({1})"), TotalWeightText, SingleWeightText);
		m_pWeight->SetText(Format);
	}
	else
	{	
		m_pWeight->SetText(FText::AsNumber(m_ItemInfo.Weight, &Options));
	}
}

#undef LOCTEXT_NAMESPACE
