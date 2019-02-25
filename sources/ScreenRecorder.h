/*
 * +---------------------------------------+
 * |    Gamebuino META Screen Recorder     | 
 * | © 2019 Stéphane Calderoni (aka Steph) |
 * |     https://gamebuino.com/@steph      |
 * +---------------------------------------+
 */

#ifndef SCREEN_RECORDER
#define SCREEN_RECORDER

#include <Gamebuino-Meta.h>

/*
 * This class allows you to capture the display on the META
 * in standard or high resolution. The data is transmitted
 * via the serial port. This data is recovered by a PHP script
 * named `screenrecord`, which rebuilds the frames one by one
 * and saves them as images in PNG format.
 * 
 * ------------------------------------------------------------------
 * To record in standard resolution:
 * ------------------------------------------------------------------
 * 
 * - insert the following line in your `setup()` function, where:
 *   + SCREEN_WIDTH  corresponds to the width of the screen  (80)
 *   + SCREEN_HEIGHT corresponds to the height of the screen (64)
 * ------------------------------------------------------------------
 * ScreenRecorder::init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_HEIGHT);
 * ------------------------------------------------------------------
 * 
 * - insert the following line in your `loop()` function:
 * ------------------------------------------------------------------
 * ScreenRecorder::monitor(gb.display._buffer);
 * ------------------------------------------------------------------
 * 
 * ------------------------------------------------------------------
 * To record in high resolution:
 * ------------------------------------------------------------------
 * 
 * - insert the following line in your `setup()` function, where:
 *   + SCREEN_WIDTH  corresponds to the width of the screen  (160)
 *   + SCREEN_HEIGHT corresponds to the height of the screen (128)
 *   + SLICE_HEIGHT  is the height of the slices buffer that you send
 *                   to the DMA controller
 * ------------------------------------------------------------------
 * ScreenRecorder::init(SCREEN_WIDTH, SCREEN_HEIGHT, SLICE_HEIGHT);
 * ------------------------------------------------------------------
 * 
 * - insert the following line when your slice buffer has been populated, where:
 *   + `buffer` is the buffer corresponding to the current slice
 *              you have just filled in
 *   + `sliceIndex` is the rank of the current slice
 * ------------------------------------------------------------------
 * ScreenRecorder::monitor(buffer, sliceIndex);
 * ------------------------------------------------------------------
 * 
 * Compile and upload your application to META, then run the
 * `screenrecord' script to listen to the serial port.
 * Press and hold `BUTTON_MENU` for one second to start recording.
 * The META will emit red flashes throughout the recording period.
 * To stop recording, briefly press `BUTTON_MENU` again.
 */

class ScreenRecorder
{
    private:

        static const uint16_t SERIAL_DATA_RATE;
        static const Color    FLASHING_COLOR;

        static uint8_t screenWidth;
        static uint8_t screenHeight;
        static uint8_t sliceHeight;

        static uint32_t timer;
        static bool     recording;
        static bool     ready;
        static bool     lightsOn;

        static void tick();
        static void checkButtons();
        static void handleLEDs();
        static void turnOnLEDs();
        static void turnOffLEDs();

        static void startRecording();
        static void stopRecording();
        static void capture(uint16_t* buffer);

    public:

        static void init(uint8_t screenWidth, uint8_t screenHeight, uint8_t sliceHeight);
        static void monitor(uint16_t* buffer, uint16_t sliceIndex = 0);

        static bool isRecording();
};

#endif