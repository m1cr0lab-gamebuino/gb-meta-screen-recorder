# Gamebuino META Screen Recorder

This tool will allow you to record your Gamebuino-META screen while running an application. Recording can be done in standard (80x64) or high (160x128) resolution. The display data on the screen is sent to the serial port via a USB cable. This data is recovered by a PHP script that rebuilds the frames one by one and saves them in PNG files.

This tool is therefore based on two components:

- `ScreenRecorder`: a class to integrate into your C++ project that sends display data to the serial port,
- `screenrecord`: a PHP script to run on your computer to retrieve all this data via the serial port and save them in PNG files.

## Required software

- **PHP-CLI 7.x** must be installed with the **GD** module to run the `screenrecord` script.
- The **imagemagick** software must be installed to generate animated GIF from the PNG files saved by the `screenrecord` script.

## Setup

- Place the two files `ScreenRecorder.h` and `ScreenRecorder.cpp` in the folder containing your sketch file.
- Save the `screenrecord' script in a directory included in your `PATH` environmental variable.

## To record in standard resolution

- insert the following line in your `setup()` function:

    ```cpp
    ScreenRecorder::init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_HEIGHT);
    ```

    where:

    + `SCREEN_WIDTH`  corresponds to the width of the screen  (80)
    + `SCREEN_HEIGHT` corresponds to the height of the screen (64)

- insert the following line in your `loop()` function:

    ```cpp
    ScreenRecorder::monitor(gb.display._buffer);
    ```

## To record in high resolution

 - insert the following line in your `setup()` function:

    ```cpp
    ScreenRecorder::init(SCREEN_WIDTH, SCREEN_HEIGHT, SLICE_HEIGHT);
    ```

    where:

    + SCREEN_WIDTH  corresponds to the width of the screen  (160)
    + SCREEN_HEIGHT corresponds to the height of the screen (128)
    + SLICE_HEIGHT  is the height of the slices buffer that you send to the DMA controller

 - insert the following line when your slice buffer has been populated:

    ```cpp
    ScreenRecorder::monitor(buffer, sliceIndex);
    ```

    where:

    + `buffer` is the buffer corresponding to the current slice you have just filled in
    + `sliceIndex` is the rank of the current slice

## How to start and stop recording

Compile and upload your application to the META, then run the `screenrecord` script to listen to the serial port. Press and hold `BUTTON_MENU` for one second to start recording. The META will emit red flashes throughout the recording period. To stop recording, briefly press `BUTTON_MENU` again.

## Animated GIF creation

To generate an animated GIF, you must have installed the `imagemagick` software beforehand and use its `convert` program. For example, to generate an animation of 160x128 pixels at 25 fps, run the following command:

```bash
convert -delay 4 -loop 0 png_dir_path/*.png -scale 160x128 screenrecording.gif
```