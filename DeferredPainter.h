// Copyright Sharundaar. All Rights Reserved.

#pragma once

#include "Components/ContentWidget.h"
#include "DeferredPainter.generated.h"

UCLASS()
class [MODULE EXPORT HERE] UDeferredPainter : public UContentWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bEnableDeferPaint = false;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

	TSharedPtr<class SDeferredPainter> DeferredPainter;
};
