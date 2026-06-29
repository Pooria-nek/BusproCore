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

#include <Arduino.h>
#include "BusproFrame.h"

class BusproTransport
{
public:
    // dePin: GPIO driving the RS485 transceiver's combined DE+RE (active
    // HIGH = transmit/driver-enabled, LOW = receive). If your transceiver
    // module ties DE and RE separately, tie RE to the inverse externally,
    // or pass -1 and manage it yourself (e.g. auto-direction transceiver).
    BusproTransport(int8_t dePin = -1);

    void begin(HardwareSerial *serial, uint32_t baud = 9600);

    // Call frequently from loop(). Reads any available bytes, decodes
    // frames, and if a complete frame addressed to this device (or
    // broadcast) arrives, returns true and fills outFrame.
    // Frames not addressed to us are silently consumed (shared bus).
    bool poll(BusproFrame &outFrame);

    // Send a frame on the shared bus (handles DE/RE direction switching
    // and waiting for transmit-complete before releasing the bus).
    void send(const BusproFrame &frame);
    // Convenience overload: build a frame from its components and send it.
    void send(uint16_t src, uint16_t devType, uint16_t opCode, uint16_t dst, const uint8_t *payload, uint8_t payloadLen)
    {
        BusproFrame frame;
        frame.srcAddress = src;
        frame.devType = devType;
        frame.opCode = opCode;
        frame.dstAddress = dst;
        if (payloadLen > BUSPRO_MAX_PAYLOAD)
            payloadLen = BUSPRO_MAX_PAYLOAD;
        frame.payloadLen = payloadLen;
        memcpy(frame.payload, payload, payloadLen);
        send(frame);
    }

    // uint16_t address() const { return myAddress_; }

    // uint8_t subnetId() const { return myAddress_ >> 8; }
    // uint8_t deviceId() const { return myAddress_ & 0xFF; }

private:
    // bool isAddressedToMe(const BusproFrame& f) const;
    void setDriverEnabled(bool enabled);

    HardwareSerial *serial_ = nullptr;
    // uint16_t myAddress_;
    int8_t dePin_;
    BusproFrameDecoder decoder_;
};
