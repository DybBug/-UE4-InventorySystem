// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_Detail.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Detail : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UImage* m_pIcon;
	UPROPERTY()
	class UTextBlock* m_pName;
	UPROPERTY()
	class UTextBlock* m_pCategory;
	UPROPERTY()
	class UTextBlock* m_pDescription;
	UPROPERTY()
	class UTextBlock* m_pWeight;

	FItemInfo m_ItemInfo;
	int m_Amount;

public :
	void InitializeWidget(const FItemInfo& _ItemInfo, const int& _Amount);
	void Update();

	/* Get */
	FORCEINLINE class UImage*     const& GetIcon()        const { return m_pIcon; }
	FORCEINLINE class UTextBlock* const& GetName()        const { return m_pName; }
	FORCEINLINE class UTextBlock* const& GetCategory()    const { return m_pCategory; }
	FORCEINLINE class UTextBlock* const& GetDescription() const { return m_pDescription; }
	FORCEINLINE class UTextBlock* const& GetWeight()      const { return m_pWeight; }

};
