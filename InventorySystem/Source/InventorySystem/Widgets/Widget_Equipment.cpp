// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_Equipment.h"
#include "Widget_WeaponSlot.h"
#include "ItemClasses/Item_Weapon.h"
#include "Components/Component_Inventory.h"
#include "InventorySystemCharacter.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>
#include <Components/UniformGridPanel.h>
#include <Components/UniformGridSlot.h>

void UWidget_Equipment::InitializeWidget(UComponent_Inventory* _pInventory)
{
	m_pInventory = _pInventory;
	m_pWeaponSlotWidget->InitializeWidget(_pInventory);

	m_pCloseButton->OnClicked.AddDynamic(this, &UWidget_Equipment::_OnCloseButtonClicked);
}

void UWidget_Equipment::UpdateEquipment()
{
	AItem_Base* pEquippedWeapon = m_pInventory->GetPlayer()->GetEquippedWeapon();
	if (pEquippedWeapon)
	{
		m_CurrentItemInfo = pEquippedWeapon->GetItemInfo();

		m_pNameText->SetText(FText::FromName(m_CurrentItemInfo.Name));
		m_pDescriptionText->SetText(m_CurrentItemInfo.Description);

		m_pNameText->SetVisibility(ESlateVisibility::Visible);
		m_pDescriptionText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		m_pNameText->SetText(FText::GetEmpty());
		m_pDescriptionText->SetText(FText::GetEmpty());

		m_pNameText->SetVisibility(ESlateVisibility::Collapsed);
		m_pDescriptionText->SetVisibility(ESlateVisibility::Collapsed);
	}

	m_pWeaponSlotWidget->UpdateWeapon();
}

void UWidget_Equipment::_OnCloseButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
}



