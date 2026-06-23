/*
 * BusproFrame.cpp
 *
 * ============================================================
 *  PLACEHOLDER WIRE FORMAT -- NOT VERIFIED AGAINST REAL HDL HW
 * ============================================================
 *
 * This file is the ONLY place that should need to change once real frame
 * captures are available (see /docs/CAPTURE_TEMPLATE.md). Everything above
 * this layer (BusproTransport, BusproDevice, Relay4R) only depends on the
 * BusproFrame struct, not on these byte-level choices.
 *
 * Current placeholder frame layout on the wire:
 *
 *   [0xAA][0xAA]                  2-byte sync (TODO_VERIFY_HDL)
 *   [LEN]                          1 byte: number of bytes from SrcSubnet
 *                                   through end of payload (TODO_VERIFY_HDL:
 *                                   exact length-field convention, e.g.
 *                                   whether it includes itself or the CRC)
 *   [SrcSubnetID][SrcDeviceID]
 *   [DstSubnetID][DstDeviceID]
 *   [OpCode_Hi][OpCode_Lo]
 *   [Payload...]                   PayloadLen = LEN - 6
 *   [Checksum_Hi][Checksum_Lo]      PLACEHOLDER: 16-bit additive sum of
 *                                   every byte from LEN through end of
 *                                   payload (NOT a real CRC16, and almost
 *                                   certainly NOT what real HDL uses --
 *                                   TODO_VERIFY_HDL)
 *
 * Do not trust this checksum to interoperate with genuine HDL devices.
 */

#include "BusproFrame.h"

namespace
{
    constexpr uint8_t SYNC1 = 0xAA; // TODO_VERIFY_HDL
    constexpr uint8_t SYNC2 = 0xAA; // TODO_VERIFY_HDL

