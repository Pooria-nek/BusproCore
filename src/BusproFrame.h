#pragma once
/*
 * BusproFrame.h
 *
 * Part of BusproCore -- the shared HDL-Buspro-style bus layer used by
 * multiple subdevice libraries (4R relay board, 4Z input board, etc.)
 * that all share one RS485 bus. Decoded frame representation, plus the
 * op-code constants used across those device libraries.
 *
 * !!! WIRE-LEVEL ENCODING IS A PLACEHOLDER, SEE BusproFrame.cpp !!!
 * This header (the decoded struct + op-code numbers) is stable and should
 * NOT need to change when the real frame format is verified -- only the
 * encode/decode functions in BusproFrame.cpp will change. Because 4R and
 * 4Z both depend on this same BusproCore, fixing the frame format once
 * here fixes it for every device library that uses it.
 */

#include <stdint.h>

// ---- Operation codes -------------------------------------------------
// Only 0x0002 (Scene Control) was given as confirmed by the spec provided
// for 4R. Everything else below follows the same op-code family pattern
// documented publicly for HDL-Buspro-style devices but is UNCONFIRMED --
// mark for verification alongside the frame format itself.
namespace BusproOp
{
    constexpr uint16_t SCENE_CONTROL = 0x0002;           // confirmed by user (4R)
    constexpr uint16_t SINGLE_CHANNEL_CONTROL = 0x0031;  // TODO_VERIFY_HDL (4R)
    constexpr uint16_t SINGLE_CHANNEL_RESPONSE = 0x0032; // TODO_VERIFY_HDL (4R)
    constexpr uint16_t READ_STATUS_REQUEST = 0x0033;     // TODO_VERIFY_HDL (4R)
    constexpr uint16_t READ_STATUS_RESPONSE = 0x0034;    // TODO_VERIFY_HDL (4R)

    // 4Z (4-channel dry-contact input board) op-codes. Same family pattern,
    // same unverified status. Picked these numbers to sit alongside the
    // 4R single-channel/status family rather than colliding with it --
    // TODO_VERIFY_HDL once real captures of an HDL input/sensor module
    // are available (input modules may use a totally different family,
    // e.g. HDL's "Universal Switch" status push uses its own op-code in
    // real Buspro -- this is a placeholder, not a researched value).
    constexpr uint16_t INPUT_STATUS_PUSH = 0x0041;     // TODO_VERIFY_HDL (4Z, unsolicited push on change)
    constexpr uint16_t INPUT_STATUS_REQUEST = 0x0042;  // TODO_VERIFY_HDL (4Z, master polls current state)
    constexpr uint16_t INPUT_STATUS_RESPONSE = 0x0043; // TODO_VERIFY_HDL (4Z, reply to poll)

    // 4T (4-channel NTC temperature sensor board) op-codes. Same family
    // pattern, same unverified status. Real HDL temperature sensor
    // modules (e.g. "DDP" dry contact/temp panels) use their own
    // researched op-codes that I don't have verified access to --
    // TODO_VERIFY_HDL once real captures of an HDL temp sensor module
    // are available.
    constexpr uint16_t TEMP_STATUS_PUSH = 0x0051;     // TODO_VERIFY_HDL (4T, periodic + on-change push)
    constexpr uint16_t TEMP_STATUS_REQUEST = 0x0052;  // TODO_VERIFY_HDL (4T, master polls current readings)
    constexpr uint16_t TEMP_STATUS_RESPONSE = 0x0053; // TODO_VERIFY_HDL (4T, reply to poll)
}

// Broadcast convention placeholder -- HDL commonly uses 255 (0xFF) as
// "all subnets" / "all devices". TODO_VERIFY_HDL against real captures.
namespace BusproAddr
{
    constexpr uint16_t BROADCAST_ADDRESS = 0xFFFF;
}

// Maximum payload bytes this library will buffer per frame.
// (Generous headroom; real HDL payloads for relay/scene ops are small.)
constexpr uint8_t BUSPRO_MAX_PAYLOAD = 32;

struct BusproFrame
{
    uint16_t srcAddress = 0;
    uint16_t dstAddress = 0;
    uint16_t opCode = 0;
    uint16_t devType = 0;
    uint8_t payload[BUSPRO_MAX_PAYLOAD] = {0};
    uint8_t payloadLen = 0;

    uint8_t srcSubnetId() const { return static_cast<uint8_t>(srcAddress >> 8); }
    uint8_t srcDeviceId() const { return static_cast<uint8_t>(srcAddress & 0xFF); }

    uint8_t devTypeHi() const { return static_cast<uint8_t>(devType >> 8); }
    uint8_t devTypeLo() const { return static_cast<uint8_t>(devType & 0xFF); }

    uint8_t opCodeHi() const { return static_cast<uint8_t>(opCode >> 8); }
    uint8_t opCodeLo() const { return static_cast<uint8_t>(opCode & 0xFF); } 

    uint8_t dstSubnetId() const { return static_cast<uint8_t>(dstAddress >> 8); }
    uint8_t dstDeviceId() const { return static_cast<uint8_t>(dstAddress & 0xFF); }

    void reset()
    {
        srcAddress = 0;
        dstAddress = 0;
        // srcSubnetId = srcDeviceId = dstSubnetId = dstDeviceId = 0;
        opCode = 0;
        payloadLen = 0;
    }
};

// Result of feeding one byte into the decoder.
enum class BusproDecodeResult : uint8_t
{
    IN_PROGRESS,   // frame not complete yet, keep feeding bytes
    FRAME_READY,   // a complete, checksum-valid frame is available
    FRAME_INVALID, // a frame boundary was found but checksum/length failed
    NO_FRAME       // byte discarded, not part of a frame (resync/noise)
};

// ---- Encode / decode entry points (implemented in BusproFrame.cpp) ----
// Encodes `frame` into `outBuf`, returns number of bytes written (0 on
// failure, e.g. payload too large).
uint16_t busproEncodeFrame(const BusproFrame &frame, uint8_t *outBuf, uint16_t outBufCap);

// Streaming decoder: call once per received byte. Internally buffers state
// between calls. When it returns FRAME_READY, `outFrame` is populated.
class BusproFrameDecoder
{
public:
    BusproDecodeResult feed(uint8_t byte, BusproFrame &outFrame);
    void resync(); // discard any partially-received frame and reset state

private:
    // Implementation detail kept private; see BusproFrame.cpp.
    // TODO_VERIFY_HDL: internal state machine encodes the PLACEHOLDER
    // sync/length/CRC assumptions documented in BusproFrame.cpp.
    enum class State : uint8_t
    {
        WAIT_SYNC1,
        WAIT_SYNC2,
        WAIT_LEN,
        READ_BODY,
        WAIT_CRC1,
        WAIT_CRC2
    } state_ = State::WAIT_SYNC1;
    uint8_t buf_[BUSPRO_MAX_PAYLOAD + 8] = {0};
    uint8_t bufIdx_ = 0;
    uint8_t expectedLen_ = 0;
    uint16_t crcAccum_ = 0;
};
