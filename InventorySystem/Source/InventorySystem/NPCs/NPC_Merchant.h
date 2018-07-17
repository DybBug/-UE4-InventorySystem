// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC_Merchant.generated.h"

UCLASS()
class INVENTORYSYSTEM_API ANPC_Merchant : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_Merchant();

protected :
	UPROPERTY(VisibleAnywhere, Category = "Configuration|Components")
	class UWidgetComponent* m_pMerchantText;
	UPROPERTY(VisibleAnywhere, Category = "Configuration|Components")
	class USphereComponent* m_pSphere;

	// #. 판매 아이템
	UPROPERTY(EditAnywhere, Category = "Configuration|Shop")
	TArray<TSubclassOf<class AItem_Base>> m_OfferItemClasses;

	class AInventorySystemCharacter* m_pPlayer;
	class UWidget_Shop* m_pShopWidget;

public :
	void OnInteract(class AInventorySystemCharacter* _pPlayer);
	void OnShopClose();

	/* Get */
	FORCEINLINE class UWidget_Shop* const& GetShopWidget() const { return m_pShopWidget; }

	FORCEINLINE const TArray<TSubclassOf<class AItem_Base>>& GetOfferItemClasses() const { return m_OfferItemClasses; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	
};
