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
    // Dimmer/Relay Opration Codes //
    constexpr uint16_t SCENE_CONTROL_REQUEST = 0x0002;      // Scene Control request (4R)
    constexpr uint16_t SCENE_CONTROL_RESPONSE = 0x0003;     // Scene Control response (4R)
    constexpr uint16_t SINGLE_CHANNEL_REQUEST = 0x0031;     // Single Channel Control request (4R)
    constexpr uint16_t SINGLE_CHANNEL_RESPONSE = 0x0032;    // Single Channel Control response (4R)
    constexpr uint16_t READ_STATUS_REQUEST = 0x0033;        // Read Status of Channels request (4R)
    constexpr uint16_t READ_STATUS_RESPONSE = 0x0034;       // Read Status of Channels response (4R)
    constexpr uint16_t REVERSING_CONTROL_REQUEST = 0xDC1C;  // Reversing Control request (4R)
    constexpr uint16_t REVERSING_CONTROL_RESPONSE = 0xDC1D; // Reversing Control response (4R)

    // Dimmer Opration Codes //

    constexpr uint16_t SEQUENCE_CONTROL_REQUEST = 0x001A;  // Sequence Control
    constexpr uint16_t SEQUENCE_CONTROL_RESPONSE = 0x001B; // Sequence Control

    // G3 Curtain Module Opration Codes //

    constexpr uint16_t CURTAIN_CONTROL_REQUEST = 0xE3E0;  // Curtain Control
    constexpr uint16_t CURTAIN_CONTROL_RESPONSE = 0xE3E1; // Curtain Control
    constexpr uint16_t READ_CURTAIN_REQUEST = 0xE3E2;     // Read Status of Curtain
    constexpr uint16_t READ_CURTAIN_RESPONSE = 0xE3E3;    // Read Status of Curtain

    // G4 Relay Module Opration Codes //

    constexpr uint16_t READ_MOTOR_TABLE_REQUEST = 0xDC23;    // Read Motor Group Table from G4 Relay module
    constexpr uint16_t READ_MOTOR_TABLE_RESPONSE = 0xDC24;   // Read Motor Group Table from G4 Relay module
    constexpr uint16_t MODIFY_MOTOR_TABLE_REQUEST = 0xDC25;  // Modify Motor Group Table from G4 Relay module
    constexpr uint16_t MODIFY_MOTOR_TABLE_RESPONSE = 0xDC26; // Modify Motor Group Table from G4 Relay module

    // 9 in 1 sensor/PIR Sensor/Logic/IREmitter Opration Codes //

    constexpr uint16_t UNIVERSAL_SWITCH_REQUEST = 0xE01C;  // Universal Switch
    constexpr uint16_t UNIVERSAL_SWITCH_RESPONSE = 0xE01D; // Universal Switch

    // DDP Opration Codes //

    constexpr uint16_t READ_TEMP_UNIT_REQUEST = 0xE120;    // Read Celsius/Fahrenheit Flag
    constexpr uint16_t READ_TEMP_UNIT_RESPONSE = 0xE121;   // Read Celsius/Fahrenheit Flag
    constexpr uint16_t MODIFY_TEMP_UNIT_REQUEST = 0xE122;  // Modify Celsius/Fahrenheit Flag
    constexpr uint16_t MODIFY_TEMP_UNIT_RESPONSE = 0xE123; // Modify Celsius/Fahrenheit Flag

    constexpr uint16_t READ_TEMP_RANGE_REQUEST = 0x1900;    // Read AC Temperature Range
    constexpr uint16_t READ_TEMP_RANGE_RESPONSE = 0x1901;   // Read AC Temperature Range
    constexpr uint16_t MODIFY_TEMP_RANGE_REQUEST = 0x1902;  // Modify AC Temperature Range
    constexpr uint16_t MODIFY_TEMP_RANGE_RESPONSE = 0x1903; // Modify AC Temperature Range

    constexpr uint16_t READ_AC_COUNTRANGE_REQUEST = 0xE124;    // Read AC the count of Fan Speed and Mode
    constexpr uint16_t READ_AC_COUNTRANGE_RESPONSE = 0xE125;   // Read AC the count of Fan Speed and Mode
    constexpr uint16_t MODIFY_AC_COUNTRANGE_REQUEST = 0xE126;  // Modify AC the count of Fan Speed and Mode
    constexpr uint16_t MODIFY_AC_COUNTRANGE_RESPONSE = 0xE127; // Modify AC the count of Fan Speed and Mode

    constexpr uint16_t READ_AC_CURENT_STATE_REQUEST = 0xE0EC;  // Read AC Current Status
    constexpr uint16_t READ_AC_CURENT_STATE_RESPONSE = 0xE0ED; // Read AC Current Status

    constexpr uint16_t PANEL_CONTROL_REQUEST = 0xE3D8;  // Panel Control
    constexpr uint16_t PANEL_CONTROL_RESPONSE = 0xE3DA; // Panel Control

    constexpr uint16_t READ_SHOWING_TEMP_REQUEST = 0xDC1E;    // Read flag of showing Temperature or Temperature & Clock
    constexpr uint16_t READ_SHOWING_TEMP_RESPONSE = 0xDC1F;   // Read flag of showing Temperature or Temperature & Clock
    constexpr uint16_t MODIFY_SHOWING_TEMP_REQUEST = 0xDC20;  // Modify flag of showing Temperature or Temperature & Clock
    constexpr uint16_t MODIFY_SHOWING_TEMP_RESPONSE = 0xDC21; // Modify flag of showing Temperature or Temperature & Clock

    constexpr uint16_t READ_DIMMER_STATE_REQUEST = 0xDC27;    // Read Read status of enabling or disabling multi-channels dimming on DDP
    constexpr uint16_t READ_DIMMER_STATE_RESPONSE = 0xDC28;   // Read Read status of enabling or disabling multi-channels dimming on DDP
    constexpr uint16_t MODIFY_DIMMER_STATE_REQUEST = 0xDC29;  // Modify Read status of enabling or disabling multi-channels dimming on DDP
    constexpr uint16_t MODIFY_DIMMER_STATE_RESPONSE = 0xDC2A; // Modify Read status of enabling or disabling multi-channels dimming on DDP

    constexpr uint16_t READ_REMOTE_CONFIG_REQUEST = 0xDC2B;    // Read configuration of remote control button
    constexpr uint16_t READ_REMOTE_CONFIG_RESPONSE = 0xDC2C;   // Read configuration of remote control button
    constexpr uint16_t MODIFY_REMOTE_CONFIG_REQUEST = 0xDC2D;  // Modify configuration of remote control button
    constexpr uint16_t MODIFY_REMOTE_CONFIG_RESPONSE = 0xDC2E; // Modify configuration of remote control button

    // Power Meter Opration Codes //

    constexpr uint16_t READ_COEFFICIENT_REQUEST = 0xD920;  // Read Coefficient from Power Meter
    constexpr uint16_t READ_COEFFICIENT_RESPONSE = 0xD921; // Read Coefficient from Power Meter

    constexpr uint16_t READ_KWH_REQUEST = 0xD92A;  // Read KWH from Power Meter
    constexpr uint16_t READ_KWH_RESPONSE = 0xD92B; // Read KWH from Power Meter

    constexpr uint16_t READ_CURRENT_REQUEST = 0xD908;  // Read Current from Power Meter
    constexpr uint16_t READ_CURRENT_RESPONSE = 0xD909; // Read Current from Power Meter

    // Security Opration Codes //

    constexpr uint16_t SECURITY_ARM_REQUEST = 0x0104;  // Arm/Disarm Security
    constexpr uint16_t SECURITY_ARM_RESPONSE = 0x0105; // Arm/Disarm Security

    constexpr uint16_t SECURITY_ALARM_REQUEST = 0x010C;  // Active Alarm
    constexpr uint16_t SECURITY_ALARM_RESPONSE = 0x010D; // Active Alarm

    // Sensors Opration Codes //

    constexpr uint16_t READ_SENSOR_STATE_REQUEST = 0xDB00;  // Read Status from 9in1 Sensor
    constexpr uint16_t READ_SENSOR_STATE_RESPONSE = 0xDB01; // Read Status from 9in1 Sensor

    constexpr uint16_t READ_SENSOR_TEMP_REQUEST = 0xDC00;  // Read temperature from 9in1/6in1 sensor
    constexpr uint16_t READ_SENSOR_TEMP_RESPONSE = 0xDC01; // Read temperature from 9in1/6in1 sensor

    constexpr uint16_t REPORT_SENSOR_STATE_REQUEST = 0x02CA;  // Forwardly Report Status by 9in1/6in1/5in1 sensor
    constexpr uint16_t REPORT_SENSOR_STATE_RESPONSE = 0x02CB; // NOT USED // Forwardly Report Status by 9in1/6in1/5in1 sensor

    constexpr uint16_t READ_SENSOR_LINKED_REQUEST = 0xDC30;    // Read the address of linked DDP for Remote Control
    constexpr uint16_t READ_SENSOR_LINKED_RESPONSE = 0xDC31;   // Read the address of linked DDP for Remote Control
    constexpr uint16_t MODIFY_SENSOR_LINKED_REQUEST = 0xDC32;  // Modify the address of linked DDP for Remote Control
    constexpr uint16_t MODIFY_SENSOR_LINKED_RESPONSE = 0xDC33; // Modify the address of linked DDP for Remote Control

    constexpr uint16_t SEND_SENSOR_REQUEST = 0xDC22;  // Send Command from sensor to DDP for remote control
    constexpr uint16_t SEND_SENSOR_RESPONSE = 0xDC23; // // Send Command from sensor to DDP for remote control

    // 4Z Opration Codes //

    constexpr uint16_t READ_4Z_STATE_REQUEST = 0x012C;  // Read Status from 4Z
    constexpr uint16_t READ_4Z_STATE_RESPONSE = 0x012D; // Read Status from 4Z

    constexpr uint16_t REPORT_4Z_STATE_REQUEST = 0x02CA;  // Forwardly Report Status by 4Z
    constexpr uint16_t REPORT_4Z_STATE_RESPONSE = 0x02CB; // NOT USED // Forwardly Report Status by 9in1/6in1/5in1 sensor

    // Address Detection Opration Codes //

    constexpr uint16_t DETECT_ADDRESS_REQUEST = 0xE124;  // Detect Address
    constexpr uint16_t DETECT_ADDRESS_RESPONSE = 0xE125; // Detect Address
    constexpr uint16_t MODIFY_ADDRESS_REQUEST = 0xE5F7;  // Modify Address
    constexpr uint16_t MODIFY_ADDRESS_RESPONSE = 0xE5F8; // Modify Address

    // firmware version Detection Opration Codes //

    constexpr uint16_t READ_FIRMWARE_REQUEST = 0XE3E7;  // Read firmware version
    constexpr uint16_t READ_FIRMWARE_RESPONSE = 0XE3E8; // Read firmware version

    // hardware version Detection Opration Codes //

    constexpr uint16_t READ_HARDWARE_REQUEST = 0XE3E7;  // Read hardware version
    constexpr uint16_t READ_HARDWARE_RESPONSE = 0XE3E8; // Read hardware version

    // MAC Address Opration Codes //

    constexpr uint16_t DETECT_MAC_ADDRESS_REQUEST = 0xF003;  // Detect Mac Address
    constexpr uint16_t DETECT_MAC_ADDRESS_RESPONSE = 0xF004; // Detect Mac Address
    constexpr uint16_t MODIFY_MAC_ADDRESS_REQUEST = 0xF001;  // Modify Mac Address
    constexpr uint16_t MODIFY_MAC_ADDRESS_RESPONSE = 0xF002; // Modify Mac Address

    // Temperature Sensor Opration Codes //

    constexpr uint16_t READ_TEMP_REQUEST = 0XE3E7;  // Read Temperature Value
    constexpr uint16_t READ_TEMP_RESPONSE = 0XE3E8; // Read Temperature Value

    constexpr uint16_t READ_TEMP_COMP_REQUEST = 0x02C6;    // Read Temperature Compensation
    constexpr uint16_t READ_TEMP_COMP_RESPONSE = 0x02C7;   // Read Temperature Compensation
    constexpr uint16_t MODIFY_TEMP_COMP_REQUEST = 0x02C8;  // Modify Temperature Compensation
    constexpr uint16_t MODIFY_TEMP_COMP_RESPONSE = 0x02C9; // Modify Temperature Compensation

    // HVAC Control Opration Codes //

    constexpr uint16_t HVAC_CONTROL_REQUEST = 0x193A;  // HVAC Automatic Control
    constexpr uint16_t HVAC_CONTROL_RESPONSE = 0x193A; // HVAC Automatic Control

    constexpr uint16_t READ_HVAC_DELAY_REQUEST = 0xE3F4;    // Read delays for Compressor and Fan
    constexpr uint16_t READ_HVAC_DELAY_RESPONSE = 0xE3F5;   // Read delays for Compressor and Fan
    constexpr uint16_t MODIFY_HVAC_DELAY_REQUEST = 0xE3F6;  // Modify delays for Compressor and Fan
    constexpr uint16_t MODIFY_HVAC_DELAY_RESPONSE = 0xE3F7; // Modify delays for Compressor and Fan

    // Z-Audio Opration Codes //

    constexpr uint16_t READ_IR_STATE_REQUEST = 0xDC36;    // Read the IR status of IR Receiver on Z-Audio
    constexpr uint16_t READ_IR_STATE_RESPONSE = 0xDC37;   // Read the IR status of IR Receiver on Z-Audio
    constexpr uint16_t MODIFY_IR_STATE_REQUEST = 0xDC38;  // Modify the IR status of IR Receiver on Z-Audio
    constexpr uint16_t MODIFY_IR_STATE_RESPONSE = 0xDC39; // Modify the IR status of IR Receiver on Z-Audio

    // Impulse Counter Opration Codes //

    constexpr uint16_t READ_CH_REMARK_REQUEST = 0xDD0A;    // Read Channel Remark
    constexpr uint16_t READ_CH_REMARK_RESPONSE = 0xDD0B;   // Read Channel Remark
    constexpr uint16_t MODIFY_CH_REMARK_REQUEST = 0xDD0C;  // Modify Channel Remark
    constexpr uint16_t MODIFY_CH_REMARK_RESPONSE = 0xDD0D; // Modify Channel Remark

    // Dry Contact Opration Codes //

    constexpr uint16_t READ_DRY_CONTACT_REQUEST = 0x041A;    // Read the status of dry contact
    constexpr uint16_t READ_DRY_CONTACT_RESPONSE = 0x041B;   // Read the status of dry contact
    constexpr uint16_t MODIFY_DRY_CONTACT_REQUEST = 0x041C;  // Modify NO/NC flag for dry contact
    constexpr uint16_t MODIFY_DRY_CONTACT_RESPONSE = 0x041D; // Modify NO/NC flag for dry contact
}

namespace BusproDev
{
    constexpr uint16_t DEVICE_DDP = 0x0079;

    constexpr uint16_t DEVICE_4R = 0x01CE;

    constexpr uint16_t DEVICE_4Z = 0x0079;

    constexpr uint16_t DEVICE_HVAC_SB = 0x0077;
    constexpr uint16_t DEVICE_HVAC_HDL = 0x0960;
} // namespace BusproDev

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
