// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Storage.h"
#include "Widget_StorageSlot.h"
#include "Widget_ActionMenu.h"
#include "Widget_InventorySlot.h"
#include "Components/Component_Storage.h"
#include "Components/Component_Inventory.h"
#include "ItemClasses/Chest.h"
#include "DragDropOperations/DDOperation_Widget.h"
#include "DragDropOperations/DDOperation_Item.h"
#include "InventorySystemCharacter.h"

#include <Components/Button.h>
#include <Components/UniformGridPanel.h>
#include <Components/UniformGridSlot.h>
#include <Components/ScrollBox.h>
#include <Components/ComboBoxString.h>
#include <WidgetBlueprintLibrary.h>
#include <WidgetLayoutLibrary.h>


void UWidget_Storage::InitializeWidget(class UComponent_Inventory* m_pInventory)
{
	m_pInventory = m_pInventory;
}

void UWidget_Storage::GenerateSlotWidgets(UComponent_Storage* _pStorage)
{

	if (m_pStorage)
	{
		if (m_pStorage == _pStorage)
		{
			return;
		}
	}
	else
	{
		m_pStorage = _pStorage;

		m_pSlotPanel->ClearChildren();
		m_SlotWidgets.Empty();

		UClass* WB_StorageSlotClass = LoadClass<UWidget_StorageSlot>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_StorageSlot.WB_StorageSlot_C'"));

		if (IsValid(WB_StorageSlotClass))
		{
			TArray<FInventorySlot> Slots = m_pStorage->GetSlots();
			for (int i = 0; i < Slots.Num(); ++i)
			{
				UWidget_StorageSlot* pSlotWidget = CreateWidget<UWidget_StorageSlot>(GetWorld(), WB_StorageSlotClass);
				pSlotWidget->InitializeWidget(i, m_pStorage);
				pSlotWidget->UpdateSlot();
				m_SlotWidgets.Emplace(pSlotWidget);

				UUniformGridSlot* pGridSlot = m_pSlotPanel->AddChildToUniformGrid(pSlotWidget);
				int Row = i / (m_pStorage->GetSlotsPerRow());
				int Column = i % (m_pStorage->GetSlotsPerRow());
				pGridSlot->SetRow(Row);
				pGridSlot->SetColumn(Column);
			}
		}
		else
		{
			UE_LOG(LogClass, Error, TEXT("UWidget_Storage::GenerateSloWidgets() Failed!!"));
		}
	}
}

void UWidget_Storage::OnSlotClicked(UWidget_StorageSlot* _pSlotWidget)
{
	m_AmountToRemove = m_pStorage->GetSlots()[_pSlotWidget->GetSlotIndex()].Amount;

	int Rest = 0;
	bool bSuccess = m_pInventory->AddItem(m_pStorage->GetSlots()[_pSlotWidget->GetSlotIndex()].ItemClass, m_AmountToRemove, false, &Rest);

	if (bSuccess)
	{
		m_pStorage->RemoveItemAtIndex(_pSlotWidget->GetSlotIndex(), m_AmountToRemove - Rest);
		_pSlotWidget->ResetStyle();
	}
}

void UWidget_Storage::NativeConstruct()
{
	Super::NativeConstruct();
	m_pCloseButton->OnClicked.AddDynamic(this, &UWidget_Storage::_OnCloseButtonClicked);
}

FReply UWidget_Storage::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// #. 드래그 감지.
	FEventReply Event = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	Reply = Event.NativeReply;

	return Reply;
}

void UWidget_Storage::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	m_pInventory = m_pStorage->GetPlayer()->GetInventory();

	// #. 스크린 공간에서의 마우스 위치.
	FVector2D MousePos = InMouseEvent.GetScreenSpacePosition();

	// #. 절대 좌표(마우스 위치)를 (Geometry)로컬 좌표로 변환.
	FVector2D LocalPos = InGeometry.AbsoluteToLocal(MousePos);

	// #. 위젯 DragDropOperation 생성.
	UDDOperation_Widget* pDDOper = Cast<UDDOperation_Widget>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDDOperation_Widget::StaticClass()));
	pDDOper->SetWidgetToDrag(this);
	pDDOper->SetMouseOffset(LocalPos);
	pDDOper->SetMainWidget(m_pInventory->GetMainWidget());
	pDDOper->DefaultDragVisual = this;
	pDDOper->Pivot = EDragPivot::MouseDown;

	// #. 현재 위젯 제거.
	RemoveFromParent();

	OutOperation = pDDOper;
}

bool UWidget_Storage::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return true;
}

void UWidget_Storage::_OnCloseButtonClicked()
{
	m_pStorage->GetChest()->OnStorageClose();
}



