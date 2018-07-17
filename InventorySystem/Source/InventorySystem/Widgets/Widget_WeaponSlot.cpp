// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_WeaponSlot.h"
#include "Widget_Inventory.h"
#include "Widget_InventorySlot.h"
#include "Widget_DraggedItem.h"
#include "Widget_Detail.h"
#include "Widget_StorageSlot.h"
#include "Components/Component_Inventory.h"
#include "DragDropOperations/DDOperation_Weapon.h"
#include "DragDropOperations/DDOperation_Item.h"
#include "ItemClasses/Item_Weapon.h"
#include "InventorySystemCharacter.h"


#include <Components/Button.h>
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <WidgetBlueprintLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/InputSettings.h>


#define LOCTEXT_NAMESPACE "Format"

void UWidget_WeaponSlot::InitializeWidget(UComponent_Inventory* _pInventory)
{
	m_pInventory = _pInventory;

	m_pSlotButton->SetClickMethod(EButtonClickMethod::PreciseClick);
	m_pSlotButton->SetTouchMethod(EButtonTouchMethod::PreciseTap);

	m_DefaultButtonStyle = m_pSlotButton->WidgetStyle;
	m_DraggedButtonOverStyle = m_DefaultButtonStyle;
	m_DraggedButtonOverStyle.Disabled.TintColor = m_DraggedButtonOverStyle.Hovered.TintColor;

	m_DraggedItemClass = LoadClass<UWidget_DraggedItem>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_DraggedItem.WB_DraggedItem_C'"));
	m_DetailWidgetClass = LoadClass<UWidget_Detail>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_Detail.WB_Detail_C'"));
}

void UWidget_WeaponSlot::UpdateWeapon()
{
	AItem_Base* pEquippedWeapon = m_pInventory->GetPlayer()->GetEquippedWeapon();
	if (pEquippedWeapon)
	{
		m_ItemClass = pEquippedWeapon->GetClass();
		m_ItemInfo = pEquippedWeapon->GetItemInfo();

		m_pItemIcon->SetBrushFromTexture(m_ItemInfo.pIcon);
		m_pItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);

		m_pSlotButton->SetIsEnabled(true);
		m_pSlotButton->SetStyle(m_DefaultButtonStyle);
	}
	else
	{
		m_ItemClass = nullptr;
		m_pItemIcon->SetVisibility(ESlateVisibility::Hidden);
		m_pSlotButton->SetIsEnabled(false);
		_ResetStyle();
	}
}

FReply UWidget_WeaponSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply EventReply;
	EventReply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		EventReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		m_pInventory->GetPlayer()->UnequipItem(false, 0);
	}

	return EventReply.NativeReply;
}

void UWidget_WeaponSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// #. 드래그된 아이템 위젯 생성.
	UWidget_DraggedItem* pDraggedItemWidget = CreateWidget<UWidget_DraggedItem>(GetWorld(), m_DraggedItemClass);
	pDraggedItemWidget->InitializeWidget(m_ItemInfo, 1);

	UDDOperation_Weapon* pOper = Cast<UDDOperation_Weapon>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDDOperation_Weapon::StaticClass()));
	pOper->SetDraggedSlot(this);
	pOper->DefaultDragVisual = pDraggedItemWidget;
	pOper->Pivot = EDragPivot::MouseDown;

	OutOperation = pOper;
}

bool UWidget_WeaponSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UDDOperation_Item* pItemOper = Cast<UDDOperation_Item>(InOperation);
	if (pItemOper)
	{
		int SlotIndex = pItemOper->GetDraggedSlot()->GetSlotIndex();
		m_pInventory->UseItemAtIndex(SlotIndex);
		_ResetStyle();		

		return true;
	}

	return false;
}

void UWidget_WeaponSlot::_ResetStyle()
{
	m_bDraggedOver = false;
	m_pSlotButton->SetStyle(m_DefaultButtonStyle);
}

#undef LOCTEXT_NAMESPACE


