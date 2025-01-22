// Copyright Sharundaar. All Rights Reserved.

#pragma once

#include "Components/ContentWidget.h"
#include "DeferredPainter.generated.h"

UCLASS()
class UDeferredPainter : public UContentWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetEnableDeferPaint(bool bInEnableDeferPaint);

	UFUNCTION(BlueprintCallable)
	bool GetEnableDeferPaint() const { return bEnableDeferPaint; }
	
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override { return INVTEXT("Deferred Paint"); }
#endif

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

protected:
	// Determine if the content will be drawn deferred (either globally after everything, or after Target)
	UPROPERTY(EditAnywhere)
	bool bEnableDeferPaint = false;

	// If this is set, will render as part of this target instead of the global one 
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UDeferredPainterTarget> Target;
	
	TSharedPtr<class SDeferredPainter> DeferredPainter;
};

UCLASS()
class UDeferredPainterTarget : public UWidget
{
	GENERATED_BODY()

public:
	void QueueDeferredPainting(const FSlateWindowElementList::FDeferredPaint& InDeferredPaint) const;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override { return INVTEXT("Deferred Paint"); }
#endif
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

protected:
	TSharedPtr<class SDeferredPainterTarget> Painter;
};