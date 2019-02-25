# Gamebuino META Screen Recorder

Here is a utility that will allow you to save your Gamebuino-META screen while running an application. Recording can be done in standard resolution (80x64) or high resolution (160x128). The display data on the screen is sent to the serial port via a USB cable. This data is recovered by a PHP script that rebuilds the frames one by one and saves them in PNG files.

This utility is therefore based on two components:

- a `ScreenRecorder` class to integrate into your C++ project that sends display data to the serial port,
- a PHP `screenrecord` script to run on your computer to retrieve all this data via the serial port and save them in PNG files.

# Required software

- **PHP-CLI 7.x** must be installed with the **GD** module to run the `screenrecord` script.
- The **imagemagick** software must be installed to generate animated GIF from the PNG files saved by the `screenrecord` script.

## To record in standard resolution

- insert the following line in your `setup()` function, where:
    + `SCREEN_WIDTH`  corresponds to the width of the screen  (80)
    + `SCREEN_HEIGHT` corresponds to the height of the screen (64)

    ```cpp
    ScreenRecorder::init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_HEIGHT);
    ```
- insert the following line in your `loop()` function:

    ```cpp
    ScreenRecorder::monitor(gb.display._buffer);
    ```

## To record in high resolution

 - insert the following line in your `setup()` function, where:
    + SCREEN_WIDTH  corresponds to the width of the screen  (160)
    + SCREEN_HEIGHT corresponds to the height of the screen (128)
    + SLICE_HEIGHT  is the height of the slices buffer that you send to the DMA controller

    ```cpp
    ScreenRecorder::init(SCREEN_WIDTH, SCREEN_HEIGHT, SLICE_HEIGHT);
    ```

 - insert the following line when your slice buffer has been populated, where:
    + `buffer` is the buffer corresponding to the current slice you have just filled in
    + `sliceIndex` is the rank of the current slice

    ```cpp
    ScreenRecorder::monitor(buffer, sliceIndex);
    ```

## Screen recording process

Compile and upload your application to the META, then run the `screenrecord' script to listen to the serial port. Press and hold `BUTTON_MENU` for one second to start recording. The META will emit red flashes throughout the recording period. To stop recording, briefly press `BUTTON_MENU` again.