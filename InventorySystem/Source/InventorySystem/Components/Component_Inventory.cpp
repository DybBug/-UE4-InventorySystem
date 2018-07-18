// Fill out your copyright notice in the Description page of Project Settings.

#include "Component_Inventory.h"
#include "Component_Storage.h"
#include "InventorySystemCharacter.h"
#include "ItemClasses/Item_Base.h"
#include "ItemClasses/PickupActor.h"
#include "NPCs/NPC_Merchant.h"
#include "Widgets/Widget_Inventory.h"
#include "Widgets/Widget_Main.h"
#include "Widgets/Widget_InventorySlot.h"
#include "Widgets/Widget_Throw.h"
#include "Widgets/Widget_ActionMenu.h"
#include "Widgets/Widget_CraftingMenu.h"
#include "Widgets/Widget_Shop.h"
#include "Widgets/Widget_Sell.h"
#include "Widgets/Widget_Hotkey.h"
#include "Widgets/Widget_HotkeySlot.h"
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
UComponent_Inventory::UComponent_Inventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}

void UComponent_Inventory::PlayStart()
{
	m_pPlayer = CastChecked<AInventorySystemCharacter>(GetOwner());

	if (UGameplayStatics::DoesSaveGameExist(TEXT("InventorySave"), 0))
	{
		_LoadInventory();

		if (m_CurrentWeight > m_OverloadedCap)
		{
			m_bOverloaded = true;
			m_pPlayer->OnOverloaded();
		}
	}

	m_Slots.SetNum(m_AmountOfSlots);	

	UClass* WB_MainClass = LoadClass<UWidget_Main>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_Main.WB_Main_C'"));
	if (WB_MainClass)
	{
		m_pMainWidget = CreateWidget<UWidget_Main>(GetWorld(), WB_MainClass);
		m_pMainWidget->InitializeWidget(this, m_SlotsPerRow, m_ObtainedVisibleDuration);

		m_pInventoryWidget = m_pMainWidget->GetInventoryWidget();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(m_pMainWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetInputMode(InputMode);

		m_pPlayer->UpdateHealthBar();
		m_pPlayer->UpdateGold();

		UpdateWeight();

		HideInventory();		

		m_pMainWidget->AddToViewport();

		m_pPlayer->LoadWeapon();

		if (!m_bShowObtainedMessage)
		{
			m_pMainWidget->GetObtainedItemBorder()->RemoveFromParent();
		}
	}

	m_Hotkeys = m_pMainWidget->GetHotkeys();
	m_bShowHotkeys = m_pMainWidget->GetShowHotkeys();
}

bool UComponent_Inventory::IsSlotEmpty(int _Index)
{
	return !IsValid(m_Slots[_Index].ItemClass);
}


void UComponent_Inventory::GetItemInfoAtIndex(
	const int& _Index,
	bool*      out_pEmptySlot,
	FItemInfo* out_pItemInfo,
	int*       out_pAmount)
{
	// #. _Index의 슬롯에 아이템이 있을 경우.
	if (IsValid(m_Slots[_Index].ItemClass))
	{
		if(out_pEmptySlot) *out_pEmptySlot = false;
		if(out_pItemInfo)  *out_pItemInfo = m_Slots[_Index].ItemClass.GetDefaultObject()->GetItemInfo();
		if(out_pAmount)    *out_pAmount = m_Slots[_Index].Amount;
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

void UComponent_Inventory::SearchEmpthSlot(bool* out_pSuccess, int* out_pIndex)
{
	// #. 슬롯들 중 비어있는 슬롯 찾기.
	for (int i = 0; i < m_Slots.Num(); ++i)
	{
		// #. 비어있는 슬롯 찾음.
		if (!IsValid(m_Slots[i].ItemClass))
		{
			if(out_pSuccess) *out_pSuccess = true;
			if(out_pIndex)   *out_pIndex = i;
			return;
		}
	}

	// #. 비어있는 슬롯 못찾음.
	if(out_pSuccess) *out_pSuccess = false;
	if(out_pIndex)   *out_pIndex = -1;
	return;
}

void UComponent_Inventory::SearchFreeStack(const TSubclassOf<class AItem_Base>& _ItemClass, bool* out_pSuccess, int* out_pIndex)
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
				if(out_pSuccess) *out_pSuccess = true;
				if(out_pIndex) *out_pIndex = i;
				return;
			}
		}
	}

	// 검색에 실패.
	if(out_pSuccess) *out_pSuccess = false;
	if(out_pIndex) *out_pIndex = -1;
	return;
}

