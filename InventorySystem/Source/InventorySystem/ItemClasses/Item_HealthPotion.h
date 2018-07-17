// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemClasses/Item_Base.h"
#include "Item_HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AItem_HealthPotion : public AItem_Base
{
	GENERATED_BODY()
	
public :
	virtual void EventOnUsed();
	
	
};
