// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "InventorySystemCharacter.h"
#include "InventorySystemPlayerController.h"
#include "Components/Component_Inventory.h"
#include "ItemClasses/Item_Weapon.h"
#include "ItemClasses/PickupActor.h"
#include "ItemClasses/Chest.h"
#include "NPCs/NPC_Merchant.h"
#include "Widgets/Widget_Main.h"
#include "Widgets/Widget_CraftingMenu.h"
#include "Widgets/Widget_Inventory.h"
#include "Widgets/Widget_Shop.h"
#include "Widgets/Widget_OfferedItem.h"
#include "Widgets/Widget_Equipment.h"
#include "Widgets/Widget_Hotkey.h"
#include "SaveGames/ISCSave.h"

#include <UObject/ConstructorHelpers.h>
#include <Camera/CameraComponent.h>
#include <Components/DecalComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/SpringArmComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Materials/Material.h>
#include <Engine/World.h>

AInventorySystemCharacter::AInventorySystemCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	m_pCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_pCameraBoom->SetupAttachment(RootComponent);
	m_pCameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	m_pCameraBoom->TargetArmLength = 800.f;
	m_pCameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	m_pCameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	m_pTopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	m_pTopDownCameraComponent->SetupAttachment(m_pCameraBoom, USpringArmComponent::SocketName);
	m_pTopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	m_pCursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	m_pCursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		m_pCursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	m_pCursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	m_pCursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// InventoryComponent...
	m_pInventory = CreateDefaultSubobject<UComponent_Inventory>(TEXT("Inventory"));

	// EquippedMesh...
	m_pEquippedMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquippedMesh"));
	m_pEquippedMesh->SetupAttachment(GetMesh());

	// SlowEffect...
	m_pSlowEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SlowEffect"));
	m_pSlowEffect->Deactivate();
	m_pSlowEffect->SetupAttachment(GetMesh());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AInventorySystemCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (m_pCursorToWorld != nullptr)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			m_pCursorToWorld->SetWorldLocation(TraceHitResult.Location);
			m_pCursorToWorld->SetWorldRotation(CursorR);
		}
	}
}

void AInventorySystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindKey(EKeys::AnyKey, IE_Pressed, this, &AInventorySystemCharacter::_AnyKey);
	PlayerInputComponent->BindKey(EKeys::C, IE_Pressed, this, &AInventorySystemCharacter::_CKey);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &AInventorySystemCharacter::_DKey);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AInventorySystemCharacter::_EKey);
	PlayerInputComponent->BindKey(EKeys::I, IE_Pressed, this, &AInventorySystemCharacter::_IKey);
	PlayerInputComponent->BindKey(EKeys::N, IE_Pressed, this, &AInventorySystemCharacter::_NKey);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &AInventorySystemCharacter::_SKey);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AInventorySystemCharacter::_LeftShiftPressed);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AInventorySystemCharacter::_LeftShiftReleased);
	PlayerInputComponent->BindKey(EKeys::Equals, IE_Released, this, &AInventorySystemCharacter::_IncreaseGold);

}

void AInventorySystemCharacter::UpdateHealthBar()
{
	m_pInventory->GetMainWidget()->GetHealthBar()->SetPercent((float)m_CurrentHealth / (float)m_MaxHealth);
}

void AInventorySystemCharacter::IncreaseHealth(int _Amount)
{
	m_CurrentHealth += _Amount;
	m_CurrentHealth = FMath::Clamp<int>(m_CurrentHealth, 0, m_MaxHealth);

	UpdateHealthBar();
}

void AInventorySystemCharacter::DecreaseHealth(int _Amount)
{
}

bool AInventorySystemCharacter::EquipItem(AItem_Weapon* _pItem, bool _bLoad)
{
	if (_bLoad)
	{
		m_pEquippedWeapon = _pItem;

		m_pEquippedMesh->SetRelativeTransform(m_pEquippedWeapon->GetRelativeTransform());
		m_pEquippedMesh->SetSkeletalMesh(m_pEquippedWeapon->GetWeaponMesh());
		m_pEquippedMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), m_pEquippedWeapon->GetSocketName());

		m_pInventory->GetMainWidget()->GetEquipmentWidget()->UpdateEquipment();
		return true;
	}
	else
	{
		if (m_pEquippedWeapon)
		{
			bool bSuccess = UnequipItem(false, 0);
			if (bSuccess)
			{
				return EquipItem(_pItem);
			}
		}
		else
		{
			bool bSuccess = m_pInventory->RemoveItemAtIndex(_pItem->GetIndex(), 1);

			if (bSuccess)
			{
				m_pEquippedWeapon = _pItem;

				m_pEquippedMesh->SetRelativeTransform(m_pEquippedWeapon->GetRelativeTransform());
				m_pEquippedMesh->SetSkeletalMesh(m_pEquippedWeapon->GetWeaponMesh());
				m_pEquippedMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), m_pEquippedWeapon->GetSocketName());

				m_pInventory->GetMainWidget()->GetEquipmentWidget()->UpdateEquipment();
				return true;
			}
		}
	}
	return false;
}

