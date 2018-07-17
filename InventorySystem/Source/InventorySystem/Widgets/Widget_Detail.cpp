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
	// #. 아이콘.
	m_pIcon->SetBrushFromTexture(m_ItemInfo.pIcon);

	// #. 항목
	FString CategoryStr =  CONVERT_DISPLAYNAME_TO_STRING(L"EItemCategories", m_ItemInfo.Category);
	m_pCategory->SetText(FText::FromString(CategoryStr));

	// #. 설명.
	m_pDescription->SetText(m_ItemInfo.Description);

	// #. 이름 및 수량.
	if (m_ItemInfo.bCanBeStacked)
	{
		FText Format = FText::Format(LOCTEXT("Format", "{0} x{1}"), FText::FromName(m_ItemInfo.Name), m_Amount);
		m_pName->SetText(Format);
	}
	else
	{
		m_pName->SetText(FText::FromName(m_ItemInfo.Name));
	}

	// #. 무게.
	FNumberFormattingOptions Options;
	Options.RoundingMode = ERoundingMode::HalfToZero;
	Options.MinimumIntegralDigits = 1;   // #. 정수 부분 최소 자릿수.
	Options.MaximumIntegralDigits = 324; // #. 정수 부분 최대 자릿수.
	Options.MinimumFractionalDigits = 1; // #. 소숫점 부분 최소 자릿수.
	Options.MaximumFractionalDigits = 1; // #. 소숫점 부분 최대 자릿수.

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
