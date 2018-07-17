// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystemStructure.h"
#include "Component_Storage.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEM_API UComponent_Storage : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UComponent_Storage();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Configuration|Storage")
	TArray<FInventorySlot> m_Slots;

	UPROPERTY(EditAnywhere, Category = "Configuration|Storage")
	int m_AmountOfSlots = 20;

	UPROPERTY(EditAnywhere, Category = "Configuration|Storage")
	int m_SlotsPerRow = 4;

	UPROPERTY(EditAnywhere, Category = "Configuration|Storage")
	int m_MaxStackSize = 99;

	class AInventorySystemCharacter* m_pPlayer;

	class AChest* m_pChest;
	class UWidget_Storage* m_pStorageWidget;

	TArray<FInventorySlot> m_StartItems;

public:
	void StartPlay();
	bool IsSlotEmpty(int _Index);

	void GetItemInfoAtIndex(
		const int& _Index,
		bool*      out_pEmpthSlot = nullptr,
		FItemInfo* out_pItemInfo = nullptr,
		int*       out_pAmount = nullptr);

	void SearchEmpthSlot(bool* out_pSuccess = nullptr, int* out_pIndex = nullptr);
	void SearchFreeStack(const TSubclassOf<class AItem_Base>& _ItemClass, bool* out_pSuccess = nullptr, int* out_pIndex = nullptr);
	bool AddItem(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, int* out_pRest);
	bool AddItemToIndex(int _Index, TSubclassOf<AItem_Base> _ItemClass, int _Amount);
	int GetAmountAtIndex(int _Index);
	void UpdateSlotAtIndex(int _Index);
	bool RemoveItemAtIndex(int _Index, int _Amount);
	bool SwapSlots(int _Index1, int _Index2);

	// #. 아이템 합치기.
	bool AddToIndex(int _FromIndex, int _ToIndex);	

	bool IncreaseAmountAtIndex(int _Index, int _Amount);

	void ShowStorage();
	void HideStorage();
	bool MoveFromInvToStorageIndex(class UComponent_Inventory* _pInventory, int _InventoryIndex, int _StorageIndex);

	/* Get */
	FORCEINLINE class AInventorySystemCharacter* const& GetPlayer()        const { return m_pPlayer; }
	FORCEINLINE class AChest*                    const& GetChest()         const { return m_pChest; }
	FORCEINLINE class UWidget_Storage*           const& GetStorageWidget() const { return m_pStorageWidget; }

	FORCEINLINE const TArray<FInventorySlot>& GetSlots() const { return m_Slots; }
	FORCEINLINE const int& GetSlotsPerRow() const { return m_SlotsPerRow; }

	FORCEINLINE void SetStorageWidget(class UWidget_Storage* _pWidget) { m_pStorageWidget = _pWidget; }
	
};
