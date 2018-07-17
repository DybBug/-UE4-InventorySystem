// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_Throw.h"
#include "InventorySystemStructure.h"

#include "Widget_Sell.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Sell : public UWidget_Throw
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UTextBlock* m_pPriceText;

	FItemInfo m_ItemInfo;
	class UWidget_Shop* m_pShopWidget;

public:
	void InitializeWidget(class UWidget_Shop* _pShopWidget, class UComponent_Inventory* _pInventory);
	virtual void Update(int _NewIndex) override;

	void UpdatePrice();

	/* Get */
	FORCEINLINE class UTextBlock* const& GetPriceText() const { return m_pPriceText; }


protected:
	virtual void _IncreaseAmount() override;
	virtual void _DecreaseAmount() override;

	UFUNCTION()
	virtual void _OnMinusButtonClicked() override;
	UFUNCTION()
	virtual void _OnPlusButtonClicked() override;
	UFUNCTION()
	virtual void _OnConfirmButtonClicked() override;
	UFUNCTION()
	virtual void _OnCancelButtonClicked() override;
};

