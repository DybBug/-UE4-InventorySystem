// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC_Merchant.h"
#include "InventorySystemCharacter.h"
#include "Widgets/Widget_Shop.h"
#include "Components/Component_Inventory.h"

#include <UObject/ConstructorHelpers.h>
#include <Components/WidgetComponent.h>
#include <Components/SphereComponent.h>

// Sets default values
ANPC_Merchant::ANPC_Merchant()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> WB_MerchantText(TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_MerchantText.WB_MerchantText_C'"));
	if (WB_MerchantText.Succeeded())
	{
		m_pMerchantText = CreateDefaultSubobject<UWidgetComponent>(TEXT("MerchantText"));
		m_pMerchantText->SetWidgetClass(WB_MerchantText.Class);
		m_pMerchantText->SetWidgetSpace(EWidgetSpace::Screen);
		m_pMerchantText->SetDrawSize(FVector2D(500.f, 200.f));
		m_pMerchantText->SetVisibility(false);
		m_pMerchantText->SetupAttachment(RootComponent);
	}

	m_pSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	m_pSphere->OnComponentBeginOverlap.AddDynamic(this, &ANPC_Merchant::_OnComponentBeginOverlap);
	m_pSphere->OnComponentEndOverlap.AddDynamic(this, &ANPC_Merchant::_OnComponentEndOverlap);
	m_pSphere->SetSphereRadius(1200.f);
	m_pSphere->SetupAttachment(RootComponent);
}

void ANPC_Merchant::OnInteract(AInventorySystemCharacter * _pPlayer)
{
	m_pPlayer = _pPlayer;

	if (!m_pPlayer->GetIsShopOpen())
	{
		UClass* WB_Shop = LoadClass<UWidget_Shop>(nullptr, TEXT("WidgetBlueprint'/Game/InventorySystem/Widgets/WB_Shop.WB_Shop_C'"));
		m_pShopWidget = CreateWidget<UWidget_Shop>(GetWorld(), WB_Shop);
		m_pShopWidget->InitializeWidget(this, m_pPlayer);
		m_pShopWidget->SetPositionInViewport(FVector2D(0.f, 0.f), false);
		m_pShopWidget->SetAnchorsInViewport(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
		m_pShopWidget->SetAlignmentInViewport((FVector2D(0.5f, 0.5f)));
		m_pShopWidget->AddToViewport();

		m_pPlayer->GetInventory()->HideInventory();
		m_pPlayer->GetInventory()->ShowInventory();

		m_pPlayer->SetIsShopOpen(true);
		m_pPlayer->GetCharacterMovement()->DisableMovement();
		m_pPlayer->GetCharacterMovement()->StopMovementImmediately();
	}
}

void ANPC_Merchant::OnShopClose()
{
	m_pPlayer->SetIsShopOpen(false);
	m_pPlayer->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	if (m_pPlayer->GetInventory()->GetIsVisible())
	{
		m_pPlayer->GetInventory()->HideInventory();
		m_pPlayer->GetInventory()->ShowInventory();	
	}

	m_pShopWidget->RemoveFromParent();
	m_pShopWidget = nullptr;
}

// Called when the game starts or when spawned
void ANPC_Merchant::BeginPlay()
{
	Super::BeginPlay();	
}

void ANPC_Merchant::_OnComponentBeginOverlap(UPrimitiveComponent * _pOverlappedComponent, AActor * _pOtherActor, UPrimitiveComponent * _pOtherComp, int32 _OtherBodyIndex, bool _bFromSweep, const FHitResult & _SweepResult)
{
	AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pOtherActor);
	if (pPlayer)
	{
		pPlayer->AddNearbyActor(this);
		m_pMerchantText->SetVisibility(true);
	}
}

void ANPC_Merchant::_OnComponentEndOverlap(UPrimitiveComponent * _pOverlappedComponent, AActor * _pOtherActor, UPrimitiveComponent * _pOtherComp, int32 _OtherBodyIndex)
{
	AInventorySystemCharacter* pPlayer = Cast<AInventorySystemCharacter>(_pOtherActor);
	if (pPlayer)
	{
		pPlayer->RemoveNearbyActor(this);
		m_pMerchantText->SetVisibility(false);
	}
}

