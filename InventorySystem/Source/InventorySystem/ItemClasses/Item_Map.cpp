// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_Map.h"
#include "Components/Component_Inventory.h"
#include "Widgets/Widget_Main.h"
#include "Widgets/Widget_Inventory.h"
#include "Widgets/Widget_OldMap.h"

#include <UObject/ConstructorHelpers.h>
#include <Components/CanvasPanel.h>
#include <Components/CanvasPanelSlot.h>
#include <Kismet/GameplayStatics.h>
#include <WidgetBlueprintLibrary.h>


AItem_Map::AItem_Map()
	: Super()
{
	static ConstructorHelpers::FClassFinder<UWidget_OldMap> WB_OldMap(TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_OldMap.WB_OldMap_C'"));
	if (WB_OldMap.Succeeded())
	{
		m_OldMapWidgetClass = WB_OldMap.Class;
	}
}


void AItem_Map::EventOnUsed()
{
	m_pInventory->GetMainWidget()->GetInventoryWidget()->SetIsEnabled(false);
	m_pOldMapWidget = CreateWidget<UWidget_OldMap>(GetWorld(), m_OldMapWidgetClass);
	m_pOldMapWidget->InitializeWidget(this);

	UCanvasPanelSlot* pSlot = m_pInventory->GetMainWidget()->GetCanvas()->AddChildToCanvas(m_pOldMapWidget);
	pSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
	pSlot->SetAutoSize(true);
	pSlot->SetPosition(FVector2D(-325.f, -240.f));
	
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(m_pOldMapWidget->TakeWidget());
	UGameplayStatics::GetPlayerController(this, 0)->SetInputMode(InputMode);
}

void AItem_Map::OnRead()
{
	m_pInventory->GetMainWidget()->GetInventoryWidget()->SetIsEnabled(true);
	m_pOldMapWidget->RemoveFromParent();


	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetWidgetToFocus(nullptr);
	UGameplayStatics::GetPlayerController(this, 0)->SetInputMode(FInputModeGameOnly());

	Destroy();
}

