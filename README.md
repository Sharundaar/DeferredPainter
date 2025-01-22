# DeferredPainter
An UMG exposed deferred paint container for Unreal.

![demo](https://github.com/Sharundaar/DeferredPainter/blob/main/demo2.gif)

Useful for tutorial highlights, you can toggle the deferred rendering by just modifying the boolean property on the umg panel.

You can wrap any umg hierarchy in this panel and enable the "Defer render" boolean on it to draw everything under it on top of everything else.

You can also set the Painter Target property on the DeferredPainter to render it to a specific UDeferredPainterTarget widget instead of the global one, useful when you want to keep some control on what's rendered on top of your deferred widgets !
