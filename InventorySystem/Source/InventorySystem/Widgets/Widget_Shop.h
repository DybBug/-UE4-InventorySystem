// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Shop.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Shop : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UButton* m_pCloseButton;
	UPROPERTY()
	class UTextBlock* m_pGoldText;
	UPROPERTY()
	class UUniformGridPanel* m_pGridPanel;
	UPROPERTY()
	class UWidget_Sell* m_pSellWidget;

	class ANPC_Merchant* m_pMerchant;
	class AInventorySystemCharacter* m_pPlayer;
	TArray<class UWidget_OfferedItem*> m_OfferedItemWidgets;


public :
	void InitializeWidget(class ANPC_Merchant* _pMerchant, class AInventorySystemCharacter* _pPlayer);
	void UpdateGold();


	/* Get */
	FORCEINLINE class UButton*           const& GetCloseButton() const { return m_pCloseButton; }
	FORCEINLINE class UTextBlock*        const& GetGoldText()    const { return m_pGoldText; }
	FORCEINLINE class UUniformGridPanel* const& GetGridPanel()   const { return m_pGridPanel; }
	FORCEINLINE class UWidget_Sell*      const& GetSellWidget()  const { return m_pSellWidget; }

	FORCEINLINE const TArray<class UWidget_OfferedItem*>& GetOfferedItemWidgets() const { return m_OfferedItemWidgets; }

	
protected :
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void _GenerateItemList();

	UFUNCTION()
	void _OnCloseButtonClicked();
};
