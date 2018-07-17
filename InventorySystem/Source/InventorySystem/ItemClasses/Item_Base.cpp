// Fill out your copyright notice in the Description page of Project Settings.

#include "Item_Base.h"
#include "Components/Component_Inventory.h"

//#include <Engine.h>


// Sets default values
AItem_Base::AItem_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AItem_Base::InitializeItem(UComponent_Inventory* _pInventory, int _Index)
{
	m_pInventory = _pInventory;
	m_Index = _Index;
}

void AItem_Base::EventOnUsed()
{
	m_pInventory->RemoveItemAtIndex(m_Index, 1);

	Destroy();
}

// Called when the game starts or when spawned
void AItem_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

