// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DDOperation_Widget.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYSYSTEM_API UDDOperation_Widget : public UDragDropOperation
{
	GENERATED_BODY()
	
protected :
	class UWidget_Main* m_pMainWidget;
	class UUserWidget* m_pWidgetToDrag;
	FVector2D m_MouseOffset;	

public :	
	void Drop_Implementation(const FPointerEvent& PointerEvent);
	void DragCancelled_Implementation(const FPointerEvent& PointerEvent);
	void Dragged_Implementation(const FPointerEvent& PointerEvent);

	/* Get */
	FORCEINLINE class UWidget_Main* const& GetMainWidget() const { return m_pMainWidget; }
	FORCEINLINE class UUserWidget* const& GetWidgetToDrag() const { return m_pWidgetToDrag; }

	FORCEINLINE const FVector2D& GetMouseOffset() const { return m_MouseOffset; }

	/* Set */
	FORCEINLINE void SetMainWidget(class UWidget_Main* _pWidget) { m_pMainWidget = _pWidget; }
	FORCEINLINE void SetWidgetToDrag(class UUserWidget* _pWidget) { m_pWidgetToDrag = _pWidget; }

	FORCEINLINE void SetMouseOffset(FVector2D _Offset) { m_MouseOffset = _Offset; }

};
