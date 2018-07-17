// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_InventorySlot.h"
#include "Widget_Inventory.h"
#include "Widget_DraggedItem.h"
#include "Widget_Detail.h"
#include "Widget_StorageSlot.h"
#include "InventorySystemCharacter.h"
#include "Components/Component_Inventory.h"
#include "DragDropOperations/DDOperation_Item.h"
#include "DragDropOperations/DDOperation_StorageSlot.h"
#include "DragDropOperations/DDOperation_Weapon.h"

#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <WidgetBlueprintLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>


#define LOCTEXT_NAMESPACE "Format"

void UWidget_InventorySlot::InitializeWidget(int _Index, UComponent_Inventory* _pInventory)
{
	m_SlotIndex = _Index;
	m_pInventory = _pInventory;

	m_pSlotButton->SetClickMethod(EButtonClickMethod::PreciseClick);
	m_pSlotButton->SetTouchMethod(EButtonTouchMethod::PreciseTap);
	m_pSlotButton->OnClicked.AddDynamic(this, &UWidget_InventorySlot::_OnClicked);

	m_DefaultButtonStyle = m_pSlotButton->WidgetStyle;
	m_DraggedButtonOverStyle = m_DefaultButtonStyle;
	m_DraggedButtonOverStyle.Disabled.TintColor = m_DraggedButtonOverStyle.Hovered.TintColor;

	m_DraggedItemClass = LoadClass<UWidget_DraggedItem>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_DraggedItem.WB_DraggedItem_C'"));
	m_DetailWidgetClass = LoadClass<UWidget_Detail>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_Detail.WB_Detail_C'"));
}

void UWidget_InventorySlot::UpdateSlot()
{
	// #. 비어있는 슬롯일 경우.
	if (m_pInventory->IsSlotEmpty(m_SlotIndex))
	{
		m_pSlotButton->SetIsEnabled(false);
		m_pSlotButton->SetToolTip(nullptr);
		m_pItemIcon->SetVisibility(ESlateVisibility::Hidden);
		m_pAmountText->SetVisibility(ESlateVisibility::Hidden);

	}
	// #. 비어있지 않은 슬롯일 경우.
	else
	{
		m_pInventory->GetItemInfoAtIndex(m_SlotIndex, nullptr, &m_ItemInfo, &m_Amount);		

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

FReply UWidget_InventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Event ;
	Event.NativeReply = Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);

	if (m_pSlotButton->bIsEnabled)
	{	
		if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
		{
			m_pInventory->GetInventoryWidget()->OnSlotClicked(this);			
		}
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			Event = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
		}
	}

	return Event.NativeReply;
}

void UWidget_InventorySlot::NativeOnDragDetected(const FGeometry & InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// #. 드래그된 아이템 위젯 생성.
	UWidget_DraggedItem* pDraggedItemWidget = CreateWidget<UWidget_DraggedItem>(GetWorld(), m_DraggedItemClass);
	pDraggedItemWidget->InitializeWidget(m_ItemInfo, m_Amount);

	UDDOperation_Item* pDDOper = Cast<UDDOperation_Item>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDDOperation_Item::StaticClass()));
	pDDOper->SetDraggedSlot(this);
	pDDOper->SetWasShiftDown(m_pInventory->GetPlayer()->GetIsShiftDown());
	pDDOper->DefaultDragVisual = pDraggedItemWidget;
	pDDOper->Pivot = EDragPivot::MouseDown;

	OutOperation = pDDOper;
}

bool UWidget_InventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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
		}
		else if (UDDOperation_StorageSlot* pStorageOper = Cast<UDDOperation_StorageSlot>(InOperation))
		{
			m_bDraggedOver = true;
			m_pSlotButton->SetStyle(m_DraggedButtonOverStyle);
		}
		else if (UDDOperation_Weapon* pWeaponOper = Cast<UDDOperation_Weapon>(InOperation))
		{
			m_bDraggedOver = true;
			m_pSlotButton->SetStyle(m_DraggedButtonOverStyle);
		}
	}

	return true;
}

void UWidget_InventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (m_bDraggedOver)
	{
		if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
		{
			m_bDraggedOver = false;
			m_pSlotButton->SetStyle(m_DefaultButtonStyle);
		}
		else if (UDDOperation_StorageSlot* pStorageOper = Cast<UDDOperation_StorageSlot>(InOperation))
		{
			m_bDraggedOver = false;
			m_pSlotButton->SetStyle(m_DefaultButtonStyle);
		}
		else if (UDDOperation_Weapon* pWeaponOper = Cast<UDDOperation_Weapon>(InOperation))
		{
			m_bDraggedOver = false;
			m_pSlotButton->SetStyle(m_DefaultButtonStyle);
		}
	}
}

bool UWidget_InventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation))
	{
		if (pItemOper->GetDraggedSlot() != this)
		{
			m_bDraggedOver = false;
			m_pSlotButton->SetStyle(m_DefaultButtonStyle);
			if (m_pInventory->AddToIndex(pItemOper->GetDraggedSlot()->GetSlotIndex(), m_SlotIndex))
			{
				return true;
			}
			else
			{
				if (pItemOper->GetWasShiftDown())
				{
					if (m_pInventory->SplitStackToIndex(pItemOper->GetDraggedSlot()->GetSlotIndex(), m_SlotIndex, pItemOper->GetDraggedSlot()->GetAmount() / 2))
					{
						return true;
					}
				}
				else
				{
					m_pInventory->SwapSlots(pItemOper->GetDraggedSlot()->GetSlotIndex(), m_SlotIndex);
					return true;
				}
			}
		}
	}
	else if (UDDOperation_StorageSlot* pStorageOper = Cast<UDDOperation_StorageSlot>(InOperation))
	{
		return m_pInventory->MoveFromStorageToInvIndex(pStorageOper->GetStorageSlotWidget()->GetStorage(), pStorageOper->GetStorageSlotWidget()->GetSlotIndex(), m_SlotIndex);
	}
	else if (UDDOperation_Weapon* pWeaponOper = Cast<UDDOperation_Weapon>(InOperation))
	{
		m_bDraggedOver = false;
		m_pInventory->GetPlayer()->UnequipItem(true, m_SlotIndex);
		m_pSlotButton->SetStyle(m_DefaultButtonStyle);
		return true;
	}


	return false;
}

void UWidget_InventorySlot::_OnClicked()
{
	++m_ClickCount;
	
	FLatentActionInfo Latent;
	Latent.CallbackTarget = this;
	Latent.ExecutionFunction = TEXT("_ClickCountReset");
	Latent.UUID = GetUniqueID();
	Latent.Linkage = 1;
	UKismetSystemLibrary::Delay(this, UInputSettings::GetInputSettings()->DoubleClickTime, Latent);

	if (m_ClickCount >= 2)
	{
		if (m_ItemInfo.bCanBeUsed)
		{
			m_ClickCount = 0;
			m_pInventory->UseItemAtIndex(m_SlotIndex);
		}
	}
}

void UWidget_InventorySlot::_ClickCountReset()
{
	m_ClickCount = 0;
}

#undef LOCTEXT_NAMESPACE

