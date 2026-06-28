#pragma once
/*
 * BusproTransport.h
 *
 * Owns the shared HardwareSerial port. Responsibilities:
 *  - Feed incoming bytes into the BusproFrameDecoder
 *  - Filter: only surface frames addressed to this device (or broadcast)
 *  - Drive the RS485 transceiver's DE/RE pin around transmission, since the
 *    bus is half-duplex and shared with other subdevices
 *
 * This layer does NOT know about relays, scenes, or op-code meaning -- it
 * only knows "bytes in, addressed BusproFrame out" and "BusproFrame in,
 * bytes out". Op-code dispatch lives in BusproDevice.
 */
// Uncomment for development bus sniffing
#define BUSPRO_PROMISCUOUS_MODE

#include <Arduino.h>
#include "BusproFrame.h"

class BusproTransport {
public:
    // dePin: GPIO driving the RS485 transceiver's combined DE+RE (active
    // HIGH = transmit/driver-enabled, LOW = receive). If your transceiver
    // module ties DE and RE separately, tie RE to the inverse externally,
    // or pass -1 and manage it yourself (e.g. auto-direction transceiver).
    BusproTransport(uint16_t myAddress, int8_t dePin = -1);

    void begin(HardwareSerial* serial, uint32_t baud = 9600);

    // Call frequently from loop(). Reads any available bytes, decodes
    // frames, and if a complete frame addressed to this device (or
    // broadcast) arrives, returns true and fills outFrame.
    // Frames not addressed to us are silently consumed (shared bus).
    bool poll(BusproFrame& outFrame);

    // Send a frame on the shared bus (handles DE/RE direction switching
    // and waiting for transmit-complete before releasing the bus).
    void send(const BusproFrame& frame);

    uint16_t address() const { return myAddress_; }

    uint8_t subnetId() const { return myAddress_ >> 8; }
    uint8_t deviceId() const { return myAddress_ & 0xFF; }

private:
    bool isAddressedToMe(const BusproFrame& f) const;
    void setDriverEnabled(bool enabled);

    HardwareSerial* serial_ = nullptr;
    uint16_t myAddress_;
    int8_t  dePin_;
    BusproFrameDecoder decoder_;
};
