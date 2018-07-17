// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Throw.h"
#include "Widget_Inventory.h"
#include "Components/Component_Inventory.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/Button.h>
#include <Components/UniformGridPanel.h>
#include <Kismet/KismetSystemLibrary.h>
#include <TimerManager.h>
#include <Gameframework/InputSettings.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_Throw::InitializeWidget(UComponent_Inventory* _pInventory)
{
	m_pInventory = _pInventory;

	// #. - 버튼
	m_pMinusButton->OnClicked.AddDynamic(this, &UWidget_Throw::_OnMinusButtonClicked);    
	m_pMinusButton->OnPressed.AddDynamic(this, &UWidget_Throw::_OnMinusButtonPressed);
	m_pMinusButton->OnReleased.AddDynamic(this, &UWidget_Throw::_OnMinusButtonReleased);

	// #. + 버튼
	m_pPlusButton->OnClicked.AddDynamic(this, &UWidget_Throw::_OnPlusButtonClicked);    
	m_pPlusButton->OnPressed.AddDynamic(this, &UWidget_Throw::_OnPlusButtonPressed);   
	m_pPlusButton->OnReleased.AddDynamic(this, &UWidget_Throw::_OnPlusButtonReleased);     

	// #. 확인 버튼
	m_pConfirmButton->OnClicked.AddDynamic(this, &UWidget_Throw::_OnConfirmButtonClicked); 

	// #. 취소 버튼
	m_pCancelButton->OnClicked.AddDynamic(this, &UWidget_Throw::_OnCancelButtonClicked);  
}

void UWidget_Throw::Update(int _NewIndex)
{
	m_SlotIndex = _NewIndex;

	bool bEmpty;
	FItemInfo Info;
	int Amount;
	m_pInventory->GetItemInfoAtIndex(m_SlotIndex, &bEmpty, &Info, &Amount);

	// #. 빈 슬롯이 아니면...
	if (!bEmpty)
	{
		// #. 선택한 아이템의 이름과 현재 갯수 설정.
		FText Fromat = FText::Format(LOCTEXT("Format", "{0} x{1}"), FText::FromName(Info.Name), Amount);
		m_pNameText->SetText(Fromat);

		// #. 헌택한 아이템 최대 수량 설정.
		m_MaxAmount = Amount;

		// #. 선택한 아이템 아이콘 설정.
		m_pIcon->SetBrushFromTexture(Info.pIcon);

		// #. 버릴 아이템 갯수 설정(기본값 : 1개)
		m_ThrowAmount = 1;
		m_pAmountText->SetText(FText::AsNumber(m_ThrowAmount));
	}
}

void UWidget_Throw::_IncreaseAmount()
{
	m_ThrowAmount++;
	m_ThrowAmount = FMath::Clamp<int>(m_ThrowAmount, 1, m_MaxAmount);
	m_pAmountText->SetText(FText::AsNumber(m_ThrowAmount));
}

void UWidget_Throw::_DecreaseAmount()
{
	m_ThrowAmount--;
	m_ThrowAmount = FMath::Clamp<int>(m_ThrowAmount, 1, m_MaxAmount);
	m_pAmountText->SetText(FText::AsNumber(m_ThrowAmount));
}

void UWidget_Throw::_ClickCountReset()
{
	m_ClickCount = 0;
}

void UWidget_Throw::_OnMinusButtonClicked()
{
	m_ClickCount++;

	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = TEXT("_ClickCountReset");
	Info.UUID = GetUniqueID();
	Info.Linkage = 1;
	UKismetSystemLibrary::Delay(this, UInputSettings::GetInputSettings()->DoubleClickTime, Info);

	if (m_ClickCount > 1)
	{
		m_ClickCount = 0;
		m_ThrowAmount = 1;
		m_pAmountText->SetText(FText::AsNumber(m_ThrowAmount));
	}
	else
	{
		_DecreaseAmount();
	}
}

void UWidget_Throw::_OnMinusButtonPressed()
{
	GetWorld()->GetTimerManager().SetTimer(m_hMinusTimer, this, &UWidget_Throw::_DecreaseAmount, 0.1f, true);
}

void UWidget_Throw::_OnMinusButtonReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(m_hMinusTimer);
}

void UWidget_Throw::_OnPlusButtonClicked()
{
	m_ClickCount++;

	FLatentActionInfo LatentAction;
	LatentAction.CallbackTarget = this;
	LatentAction.ExecutionFunction = TEXT("_ClickCountReset");
	LatentAction.UUID = GetUniqueID();
	LatentAction.Linkage = 1;
	UKismetSystemLibrary::Delay(this, UInputSettings::GetInputSettings()->DoubleClickTime, LatentAction);

	if (m_ClickCount > 1)
	{
		m_ClickCount = 0;
		m_ThrowAmount = m_MaxAmount;
		m_pAmountText->SetText(FText::AsNumber(m_ThrowAmount));
	}
	else
	{
		_IncreaseAmount();
	}
}

void UWidget_Throw::_OnPlusButtonPressed()
{
	GetWorld()->GetTimerManager().SetTimer(m_hPlusTimer, this, &UWidget_Throw::_IncreaseAmount, 0.1f, true);
}

void UWidget_Throw::_OnPlusButtonReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(m_hPlusTimer);
}

void UWidget_Throw::_OnConfirmButtonClicked()
{
	bool bSuccess = m_pInventory->RemoveItemAtIndex(m_SlotIndex, m_ThrowAmount);

	if (bSuccess)
	{
		m_pInventory->GetInventoryWidget()->GetSlotPanel()->SetIsEnabled(true);
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWidget_Throw::_OnCancelButtonClicked()
{
	m_pInventory->GetInventoryWidget()->GetSlotPanel()->SetIsEnabled(true);
	SetVisibility(ESlateVisibility::Hidden);
}

#undef LOCTEXT_NAMESPACE

