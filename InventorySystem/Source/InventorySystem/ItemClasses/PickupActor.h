// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

UCLASS()
class INVENTORYSYSTEM_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

protected :
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class USphereComponent* m_pSphere;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* m_pStaticMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UWidgetComponent* m_pPickupText;


	UPROPERTY(EditAnywhere, Category = "Configuration|Data")
	int m_ID;

	UPROPERTY(EditAnywhere, Category = "Configuration|Data")
	TSubclassOf<class AItem_Base> m_ItemToAdd;

	UPROPERTY(EditAnywhere, Category = "Configuration|Data")
	int m_Amount;

	UPROPERTY(EditAnywhere, Category = "Configuration|Material")
	class UMaterial* m_pDefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Configuration|Material")
	class UMaterial* m_pHoveredMaterial;

	bool m_bHovered;
	bool m_bIsInRange;
	bool m_bHasPickupCommand;

	UMaterialInterface* m_pOriginalMaterial;

public :
	virtual void OnConstruction(const FTransform& Transform) override;

	void UpdateText();
	void OnPickedUp(class AActor* _pPlayer);


	/* Get */
	FORCEINLINE class USphereComponent* const& GetSphere() const { return m_pSphere; }

	FORCEINLINE class UWidgetComponent* const& GetPickupText() const { return m_pPickupText; }

	FORCEINLINE const int& GetID()     const { return m_ID; }
	FORCEINLINE const int& GetAmount() const { return m_Amount; }

	/* Set */
	FORCEINLINE void SetAmount(int _Amount)           { m_Amount = _Amount; }
	FORCEINLINE void SetHasPickupCommand(bool _bBool) { m_bHasPickupCommand = _bBool; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected :
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

	UFUNCTION()
	void _OnHovered(UPrimitiveComponent* pTouchedComponent);

	UFUNCTION()
	void _OnUnhovered(UPrimitiveComponent* pTouchedComponent);


};
