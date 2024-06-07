// Copyright Sharundaar. All Rights Reserved.

#include "DeferredPainter.h"

class SDeferredPainter : public SBox
{
public:
	SLATE_BEGIN_ARGS(SDeferredPainter) {}
		SLATE_ARGUMENT(bool, bDrawDefer)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		bDrawDefer = InArgs._bDrawDefer;
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
				OutDrawElements.QueueDeferredPainting({
								ChildSlot.GetWidget(),
								Args.WithNewParent(this),
								TheChild.Geometry,
								InWidgetStyle,
								ShouldBeEnabled( bParentEnabled )
							});
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
};

void UDeferredPainter::SetEnableDeferPaint(bool bInEnableDeferPaint)
{
	bEnableDeferPaint = bInEnableDeferPaint;
	DeferredPainter->SetDeferPaint(bInEnableDeferPaint);
}

TSharedRef<SWidget> UDeferredPainter::RebuildWidget()
{
	TSharedRef<SDeferredPainter> DeferPainter = SAssignNew(DeferredPainter, SDeferredPainter).bDrawDefer(bEnableDeferPaint);
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
