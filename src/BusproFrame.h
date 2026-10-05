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
    // struct //

    struct RWopration
    {
        uint16_t base;

        constexpr uint16_t readReq() const { return base; }
        constexpr uint16_t readResp() const { return base + 1; }
        constexpr uint16_t writeReq() const { return base + 2; }
        constexpr uint16_t writeResp() const { return base + 3; }
    };

    struct Copration
    {
        uint16_t base;

        constexpr uint16_t req() const { return base; }
        constexpr uint16_t resp() const { return base + 1; }
    };

    constexpr uint16_t SUCCESS = 0xF8;

    /////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////// UNIVERSAL REQUEST /////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    constexpr RWopration DEVICE_REMARK{0x000E};      // Device remark
    constexpr RWopration DEVICE_MAC_ADDRESS{0xF003}; // Mac Address
    constexpr Copration DEVICE_SEARCH_HDL{0xE548};   // find device
    constexpr Copration DEVICE_FIRMWARE{0xEFFD};     // Read firmware version
    constexpr Copration DEVICE_HARDWARE{0x3024};     // Read hardware version
    constexpr Copration DEVICE_FINDIT{0xE442};       // find device

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////// RELAY DEVICES ///////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////// RELAY BASIC INFORMATION ///////////////////////////////

    constexpr RWopration CHANNEL_REMARK{0xF00E};       // Channel remark
    constexpr RWopration CHANNEL_ONDELAY{0xF04D};      // Channel on delay
    constexpr RWopration CHANNEL_ONPROTECT{0xF03F};    // Channel on protect
    constexpr RWopration RELAY_CHANNEL_ENABLE{0x1F54}; // Relay Channel enable

    /////////////////////////////// RELAY ZONE SETTING ///////////////////////////////

    constexpr RWopration ZONE_MEMBERS{0x0004}; // Zone members
    constexpr RWopration ZONE_REMARK{0xF00A};  // Zone remark

    /////////////////////////////// RELAY SCENE SETTING ///////////////////////////////

    constexpr Copration SCENE_READ{0x0000};         //
    constexpr Copration SCENE_MODIFY{0x0008};       //
    constexpr RWopration SCENE_REMARK{0xF024};      // Scene remark
    constexpr RWopration SCENE_POWERON_EN{0xF051};  // set if after power on it goes on spesific scen or not
    constexpr RWopration SCENE_POWERON_NUM{0XF055}; // after power on it goes to witch scene

    /////////////////////////////// RELAY CURTAIN ///////////////////////////////

    constexpr RWopration CURTAIN_CONFIG{0x1F50}; // Curtain enable

    /////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////// RELAY CONTROLL ///////////////////////////////////////

    constexpr Copration CONTROL_SINGLE{0x0031};
    constexpr Copration READ_STATE{0x0032};        // Single Channel
    constexpr Copration CONTROL_REVERSING{0xDC1C}; // Reversing Control request (Relay module)

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////// INPUT DEVICES ///////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////// RELAY SCENE SETTING ///////////////////////////////

    constexpr RWopration INPUT_CHANNEL_ENABLE{0x0128};
    constexpr RWopration INPUT_UNKNOWN{0x0158};
    constexpr RWopration INPUT_CHANNEL_MODE{0xD205};
    constexpr RWopration INPUT_CHANNEL_DIM_MODE{0xD230};
    constexpr RWopration INPUT_CHANNEL_ENABLE_LOCK{0xE134};

    constexpr RWopration INPUT_UNKNOWN1{0xE0E0};

    constexpr Copration INPUT_REMARK_READ{0xD210};

    constexpr RWopration INPUT_UNKNOWN2{0xD21D};

    constexpr Copration INPUT_REMARK_WRITE{0xD220};

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////// TOUCH DEVICES ///////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // /////////////////////////////// BUTTON SETTING ///////////////////////////////

    // constexpr RWopration CHANNEL_TARGET{0xE000};
    // constexpr RWopration CHANNEL_REMARK{0xE004};       // Load button by id
    // constexpr RWopration CHANNEL_MODE{0xE008};         // Read: 1 byte for each button | Write: 1 byte for each button -> F8
    // constexpr Copration CHANNEL_STATUS{0xE130};        // button state flag
    // constexpr Copration CHANNEL_DIMMING{0xE134};       // button dimming flag
    // constexpr Copration CHANNEL_DIMMING_VALUE{0xE320}; // button dimming value

    // constexpr Copration OPRATION1{0xE148}; // Unknown ask you confirm!
    constexpr Copration RESTORE{0x3000};
    constexpr Copration RESTORE_E112{0xE112};

    namespace Touch
    {
        // /////////////////////////////////////////////////////////////////////////////////////////////
        // /////////////////////////////////////// PANEL DEVICES ///////////////////////////////////////
        // /////////////////////////////////////////////////////////////////////////////////////////////

        // /////////////////////////////// SETTING ///////////////////////////////

        constexpr RWopration INDENSITY{0xE010};   // Indicator indensity (backlight - indicator)
        constexpr RWopration UI_VALUES{0xE0E0};   //
        constexpr RWopration PAGE_ENABLE{0xE12C}; // 7 byte 7 page
        // constexpr RWopration OPRATION2{0xE0E4};   // Unknown
        constexpr RWopration TEMP_CALIBRATE{0xE0F8}; // Unknown
        constexpr RWopration TEMP_FLAG{0xE120};      // Unknown
        constexpr RWopration SLEEPING{0xE138};       //
        constexpr RWopration TYPE_TIMEDATE{0xE128};  // Unknown

        // /////////////////////////////// 1 TO 4 PAGE ///////////////////////////////

        constexpr RWopration CHANNEL_FUNCTION{0xE000};      // Load function of button by id
        constexpr RWopration CHANNEL_REMARK{0xE004};        // Load button by id
        constexpr RWopration CHANNEL_MODE{0xE008};          // button Mode
        constexpr RWopration CHANNEL_STATUS{0xE130};        // button state flag
        constexpr RWopration CHANNEL_DIMMING{0xE134};       // button dimming flag
        constexpr RWopration CHANNEL_DIMMING_VALUE{0xE320}; // button dimming value

        // /////////////////////////////// AC ///////////////////////////////

        constexpr RWopration AC_INFORMATION{0xE0E4};    //
        constexpr RWopration AC_OPRATION_MODEL{0xE124}; //
        constexpr RWopration AC_TEMPERATURE{0x1900};    //
        constexpr RWopration AC_TEMPSENSOR{0x1913};     //
        constexpr RWopration AC_ECOMODE{0x190F};        //
        constexpr RWopration AC_IRREAD{0xE0F0};         //
        constexpr RWopration AC_IRCONTROL{0x1906};      //
        constexpr RWopration AC_TEST_PANEL{0xE0EC};     //

        // /////////////////////////////// FLOOR HEAT ///////////////////////////////

        // constexpr RWopration FH_TEMPERATURE{0x1900}; //
        constexpr RWopration FH_INFORMATION{0x1940}; //
        constexpr RWopration FH_STATE{0x1944};       //

        // /////////////////////////////// MUSIC ///////////////////////////////

        constexpr RWopration MUSIC_SETTING{0x1930};  // Enable - Zone - Mode
        constexpr RWopration MUSIC_OPRATION{0x1934}; //
        constexpr RWopration MUSIC_CMD{0x195A};      //

        // /////////////////////////////// IMAGE ///////////////////////////////
        constexpr Copration IMAGE_READING{0x194C}; // image number + part id
        constexpr Copration IMAGE_MODIFY{0xE118};  // image number + part id + 20 byte data


        constexpr Copration PANEL_CONTROL{0xE3D8};

    } // namespace Touch

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    constexpr Copration SCENE_CONTROL{0x0002}; // Scene Control

    // constexpr RWopration FIRMWARE_UPGRADE1{0xF81A};
    // constexpr RWopration FIRMWARE_UPGRADE2{0xF81D};

    constexpr uint16_t SEQUENCE_CONTROL_REQUEST = 0x001A;  // Sequence Control
    constexpr uint16_t SEQUENCE_CONTROL_RESPONSE = 0x001B; // Sequence Control

    // Security Opration Codes //

    constexpr uint16_t SECURITY_ARM_REQUEST = 0x0104;  // Arm/Disarm Security
    constexpr uint16_t SECURITY_ARM_RESPONSE = 0x0105; // Arm/Disarm Security

    constexpr uint16_t SECURITY_ALARM_REQUEST = 0x010C;  // Active Alarm
    constexpr uint16_t SECURITY_ALARM_RESPONSE = 0x010D; // Active Alarm

    constexpr uint16_t READ_4Z_STATE_REQUEST = 0x012C;  // Read Status from 4Z
    constexpr uint16_t READ_4Z_STATE_RESPONSE = 0x012D; // Read Status from 4Z

    constexpr uint16_t READ_TEMP_COMP_REQUEST = 0x02C6;    // Read Temperature Compensation
    constexpr uint16_t READ_TEMP_COMP_RESPONSE = 0x02C7;   // Read Temperature Compensation
    constexpr uint16_t MODIFY_TEMP_COMP_REQUEST = 0x02C8;  // Modify Temperature Compensation
    constexpr uint16_t MODIFY_TEMP_COMP_RESPONSE = 0x02C9; // Modify Temperature Compensation

    constexpr uint16_t REPORT_SENSOR_STATE_REQUEST = 0x02CA;  // Forwardly Report Status by 9in1/6in1/5in1 sensor
    constexpr uint16_t REPORT_SENSOR_STATE_RESPONSE = 0x02CB; // NOT USED // Forwardly Report Status by 9in1/6in1/5in1 sensor

    // Dry Contact Opration Codes //

    constexpr uint16_t READ_DRY_CONTACT_REQUEST = 0x041A;    // Read the status of dry contact
    constexpr uint16_t READ_DRY_CONTACT_RESPONSE = 0x041B;   // Read the status of dry contact
    constexpr uint16_t MODIFY_DRY_CONTACT_REQUEST = 0x041C;  // Modify NO/NC flag for dry contact
    constexpr uint16_t MODIFY_DRY_CONTACT_RESPONSE = 0x041D; // Modify NO/NC flag for dry contact

    // G3 Curtain Module Opration Codes //

    constexpr uint16_t CURTAIN_CONTROL_REQUEST = 0xE3E0; // Curtain Control
    // constexpr uint16_t CURTAIN_CONTROL_RESPONSE = 0xE3E1; // Curtain Control
    // constexpr uint16_t READ_CURTAIN_REQUEST = 0xE3E2;     // Read Status of Curtain
    // constexpr uint16_t READ_CURTAIN_RESPONSE = 0xE3E3;    // Read Status of Curtain

    // G4 Relay Module Opration Codes //

    // 9 in 1 sensor/PIR Sensor/Logic/IREmitter Opration Codes //

    constexpr uint16_t UNIVERSAL_SWITCH_REQUEST = 0xE01C;  // Universal Switch
    constexpr uint16_t UNIVERSAL_SWITCH_RESPONSE = 0xE01D; // Universal Switch

    // DDP Opration Codes //

    constexpr uint16_t READ_TEMP_UNIT_REQUEST = 0xE120; // Read Celsius/Fahrenheit Flag

    constexpr uint16_t READ_AC_COUNTRANGE_REQUEST = 0xE124; // Read AC the count of Fan Speed and Mode

    constexpr uint16_t READ_AC_CURENT_STATE_REQUEST = 0xE0EC;  // Read AC Current Status
    constexpr uint16_t READ_AC_CURENT_STATE_RESPONSE = 0xE0ED; // Read AC Current Status

    // constexpr uint16_t PANEL_CONTROL_REQUEST = 0xE3D8;  // Panel Control
    // constexpr uint16_t PANEL_CONTROL_RESPONSE = 0xE3DA; // Panel Control

    constexpr uint16_t READ_MOTOR_TABLE_REQUEST = 0xDC23; // Read Motor Group Table from G4 Relay module

    constexpr uint16_t READ_SHOWING_TEMP_REQUEST = 0xDC1E; // Read flag of showing Temperature or Temperature & Clock

    constexpr uint16_t READ_DIMMER_STATE_REQUEST = 0xDC27; // Read Read status of enabling or disabling multi-channels dimming on DDP

    constexpr uint16_t READ_REMOTE_CONFIG_REQUEST = 0xDC2B; // Read configuration of remote control button
    // Power Meter Opration Codes //

    constexpr uint16_t READ_COEFFICIENT_REQUEST = 0xD920;  // Read Coefficient from Power Meter
    constexpr uint16_t READ_COEFFICIENT_RESPONSE = 0xD921; // Read Coefficient from Power Meter

    constexpr uint16_t READ_KWH_REQUEST = 0xD92A;  // Read KWH from Power Meter
    constexpr uint16_t READ_KWH_RESPONSE = 0xD92B; // Read KWH from Power Meter

    constexpr uint16_t READ_CURRENT_REQUEST = 0xD908;  // Read Current from Power Meter
    constexpr uint16_t READ_CURRENT_RESPONSE = 0xD909; // Read Current from Power Meter

    // Sensors Opration Codes //

    constexpr uint16_t READ_SENSOR_STATE_REQUEST = 0xDB00;  // Read Status from 9in1 Sensor
    constexpr uint16_t READ_SENSOR_STATE_RESPONSE = 0xDB01; // Read Status from 9in1 Sensor

    constexpr uint16_t READ_SENSOR_TEMP_REQUEST = 0xDC00;  // Read temperature from 9in1/6in1 sensor
    constexpr uint16_t READ_SENSOR_TEMP_RESPONSE = 0xDC01; // Read temperature from 9in1/6in1 sensor

    constexpr uint16_t READ_SENSOR_LINKED_REQUEST = 0xDC30;    // Read the address of linked DDP for Remote Control
    constexpr uint16_t READ_SENSOR_LINKED_RESPONSE = 0xDC31;   // Read the address of linked DDP for Remote Control
    constexpr uint16_t MODIFY_SENSOR_LINKED_REQUEST = 0xDC32;  // Modify the address of linked DDP for Remote Control
    constexpr uint16_t MODIFY_SENSOR_LINKED_RESPONSE = 0xDC33; // Modify the address of linked DDP for Remote Control

    constexpr uint16_t SEND_SENSOR_REQUEST = 0xDC22;  // Send Command from sensor to DDP for remote control
    constexpr uint16_t SEND_SENSOR_RESPONSE = 0xDC23; // // Send Command from sensor to DDP for remote control

    // Address Detection Opration Codes //

    // constexpr uint16_t DETECT_ADDRESS_REQUEST = 0xE124;  // Detect Address
    // constexpr uint16_t DETECT_ADDRESS_RESPONSE = 0xE125; // Detect Address
    // constexpr uint16_t MODIFY_ADDRESS_REQUEST = 0xE5F7;  // Modify Address
    // constexpr uint16_t MODIFY_ADDRESS_RESPONSE = 0xE5F8; // Modify Address

    // Temperature Sensor Opration Codes //

    constexpr uint16_t READ_TEMP_REQUEST = 0XE3E7;  // Read Temperature Value
    constexpr uint16_t READ_TEMP_RESPONSE = 0XE3E8; // Read Temperature Value

    constexpr uint16_t READ_TEMP_RANGE_REQUEST = 0x1900; // Read AC Temperature Range

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

}

// Broadcast convention placeholder -- HDL commonly uses 255 (0xFF) as
// "all subnets" / "all devices". TODO_VERIFY_HDL against real captures.
namespace BusproAddr
{
    constexpr uint16_t BROADCAST_ADDRESS = 0xFFFF;
    constexpr uint16_t PPC_ADDRESS = 0xFDFE;
}

// Maximum payload bytes this library will buffer per frame.
// (Generous headroom; real HDL payloads for relay/scene ops are small.)
constexpr uint8_t BUSPRO_MAX_PAYLOAD = 80;

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