// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySystemEnums.h"
#include <Engine/DataTable.h>
#include "InventorySystemStructure.generated.h"


USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
		
public :
	FInventorySlot() {};
	FInventorySlot(const TSubclassOf<class AItem_Base> _Class, const int& _Amount)
		:ItemClass(_Class), Amount(_Amount) {};

public :
	// #. 이 슬롯의 아이템 클레스.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot")
		TSubclassOf<class AItem_Base> ItemClass;

	// #. 이 슬롯의 아이템 수량.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlot")
		int Amount = 0;
};


USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_BODY()

public :
	// #. 아이템 이름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FName Name;	

	// #. 아이템 설명.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FText Description; 

	// #. 아이템 아이콘.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	class UTexture2D* pIcon;  

	// #. 사용 가능한가?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	bool bCanBeUsed;	

	// #. 사용 텍스트.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FText UseText;		

	// #. 누적이 가능한가?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	bool bCanBeStacked; 

	// #. 아이템 카테고리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	EItemCategories Category;     

	// #. 아이템 무게.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	float Weight;

	// #. 레시피.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	TArray<FInventorySlot> Recipe;

	// #. 가격.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	int Value;

	// #. 아이템 메쉬.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	class UStaticMesh* pStaticMesh;

	// #. 메쉬 비율.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FVector MeshScale = FVector(1.f, 1.f, 1.f);

	// #. 상호작용 반경.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	float InteractRadius = 160.f;
};

USTRUCT(BlueprintType)
struct FSavedPickup
{
	GENERATED_BODY()

public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SavedPickup")
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SavedPickup")
	int Rest;
};

USTRUCT(BlueprintType)
struct FChestContent : public FTableRowBase
{
	GENERATED_BODY()
	
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChestContent")
	TSubclassOf<class AItem_Base> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChestContent")
	int MinAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChestContent")
	int MaxAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChestContent")
	int Chance;
};

USTRUCT(BlueprintType)
struct FSavedHotkey
{
	GENERATED_BODY()

public :
	FSavedHotkey() {};
	FSavedHotkey(bool _bEmpty, int _InventoryIndex)
		:bEmpty(_bEmpty), InventoryIndex(_InventoryIndex) {};

public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SavedHotkey")
	bool bEmpty = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SavedHotkey")
	int InventoryIndex = 0;
};


