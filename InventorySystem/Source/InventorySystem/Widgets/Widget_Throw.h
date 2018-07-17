// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_Throw.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Throw : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UImage* m_pIcon;
	UPROPERTY()
	class UTextBlock* m_pNameText;
	UPROPERTY()
	class UTextBlock* m_pAmountText;
	UPROPERTY()
	class UButton* m_pMinusButton;
	UPROPERTY()
	class UButton* m_pPlusButton;
	UPROPERTY()
	class UButton* m_pConfirmButton;
	UPROPERTY()
	class UButton* m_pCancelButton;

	int m_MaxAmount;
	int m_ThrowAmount;
	int m_SlotIndex;
	int m_ClickCount;
	class UComponent_Inventory* m_pInventory;

	FTimerHandle m_hMinusTimer;
	FTimerHandle m_hPlusTimer;

public :
	void InitializeWidget(class UComponent_Inventory* _pInventory);
	virtual void Update(int _NewIndex);

	/* Get */
	FORCEINLINE class UImage*     const& GetIcon()          const { return m_pIcon; }
	FORCEINLINE class UTextBlock* const& GetNameText()      const { return m_pNameText; }
	FORCEINLINE class UTextBlock* const& GetAmountText()    const { return m_pAmountText; }
	FORCEINLINE class UButton*    const& GetMinusButton()   const { return m_pMinusButton; }
	FORCEINLINE class UButton*    const& GetPlusButton()    const { return m_pPlusButton; }
	FORCEINLINE class UButton*    const& GetConfirmButton() const { return m_pConfirmButton; }
	FORCEINLINE class UButton*    const& GetCancelButton()  const { return m_pCancelButton; }
	
protected :
	virtual void _IncreaseAmount();
	virtual void _DecreaseAmount();

	UFUNCTION(BlueprintCallable)
	void _ClickCountReset();


	UFUNCTION()
	virtual void _OnMinusButtonClicked();
	UFUNCTION()
	virtual void _OnMinusButtonPressed();
	UFUNCTION()
	virtual void _OnMinusButtonReleased();
	UFUNCTION()
	virtual void _OnPlusButtonClicked();
	UFUNCTION()
	virtual void _OnPlusButtonPressed();
	UFUNCTION()
	virtual void _OnPlusButtonReleased();
	UFUNCTION()
	virtual void _OnConfirmButtonClicked();
	UFUNCTION()
	virtual void _OnCancelButtonClicked();
	
};
