// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_OldMap.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_OldMap : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UButton* m_pCloseButton;

	class AItem_Map* m_pOldMap;
	
public :
	void InitializeWidget(AItem_Map* _pOldMap);

	/* Get */
	FORCEINLINE class UButton*      const& GetCloseButton() const { return m_pCloseButton; }
	FORCEINLINE class AItem_Map* const& GetOldMap()      const { return m_pOldMap; }

protected :
	UFUNCTION()
	void _OnCloseButtonClicked();
};
