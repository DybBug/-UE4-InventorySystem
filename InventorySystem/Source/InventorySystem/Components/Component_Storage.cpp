// Fill out your copyright notice in the Description page of Project Settings.

#include "Component_Storage.h"
#include "Component_Inventory.h"
#include "InventorySystemCharacter.h"
#include "ItemClasses/Item_Base.h"
#include "ItemClasses/PickupActor.h"
#include "ItemClasses/Chest.h"
#include "NPCs/NPC_Merchant.h"
#include "Widgets/Widget_Inventory.h"
#include "Widgets/Widget_Main.h"
#include "Widgets/Widget_InventorySlot.h"
#include "Widgets/Widget_Throw.h"
#include "Widgets/Widget_ActionMenu.h"
#include "Widgets/Widget_CraftingMenu.h"
#include "Widgets/Widget_Shop.h"
#include "Widgets/Widget_Sell.h"
#include "Widgets/Widget_Storage.h"
#include "Widgets/Widget_StorageSlot.h"
#include "SaveGames/InventorySave.h"

#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetStringLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetBlueprintLibrary.h>
#include <Components/UniformGridPanel.h>
#include <Components/Border.h>
#include <Components/TextBlock.h>

#define LOCTEXT_NAMESPACE "Format"

// Sets default values for this component's properties
UComponent_Storage::UComponent_Storage()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UComponent_Storage::StartPlay()
{
	m_pChest = Cast<AChest>(GetOwner());
	m_StartItems = m_pChest->GetStartingItems();

	m_Slots = m_StartItems;
	
	m_Slots.SetNumZeroed(m_AmountOfSlots);
}

bool UComponent_Storage::IsSlotEmpty(int _Index)
{
	return !IsValid(m_Slots[_Index].ItemClass);
}


void UComponent_Storage::GetItemInfoAtIndex(
	const int& _Index,
	bool*      out_pEmptySlot,
	FItemInfo* out_pItemInfo,
	int*       out_pAmount)
{
	// #. _Index의 슬롯에 아이템이 있을 경우.
	if (IsValid(m_Slots[_Index].ItemClass))
	{
		if (out_pEmptySlot) *out_pEmptySlot = false;
		if (out_pItemInfo)  *out_pItemInfo = m_Slots[_Index].ItemClass.GetDefaultObject()->GetItemInfo();
		if (out_pAmount)    *out_pAmount = m_Slots[_Index].Amount;
		return;
	}
	// #. _Index의 슬롯에 아이템이 없을 경우.
	else
	{
		if (out_pEmptySlot) *out_pEmptySlot = true;
		if (out_pItemInfo)  *out_pItemInfo = FItemInfo();
		if (out_pAmount)    *out_pAmount = 0;
		return;
	}
}

void UComponent_Storage::SearchEmpthSlot(bool* out_pSuccess, int* out_pIndex)
{
	// #. 슬롯들 중 비어있는 슬롯 찾기.
	for (int i = 0; i < m_Slots.Num(); ++i)
	{
		// #. 비어있는 슬롯 찾음.
		if (!IsValid(m_Slots[i].ItemClass))
		{
			if (out_pSuccess) *out_pSuccess = true;
			if (out_pIndex)   *out_pIndex = i;
			return;
		}
	}

	// #. 비어있는 슬롯 못찾음.
	if (out_pSuccess) *out_pSuccess = false;
	if (out_pIndex)   *out_pIndex = -1;
	return;
}

void UComponent_Storage::SearchFreeStack(const TSubclassOf<class AItem_Base>& _ItemClass, bool* out_pSuccess, int* out_pIndex)
{
	// #. 모든 아이템 슬롯 검색.
	for (int i = 0; i < m_Slots.Num(); ++i)
	{
		// #. i번째 슬롯에 아이템이 있을 경우.
		if (IsValid(m_Slots[i].ItemClass))
		{
			// #. 검색 성공(찾을 아이템이 맞고, 수량이 최대치 이하일 경우).
			if (m_Slots[i].ItemClass == _ItemClass &&
				m_Slots[i].Amount < m_MaxStackSize)
			{
				if (out_pSuccess) *out_pSuccess = true;
				if (out_pIndex) *out_pIndex = i;
				return;
			}
		}
	}

	// 검색에 실패.
	if (out_pSuccess) *out_pSuccess = false;
	if (out_pIndex) *out_pIndex = -1;
	return;
}



