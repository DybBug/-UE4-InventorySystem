// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget_OldMap.h"
#include "ItemClasses/Item_Map.h"

#include <Components/Button.h>

void UWidget_OldMap::InitializeWidget(AItem_Map* _pOldMap)
{
	m_pOldMap = _pOldMap;
	m_pCloseButton->OnClicked.AddDynamic(this, &UWidget_OldMap::_OnCloseButtonClicked);
}

void UWidget_OldMap::_OnCloseButtonClicked()
{
	m_pOldMap->OnRead();
}
