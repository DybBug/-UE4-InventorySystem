// Fill out your copyright notice in the Description page of Project Settings.

#include "Chest.h"
#include "Components/Component_Storage.h"
#include "Components/Component_Inventory.h"
#include "InventorySystemCharacter.h"
#include "Widgets/Widget_Storage.h"
#include "Widgets/Widget_Main.h"
#include "InventorySystemCharacter.h"

#include <Components/StaticMeshComponent.h>
#include <Components/WidgetComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <UObject/ConstructorHelpers.h>

// Sets default values
AChest::AChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	m_pSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	m_pSphere->OnComponentBeginOverlap.AddDynamic(this, &AChest::_OnComponentBeginOverlap);
	m_pSphere->OnComponentEndOverlap.AddDynamic(this, &AChest::_OnComponentEndOverlap);
	RootComponent = m_pSphere;

	m_pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_pMesh->SetupAttachment(RootComponent);


	static ConstructorHelpers::FClassFinder<UUserWidget> WB_ChestText(TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_ChestText.WB_ChestText_C'"));
	if (WB_ChestText.Succeeded())
	{
		m_pChestText = CreateDefaultSubobject<UWidgetComponent>(TEXT("ChestText"));
		m_pChestText->SetWidgetClass(WB_ChestText.Class);
		m_pChestText->SetWidgetSpace(EWidgetSpace::Screen);
		m_pChestText->SetDrawSize((FVector2D(500.f, 200.f)));
		m_pChestText->SetVisibility(false);
		m_pChestText->SetupAttachment(RootComponent);
	}

	m_pStorage = CreateDefaultSubobject<UComponent_Storage>(TEXT("Storage"));

}

void AChest::OnStorageClose()
{
	m_pStorage->HideStorage();
	m_pPlayer->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (m_pPlayer->GetInventory()->GetIsVisible())
	{
		m_pPlayer->GetInventory()->HideInventory();
		m_pPlayer->GetInventory()->ShowInventory();
	}

	m_bIsInteract = false;
}

void AChest::OnInteract(class AInventorySystemCharacter* _pPlayer)
{
	m_pPlayer = _pPlayer;
	if (m_pStorage && !m_bIsInteract)
	{
		m_pStorage->SetStorageWidget(m_pPlayer->GetInventory()->GetMainWidget()->GetStorageWidget());
		m_pStorage->GetStorageWidget()->GenerateSlotWidgets(m_pStorage);
		m_pStorage->ShowStorage();

		m_pPlayer->GetInventory()->HideInventory();
		m_pPlayer->GetInventory()->ShowInventory();

		m_pPlayer->GetCharacterMovement()->DisableMovement();
		m_pPlayer->GetCharacterMovement()->StopMovementImmediately();

		m_bIsInteract = true;
	}
}

// Called when the game starts or when spawned
void AChest::BeginPlay()
{
	Super::BeginPlay();
	
	if (!m_bGenerateStartingItems)
	{
		_GenerateStartingItems();
	}

	m_pStorage->StartPlay();	
}

void AChest::_OnComponentBeginOverlap(
	UPrimitiveComponent* _pOverlappedComponent, 
	AActor* _pOtherActor, 
	UPrimitiveComponent* _pOtherComp, 
	int32 _OtherBodyIndex, 
	bool _bFromSweep,
	const FHitResult & _SweepResult)
{
	if (AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pOtherActor))
	{
		m_pChestText->SetVisibility(true);
		pPlayer->AddNearbyActor(this);
	}

}

void AChest::_OnComponentEndOverlap(
	UPrimitiveComponent* _pOverlappedComponent, 
	AActor* _pOtherActor, 
	UPrimitiveComponent* _pOtherComp, 
	int32 _OtherBodyIndex)
{
	if (AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pOtherActor))
	{
		m_pChestText->SetVisibility(false);
		pPlayer->RemoveNearbyActor(this);
	}
}

void AChest::_GenerateStartingItems()
{
	UDataTable* DT_ChestStartItem = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/InventorySystem/DataTable/DT_ChestStartItems.DT_ChestStartItems'"));

	TArray<FName> RowNames = DT_ChestStartItem->GetRowNames();
	for (int i = 0; i < RowNames.Num(); ++i)
	{
		FChestContent Data = *(DT_ChestStartItem->FindRow<FChestContent>(RowNames[i], RowNames[i].ToString()));
		if (Data.ItemClass)
		{
			if (UKismetMathLibrary::RandomIntegerInRange(1, 100) <= Data.Chance)
			{
				int Amount = UKismetMathLibrary::RandomIntegerInRange(Data.MinAmount, Data.MaxAmount);
				m_StartingItems.Emplace(FInventorySlot(Data.ItemClass, Amount));
			}
		}
	}
	
}