bool UComponent_Inventory::AddItem(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, bool _bShowNotification, int* out_pRest)
{
	if (_bShowNotification)
	{
		if (m_bShowObtainedMessage)
		{
			bool bSuccess = _AddItem(_ItemClass, _Amount, out_pRest);
			if (bSuccess)
			{
				AddWeightForItem(_ItemClass, _Amount - (*out_pRest));
				_UpdateCraftingMenu();
				return ShowObtainedItem(_ItemClass, _Amount - (*out_pRest));
			}

			return bSuccess;
		}
	}

	bool bSuccessed = _AddItem(_ItemClass, _Amount, out_pRest);
	if (bSuccessed)
	{
		AddWeightForItem(_ItemClass, _Amount - (*out_pRest));
		_UpdateCraftingMenu();
		return true;
	}

	return false;
}

bool UComponent_Inventory::AddItemToIndex(int _Index, TSubclassOf<AItem_Base> _ItemClass, int _Amount)
{
	if (IsSlotEmpty(_Index) && (_Amount <= m_MaxStackSize))
	{
		m_Slots[_Index].ItemClass = _ItemClass;
		m_Slots[_Index].Amount = _Amount;

		AddWeightForItem(_ItemClass, _Amount);
		UpdateSlotAtIndex(_Index);

		return true;
	}

	return false;
}

int UComponent_Inventory::GetAmountAtIndex(int _Index)
{
	return m_Slots[_Index].Amount;
}

void UComponent_Inventory::UpdateSlotAtIndex(int _Index)
{
	m_pInventoryWidget->GetSlotWidgets()[_Index]->UpdateSlot();
	UpdateHotkeyForIndex(_Index);
}

bool UComponent_Inventory::RemoveItemAtIndex(int _Index, int _Amount)
{
	if (!IsSlotEmpty(_Index) &&	(_Amount > 0))
	{
		if (_Amount >= GetAmountAtIndex(_Index))
		{
			RemoveWeightForItem(m_Slots[_Index].ItemClass, m_Slots[_Index].Amount);

			m_Slots[_Index].ItemClass = nullptr;
			m_Slots[_Index].Amount = 0;			
		}
		else
		{
			RemoveWeightForItem(m_Slots[_Index].ItemClass, _Amount);

			m_Slots[_Index].Amount -= _Amount;
		}

		UpdateSlotAtIndex(_Index);
		_UpdateCraftingMenu();
		return true;
	}

	return false;
}

bool UComponent_Inventory::SwapSlots(int _Index1, int _Index2)
{
	int SlotsLastIndex = m_Slots.Num()-1;
	if ((_Index1 <= SlotsLastIndex) && (_Index2 <= SlotsLastIndex))
	{
		HandleSwapForHotkeys(_Index1, _Index2);

		FInventorySlot TempSlot = m_Slots[_Index1];
		m_Slots[_Index1] = m_Slots[_Index2];
		m_Slots[_Index2] = TempSlot;

		UpdateSlotAtIndex(_Index1);
		UpdateSlotAtIndex(_Index2);
	}

	return false;
}

bool UComponent_Inventory::SplitStack(int _StackIndex, int _Amount)
{
	bool bEmpty;
	FItemInfo ItemInfo;
	int Amount;
	GetItemInfoAtIndex(_StackIndex, &bEmpty, &ItemInfo, &Amount);

	// #. 현재 슬롯에 아이템이 들어 있음.
	if (!bEmpty)
	{
		if (ItemInfo.bCanBeStacked && (Amount > _Amount))
		{
			// #. 분할한 아이템을 넣을 빈 슬롯 찾기.
			bool bSuccess;
			int EmptySlotIndex;
			SearchEmpthSlot(&bSuccess, &EmptySlotIndex);

			if (bSuccess)
			{
				// #. 새로운 슬롯 업데이트.
				m_Slots[EmptySlotIndex].ItemClass = m_Slots[_StackIndex].ItemClass;
				m_Slots[EmptySlotIndex].Amount = _Amount;
				UpdateSlotAtIndex(EmptySlotIndex);

				// #. 기존 슬롯 업데이트.
				m_Slots[_StackIndex].Amount -= _Amount;
				UpdateSlotAtIndex(_StackIndex);

				return true;
			}
		}
		
	}

	return false;
}

