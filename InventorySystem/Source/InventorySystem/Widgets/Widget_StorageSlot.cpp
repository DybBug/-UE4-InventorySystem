// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_StorageSlot.h"
#include "Widget_Inventory.h"
#include "Widget_InventorySlot.h"
#include "Widget_DraggedItem.h"
#include "Widget_Detail.h"
#include "Widget_Storage.h"
#include "InventorySystemCharacter.h"
#include "Components/Component_Storage.h"
#include "DragDropOperations/DDOperation_Item.h"
#include "DragDropOperations/DDOperation_StorageSlot.h"

#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <WidgetBlueprintLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>


#define LOCTEXT_NAMESPACE "Format"

void UWidget_StorageSlot::InitializeWidget(int _Index, UComponent_Storage* _pStorage)
{
	m_SlotIndex = _Index;
	m_pStorage = _pStorage;

	m_pSlotButton->SetClickMethod(EButtonClickMethod::PreciseClick);
	m_pSlotButton->SetTouchMethod(EButtonTouchMethod::PreciseTap);

	m_DefaultButtonStyle = m_pSlotButton->WidgetStyle;
	m_DraggedButtonOverStyle = m_DefaultButtonStyle;
	m_DraggedButtonOverStyle.Disabled.TintColor = m_DraggedButtonOverStyle.Hovered.TintColor;

	m_DraggedItemClass = LoadClass<UWidget_DraggedItem>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_DraggedItem.WB_DraggedItem_C'"));
	m_DetailWidgetClass = LoadClass<UWidget_Detail>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_Detail.WB_Detail_C'"));
}

void UWidget_StorageSlot::UpdateSlot()
{
	// #. 비어있는 슬롯일 경우.
	if (m_pStorage->IsSlotEmpty(m_SlotIndex))
	{
		m_pSlotButton->SetIsEnabled(false);
		m_pSlotButton->SetToolTip(nullptr);
		m_pItemIcon->SetVisibility(ESlateVisibility::Hidden);
		m_pAmountText->SetVisibility(ESlateVisibility::Hidden);

	}
	// #. 비어있지 않은 슬롯일 경우.
	else
	{
		m_pStorage->GetItemInfoAtIndex(m_SlotIndex, nullptr, &m_ItemInfo, &m_Amount);

		m_pSlotButton->SetIsEnabled(true);

		m_pItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		m_pItemIcon->SetBrushFromTexture(m_ItemInfo.pIcon);

		if (m_ItemInfo.bCanBeStacked)
		{
			FText Fromat = FText::Format(LOCTEXT("Format", "x{0}"), m_Amount);
			m_pAmountText->SetText(Fromat);
			m_pAmountText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			m_pAmountText->SetVisibility(ESlateVisibility::Hidden);
		}

		if (m_pDetailWidget)
		{
			m_pDetailWidget->InitializeWidget(m_ItemInfo, m_Amount);
			m_pDetailWidget->Update();
			m_pSlotButton->SetToolTip(m_pDetailWidget);
		}
		else
		{
			m_pDetailWidget = CreateWidget<UWidget_Detail>(GetWorld(), m_DetailWidgetClass);
			m_pDetailWidget->InitializeWidget(m_ItemInfo, m_Amount);
			m_pDetailWidget->Update();
			m_pSlotButton->SetToolTip(m_pDetailWidget);
		}
	}
}

void UWidget_StorageSlot::ResetStyle()
{
	m_bDraggedOver = false;
	m_pSlotButton->SetStyle(m_DefaultButtonStyle);
}

FReply UWidget_StorageSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Event;
	Event.NativeReply = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	if (m_pSlotButton->bIsEnabled)
	{
		if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
		{
			
		}
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			Event = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		}
	}

	return Event.NativeReply;
}

void UWidget_StorageSlot::NativeOnDragDetected(const FGeometry & InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// #. 드래그된 아이템 위젯 생성.
	UWidget_DraggedItem* pDraggedItemWidget = CreateWidget<UWidget_DraggedItem>(GetWorld(), m_DraggedItemClass);
	pDraggedItemWidget->InitializeWidget(m_ItemInfo, m_Amount);

	UDDOperation_StorageSlot* pOper = Cast<UDDOperation_StorageSlot>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDDOperation_StorageSlot::StaticClass()));
	pOper->SetStorageSlotWidget(this);
	pOper->DefaultDragVisual = pDraggedItemWidget;
	pOper->Pivot = EDragPivot::MouseDown;

	OutOperation = pOper;
}

bool UWidget_StorageSlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (m_bDraggedOver)
	{
		return true;
	}
	else
	{
		if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
		{
			m_bDraggedOver = true;
			m_pSlotButton->SetStyle(m_DraggedButtonOverStyle);
			return true;
		}
		else if (UDDOperation_StorageSlot* pStorageSlotOper = Cast<UDDOperation_StorageSlot>(InOperation))
		{
			m_bDraggedOver = true;
			m_pSlotButton->SetStyle(m_DraggedButtonOverStyle);
			return true;
		}
	}

	return true;
}

void UWidget_StorageSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (m_bDraggedOver)
	{
		if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
		{
			m_bDraggedOver = false;
			m_pSlotButton->SetStyle(m_DefaultButtonStyle);
		}
		else if (UDDOperation_StorageSlot* pStorageSlotOper = Cast<UDDOperation_StorageSlot>(InOperation))
		{
			m_bDraggedOver = false;
			m_pSlotButton->SetStyle(m_DefaultButtonStyle);
		}
	}
}

bool UWidget_StorageSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UDDOperation_StorageSlot* pStorageSlotOper = Cast<UDDOperation_StorageSlot>(InOperation))
	{
		bool bSuccess = m_pStorage->AddToIndex(pStorageSlotOper->GetStorageSlotWidget()->GetSlotIndex(), m_SlotIndex);
		if (bSuccess)
		{
			return true;
		}
		else
		{
			return m_pStorage->SwapSlots(pStorageSlotOper->GetStorageSlotWidget()->GetSlotIndex(), m_SlotIndex);
		}
	}
	else if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
	{
		return m_pStorage->MoveFromInvToStorageIndex(pItemOper->GetDraggedSlot()->GetInventory(), pItemOper->GetDraggedSlot()->GetSlotIndex(), m_SlotIndex);
	}

	return true;
	
}

void UWidget_StorageSlot::_OnClicked()
{
	
}


#undef LOCTEXT_NAMESPACE


