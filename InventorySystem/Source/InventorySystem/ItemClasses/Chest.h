// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventorySystemStructure.h"
#include "Chest.generated.h"

UCLASS()
class INVENTORYSYSTEM_API AChest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChest();

protected :
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* m_pMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USphereComponent* m_pSphere;
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UWidgetComponent* m_pChestText;
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UComponent_Storage* m_pStorage;

	class AInventorySystemCharacter* m_pPlayer;

	bool m_bIsInteract;

	TArray<FInventorySlot> m_StartingItems;

	bool m_bGenerateStartingItems;

public :
	void OnStorageClose();
	void OnInteract(class AInventorySystemCharacter* _pPlayer);

	/* Get */
	FORCEINLINE const TArray<FInventorySlot>& GetStartingItems() const { return m_StartingItems; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:
	UFUNCTION()
	void _OnComponentBeginOverlap(
		UPrimitiveComponent* _pOverlappedComponent,
		AActor* _pOtherActor,
		UPrimitiveComponent* _pOtherComp,
		int32 _OtherBodyIndex,
		bool _bFromSweep,
		const FHitResult& _SweepResult);

	UFUNCTION()
	void _OnComponentEndOverlap(
		UPrimitiveComponent* _pOverlappedComponent,
		AActor* _pOtherActor,
		UPrimitiveComponent* _pOtherComp,
		int32 _OtherBodyIndex);

	void _GenerateStartingItems();

};
