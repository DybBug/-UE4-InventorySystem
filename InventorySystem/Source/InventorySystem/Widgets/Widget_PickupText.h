// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PickupText.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_PickupText : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UTextBlock* m_pNameText;
	
public :
	FORCEINLINE class UTextBlock* const& GetNameText() const { return m_pNameText; }
};
