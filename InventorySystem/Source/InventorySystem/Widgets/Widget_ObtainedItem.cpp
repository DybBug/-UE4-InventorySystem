// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_ObtainedItem.h"
#include "Widget_Main.h"
#include "ItemClasses/Item_Base.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/Border.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Animation/WidgetAnimation.h>
#include <MovieScene.h>

#define LOCTEXT_NAMESPACE "Format"

void UWidget_ObtainedItem::InitializeWidget(const TSubclassOf<class AItem_Base>& _ItemClass, int _Amount, float _VisibleDuration, class UWidget_Main* _pMainWidget)
{
	_SetupWidgetAnimations();

	m_ItemClass = _ItemClass;
	m_Amount = _Amount;
	m_VisibleDuration = _VisibleDuration;
	m_pMainWidget = _pMainWidget;

	m_pIcon->SetBrushFromTexture(m_ItemClass.GetDefaultObject()->GetItemInfo().pIcon);
	m_pName->SetText(FText::FromName(m_ItemClass.GetDefaultObject()->GetItemInfo().Name));

	FText Format = FText::Format(LOCTEXT("Format", "x{0}"), m_Amount);
	m_pAmount->SetText(Format);

	// #. 항목에 따라 색상 변경.
	FLinearColor Color;
	switch (m_ItemClass.GetDefaultObject()->GetItemInfo().Category)
	{
		case EItemCategories::Consumeable:
		{
			Color = FColor(0xFFFFFFFF);
			break;
		}
		case EItemCategories::Equipment :
		{
			Color = FColor(0xFFDB9A00);
			break;
		}
		case EItemCategories::QuestItems :
		{
			Color = FColor(0xFFFFFF00);
			break;
		}
		case EItemCategories::Readables :
		{
			Color = FColor(0xFF00B9FF);
			break;
		}
		case EItemCategories::Weapon :
		{
			Color = FColor(0xFFFF6800);
			break;
		}
		case EItemCategories::Material :
		{
			Color = FColor(0xFF7AE50F);
			break;
		}
		default :
		{
			Color = FColor(0xFF000000);
			break;
		}
	}
	m_pObtained->SetBrushColor(Color);
	m_pName->SetColorAndOpacity(Color);
	m_pAmount->SetColorAndOpacity(Color);
}

void UWidget_ObtainedItem::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(m_pBlendIn);
	float DelayTime = m_pBlendIn->GetEndTime();

	FLatentActionInfo Latent;
	Latent.CallbackTarget = this;
	Latent.ExecutionFunction = TEXT("_Visible");
	Latent.Linkage = 1;
	Latent.UUID = GetUniqueID();
	UKismetSystemLibrary::Delay(this, DelayTime, Latent);
}

void UWidget_ObtainedItem::_SetupWidgetAnimations()
{
	UProperty* pProperty = GetClass()->PropertyLink;

	// #.이 클래스의 모든 속성을 실행하여 위젯 애니메이션을 찾기.
	while (pProperty != nullptr)
	{
		// #.찾은 속성이 '오브젝트 속성'이라면...
		if (pProperty->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* pObjProperty = Cast<UObjectProperty>(pProperty);

			// #. '오브젝트 속성'이 위젯 애니메이션이라면...
			if (pObjProperty->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* pObj = pObjProperty->GetObjectPropertyValue_InContainer(this, 0);

				UWidgetAnimation* pWidgetAnim = Cast<UWidgetAnimation>(pObj);

				if (pWidgetAnim != nullptr && pWidgetAnim->MovieScene != nullptr)
				{
					if (pWidgetAnim->MovieScene->GetFName() == "BlendOut")
					{
						m_pBlendOut = pWidgetAnim;
					}
					else if (pWidgetAnim->MovieScene->GetFName() == "BlendIn")
					{
						m_pBlendIn = pWidgetAnim;
					}

				}
			}
		}
		// #. 다음 속성으로 이동.
		pProperty = pProperty->PropertyLinkNext;
	}

}

void UWidget_ObtainedItem::_Visible()
{
	FLatentActionInfo Latent;
	Latent.CallbackTarget = this;
	Latent.ExecutionFunction = TEXT("_BlendOut");
	Latent.Linkage = 1;
	Latent.UUID = GetUniqueID();
	UKismetSystemLibrary::Delay(this, m_VisibleDuration, Latent);
}

void UWidget_ObtainedItem::_BlendOut()
{
	PlayAnimation(m_pBlendOut);

	float DelayTime = m_pBlendOut->GetEndTime();

	FLatentActionInfo Latent;
	Latent.CallbackTarget = this;
	Latent.ExecutionFunction = TEXT("_Hidden");
	Latent.Linkage = 1;
	Latent.UUID = GetUniqueID();
	UKismetSystemLibrary::Delay(this, DelayTime, Latent);


}

void UWidget_ObtainedItem::_Hidden()
{
	SetVisibility(ESlateVisibility::Hidden);
	m_pMainWidget->OnObtainMessageEnd();
}

#undef LOCTEXT_NAMESPACE

