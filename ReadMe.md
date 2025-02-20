# PInky

## Dependencies

### lg for low level gpio access

https://github.com/joan2937/lg/tree/master

## CMAKE

cmake . -B ./build -DCMAKE_TOOLCHAIN_FILE=/opt/poky/5.0.7/sysroots/x86_64-pokysdk-linux/usr/share/cmake/arm1176jzfshf-vfp-poky-linux-gnueabi-toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

cmake --build ./build


## References

### GPIO Pinout Orientation RaspberyPi Zero W

https://raspberrypi.stackexchange.com/questions/83610/gpio-pinout-orientation-raspberypi-zero-w