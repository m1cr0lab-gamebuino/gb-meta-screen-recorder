/*
 * +---------------------------------------+
 * |    Gamebuino META Screen Recorder     | 
 * | © 2019 Stéphane Calderoni (aka Steph) |
 * |     https://gamebuino.com/@steph      |
 * +---------------------------------------+
 */

#include <Gamebuino-Meta.h>
#include "ScreenRecorder.h"

const uint16_t ScreenRecorder::SERIAL_DATA_RATE = 9600;
const Color    ScreenRecorder::FLASHING_COLOR   = gb.createColor(255, 0, 0);

uint8_t  ScreenRecorder::screenWidth  = 0;
uint8_t  ScreenRecorder::screenHeight = 0;
uint8_t  ScreenRecorder::sliceHeight  = 0;
bool     ScreenRecorder::indexed      = false;

uint32_t ScreenRecorder::timer        = 0;
uint32_t ScreenRecorder::sendedBytes  = 0;
bool     ScreenRecorder::windows      = false;
bool     ScreenRecorder::initialized  = false;
bool     ScreenRecorder::recording    = false;
bool     ScreenRecorder::readyToStart = false;
bool     ScreenRecorder::readyToStop  = false;
bool     ScreenRecorder::lightsOn     = false;

void ScreenRecorder::init(uint8_t sliceHeight, bool indexed) {
    initialized = (sliceHeight ==  2) ||
                  (sliceHeight ==  4) ||
                  (sliceHeight ==  8) ||
                  (sliceHeight == 16) ||
                  (sliceHeight == 64);

    bool hd = sliceHeight != 64;
    ScreenRecorder::screenWidth  = hd ? 160 : 80;
    ScreenRecorder::screenHeight = hd ? 128 : 64;
    ScreenRecorder::sliceHeight  = sliceHeight;
    ScreenRecorder::indexed = indexed;
}

void ScreenRecorder::setForWindows() {
    windows = true;
}

void ScreenRecorder::tick() {
    if (initialized) {
        checkButtons();
        if (recording) {
            handleLEDs();
        }
    }
}

void ScreenRecorder::checkButtons() {
    if (recording) {
        if (gb.buttons.pressed(BUTTON_MENU)) {
            stopRecording();
        }
    } else {
        if (gb.buttons.pressed(BUTTON_MENU)) {
            timer = millis();
        } else if (gb.buttons.repeat(BUTTON_MENU, 0)) {
            if (millis() - timer > 1000) {
                startRecording();
             }
        }
    }
}

void ScreenRecorder::handleLEDs() {
    if (gb.frameCount % 10 == 0) {
        turnOnLEDs();
    } else if (lightsOn) {
        turnOffLEDs();
    }
}

void ScreenRecorder::turnOnLEDs() {
    gb.lights.fill(FLASHING_COLOR);
    lightsOn = true;
}

void ScreenRecorder::turnOffLEDs() {
    gb.lights.fill(BLACK);
    lightsOn = false;
}

void ScreenRecorder::startRecording() {
    if (!recording) {
        recording   = true;
        readyToStop = false;
        SerialUSB.print("start");      // 5 bytes
        SerialUSB.write(screenWidth);  // 1 byte
        SerialUSB.write(screenHeight); // 1 byte
        SerialUSB.write(sliceHeight);  // 1 byte
        sendedBytes += 8;
    }
}

void ScreenRecorder::stopRecording() {
    if (recording) {
        readyToStop = true;
    }
}

inline uint16_t swap_endians_16(uint16_t b) {
  return (b << 8) | (b >> 8);
}

void ScreenRecorder::capture(uint16_t* buffer) {
    uint16_t sizeInPixels = screenWidth * sliceHeight;
    uint16_t sizeInBytes = 2 * sizeInPixels;
    if (!indexed) {
        SerialUSB.write((const uint8_t*) buffer, sizeInBytes);
        sendedBytes += sizeInBytes;
    } else {
        // Before sending, convert 4-bit index into 2-byte colors
        uint16_t dstArray[sizeInPixels];
        uint16_t* dstEnd = dstArray + sizeInPixels;
        Color* index = gb.display.colorIndex;
        uint16_t* src = buffer;
        for (int i = screenHeight / sliceHeight; --i >= 0; ) {
            uint16_t* dst = dstArray;
            while (dst != dstEnd) {
                uint16_t index1 = (*src >>  4) & 0x000F;
                uint16_t index2 = (*src >>  0) & 0x000F;
                uint16_t index3 = (*src >> 12) & 0x000F;
                uint16_t index4 = (*src >>  8) & 0x000F;
                // Change pixel order (because of words endianness) at the same time
                *dst++ = swap_endians_16((uint16_t)index[index1]);
                *dst++ = swap_endians_16((uint16_t)index[index2]);
                *dst++ = swap_endians_16((uint16_t)index[index3]);
                *dst++ = swap_endians_16((uint16_t)index[index4]);
                src++;
            }
            SerialUSB.write((const uint8_t*) dstArray, sizeInBytes);
            sendedBytes += sizeInBytes;
        }
    }
}

void ScreenRecorder::monitor(uint16_t* buffer, uint16_t sliceIndex) {
    tick();

    if (recording) {
        if (!readyToStart) {
            readyToStart = sliceIndex == 0;
        }

        if (readyToStop && sliceIndex == 0) {
            recording = false;
            timer     = millis();
            turnOffLEDs();
            SerialUSB.print("stop");
            sendedBytes += 4;
            if (windows) {
                // The implementation of the PHP `fread` function on Windows is buggy:
                // indeed, the read buffer only reveals the accumulated data in 8K packets.
                // Therefore, we are forced here to add a trick to get rid of this bug.
                // 
                // Strangely enough, I tried to simply send the number of bytes needed
                // to complete the 8Ks after the `stop` tag... but it doesn't seem to work.
                // So we are sending a big bogus data packet with a size of 8K and it works!
                // 
                // If any of you find a better way to proceed, it would be nice to share it
                // with me ;-)
                // 
                // uint16_t windowsFlushingBytes = 8192 - (sendedBytes % 8192);
                for (uint16_t i=0; i<8192; i++) SerialUSB.write(0xff);
            }
        }
        
        if (readyToStart && recording) {
            capture(buffer);
        }
    }
}

bool ScreenRecorder::isRecording() {
    return recording;
}
