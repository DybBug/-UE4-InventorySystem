// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventorySystemStructure.h"
#include "Item_Base.generated.h"

UCLASS()
class INVENTORYSYSTEM_API AItem_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem_Base();

protected :
	UPROPERTY(EditAnywhere, Category = "Configuration|Info")
	FItemInfo m_ItemInfo;

	class UComponent_Inventory* m_pInventory;
	int m_Index;

public :	
	void InitializeItem(class UComponent_Inventory* _pInventory, int _Index);
	virtual void EventOnUsed();

	/* Get */
	FORCEINLINE const FItemInfo& GetItemInfo() const { return m_ItemInfo; }
	FORCEINLINE const int&       GetIndex()    const { return m_Index; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
