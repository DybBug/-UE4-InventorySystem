// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystemStructure.h"
#include "Component_Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UComponent_Inventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Inventory();

protected :
	UPROPERTY(VisibleAnywhere, Category = "Configuration|Inventory")
	TArray<FInventorySlot> m_Slots;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Inventory")
	bool m_bIsVisible;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory")
	int m_AmountOfSlots = 20;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory")
	int m_MaxStackSize = 99;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory")
	int m_SlotsPerRow = 4;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory")
	bool m_bShowObtainedMessage;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory")
	float m_ObtainedVisibleDuration;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory|Weight")
	float m_OverloadedCap;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Inventory|Weight")
	float m_CurrentWeight;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Inventory|Weight")
	bool m_bOverloaded;

	UPROPERTY(EditAnywhere, Category = "Configuration|Inventory|Craft")
	TArray<TSubclassOf<AItem_Base>> m_UnlockedCraftableItenms;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Widgets")
	class UWidget_Main* m_pMainWidget;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Widgets")
	class UWidget_Inventory* m_pInventoryWidget;
	
	class AInventorySystemCharacter* m_pPlayer;

	TArray<FSavedPickup> m_LootedPickups;

	class UInventorySave* m_pSaveGameObject;

	bool m_bStorageOpen;

	TArray<FKey> m_Hotkeys;
	bool m_bShowHotkeys;

public :
	void PlayStart();

	bool IsSlotEmpty(int _Index);

	void GetItemInfoAtIndex(
		const int& _Index, 
		bool*      out_pEmpthSlot = nullptr, 
		FItemInfo* out_pItemInfo = nullptr, 
		int*       out_pAmount = nullptr);

	void SearchEmpthSlot(bool* out_pSuccess = nullptr, int* out_pIndex = nullptr);
	void SearchFreeStack(const TSubclassOf<class AItem_Base>& _ItemClass, bool* out_pSuccess = nullptr, int* out_pIndex = nullptr);
	bool AddItem(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, bool _bShowNotification, int* out_pRest);
	bool AddItemToIndex(int _Index, TSubclassOf<AItem_Base> _ItemClass, int _Amount);
	int GetAmountAtIndex(int _Index);
	void UpdateSlotAtIndex(int _Index);
	bool RemoveItemAtIndex(int _Index, int _Amount);
	bool SwapSlots(int _Index1, int _Index2);

	// #. 비어있는 첫 슬롯에 아이템을 나눔.
	// Param1. 나눌 아이템슬롯 인덱스.
	// Param2. 나눌 수량.
	bool SplitStack(int _StackIndex, int _Amount); 

	// #. 원하는 슬롯의 위치에 아이템을 나눔.
	// Param1. 나눌 아이템슬롯 인덱스.
	// Param2. 나누어진 아이템이 위치할 슬롯의 인덱스.
	// Param3. 나눌 수량.
	bool SplitStackToIndex(int _FromIndex, int _ToIndex, int _Amount);

	void UseItemAtIndex(int _Index);

	void ShowInventory();
	void HideInventory();

	// #. 아이템 합치기.
	bool AddToIndex(int _FromIndex, int _ToIndex);

	bool ShowObtainedItem(TSubclassOf<AItem_Base> _ItemClass, int _Amount);

	// #. 아이템 정렬.
	void SortInvnetory(ESortType _SortType, bool _bReversed);

	TArray<FInventorySlot> FilterEmptySlots();

	TArray<FInventorySlot> BubbleSortArray(const TArray<FInventorySlot>& _Array, ESortType _Type, bool _bReversed);
	void CompareStrings(FString _String1, FString _String2, bool* out_p1Before2, bool* out_pEqual);

	void UpdateWeight();
	void AddWeight(float _Amount);
	void RemoveWeight(float _Amount);
	void AddWeightForItem(TSubclassOf<class AItem_Base> _ItemClass, int _Amount);
	void RemoveWeightForItem(TSubclassOf<class AItem_Base> _ItemClass, int _Amount);

	void GetTotalAmountOfItem(TSubclassOf<AItem_Base> _Item, int& out_TotalAmount, TArray<int>& out_SlotIndices);
	bool RemoveItem(TSubclassOf<AItem_Base> _Item, int _Amount);

	// #. 저장할 아이템 입력.
	void AddLootedPickups(int _ID, int _Rest);

	// 저장.
	void Save();

	void LoadHotkeys();

	bool IncreaseAmountAtIndex(int _Index, int _Amount);
	bool MoveFromStorageToInvIndex(class UComponent_Storage* _pStorage, int _StorageIndex, int _InventoryIndex);

	void AddInventorySlots(int _Amount);

	void UpdateHotkeyForIndex(int _Index);
	void HandleHotkeyPress(int _HotkeyIndex);
	void HandleSwapForHotkeys(int _Index1, int _Index2);

	/* Get */
	FORCEINLINE class UWidget_Main*              const& GetMainWidget()      const { return m_pMainWidget; }
	FORCEINLINE class UWidget_Inventory*         const& GetInventoryWidget() const { return m_pInventoryWidget; }
	FORCEINLINE class AInventorySystemCharacter* const& GetPlayer()          const { return m_pPlayer; }

	FORCEINLINE const TArray<FInventorySlot>&          GetSlots()                  const { return m_Slots; }
	FORCEINLINE const TArray<TSubclassOf<AItem_Base>>& GetUnlockedCraftableItems() const { return m_UnlockedCraftableItenms; }

	FORCEINLINE const bool& GetIsVisible() const { return m_bIsVisible; }

	/* Set */
	FORCEINLINE void SetStorageOpen(bool _bOpen) { m_bStorageOpen = _bOpen; }

protected:

	bool _AddItem(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, int* out_pRest);
	void _UpdateCraftingMenu();

	void _SaveInvnetory();
	bool _ContainsID(int _ID, int* out_pRest);
	void _LoadPickupActors();
	void _LoadInventory();

	
};
