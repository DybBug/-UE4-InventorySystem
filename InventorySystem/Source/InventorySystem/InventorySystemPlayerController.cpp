// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "InventorySystemPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "InventorySystemCharacter.h"
#include "ItemClasses/PickupActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

AInventorySystemPlayerController::AInventorySystemPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	bEnableMouseOverEvents = true;
}

void AInventorySystemPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AInventorySystemPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AInventorySystemPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AInventorySystemPlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("PickupCommand", IE_Pressed, this, &AInventorySystemPlayerController::OnPickupCommand);


	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AInventorySystemPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AInventorySystemPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AInventorySystemPlayerController::OnResetVR);
}

void AInventorySystemPlayerController::OnResetVR()
{
	
}

void AInventorySystemPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void AInventorySystemPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AInventorySystemPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys && (Distance > 120.0f))
		{
			if (m_pPickupActor)
			{
				m_pPickupActor->SetHasPickupCommand(false);
				m_pPickupActor = nullptr;
			}

			NavSys->SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AInventorySystemPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AInventorySystemPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void AInventorySystemPlayerController::OnPickupCommand()
{	
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
	if (APickupActor* pPickupActor = Cast<APickupActor>(HitResult.GetActor()))
	{
		if (m_pPickupActor)
		{
			if (m_pPickupActor == pPickupActor)
			{
				if (m_pPickupActor->GetDistanceTo(GetPawn()) <= UKismetMathLibrary::Max(300.f , pPickupActor->GetSphere()->GetUnscaledSphereRadius()))
				{
					m_pPickupActor->OnPickedUp(GetPawn());
				}
				else
				{
					MoveToMouseCursor();
				}
			}
			else
			{
				m_pPickupActor->SetHasPickupCommand(false);
				m_pPickupActor = pPickupActor;

				m_pPickupActor->SetHasPickupCommand(true);
				if (m_pPickupActor->GetDistanceTo(GetPawn()) <= UKismetMathLibrary::Max(300.f , pPickupActor->GetSphere()->GetUnscaledSphereRadius()))
				{
					m_pPickupActor->OnPickedUp(GetPawn());
				}
				else
				{
					MoveToMouseCursor();
				}

			}
		}
		else
		{
			m_pPickupActor = pPickupActor;
			
			m_pPickupActor->SetHasPickupCommand(true);
			if (m_pPickupActor->GetDistanceTo(GetPawn()) <= 300.f)
			{
				m_pPickupActor->OnPickedUp(GetPawn());
			}
			else
			{
				MoveToMouseCursor();
			}
		}
	}
	
}
