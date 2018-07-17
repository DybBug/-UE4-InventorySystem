// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupActor.h"
#include "InventorySystemCharacter.h"
#include "ItemClasses/Item_Base.h"
#include "Components/Component_Inventory.h"
#include "Widgets/Widget_PickupText.h"

#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/WidgetComponent.h>
#include <Components/TextBlock.h>
#include <UObject/ConstructorHelpers.h>

#define LOCTEXT_NAMESPACE "Format"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UMaterial> M_Default(TEXT("Material'/Game/InventorySystem/Material/M_Pickup.M_Pickup'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> M_Hovered(TEXT("Material'/Game/InventorySystem/Material/M_HoveredPickup.M_HoveredPickup'"));

	m_pDefaultMaterial = M_Default.Object;
	m_pHoveredMaterial = M_Hovered.Object;

	m_pSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	m_pSphere->SetSphereRadius(160.f);
	m_pSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::_OnComponentBeginOverlap);
	m_pSphere->OnComponentEndOverlap.AddDynamic(this, &APickupActor::_OnComponentEndOverlap);
	RootComponent = m_pSphere;

	m_pStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));	
	m_pStaticMesh->OnBeginCursorOver.AddDynamic(this, &APickupActor::_OnHovered);
	m_pStaticMesh->OnEndCursorOver.AddDynamic(this, &APickupActor::_OnUnhovered);
	m_pStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_pStaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	m_pStaticMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UUserWidget> WB_PickupText(TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_PickupText.WB_PickupText_C'"));
	if (WB_PickupText.Succeeded())
	{
		m_pPickupText = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupText"));
		m_pPickupText->SetWidgetClass(WB_PickupText.Class);
		m_pPickupText->SetWidgetSpace(EWidgetSpace::Screen);
		m_pPickupText->SetDrawSize(FVector2D(500.f, 200.f));
		m_pPickupText->SetVisibility(false);
		m_pPickupText->SetupAttachment(RootComponent);
	}
}

void APickupActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (m_ItemToAdd)
	{
		FItemInfo ItemInfo = m_ItemToAdd.GetDefaultObject()->GetItemInfo();

		m_pStaticMesh->SetStaticMesh(ItemInfo.pStaticMesh);
		m_pOriginalMaterial = m_pStaticMesh->GetMaterial(0);

		m_pStaticMesh->SetRelativeScale3D(ItemInfo.MeshScale);

		m_pSphere->SetSphereRadius(ItemInfo.InteractRadius);
	}
}

void APickupActor::UpdateText()
{
	UWidget_PickupText* pPickupTextWidget = Cast<UWidget_PickupText>(m_pPickupText->GetUserWidgetObject());
	if (pPickupTextWidget)
	{
		FName ItemName = m_ItemToAdd.GetDefaultObject()->GetItemInfo().Name;
		FText Format = FText::Format(LOCTEXT("Format", "{0} ({1})"), FText::FromName(ItemName), m_Amount);
		pPickupTextWidget->GetNameText()->SetText(Format);
	}
}

void APickupActor::OnPickedUp(AActor* _pPlayer)
{
	AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pPlayer);
	if (IsValid(pPlayer))
	{
		bool bSuccess;
		int Reset;
		bSuccess = pPlayer->GetInventory()->AddItem(m_ItemToAdd, m_Amount, true, &Reset);

		if (bSuccess)
		{
			if (Reset > 0)
			{
				m_Amount = Reset;
				UpdateText();

				pPlayer->GetInventory()->AddLootedPickups(m_ID, m_Amount);
			}
			else
			{

				pPlayer->GetInventory()->AddLootedPickups(m_ID, 0);
				pPlayer->RemoveNearbyActor(this);

				if (pPlayer->GetNearbyPickupActors().Num() > 0)
				{
					pPlayer->GetNearbyPickupActors()[0]->GetPickupText()->SetVisibility(true);
				}

				if (m_bHovered)
				{
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->CurrentMouseCursor = EMouseCursor::Default;
				}

				Destroy();
				
				
			}
		}
	}
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateText();
}

void APickupActor::_OnComponentBeginOverlap(UPrimitiveComponent* _pOverlappedComponent, AActor* _pOtherActor, UPrimitiveComponent* _pOtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult& _SweepResult)
{
	AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pOtherActor);
	if (IsValid(pPlayer))
	{
		if (m_bHasPickupCommand)
		{
			OnPickedUp(pPlayer);
		}
		else
		{
			if (pPlayer->GetNearbyPickupActors().Num() > 0)
			{
				pPlayer->AddNearbyActor(this);
			}
			else
			{
				m_pPickupText->SetVisibility(true);
				pPlayer->AddNearbyActor(this);
				m_bIsInRange = true;
			}
		}
	}
}

void APickupActor::_OnComponentEndOverlap(UPrimitiveComponent* _pOverlappedComponent, AActor* _pOtherActor, UPrimitiveComponent* _pOtherComp, int32 _OtherBodyIndex)
{
	AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pOtherActor);
	if (IsValid(pPlayer))
	{
		m_bIsInRange = false;

		if ((pPlayer->GetNearbyPickupActors().Num() > 0) && 
			(pPlayer->GetNearbyPickupActors()[0] == this))
		{
			m_pPickupText->SetVisibility(false);
			pPlayer->RemoveNearbyActor(this);

			if (pPlayer->GetNearbyPickupActors().Num() > 0)
			{
				pPlayer->GetNearbyPickupActors()[0]->GetPickupText()->SetVisibility(true);
			}
		}
		else
		{
			pPlayer->RemoveNearbyActor(this);
		}

	}
}

void APickupActor::_OnHovered(UPrimitiveComponent* TouchedComponent )
{
	m_bHovered = true;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->CurrentMouseCursor = EMouseCursor::Hand;
	m_pStaticMesh->SetMaterial(0, m_pHoveredMaterial);
	m_pPickupText->SetVisibility(true);
}

void APickupActor::_OnUnhovered(UPrimitiveComponent* TouchedComponent )
{
	m_bHovered = false;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->CurrentMouseCursor = EMouseCursor::Default;
	//m_pStaticMesh->SetMaterial(0, m_pDefaultMaterial);
	m_pStaticMesh->SetMaterial(0, m_pOriginalMaterial);

	m_pPickupText->SetVisibility(m_bIsInRange);
}

#undef LOCTEXT_NAMESPACE


