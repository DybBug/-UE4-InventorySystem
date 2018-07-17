// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemClasses/Item_Base.h"
#include "Item_Map.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API AItem_Map : public AItem_Base
{
	GENERATED_BODY()
	
public :
	AItem_Map();

protected :
	TSubclassOf<class UWidget_OldMap> m_OldMapWidgetClass;
	class UWidget_OldMap* m_pOldMapWidget;

public :
	virtual void EventOnUsed() override;

	void OnRead();
	
};