bool UComponent_Storage::AddItem(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, int* out_pRest)
{
	const FItemInfo& NewItemInfo = _ItemClass.GetDefaultObject()->GetItemInfo();

	//
	// #. 새로운 아이템이 "누적 가능"한 경우.
	//
	if (NewItemInfo.bCanBeStacked)
	{
		// #. 누적가능한 슬롯 찾기.
		bool bSuccess;
		int Index;
		SearchFreeStack(_ItemClass, &bSuccess, &Index);

		// #. 누적 가능한 슬롯 찾음.
		if (bSuccess)
		{
			// #. 넘쳤을 경우. (현재 슬롯의 아이템 갯수 + 새로 추가할 갯수) > 최대 갯수
			if ((m_Slots[Index].Amount + _Amount) > m_MaxStackSize)
			{
				int RemainAmount = (m_Slots[Index].Amount + _Amount) - m_MaxStackSize;
				m_Slots[Index].Amount = m_MaxStackSize;
				m_Slots[Index].ItemClass = _ItemClass;

				UpdateSlotAtIndex(Index);

				// #. 다른 슬롯에 추가.
				AddItem(_ItemClass, RemainAmount, out_pRest);
				return true;
			}
			// #. 넘치지 않은 경우.
			else
			{
				m_Slots[Index].Amount += _Amount;
				m_Slots[Index].ItemClass = _ItemClass;

				UpdateSlotAtIndex(Index);

				*out_pRest = 0;
				return true;
			}
		}
		// #. 누적 가능한 슬롯 못 찾음.
		else
		{
			// #. 빈 슬롯을 찾는다.
			int EmptySlotIndex;
			SearchEmpthSlot(&bSuccess, &EmptySlotIndex);

			// #. 빈 슬롯을 찾음.
			if (bSuccess)
			{
				// #. 추가할 갯수가 최대갯수보다 많다.
				if (_Amount > m_MaxStackSize)
				{
					m_Slots[EmptySlotIndex].Amount = m_MaxStackSize;
					m_Slots[EmptySlotIndex].ItemClass = _ItemClass;

					UpdateSlotAtIndex(EmptySlotIndex);

					// #. 남은 아이템 갯수 다시 추가.
					int RemainAmount = _Amount - m_MaxStackSize;
					AddItem(_ItemClass, RemainAmount, out_pRest);
					return true;
				}
				// #. 추가할 갯수가 최대갯수보다 적거나 같다.
				else
				{
					m_Slots[EmptySlotIndex].Amount = _Amount;
					m_Slots[EmptySlotIndex].ItemClass = _ItemClass;

					UpdateSlotAtIndex(EmptySlotIndex);

					*out_pRest = 0;
					return true;
				}
			}

			*out_pRest = _Amount;
			return bSuccess;
		}

	}
	//
	// #. 새로운 아이템이 "누적 불가능"한 경우.
	//
	else
	{
		// #. 빈 슬롯을 찾는다.
		bool bSuccess;
		int EmptySlotIndex;
		SearchEmpthSlot(&bSuccess, &EmptySlotIndex);

		// #. 빈 슬롯을 찾음.
		if (bSuccess)
		{
			// #. 빈 슬롯에 새로운 아이템(한개) 추가.
			m_Slots[EmptySlotIndex].Amount = 1;
			m_Slots[EmptySlotIndex].ItemClass = _ItemClass;

			UpdateSlotAtIndex(EmptySlotIndex);

			// #. 새로 추가할 아이템의 갯수 1개 감소.
			_Amount--;

			// #. 새로 추가할 아이템의 갯수가 남아있으면 다른 빈슬롯에 아이템 추가.
			if (_Amount > 0)
			{
				AddItem(_ItemClass, _Amount, out_pRest);
				return true;
			}
			*out_pRest = 0;
			return true;
		}

		// #. 빈 슬롯을 못찾음.(인벤토리가 꽉참)
		*out_pRest = _Amount;
		return false;
	}
}


bool UComponent_Storage::AddItemToIndex(int _Index, TSubclassOf<AItem_Base> _ItemClass, int _Amount)
{
	if (IsSlotEmpty(_Index) && (_Amount <= m_MaxStackSize))
	{
		m_Slots[_Index].ItemClass = _ItemClass;
		m_Slots[_Index].Amount = _Amount;

		UpdateSlotAtIndex(_Index);

		return true;
	}

	return false;
}

int UComponent_Storage::GetAmountAtIndex(int _Index)
{
	return m_Slots[_Index].Amount;
}

void UComponent_Storage::UpdateSlotAtIndex(int _Index)
{
	m_pStorageWidget->GetSlotWidgets()[_Index]->UpdateSlot();
}

bool UComponent_Storage::RemoveItemAtIndex(int _Index, int _Amount)
{
	if (!IsSlotEmpty(_Index) && (_Amount > 0))
	{
		if (_Amount >= GetAmountAtIndex(_Index))
		{
			
			m_Slots[_Index].ItemClass = nullptr;
			m_Slots[_Index].Amount = 0;
		}
		else
		{
			
			m_Slots[_Index].Amount -= _Amount;
		}

		UpdateSlotAtIndex(_Index);
		return true;
	}

	return false;
}

