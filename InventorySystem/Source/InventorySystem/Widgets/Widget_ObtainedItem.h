// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_ObtainedItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_ObtainedItem : public UUserWidget
{
	GENERATED_BODY()
	
protected :
	UPROPERTY()
	class UBorder* m_pObtained;
	UPROPERTY()
	class UTextBlock* m_pAmount;
	UPROPERTY()
	class UTextBlock* m_pName;
	UPROPERTY()
	class UImage* m_pIcon;

	TSubclassOf<class AItem_Base> m_ItemClass;
	int m_Amount;
	float m_VisibleDuration;
	class UWidget_Main* m_pMainWidget;

	class UWidgetAnimation* m_pBlendOut;
	class UWidgetAnimation* m_pBlendIn;

public :
	void InitializeWidget(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, float _VisibleDuration, class UWidget_Main* _pMainWidget);

	/* Get */
	FORCEINLINE class UBorder*    const& GetObtained() const { return m_pObtained; }
	FORCEINLINE class UTextBlock* const& GetAmount()   const { return  m_pAmount; }
	FORCEINLINE class UTextBlock* const& GetName()     const { return  m_pName; }
	FORCEINLINE class UImage*     const& GetIcon()     const { return  m_pIcon; }
	
protected :
	virtual void NativeConstruct() override;

	void _SetupWidgetAnimations();

	UFUNCTION()
	void _Visible();

	UFUNCTION()
	void _BlendOut();

	UFUNCTION()
	void _Hidden();
};
