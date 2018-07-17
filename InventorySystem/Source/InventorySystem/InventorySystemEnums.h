// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "InventorySystemEnums.generated.h"

#define CONVERT_TO_STRING(eType, eValue) ( (FindObject<UEnum>(ANY_PACKAGE, eType, true) != nullptr) ? (FindObject<UEnum>(ANY_PACKAGE, eType, true)->GetNameStringByIndex((uint8)eValue)) : FString("Convert Failed!") );
#define CONVERT_DISPLAYNAME_TO_STRING(eType, eValue) ( (FindObject<UEnum>(ANY_PACKAGE, eType, true) != nullptr) ? (FindObject<UEnum>(ANY_PACKAGE, eType, true)->GetDisplayNameTextByIndex((int8)eValue)).ToString() : FString("Convert Failed!") );


UENUM(BlueprintType)
enum class EItemCategories : uint8
{
	Consumeable UMETA(DisplayName = "소모품"),
	Equipment   UMETA(DisplayName = "장비"),
	QuestItems  UMETA(DisplayName = "퀘스트 아이템"),
	Readables   UMETA(DisplayName = "읽을 수 있는"),
	Weapon      UMETA(DisplayName = "무기"),
	Material    UMETA(DisplayName = "재료"),
	Max
};

UENUM(BlueprintType)
enum class ESortType : uint8
{
	Category UMETA(DisplayName = "종류"),
	Amount   UMETA(DisplayName = "수량"),
	Name     UMETA(DisplayName = "이름"),
	Max
};

