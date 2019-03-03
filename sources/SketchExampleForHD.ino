/*
 * This code is an adaptation of Andy O'Neill's,
 * from his article "High Resolution without gb.display",
 * which I slightly modified to apply a division
 * of the screen into horizontal slices.
 */

#include <Gamebuino-Meta.h>
#include "ScreenRecorder.h"

// Initialization of display constants
#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 128
#define SLICE_HEIGHT    8 // each slice measures 160x8 pixels

// Magic to get access to wait_for_transfers_done
namespace Gamebuino_Meta {
    #define DMA_DESC_COUNT (3)
    extern volatile uint32_t dma_desc_free_count;
    static inline void wait_for_transfers_done(void) {
        while (dma_desc_free_count < DMA_DESC_COUNT);
    }
    static SPISettings tftSPISettings = SPISettings(
        24000000,
        MSBFIRST,
        SPI_MODE0
    );
};

// Double buffers for screen data. Fill one while the other is being sent to the screen.
uint16_t buffer1[SCREEN_WIDTH * SLICE_HEIGHT];
uint16_t buffer2[SCREEN_WIDTH * SLICE_HEIGHT];

void setup() {
    gb.begin();
    // We aren't using the normal screen buffer, so initialize it to 0x0 pixels.
    gb.display.init(0, 0, ColorMode::rgb565);
    // Just to push things to the limit for this example, increase to 40fps.
    gb.setFrameRate(32);
    // Initialization of the screen recorder
    ScreenRecorder::init(SLICE_HEIGHT);
    // !!! ADD THIS LINE IF YOU ARE ON WINDOWS !!!
    ScreenRecorder::setForWindows();
}

void loop() {
    while (!gb.update());

    // Loop over each 160x8 pixels slice of the screen.
    for (
        uint8_t sliceIndex = 0;
        sliceIndex < SCREEN_HEIGHT / SLICE_HEIGHT;
        sliceIndex++
    ) {
        // Alternate between buffers. While one is being sent to
        // the screen with the DMA controller, the other can be
        // used for buffering the next slice of the screen.
        uint16_t *buffer = sliceIndex % 2 == 0 ? buffer1 : buffer2;

        // BEGIN DRAWING TO BUFFER
        uint16_t sliceY  = sliceIndex * SLICE_HEIGHT;
        uint16_t initRed = sliceY + gb.frameCount;
        uint16_t blue    = gb.frameCount % 32;
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
                uint16_t red = initRed;
                ++blue %= 32;
                for (uint8_t y = 0; y < SLICE_HEIGHT; y++) {
                        ++red %= 32;
                        uint16_t green = x + y + initRed;
                        buffer[x + y * SCREEN_WIDTH] =
                            (blue  <<  8) |
                            (red   <<  3) |
                            (green << 13) |
                            ((0b111000 & green) >> 3);
                }
        }
        // END DRAWING TO BUFFER

        // As long as this isn't the first time through the loop,
        // make sure the previous write to the screen is done.
        if (sliceIndex != 0) waitForPreviousDraw();
        // And finally send the current buffer slice to the screen!
        customDrawBuffer(0, sliceY, buffer, SCREEN_WIDTH, SLICE_HEIGHT);
        // Record the buffer slice
        ScreenRecorder::monitor(buffer, sliceIndex);
    }
    // Wait for the final slice to complete before leaving the function.
    waitForPreviousDraw();
}

// Use gb.tft calls to communicate with the screen.
void customDrawBuffer(
    int16_t x,
    int16_t y,
    uint16_t *buffer,
    uint16_t w,
    uint16_t h) {
    gb.tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
    SPI.beginTransaction(Gamebuino_Meta::tftSPISettings);
    gb.tft.dataMode();
    gb.tft.sendBuffer(buffer, w*h);
}

void waitForPreviousDraw() {
    Gamebuino_Meta::wait_for_transfers_done();
    gb.tft.idleMode();
    SPI.endTransaction();
}