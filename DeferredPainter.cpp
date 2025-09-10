// Copyright Sharundaar. All Rights Reserved.

#include "DeferredPainter.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/SLeafWidget.h"
#include "Layout/ArrangedChildren.h"

class SDeferredPainter : public SBox
{
public:
	SLATE_BEGIN_ARGS(SDeferredPainter) :
		_bDrawDefer(false), 
		_Target(nullptr)
	{}
		SLATE_ARGUMENT(bool, bDrawDefer)
		SLATE_ARGUMENT(UDeferredPainterTarget*, Target);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		bDrawDefer = InArgs._bDrawDefer;
		PaintTarget = InArgs._Target;
	}

	void SetTarget(UDeferredPainterTarget* Target)
	{
		PaintTarget = Target;
		Invalidate(EInvalidateWidgetReason::Paint);
	}
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		if(bDrawDefer)
		{
			// An SBox just draws its only child
			FArrangedChildren ArrangedChildren(EVisibility::Visible);
			this->ArrangeChildren(AllottedGeometry, ArrangedChildren);

			// Maybe none of our children are visible
			if( ArrangedChildren.Num() > 0 )
			{
				check( ArrangedChildren.Num() == 1 );
				FArrangedWidget& TheChild = ArrangedChildren[0];

				FSlateWindowElementList::FDeferredPaint PaintArgs = {
					ChildSlot.GetWidget(),
					Args.WithNewParent(this),
					TheChild.Geometry,
					InWidgetStyle,
					ShouldBeEnabled( bParentEnabled )
				};
				if (const UDeferredPainterTarget* Target = PaintTarget.Get())
				{
					Target->QueueDeferredPainting(MoveTemp(PaintArgs));
				}
				else
				{
					OutDrawElements.QueueDeferredPainting(MoveTemp(PaintArgs));
				}
			}
			return LayerId;
		}
		else
		{
			return SBox::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		}
	}

	void SetDeferPaint(bool bEnableDeferPaint)
	{
		if(bDrawDefer != bEnableDeferPaint)
		{
			bDrawDefer = bEnableDeferPaint;
			Invalidate(EInvalidateWidgetReason::Paint);
		}
	}

protected:
	bool bDrawDefer = false;
	TWeakObjectPtr<UDeferredPainterTarget> PaintTarget;
};

void UDeferredPainter::SetEnableDeferPaint(bool bInEnableDeferPaint)
{
	bEnableDeferPaint = bInEnableDeferPaint;
	DeferredPainter->SetDeferPaint(bInEnableDeferPaint);
}

TSharedRef<SWidget> UDeferredPainter::RebuildWidget()
{
	TSharedRef<SDeferredPainter> DeferPainter = SAssignNew(DeferredPainter, SDeferredPainter).bDrawDefer(bEnableDeferPaint).Target(Target);
	if(UWidget* Content = GetContent())
	{
		DeferPainter->SetContent(Content->TakeWidget());
	}
	else
	{
		DeferPainter->SetContent(SNullWidget::NullWidget);
	}
	return DeferPainter;
}

void UDeferredPainter::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if ( !DeferredPainter.IsValid() )
	{
		return;
	}

	DeferredPainter->SetDeferPaint(bEnableDeferPaint);
	DeferredPainter->SetTarget(Target);
}

void UDeferredPainter::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	
	DeferredPainter.Reset();
}

void UDeferredPainter::OnSlotAdded(UPanelSlot* InSlot)
{
	if ( !DeferredPainter.IsValid() )
	{
		return;
	}

	DeferredPainter->SetContent(InSlot->Content ? InSlot->Content->TakeWidget() : SNullWidget::NullWidget);
}

void UDeferredPainter::OnSlotRemoved(UPanelSlot* InSlot)
{
	if ( !DeferredPainter.IsValid() )
	{
		return;
	}

	DeferredPainter->SetContent(SNullWidget::NullWidget);
}

class SDeferredPainterTarget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SDeferredPainterTarget) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& Args) {}

	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override { return FVector2D(0, 0); }
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		for (auto& Data: PaintDatas)
		{
			Data.ExecutePaint(LayerId, OutDrawElements, MyCullingRect);
		}
		PaintDatas.Reset();
		
		return LayerId;
	}
	
	void QueueDeferredPainting(const FSlateWindowElementList::FDeferredPaint& InDeferredPaint)
	{
		PaintDatas.Add(InDeferredPaint);
		Invalidate(EInvalidateWidgetReason::Paint);
	}

protected:
	mutable TArray<FSlateWindowElementList::FDeferredPaint> PaintDatas;
};

void UDeferredPainterTarget::QueueDeferredPainting(const FSlateWindowElementList::FDeferredPaint& InDeferredPaint) const
{
	if (Painter)
	{
		Painter->QueueDeferredPainting(InDeferredPaint);
	}
}

void UDeferredPainterTarget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Painter = nullptr;
}

TSharedRef<SWidget> UDeferredPainterTarget::RebuildWidget()
{
	return SAssignNew(Painter, SDeferredPainterTarget);
}