bool UComponent_Inventory::SplitStackToIndex(int _FromIndex, int _ToIndex, int _Amount)
{
	bool bSuccess;
	FItemInfo FromItemInfo;
	int FromAmount;
	GetItemInfoAtIndex(_FromIndex, &bSuccess, &FromItemInfo, &FromAmount);

	if (IsSlotEmpty(_ToIndex) && 
		!IsSlotEmpty(_FromIndex) && 
		(FromItemInfo.bCanBeStacked) && 
		(FromAmount > 1) && 
		(FromAmount > _Amount))
	{
		m_Slots[_ToIndex].ItemClass = m_Slots[_FromIndex].ItemClass;
		m_Slots[_ToIndex].Amount = _Amount;
		UpdateSlotAtIndex(_ToIndex);

		m_Slots[_FromIndex].Amount -= _Amount;
		UpdateSlotAtIndex(_FromIndex);		
	}

	return false;
}

void UComponent_Inventory::UseItemAtIndex(int _Index)
{
	if (!IsSlotEmpty(_Index))
	{
		AItem_Base* pItem = NewObject<AItem_Base>(this, m_Slots[_Index].ItemClass);
		pItem->InitializeItem(this, _Index);
		pItem->EventOnUsed();
	}
}

void UComponent_Inventory::ShowInventory()
{
	m_bIsVisible = true;
	m_pInventoryWidget->SetIsEnabled(true);
	m_pInventoryWidget->SetVisibility(ESlateVisibility::Visible);
}

