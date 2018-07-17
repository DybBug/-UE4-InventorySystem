// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_DraggedItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_DraggedItem : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UImage* m_pIcon;
	UPROPERTY()
	class UTextBlock* m_pAmountText;

	FItemInfo m_ItemInfo;
	int m_Amount;

public :
	void InitializeWidget(FItemInfo _ItemInfo, int _Amount);

	/* Get */
	FORCEINLINE class UImage*     const& GetIcon()       const { return m_pIcon; }
	FORCEINLINE class UTextBlock* const& GetAmountText() const { return m_pAmountText; }

	FORCEINLINE const FItemInfo& GetItemInfo() const { return m_ItemInfo; }
	FORCEINLINE const int&       GetAmount()   const { return m_Amount; }
	
};
