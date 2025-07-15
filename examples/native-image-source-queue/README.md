# Native Image Source Queue Example

Test for `NativeImageSource` and `NativeImageSourceQueue` works well.

Left half is `NativeImageSource` with static image.
Right half is `NativeImageSourceQueue` with dynamic image.

This demo test each image buffer write at custom thread, and render result applied well.

Press 1 to add BackgroundBlur effected view on the scene.
Press 2 to reset the `NativeImageSource` image.

> Warning : This demo application works only for Tizen target.

![](./native-image-source-queue.png)
