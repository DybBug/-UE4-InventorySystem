// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Inventory.h"
#include "Widget_InventorySlot.h"
#include "Widget_ActionMenu.h"
#include "Components/Component_Inventory.h"
#include "DragDropOperations/DDOperation_Widget.h"

#include <Components/Button.h>
#include <Components/UniformGridPanel.h>
#include <Components/UniformGridSlot.h>
#include <Components/ScrollBox.h>
#include <Components/ComboBoxString.h>
#include <WidgetBlueprintLibrary.h>
#include <WidgetLayoutLibrary.h>


void UWidget_Inventory::InitializeWidget(UComponent_Inventory* _pInventory, int _SlotsPerRow)
{
	m_pInventory = _pInventory;
	m_SlotsPerRow = _SlotsPerRow;
	m_pActionMenu->InitializeWidget(m_pInventory);

	GenerateSlotWidgets();

	// #. ComboBoxString ����.
	for (int i = 0; i < (uint8)ESortType::Max; ++i)
	{
		FString SortTypeStr = CONVERT_DISPLAYNAME_TO_STRING(L"ESortType", (ESortType)i);
		m_pSortTypeBox->AddOption(SortTypeStr);
	}

	m_pSortTypeBox->SetSelectedOption(m_pSortTypeBox->GetOptionAtIndex(0));
}

void UWidget_Inventory::GenerateSlotWidgets()
{
	m_pSlotPanel->ClearChildren();
	m_SlotWidgets.Empty();

	UClass* WB_InventorySlotClass = LoadClass<UWidget_InventorySlot>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_InventorySlot.WB_InventorySlot_C'"));
	
	if (IsValid(WB_InventorySlotClass))
	{
		TArray<FInventorySlot> Slots = m_pInventory->GetSlots();
		for (int i = 0; i < Slots.Num(); ++i)
		{
			UWidget_InventorySlot* pSlotWidget = CreateWidget<UWidget_InventorySlot>(GetWorld(), WB_InventorySlotClass);
			pSlotWidget->InitializeWidget(i, m_pInventory);
			pSlotWidget->UpdateSlot();
			m_SlotWidgets.Emplace(pSlotWidget);

			UUniformGridSlot* pGridSlot = m_pSlotPanel->AddChildToUniformGrid(pSlotWidget);
			int Row = i / (m_SlotsPerRow);
			int Column = i % (m_SlotsPerRow);
			pGridSlot->SetRow(Row);
			pGridSlot->SetColumn(Column);
		}
	}
	else
	{
		UE_LOG(LogClass, Error, TEXT("UWidget_Inventory::GenerateSloWidgets() Failed!!"));
	}
}

void UWidget_Inventory::OnSlotClicked(UWidget_InventorySlot* _pSlotWidget)
{
	UUniformGridSlot* pGridSlot = Cast<UUniformGridSlot>(_pSlotWidget->Slot);
	int Width  = pGridSlot->Column * 64; // #. ���� ���� * ���� ����(64)
	int Height = pGridSlot->Row    * 64; // #. ���� ���� * ���� ����(64)

	Height -= m_pInventoryScrollBox->GetScrollOffset() / 1.5f;

	// #. �׼� �޴� ��ġ.
	FVector2D ActionMenuPos(Width, FMath::Clamp<int>(Height, 0, 200));

	m_pActionMenu->SetRenderTranslation(ActionMenuPos);
	m_pActionMenu->Update(_pSlotWidget->GetSlotIndex());
	m_pActionMenu->SetVisibility(ESlateVisibility::Visible);
}

void UWidget_Inventory::NativeConstruct()
{
	Super::NativeConstruct();
	m_pCloseButton->OnClicked.AddDynamic(this, &UWidget_Inventory::_OnCloseButtonClicked);
	m_pSortButton->OnClicked.AddDynamic(this, &UWidget_Inventory::_OnSortButtonClicked);
}

FReply UWidget_Inventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// #. �巡�� ����.
	FEventReply Event = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);

	Reply = Event.NativeReply;

	return Reply;
}

void UWidget_Inventory::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// #. ��ũ�� ���������� ���콺 ��ġ.
	FVector2D MousePos = InMouseEvent.GetScreenSpacePosition();

	// #. ���� ��ǥ(���콺 ��ġ)�� (Geometry)���� ��ǥ�� ��ȯ.
	FVector2D LocalPos = InGeometry.AbsoluteToLocal(MousePos);

	// #. ���� DragDropOperation ����.
	UDDOperation_Widget* pDDOper = Cast<UDDOperation_Widget>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDDOperation_Widget::StaticClass()));
	pDDOper->SetWidgetToDrag(this);
	pDDOper->SetMouseOffset(LocalPos);
	pDDOper->SetMainWidget(m_pInventory->GetMainWidget());
	pDDOper->DefaultDragVisual = this;
	pDDOper->Pivot = EDragPivot::MouseDown;

	// #. ���� ���� ����.
	RemoveFromParent();

	OutOperation = pDDOper;	
}

void UWidget_Inventory::_OnCloseButtonClicked()
{
	m_pInventory->HideInventory();
}

void UWidget_Inventory::_OnSortButtonClicked()
{
	m_bReversedSort = !m_bReversedSort;

	FString SortStr = m_pSortTypeBox->GetSelectedOption();
	int SortIndex =	m_pSortTypeBox->FindOptionIndex(SortStr);

	m_pInventory->SortInvnetory((ESortType)SortIndex, m_bReversedSort);
}


