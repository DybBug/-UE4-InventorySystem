// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemClasses/Item_Base.h"
#include "Item_Weapon.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AItem_Weapon : public AItem_Base
{
	GENERATED_BODY()
	
protected :
	UPROPERTY(EditAnywhere, Category = "Configuration")
	class USkeletalMesh* m_pWeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FName m_SocketName;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	FTransform m_RelativeTransform;

public :
	virtual void EventOnUsed() override;

	/* Get */
	FORCEINLINE class USkeletalMesh* const& GetWeaponMesh() const { return m_pWeaponMesh; }
 
	FORCEINLINE const FName&      GetSocketName()        const { return m_SocketName; }
	FORCEINLINE const FTransform& GetRelativeTransform() const { return m_RelativeTransform; }
};
