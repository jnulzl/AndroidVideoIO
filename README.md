# Android Video IO

## 开发环境

- FFMPEG 4.X+

- Android NDK r25c

- OpenCV for Android

## Build

- 编译demo

```shell
>>mkdir build && cd build
>>cmake —DCMAKE_BUILD_TYPE=Release -DOpenCV_SDK_NATIVE_ROOT="XXX/OpenCV-android-sdk/sdk/native" -DCMAKE_TOOLCHAIN_FILE="YYY/android-ndk-r25c/build/cmake/android.toolchain.cmake" ..
>>make VERBOSE=1 -j8 
```

编译好的文件位于`$YOUR_ROOT/bin/Android`

## Demo

上一步已经在`$YOUR_ROOT/bin/Android`目录中编译出`android_videoio_demo`，这里直接可以运行，如下所示

```shell
>>adb push $YOUR_ROOT/bin/Android/android_videoio_demo /data/local/tmp/
>>adb shell chmod 777 /data/local/tmp/android_videoio_demo
>>adb shell /data/local/tmp/android_videoio_demo /data/local/tmp/1.mp4
```