bool AInventorySystemCharacter::UnequipItem(bool _ToSpecificIndex, int _Index)
{
	if (m_pEquippedWeapon)
	{
		if (_ToSpecificIndex)
		{
			bool bSuccess = m_pInventory->AddItemToIndex(_Index, m_pEquippedWeapon->GetClass(), 1);
			if (bSuccess)
			{
				m_pEquippedMesh->SetSkeletalMesh(nullptr);
				m_pEquippedWeapon->Destroy();
				m_pEquippedWeapon = nullptr;

				m_pInventory->GetMainWidget()->GetEquipmentWidget()->UpdateEquipment();
				return true;
			}
		}
		else
		{
			int Result;
			bool bSuccess = m_pInventory->AddItem(m_pEquippedWeapon->GetClass(), 1, false, &Result);
			if (bSuccess)
			{
				m_pEquippedMesh->SetSkeletalMesh(nullptr);
				m_pEquippedWeapon->Destroy();
				m_pEquippedWeapon = nullptr;

				m_pInventory->GetMainWidget()->GetEquipmentWidget()->UpdateEquipment();
				return true;
			}
		}
	}
	return false;
}

void AInventorySystemCharacter::OnOverloaded()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	m_pSlowEffect->Activate(false);
}

void AInventorySystemCharacter::OnOverloadedEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	m_pSlowEffect->Deactivate();
}

void AInventorySystemCharacter::UpdateGold()
{
	m_pInventory->GetInventoryWidget()->GetGoldText()->SetText(FText::AsNumber(m_CurrentGold));

	if (m_bIsShopOpen)
	{
		m_NearbyMerchantActors[0]->GetShopWidget()->UpdateGold();
		for (auto& Iter : m_NearbyMerchantActors[0]->GetShopWidget()->GetOfferedItemWidgets())
		{
			Iter->UpdatePrice();
		}

	}
}

void AInventorySystemCharacter::IncreaseGold(int _Amount)
{
	m_CurrentGold += _Amount;

	m_CurrentGold = FMath::Clamp<int>(m_CurrentGold, 0, m_MaxGold);

	UpdateGold();
}

void AInventorySystemCharacter::DecreaseGold(int _Amount)
{
	m_CurrentGold -= _Amount;

	m_CurrentGold = FMath::Clamp<int>(m_CurrentGold, 0, m_MaxGold);

	UpdateGold();
}

void AInventorySystemCharacter::AddNearbyActor(AActor* _pActor)
{
	if (APickupActor* pPickupActor = Cast<APickupActor>(_pActor))
	{
		m_NearbyPickupActors.Add(pPickupActor);
	}
	else if (ANPC_Merchant* pMerchant = Cast<ANPC_Merchant>(_pActor))

	{
		m_NearbyMerchantActors.Add(pMerchant);
	}
	else if (AChest* pChest = Cast<AChest>(_pActor))
	{
		m_NearbyChestActors.Add(pChest);
	}
}

void AInventorySystemCharacter::RemoveNearbyActor(AActor* _pActor)
{
	if (APickupActor* pPickupActor = Cast<APickupActor>(_pActor))
	{
		m_NearbyPickupActors.Remove(pPickupActor);
	}
	else if (ANPC_Merchant* pMerchant = Cast<ANPC_Merchant>(_pActor))

	{
		m_NearbyMerchantActors.Remove(pMerchant);
	}
	else if (AChest* pChest = Cast<AChest>(_pActor))
	{
		m_NearbyChestActors.Remove(pChest);
	}
}

