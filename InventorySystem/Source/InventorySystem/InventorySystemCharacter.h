// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventorySystemCharacter.generated.h"

UCLASS(Blueprintable)
class AInventorySystemCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AInventorySystemCharacter();

	
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* m_pTopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* m_pCameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UDecalComponent* m_pCursorToWorld;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	class UComponent_Inventory* m_pInventory;

	UPROPERTY(EditAnywhere, Category = "Configuration|CharacterInfo")
	int m_MaxHealth = 100;

	UPROPERTY(EditAnywhere, Category = "Configuration|CharacterInfo")
	int m_CurrentHealth = 0;

	UPROPERTY(EditAnywhere, Category = "Configuration|CharacterInfo")
	int m_MaxGold = 9999;

	UPROPERTY(EditAnywhere, Category = "Configuration|CharacterInfo")
	int m_CurrentGold = 0;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|CharacterInfo")
	bool m_bIsShopOpen;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|NearbyActors")
	TArray<class APickupActor*> m_NearbyPickupActors;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|NearbyActors")
	TArray<class ANPC_Merchant*> m_NearbyMerchantActors;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|NearbyActors")
	TArray<class AChest*> m_NearbyChestActors;

	bool m_bIsShiftDown;

	class AItem_Weapon* m_pEquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Components")
	class USkeletalMeshComponent* m_pEquippedMesh;

	UPROPERTY(VisibleAnywhere, Category = "Configuration|Components")
	class UParticleSystemComponent* m_pSlowEffect;

	class UISCSave* m_pSaveGameObject;

public :
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// #. 체력.
	void UpdateHealthBar();
	void IncreaseHealth(int _Amount);
	void DecreaseHealth(int _Amount);

	// #. 장비.
	bool EquipItem(class AItem_Weapon* _pItem, bool _bLoad = false);
	bool UnequipItem(bool _ToSpecificIndex, int _Index);

	// #. 무게.
	void OnOverloaded();
	void OnOverloadedEnd();

	// #. 골드.
	void UpdateGold();
	void IncreaseGold(int _Amount);
	void DecreaseGold(int _Amount);

	// #. 근처 액터.
	void AddNearbyActor(class AActor* _pActor);
	void RemoveNearbyActor(class AActor* _pActor);
	

	void LoadWeapon();


	/* Get */
	FORCEINLINE class UCameraComponent*     const& GetTopDownCameraComponent() const { return m_pTopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent*  const& GetCameraBoom()             const { return m_pCameraBoom; }
	FORCEINLINE class UDecalComponent*      const& GetCursorToWorld()          const { return m_pCursorToWorld; }
	FORCEINLINE class UComponent_Inventory* const& GetInventory()              const { return m_pInventory; }
	FORCEINLINE class AItem_Weapon*         const& GetEquippedWeapon()         const { return m_pEquippedWeapon; }

	FORCEINLINE const int&  GetMaxHealth()     const { return m_MaxHealth; }
	FORCEINLINE const int&  GetCurrentHealth() const { return m_CurrentHealth; }
	FORCEINLINE const int&  GetCurrentGold()   const { return m_CurrentGold; }
	FORCEINLINE const bool& GetIsShiftDown()   const { return m_bIsShiftDown; }
	FORCEINLINE const bool& GetIsShopOpen()    const { return m_bIsShopOpen; }

	FORCEINLINE const TArray<class APickupActor*>& GetNearbyPickupActors()    const { return m_NearbyPickupActors; }
	FORCEINLINE const TArray<class ANPC_Merchant*>& GetNearbyMerchantActors() const { return m_NearbyMerchantActors; }
	FORCEINLINE const TArray<class AChest*>&        GetNearbyChestActors()    const { return m_NearbyChestActors; }


	/* Set */
	FORCEINLINE void SetIsShopOpen(bool _bOpen)                      { m_bIsShopOpen = _bOpen; }
	FORCEINLINE void SetEquippedWeapon(class AItem_Weapon* _pWeapon) { m_pEquippedWeapon = _pWeapon; }


protected :
	virtual void BeginPlay() override;

	UFUNCTION()
	void _AnyKey();

	UFUNCTION()
	void _CKey();

	UFUNCTION()
	void _DKey();

	UFUNCTION()
	void _EKey();

	UFUNCTION()
	void _IKey();

	UFUNCTION()
	void _NKey();

	UFUNCTION()
	void _SKey();

	UFUNCTION()
	void _LeftShiftPressed();

	UFUNCTION()
	void _LeftShiftReleased();

	UFUNCTION()
	void _IncreaseGold();

	void _SaveGame();
	void _LoadGame();
};