    constexpr uint16_t CRC_TAB[256] =
        {
            0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
            0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
            0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
            0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
            0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
            0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
            0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
            0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
            0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
            0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
            0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
            0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
            0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
            0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
            0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
            0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
            0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
            0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
            0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
            0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
            0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
            0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
            0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
            0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
            0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
            0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
            0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
            0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
            0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
            0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
            0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
            0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

    uint16_t calculateCrc16(const uint8_t *data, uint16_t len)
    {
        uint16_t crc = 0;

        while (len--)
        {
            uint8_t dat = static_cast<uint8_t>(crc >> 8);
            crc <<= 8;
            crc ^= CRC_TAB[dat ^ *data++];
        }

        return crc;
    }

    void appendCrc16(uint8_t *buffer, uint16_t payloadLen)
    {
        uint16_t crc = calculateCrc16(buffer, payloadLen);

        buffer[payloadLen] = static_cast<uint8_t>(crc >> 8);
        buffer[payloadLen + 1] = static_cast<uint8_t>(crc);
    }

    bool verifyCrc16(const uint8_t *buffer, uint16_t payloadLen)
    {
        uint16_t crc = calculateCrc16(buffer, payloadLen);

        return buffer[payloadLen] == static_cast<uint8_t>(crc >> 8) &&
               buffer[payloadLen + 1] == static_cast<uint8_t>(crc);
    }
}

uint16_t busproEncodeFrame(const BusproFrame &frame, uint8_t *outBuf, uint16_t outBufCap)
{
    // body = SrcSubnet,SrcDev,DstSubnet,DstDev,OpHi,OpLo,Payload...
    const uint16_t bodyLen = 6 + frame.payloadLen;
    const uint16_t totalLen = 2 /*sync*/ + 1 /*len*/ + bodyLen + 2 /*checksum*/;

    if (frame.payloadLen > BUSPRO_MAX_PAYLOAD || totalLen > outBufCap || bodyLen > 255)
    {
        return 0; // doesn't fit / invalid
    }

    uint16_t idx = 0;
    outBuf[idx++] = SYNC1;
    outBuf[idx++] = SYNC2;
    outBuf[idx++] = static_cast<uint8_t>(bodyLen); // TODO_VERIFY_HDL: len convention

    const uint16_t bodyStart = idx;
    outBuf[idx++] = frame.srcSubnetId;
    outBuf[idx++] = frame.srcDeviceId;
    outBuf[idx++] = frame.dstSubnetId;
    outBuf[idx++] = frame.dstDeviceId;
    outBuf[idx++] = static_cast<uint8_t>(frame.opCode >> 8);
    outBuf[idx++] = static_cast<uint8_t>(frame.opCode & 0xFF);
    for (uint8_t i = 0; i < frame.payloadLen; ++i)
    {
        outBuf[idx++] = frame.payload[i];
    }

    // Checksum covers LEN byte + body (placeholder choice; TODO_VERIFY_HDL
    // whether real HDL includes the length byte and/or sync bytes in CRC).
    const uint16_t checksumLen = 1 + bodyLen;
    const uint16_t checksum = calculateCrc16(&outBuf[2], checksumLen);
    outBuf[idx++] = static_cast<uint8_t>(checksum >> 8);
    outBuf[idx++] = static_cast<uint8_t>(checksum & 0xFF);

    (void)bodyStart;
    return idx;
}

BusproDecodeResult BusproFrameDecoder::feed(uint8_t byte, BusproFrame &outFrame)
{
    switch (state_)
    {
    case State::WAIT_SYNC1:
        if (byte == SYNC1)
        {
            state_ = State::WAIT_SYNC2;
        }
        return BusproDecodeResult::NO_FRAME;

    case State::WAIT_SYNC2:
        if (byte == SYNC2)
        {
            state_ = State::WAIT_LEN;
            bufIdx_ = 0;
        }
        else if (byte != SYNC1)
        {
            state_ = State::WAIT_SYNC1;
        }
        // if byte == SYNC1 again, stay in WAIT_SYNC2 (handles AA AA AA ...)
        return BusproDecodeResult::NO_FRAME;

    case State::WAIT_LEN:
        expectedLen_ = byte; // bodyLen, TODO_VERIFY_HDL convention
        if (expectedLen_ < 9 // minimum bodyLen = 6 + 0 payload + 2 CRC
                             // || expectedLen_ > (BUSPRO_MAX_PAYLOAD + 6)
        )
        {
            // implausible length, likely noise -- resync
            resync();
            return BusproDecodeResult::FRAME_INVALID;
        }
        buf_[bufIdx_++] = byte; // include LEN in checksum coverage
        state_ = State::READ_BODY;
        return BusproDecodeResult::IN_PROGRESS;

    case State::READ_BODY:
        buf_[bufIdx_++] = byte;
        if (bufIdx_ == static_cast<uint8_t>(expectedLen_ - 2))
        {
            state_ = State::WAIT_CRC1;

            // crcAccum_ |= byte;
            // const uint16_t computed = placeholderChecksum(buf_, bufIdx_);
            // const uint16_t received = crcAccum_; // capture before resync() clears it
            // resync();                            // ready for next frame regardless of outcome

            // const uint8_t bodyLen = buf_[0];
            // const uint8_t payloadLen = static_cast<uint8_t>(bodyLen - 9);

            // outFrame.reset();
            // outFrame.srcSubnetId = buf_[1];
            // outFrame.srcDeviceId = buf_[2];
            // outFrame.devType = static_cast<uint16_t>((buf_[3] << 8) | buf_[4]);
            // outFrame.opCode = static_cast<uint16_t>((buf_[5] << 8) | buf_[6]);
            // outFrame.dstSubnetId = buf_[7];
            // outFrame.dstDeviceId = buf_[8];
            // outFrame.payloadLen = payloadLen;
            // for (uint8_t i = 0; i < payloadLen; ++i)
            // {
            //     outFrame.payload[i] = buf_[9 + i];
            // }
        }
        return BusproDecodeResult::IN_PROGRESS;

    case State::WAIT_CRC1:
        crcAccum_ = static_cast<uint16_t>(byte) << 8;
        state_ = State::WAIT_CRC2;
        return BusproDecodeResult::IN_PROGRESS;

    case State::WAIT_CRC2:
    {
        crcAccum_ |= byte;
        const uint16_t computed = calculateCrc16(buf_, bufIdx_);
        const uint16_t received = crcAccum_; // capture before resync() clears it
        resync();                    // ready for next frame regardless of outcome

        if (computed != received)
        {
            return BusproDecodeResult::FRAME_INVALID;
        }

        // buf_[0] = LEN, buf_[1..6] = src/dst ids + opcode, buf_[7..] = payload
        const uint8_t bodyLen = buf_[0];
        const uint8_t payloadLen = static_cast<uint8_t>(bodyLen - 11);
        if (payloadLen > BUSPRO_MAX_PAYLOAD)
        {
            return BusproDecodeResult::FRAME_INVALID;
        }

        outFrame.reset();
        outFrame.srcSubnetId = buf_[1];
        outFrame.srcDeviceId = buf_[2];
        outFrame.devType = static_cast<uint16_t>((buf_[3] << 8) | buf_[4]);
        outFrame.opCode = static_cast<uint16_t>((buf_[5] << 8) | buf_[6]);
        outFrame.dstSubnetId = buf_[7];
        outFrame.dstDeviceId = buf_[8];
        outFrame.payloadLen = payloadLen;
        for (uint8_t i = 0; i < payloadLen; ++i)
        {
            outFrame.payload[i] = buf_[9 + i];
        }
        return BusproDecodeResult::FRAME_READY;
    }
    }
    return BusproDecodeResult::NO_FRAME;
}

void BusproFrameDecoder::resync()
{
    state_ = State::WAIT_SYNC1;
    bufIdx_ = 0;
    expectedLen_ = 0;
    crcAccum_ = 0;
}