void AInventorySystemCharacter::LoadWeapon()
{
	if (UGameplayStatics::DoesSaveGameExist("ISCSave", 0))
	{
		if (!m_pSaveGameObject)
		{
			UISCSave* SaveObj = Cast<UISCSave>(UGameplayStatics::LoadGameFromSlot("ISCSave", 0));
			m_pSaveGameObject = SaveObj;			
		}

		if (m_pSaveGameObject->m_SavedEquippedWeapon)
		{
			AItem_Weapon* pWeapon = NewObject<AItem_Weapon>(this, m_pSaveGameObject->m_SavedEquippedWeapon);
			EquipItem(pWeapon, true);
		}
	}
}


void AInventorySystemCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UGameplayStatics::DoesSaveGameExist(TEXT("ISCSave"), 0))
	{
		_LoadGame();
	}

	m_pInventory->PlayStart();
}

void AInventorySystemCharacter::_AnyKey()
{
	APlayerController* pController = Cast<APlayerController>(GetController());

	if (m_pInventory->GetMainWidget()->GetShowHotkeys())
	{
		const TArray<UWidget_Hotkey*>& pHotkeyWidgets = m_pInventory->GetMainWidget()->GetHotkeyWidgets();
		int Index = 0;
		for (auto& Iter : pHotkeyWidgets)
		{
			if (pController->WasInputKeyJustPressed(Iter->GetKey()))
			{
				m_pInventory->HandleHotkeyPress(Index);
				return;
			}
			++Index;
		}	
	}
}

void AInventorySystemCharacter::_CKey()
{
	bool bVisible = m_pInventory->GetMainWidget()->GetCraftingMenuWidget()->IsVisible();

	ESlateVisibility Visibility = bVisible ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	m_pInventory->GetMainWidget()->GetCraftingMenuWidget()->SetVisibility(Visibility);
}

void AInventorySystemCharacter::_DKey()
{
	if (m_pInventory->GetMainWidget()->GetEquipmentWidget()->IsVisible())
	{
		m_pInventory->GetMainWidget()->GetEquipmentWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		m_pInventory->GetMainWidget()->GetEquipmentWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

}

void AInventorySystemCharacter::_EKey()
{
	if (m_NearbyPickupActors.Num() > 0)
	{
		m_NearbyPickupActors[0]->OnPickedUp(this);
	}
	else if (m_NearbyMerchantActors.Num() > 0)
	{
		m_NearbyMerchantActors[0]->OnInteract(this);
	}
	else if (m_NearbyChestActors.Num() > 0)
	{
		m_NearbyChestActors[0]->OnInteract(this);
	}
}

void AInventorySystemCharacter::_IKey()
{
	m_pInventory->GetIsVisible() ? m_pInventory->HideInventory() : m_pInventory->ShowInventory();
}

void AInventorySystemCharacter::_NKey()
{
	m_pInventory->AddInventorySlots(5);
}

void AInventorySystemCharacter::_SKey()
{
	_SaveGame();

	if (m_pInventory)
	{
		m_pInventory->Save();
	}
}

void AInventorySystemCharacter::_LeftShiftPressed()
{
	m_bIsShiftDown = true;
}

void AInventorySystemCharacter::_LeftShiftReleased()
{
	m_bIsShiftDown = false;
}

void AInventorySystemCharacter::_IncreaseGold()
{
	IncreaseGold(75);
}

void AInventorySystemCharacter::_SaveGame()
{
	if (!m_pSaveGameObject)
	{
		m_pSaveGameObject = Cast<UISCSave>(UGameplayStatics::CreateSaveGameObject(UISCSave::StaticClass()));
	}

	m_pSaveGameObject->m_SavedGold = m_CurrentGold;
	m_pSaveGameObject->m_SavedHP = m_CurrentHealth;
	if (m_pEquippedWeapon)
	{
		m_pSaveGameObject->m_SavedEquippedWeapon = m_pEquippedWeapon->GetClass();
	}
	else
	{
		m_pSaveGameObject->m_SavedEquippedWeapon = nullptr;
	}

	UGameplayStatics::SaveGameToSlot(m_pSaveGameObject, "ISCSave", 0);
}

void AInventorySystemCharacter::_LoadGame()
{
	if (!m_pSaveGameObject)
	{
		m_pSaveGameObject = Cast<UISCSave>(UGameplayStatics::LoadGameFromSlot("ISCSave", 0));
	}

	if (m_pSaveGameObject)
	{
		m_CurrentGold = m_pSaveGameObject->m_SavedGold;
		m_CurrentHealth = m_pSaveGameObject->m_SavedHP;
	}
}