void UComponent_Inventory::HideInventory()
{
	m_bIsVisible = false;
	m_pInventoryWidget->SetIsEnabled(false);
	m_pInventoryWidget->SetVisibility(ESlateVisibility::Hidden);

	m_pInventoryWidget->GetActionMenu()->SetVisibility(ESlateVisibility::Hidden);

	m_pMainWidget->GetThrowWidget()->SetVisibility(ESlateVisibility::Hidden);
	m_pInventoryWidget->GetSlotPanel()->SetIsEnabled(true);

	if (m_pPlayer->GetIsShopOpen())
	{
		m_pPlayer->GetNearbyMerchantActors()[0]->GetShopWidget()->GetGridPanel()->SetIsEnabled(true);
		m_pPlayer->GetNearbyMerchantActors()[0]->GetShopWidget()->GetSellWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UComponent_Inventory::ShowObtainedItem(TSubclassOf<AItem_Base> _ItemClass, int _Amount)
{
	if (m_bShowObtainedMessage)
	{
		if (m_pMainWidget)
		{
			m_pMainWidget->AddItemToObtainedQueue(_ItemClass, _Amount);
			return true;
		}
	}
	return false;
}

bool UComponent_Inventory::AddToIndex(int _FromIndex, int _ToIndex)
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

void UComponent_Inventory::SortInvnetory(ESortType _SortType, bool _bReversed)
{
	TArray<FInventorySlot> FillSlots = FilterEmptySlots();

	if (FillSlots.Num() > 1)
	{
		m_Slots = BubbleSortArray(FillSlots, _SortType, _bReversed);
		
	}
	else
	{
		m_Slots = FillSlots;
	}

	m_Slots.SetNum(m_AmountOfSlots);
	for (int i = 0; i < m_Slots.Num(); ++i)
	{
		UpdateSlotAtIndex(i);
	}
}

TArray<FInventorySlot> UComponent_Inventory::FilterEmptySlots()
{
	TArray<FInventorySlot> OutSlots;
	for (int i = 0; i < m_Slots.Num(); ++i)
	{
		if (m_Slots[i].ItemClass)
		{
			int OutSlotsIndex = OutSlots.Emplace(m_Slots[i]);
			HandleSwapForHotkeys(i, OutSlotsIndex);
		}
	}
	return OutSlots;
;
}

TArray<FInventorySlot> UComponent_Inventory::BubbleSortArray(const TArray<FInventorySlot>& _Array, ESortType _Type, bool _bReversed)
{
	TArray<FInventorySlot> SortedArray = _Array;

	for (int i = 0; i < SortedArray.Num(); ++i)
	{
		for(int j = 0; j < SortedArray.Num()-1-i; ++j)
		{
			switch (_Type)
			{
				case ESortType::Category:
				{
					uint8 CategoryIndex0 = (uint8)(SortedArray[j].ItemClass.GetDefaultObject()->GetItemInfo().Category);
					uint8 CategoryIndex1 = (uint8)(SortedArray[j + 1].ItemClass.GetDefaultObject()->GetItemInfo().Category);

					// #. 정렬 하기.
					if (_bReversed ? !(CategoryIndex0 > CategoryIndex1) : (CategoryIndex0 > CategoryIndex1))
					{
						HandleSwapForHotkeys(j, j+1);
						FInventorySlot TempSlot = SortedArray[j + 1];
						SortedArray[j + 1] = SortedArray[j];
						SortedArray[j] = TempSlot;
					}
					break;
				}
				case ESortType::Amount:
				{
					int AmountIndex1 = SortedArray[j].Amount;
					int AmountIndex2 = SortedArray[j + 1].Amount;

					// #. Index0의 수량 > Index1의 수량.
					bool bIndex1IsBigger = (AmountIndex1 > AmountIndex2) || 
						            ((AmountIndex1 == AmountIndex2) && SortedArray[j].ItemClass.GetDefaultObject()->GetItemInfo().bCanBeStacked);

					// #. 정렬 하기.
					if (_bReversed ? !bIndex1IsBigger : bIndex1IsBigger)
					{
						HandleSwapForHotkeys(j, j+1);
						FInventorySlot TempSlot = SortedArray[j + 1];
						SortedArray[j + 1] = SortedArray[j];
						SortedArray[j] = TempSlot;
					}
					break;
				}
				case ESortType::Name:
				{
					bool bIndex1IsBigger, bEqual;
					FString NameIndex0 = (SortedArray[j].ItemClass.GetDefaultObject()->GetItemInfo().Name).ToString();
					FString NameIndex1 = (SortedArray[j + 1].ItemClass.GetDefaultObject()->GetItemInfo().Name).ToString();

					CompareStrings(NameIndex0, NameIndex1, &bIndex1IsBigger, &bEqual);

					if (!bEqual)
					{
						// #. 정렬 하기.
						if (_bReversed ? !bIndex1IsBigger : bIndex1IsBigger)
						{
							HandleSwapForHotkeys(j, j+1);
							FInventorySlot TempSlot = SortedArray[j + 1];
							SortedArray[j + 1] = SortedArray[j];
							SortedArray[j] = TempSlot;
						}
					}

					break;
				}
				default:
				{
					break;
				}
			}
		}		
	}

	return SortedArray;
}

void UComponent_Inventory::CompareStrings(FString _String1, FString _String2, bool* out_p1Before2, bool* out_pEqual)
{
	if (_String1 == _String2)
	{
		*out_p1Before2 = false;
		*out_pEqual = true;
		return;
	}
	else
	{
		// #. 문자열을 문자들의 집합으로 변경.
		TArray<FString> Characters1 = UKismetStringLibrary::GetCharacterArrayFromString(_String1);
		TArray<FString> Characters2 = UKismetStringLibrary::GetCharacterArrayFromString(_String2);

		bool bChar1IsShorter = (Characters1.Num() <= Characters2.Num());			

		int Count = (bChar1IsShorter ? Characters1.Num() : Characters2.Num());

		for (int i = 0; i < Count; ++i)
		{
			int32 Num1 = UKismetStringLibrary::GetCharacterAsNumber(Characters1[i], 0);
			int32 Num2 = UKismetStringLibrary::GetCharacterAsNumber(Characters2[i], 0);

			if (Num1 > Num2)
			{
				*out_p1Before2 = true;
				*out_pEqual = false;
				return;
			}
			else if (Num1 < Num2)
			{
				*out_p1Before2 = false;
				*out_pEqual = false;
				return;
			}
		}		

		*out_p1Before2 = bChar1IsShorter;
		*out_pEqual = false;
		return;
	}
}

void UComponent_Inventory::UpdateWeight()
{
	FNumberFormattingOptions FormatOptions;
	FormatOptions.RoundingMode = ERoundingMode::HalfToZero;
	FormatOptions.MinimumIntegralDigits = 1;   // #. 정수 부분 최소 자릿수.
	FormatOptions.MaximumIntegralDigits = 324; // #. 정수 부분 최대 자릿수.
	FormatOptions.MinimumFractionalDigits = 1; // #. 소숫점 부분 최소 자릿수.
	FormatOptions.MaximumFractionalDigits = 1; // #. 소숫점 부분 최대 자릿수.

	FText CurrentWeightText = FText::AsNumber(m_CurrentWeight, &FormatOptions);
	FText OverloadedCapText = FText::AsNumber(m_OverloadedCap, &FormatOptions);

	FText Format = FText::Format(LOCTEXT("Format", "{0}/{1}"), CurrentWeightText, OverloadedCapText);
	m_pInventoryWidget->GetWeightText()->SetText(Format);

	if (m_CurrentWeight > m_OverloadedCap)
	{
		m_pInventoryWidget->GetWeightText()->SetColorAndOpacity(FLinearColor(FColor(0xFFE33A10)));
	}
	else
	{
		m_pInventoryWidget->GetWeightText()->SetColorAndOpacity(FLinearColor::White);
	}

}

void UComponent_Inventory::AddWeight(float _Amount)
{
	m_CurrentWeight += _Amount;
	UpdateWeight();

	// #. 무게가 넘쳤을 경우.
	if (!m_bOverloaded && (m_CurrentWeight > m_OverloadedCap))
	{
		m_bOverloaded = true;
		m_pPlayer->OnOverloaded();
	}
}

void UComponent_Inventory::RemoveWeight(float _Amount)
{
	m_CurrentWeight -= _Amount;
	UpdateWeight();

	// #. 무게가 넘쳤을 경우.
	if (m_bOverloaded && (m_CurrentWeight <= m_OverloadedCap))
	{
		m_bOverloaded = false;
		m_pPlayer->OnOverloadedEnd();
	}
}

void UComponent_Inventory::AddWeightForItem(TSubclassOf<AItem_Base> _ItemClass, int _Amount)
{
	float TotalWeight = _ItemClass.GetDefaultObject()->GetItemInfo().Weight * _Amount;

	AddWeight(TotalWeight);
}

void UComponent_Inventory::RemoveWeightForItem(TSubclassOf<class AItem_Base> _ItemClass, int _Amount)
{

	float TotalWeight = _ItemClass.GetDefaultObject()->GetItemInfo().Weight * _Amount;

	RemoveWeight(TotalWeight);
}

void UComponent_Inventory::GetTotalAmountOfItem(TSubclassOf<AItem_Base> _Item, int& out_TotalAmount, TArray<int>& out_SlotIndices)
{
	out_TotalAmount = 0;

	for (int i = 0; i < m_Slots.Num(); ++i)
	{
		if (m_Slots[i].ItemClass)
		{
			if (m_Slots[i].ItemClass == _Item)
			{				
				out_TotalAmount += m_Slots[i].Amount;
				out_SlotIndices.Add(i);
				
			}
		}
	}

}

bool UComponent_Inventory::RemoveItem(TSubclassOf<AItem_Base> _Item, int _Amount)
{


	int TotalAmount;
	TArray<int> SlotIndices;
	GetTotalAmountOfItem(_Item, TotalAmount, SlotIndices);

	if (TotalAmount >= _Amount)
	{
		int AmountToRemove = _Amount;

		for (int i = 0; i < SlotIndices.Num(); ++i)
		{
			int Amount = GetAmountAtIndex(SlotIndices[i]);
			
			if (Amount < AmountToRemove)
			{
				AmountToRemove -= Amount;
				RemoveItemAtIndex(SlotIndices[i], Amount);

			}
			else
			{
				RemoveItemAtIndex(SlotIndices[i], AmountToRemove);
				AmountToRemove = 0;
				return true;
			}
		}
	}
	return false;
}

void UComponent_Inventory::AddLootedPickups(int _ID, int _Rest)
{
	FSavedPickup SavedPickup;
	SavedPickup.ID = _ID;
	SavedPickup.Rest = _Rest;

	m_LootedPickups.Emplace(SavedPickup);
}

void UComponent_Inventory::Save()
{
	_SaveInvnetory();
}

void UComponent_Inventory::LoadHotkeys()
{
	if (!m_pSaveGameObject)
	{
		m_pSaveGameObject = Cast<UInventorySave>(UGameplayStatics::LoadGameFromSlot(TEXT("InventorySave"), 0));
	}

	if (m_pSaveGameObject)
	{
		for (int i = 0; i < m_pMainWidget->GetHotkeyWidgets().Num(); ++i)
		{
			bool bEmpty = m_pSaveGameObject->m_SavedHotkeys[i].bEmpty;
			m_pMainWidget->GetHotkeyWidgets()[i]->GetHotkeySlotWidget()->SetEmpty(bEmpty);

			int Index = m_pSaveGameObject->m_SavedHotkeys[i].InventoryIndex;
			m_pMainWidget->GetHotkeyWidgets()[i]->GetHotkeySlotWidget()->SetInventorySlotIndex(Index);

			m_pMainWidget->GetHotkeyWidgets()[i]->GetHotkeySlotWidget()->Update();
		}
		m_pSaveGameObject->m_SavedHotkeys.Empty();

	}
}

bool UComponent_Inventory::IncreaseAmountAtIndex(int _Index, int _Amount)
{
	if (!IsSlotEmpty(_Index) && 
		((GetAmountAtIndex(_Index) + _Amount) <= m_MaxStackSize))
	{
		m_Slots[_Index].Amount += _Amount;

		AddWeightForItem(m_Slots[_Index].ItemClass, _Amount);
		UpdateSlotAtIndex(_Index);

		return true;
	}
	return false;
}

bool UComponent_Inventory::MoveFromStorageToInvIndex(UComponent_Storage * _pStorage, int _StorageIndex, int _InventoryIndex)
{
	int AmountToAdd = 0;

	if (IsSlotEmpty(_InventoryIndex))
	{
		AmountToAdd = _pStorage->GetAmountAtIndex(_StorageIndex);
		bool bSuccess = AddItemToIndex(_InventoryIndex, _pStorage->GetSlots()[_StorageIndex].ItemClass, AmountToAdd);

		if (bSuccess)
		{
			_pStorage->RemoveItemAtIndex(_StorageIndex, AmountToAdd);
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
		GetItemInfoAtIndex(_InventoryIndex, nullptr, &ItemInfo, nullptr);

		if ((_pStorage->GetSlots()[_StorageIndex].ItemClass == m_Slots[_InventoryIndex].ItemClass) &&
			ItemInfo.bCanBeStacked &&
			(GetAmountAtIndex(_InventoryIndex) < m_MaxStackSize))
		{
			if (_pStorage->GetAmountAtIndex(_StorageIndex) > (m_MaxStackSize - GetAmountAtIndex(_InventoryIndex)))
			{
				AmountToAdd = m_MaxStackSize - GetAmountAtIndex(_InventoryIndex);
			}
			else
			{
				AmountToAdd = _pStorage->GetAmountAtIndex(_StorageIndex);
			}

			bool bSuccess = IncreaseAmountAtIndex(_InventoryIndex, AmountToAdd);
			if (bSuccess)
			{
				_pStorage->RemoveItemAtIndex(_StorageIndex, AmountToAdd);
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

void UComponent_Inventory::AddInventorySlots(int _Amount)
{
	m_AmountOfSlots += _Amount;

	m_Slots.SetNum(m_AmountOfSlots);

	HideInventory();
	m_pInventoryWidget->GenerateSlotWidgets();
	ShowInventory();
}

void UComponent_Inventory::UpdateHotkeyForIndex(int _Index)
{
	if (m_bShowHotkeys)
	{
		TArray<UWidget_Hotkey*> pHotkeyWidgets = m_pMainWidget->GetHotkeyWidgets();
		for (int i = 0; i < pHotkeyWidgets.Num(); ++i)
		{
			bool bHotkeySlotEmpty = pHotkeyWidgets[i]->GetHotkeySlotWidget()->GetEmpty();
			int InventorySlotIndex = pHotkeyWidgets[i]->GetHotkeySlotWidget()->GetInventorySlotIndex();
			if ((!bHotkeySlotEmpty) && (InventorySlotIndex == _Index))
			{
				pHotkeyWidgets[i]->GetHotkeySlotWidget()->Update();
			}
		}
	}
}

void UComponent_Inventory::HandleHotkeyPress(int _HotkeyIndex)
{
	UseItemAtIndex(GetMainWidget()->GetHotkeyWidgets()[_HotkeyIndex]->GetHotkeySlotWidget()->GetInventorySlotIndex());	
}

void UComponent_Inventory::HandleSwapForHotkeys(int _Index1, int _Index2)
{
	if (m_bShowHotkeys)
	{
		const TArray<UWidget_Hotkey*>& pHotkeyWidgets = m_pMainWidget->GetHotkeyWidgets();
		for (auto& Iter : pHotkeyWidgets)
		{
			if (!Iter->GetHotkeySlotWidget()->GetEmpty())
			{
				if (Iter->GetHotkeySlotWidget()->GetInventorySlotIndex() == _Index1)
				{
					Iter->GetHotkeySlotWidget()->SetInventorySlotIndex(_Index2);
				}
				else if (Iter->GetHotkeySlotWidget()->GetInventorySlotIndex() == _Index2)
				{
					Iter->GetHotkeySlotWidget()->SetInventorySlotIndex(_Index1);
				}
				return;
			}			
		}
	}
}

bool UComponent_Inventory::_AddItem(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, int* out_pRest)
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
				_AddItem(_ItemClass, RemainAmount, out_pRest);
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
					_AddItem(_ItemClass, RemainAmount, out_pRest);
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
			if (_Amount >= 1)
			{
				_AddItem(_ItemClass, _Amount, out_pRest);
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

void UComponent_Inventory::_UpdateCraftingMenu()
{
	TSubclassOf<AItem_Base> ItemClass = m_pMainWidget->GetCraftingMenuWidget()->GetCurrentItemClass();
	if (ItemClass)
	{
		m_pMainWidget->GetCraftingMenuWidget()->UpdateDetailWindow(ItemClass);
	}
}

void UComponent_Inventory::_SaveInvnetory()
{
	if (!m_pSaveGameObject)
	{
		m_pSaveGameObject = Cast<UInventorySave>(UGameplayStatics::CreateSaveGameObject(UInventorySave::StaticClass()));
	}
	m_pSaveGameObject->m_SavedWeight = m_CurrentWeight;
	m_pSaveGameObject->m_SavedSlots = m_Slots;
	m_pSaveGameObject->m_SavedPickups = m_LootedPickups;
	m_pSaveGameObject->m_SavedAmountOfSlots = m_AmountOfSlots;

	for (auto& Iter : m_pMainWidget->GetHotkeyWidgets())
	{
		bool bEmpty = Iter->GetHotkeySlotWidget()->GetEmpty();
		int Index = Iter->GetHotkeySlotWidget()->GetInventorySlotIndex();
		FSavedHotkey SavedHotkey(bEmpty, Index);
	
		m_pSaveGameObject->m_SavedHotkeys.Add(SavedHotkey);
	}

	UGameplayStatics::SaveGameToSlot(m_pSaveGameObject, TEXT("InventorySave"), 0);
}

bool UComponent_Inventory::_ContainsID(int _ID, int* out_pRest)
{
	for (FSavedPickup& Iter : m_LootedPickups)
	{
		if (Iter.ID == _ID)
		{				
			*out_pRest = Iter.Rest;
			return true;
		}
	}
	return false;
}

void UComponent_Inventory::_LoadPickupActors()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, APickupActor::StaticClass(), Actors);

	for (auto& Iter : Actors)
	{
		APickupActor* pPickupActor = Cast<APickupActor>(Iter);
		if (pPickupActor)
		{
			int Rest;
			bool bSuccess = _ContainsID(pPickupActor->GetID(), &Rest);
			if(bSuccess)
			{
				if (Rest > 0)
				{
					pPickupActor->SetAmount(Rest);
					pPickupActor->UpdateText();
				}
				else
				{
					pPickupActor->Destroy();
				}
			}
		}
	}
}

void UComponent_Inventory::_LoadInventory()
{
	if (!m_pSaveGameObject)
	{
		m_pSaveGameObject = Cast<UInventorySave>(UGameplayStatics::LoadGameFromSlot(TEXT("InventorySave"), 0));
	}

	if (m_pSaveGameObject)
	{
		m_CurrentWeight = m_pSaveGameObject->m_SavedWeight;
		m_Slots = m_pSaveGameObject->m_SavedSlots;
		m_LootedPickups = m_pSaveGameObject->m_SavedPickups;
		m_AmountOfSlots = m_pSaveGameObject->m_SavedAmountOfSlots;

		_LoadPickupActors();
		
	}
}

#undef LOCTEXT_NAMESPACE