bool UComponent_Storage::SwapSlots(int _Index1, int _Index2)
{
	int SlotsLastIndex = m_Slots.Num() - 1;
	if ((_Index1 <= SlotsLastIndex) && (_Index2 <= SlotsLastIndex))
	{
		FInventorySlot TempSlot = m_Slots[_Index1];
		m_Slots[_Index1] = m_Slots[_Index2];
		m_Slots[_Index2] = TempSlot;

		UpdateSlotAtIndex(_Index1);
		UpdateSlotAtIndex(_Index2);
	}

	return false;
}

bool UComponent_Storage::AddToIndex(int _FromIndex, int _ToIndex)
{
	FItemInfo FromItemInfo;
	GetItemInfoAtIndex(_FromIndex, nullptr, &FromItemInfo, nullptr);

	if (m_Slots[_FromIndex].ItemClass == m_Slots[_ToIndex].ItemClass &&
		m_Slots[_ToIndex].Amount < m_MaxStackSize &&
		FromItemInfo.bCanBeStacked)
	{
		// #. 합쳤을때 갯수가 최대갯수를 넘지 않음.
		if ((m_MaxStackSize - m_Slots[_ToIndex].Amount) >= m_Slots[_FromIndex].Amount)
		{
			m_Slots[_ToIndex].Amount += m_Slots[_FromIndex].Amount;
			UpdateSlotAtIndex(_ToIndex);

			m_Slots[_FromIndex].ItemClass = nullptr;
			m_Slots[_FromIndex].Amount = 0;
			UpdateSlotAtIndex(_FromIndex);

			return true;
		}
		// #. 합쳤을때 갯수가 최대갯수를 넘음.
		else
		{
			m_Slots[_FromIndex].Amount -= (m_MaxStackSize - m_Slots[_ToIndex].Amount);
			UpdateSlotAtIndex(_FromIndex);

			m_Slots[_ToIndex].Amount = m_MaxStackSize;
			UpdateSlotAtIndex(_ToIndex);

			return true;
		}
	}

	return false;
}

bool UComponent_Storage::IncreaseAmountAtIndex(int _Index, int _Amount)
{
	if (!IsSlotEmpty(_Index) &&
		((GetAmountAtIndex(_Index) + _Amount) <= m_MaxStackSize))
	{
		m_Slots[_Index].Amount += _Amount;

		UpdateSlotAtIndex(_Index);

		return true;
	}
	return false;
}

void UComponent_Storage::ShowStorage()
{
	m_pPlayer = Cast<AInventorySystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	m_pStorageWidget->SetVisibility(ESlateVisibility::Visible);
	m_pPlayer->GetInventory()->SetStorageOpen(true);

	AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	m_pStorageWidget = m_pPlayer->GetInventory()->GetMainWidget()->GetStorageWidget();
}

void UComponent_Storage::HideStorage()
{
	m_pPlayer = Cast<AInventorySystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	m_pStorageWidget->SetVisibility(ESlateVisibility::Hidden);
	m_pPlayer->GetInventory()->SetStorageOpen(false);
}

bool UComponent_Storage::MoveFromInvToStorageIndex(UComponent_Inventory* _pInventory, int _InventoryIndex, int _StorageIndex)
{
	int AmountToAdd = 0;

	if (IsSlotEmpty(_StorageIndex))
	{
		AmountToAdd = _pInventory->GetAmountAtIndex(_InventoryIndex);		
		bool bSuccess = AddItemToIndex(_StorageIndex, _pInventory->GetSlots()[_InventoryIndex].ItemClass , AmountToAdd);

		if (bSuccess)
		{
			_pInventory->RemoveItemAtIndex(_InventoryIndex, AmountToAdd);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		FItemInfo ItemInfo;
		GetItemInfoAtIndex(_StorageIndex, nullptr, &ItemInfo, nullptr);

		if ((_pInventory->GetSlots()[_InventoryIndex].ItemClass == m_Slots[_StorageIndex].ItemClass) &&
			ItemInfo.bCanBeStacked &&
			(GetAmountAtIndex(_StorageIndex) < m_MaxStackSize))
		{
			if (_pInventory->GetAmountAtIndex(_InventoryIndex) > (m_MaxStackSize - GetAmountAtIndex(_StorageIndex)))
			{
				AmountToAdd = m_MaxStackSize - GetAmountAtIndex(_StorageIndex);
			}
			else
			{
				AmountToAdd =  _pInventory->GetAmountAtIndex(_InventoryIndex);
			}

			bool bSuccess = IncreaseAmountAtIndex(_StorageIndex, AmountToAdd);
			if (bSuccess)
			{
				_pInventory->RemoveItemAtIndex(_InventoryIndex, AmountToAdd);
				return true;
			}
			else
			{
				return false;
			}
		
		}
		else
		{
			return false;
		}

	}
	return false;
}

#undef LOCTEXT_NAMESPACE

