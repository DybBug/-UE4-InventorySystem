// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemStructure.h"
#include "Widget_Main.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UWidget_Main : public UUserWidget
{
	GENERATED_BODY()

protected :
	//
	// #. ÆÈ·¹Æ®.
	//
	UPROPERTY()
	class UCanvasPanel* m_pCanvas;
	UPROPERTY()
	class UWidget_Inventory* m_pInventoryWidget;
	UPROPERTY()
	class UWidget_Throw* m_pThrowWidget;
	UPROPERTY()
	class UWidget_CraftingMenu* m_pCraftingMenuWidget;
	UPROPERTY()
	class UWidget_Storage* m_pStorageWidget;
	UPROPERTY()
	class UWidget_Equipment* m_pEquipmentWidget;
	UPROPERTY()
	class UBorder* m_pObtainedItemBorder;
	UPROPERTY()
	class UProgressBar* m_pHealthBar;
	UPROPERTY()
	class UHorizontalBox* m_pHotkeyBar;

	//
	// #. º¯¼ö.
	//
	class UComponent_Inventory* m_pInventory;
	int m_SlotsPerRow;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	float m_VisibleDuration;

	UPROPERTY(EditAnywhere, Category = "Configuration|Hotkey")
	bool m_bShowHotkeys = true;
	UPROPERTY(EditAnywhere, Category = "Configuration|Hotkey")
	TArray<FKey> m_Hotkeys;

	TArray<class UWidget_Hotkey*> m_HotkeyWidgets;

	TArray<FInventorySlot> m_ObtainedItemsQueue;

	TSubclassOf<class UWidget_ObtainedItem> m_ObtainedItemWidgetClass;

public :
	void InitializeWidget(class UComponent_Inventory* _pInventory, int _SlotsPerRow, float _VisibleDuration);
	void AddItemToObtainedQueue(TSubclassOf<class AItem_Base> _ItemClass, int _Amount);
	void OnObtainMessageEnd();

	/* Get */
	FORCEINLINE class UCanvasPanel*         const& GetCanvas()             const { return m_pCanvas; }
	FORCEINLINE class UWidget_Inventory*    const& GetInventoryWidget()    const { return m_pInventoryWidget; }
	FORCEINLINE class UWidget_CraftingMenu* const& GetCraftingMenuWidget() const { return  m_pCraftingMenuWidget; }
	FORCEINLINE class UWidget_Throw*        const& GetThrowWidget()        const { return m_pThrowWidget; }
	FORCEINLINE class UWidget_Storage*      const& GetStorageWidget()      const { return m_pStorageWidget; }
	FORCEINLINE class UWidget_Equipment*    const& GetEquipmentWidget()    const { return m_pEquipmentWidget; }
	FORCEINLINE class UBorder*              const& GetObtainedItemBorder() const { return m_pObtainedItemBorder; }
	FORCEINLINE class UProgressBar*         const& GetHealthBar()          const { return m_pHealthBar; }
	FORCEINLINE class UHorizontalBox*       const& GetHotkeyBar()          const { return m_pHotkeyBar; }
	
	FORCEINLINE const bool&         GetShowHotkeys() const { return m_bShowHotkeys; }

	FORCEINLINE const TArray<FKey>&                  GetHotkeys()       const { return m_Hotkeys; }
	FORCEINLINE const TArray<class UWidget_Hotkey*>& GetHotkeyWidgets() const { return m_HotkeyWidgets; }


protected :
	virtual bool NativeOnDrop( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;

	void _GenerateHotkeys();
};
