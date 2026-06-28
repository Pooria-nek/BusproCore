#include "BusproTransport.h"

BusproTransport::BusproTransport(uint16_t myAddress, int8_t dePin)
    : myAddress_(myAddress), dePin_(dePin) {}

void BusproTransport::begin(HardwareSerial *serial, uint32_t baud)
{
    serial_ = serial;
    serial_->begin(baud, SERIAL_8O1);
    if (dePin_ >= 0)
    {
        pinMode(dePin_, OUTPUT);
        setDriverEnabled(false); // start in receive mode -- shared bus
    }
}

void BusproTransport::setDriverEnabled(bool enabled)
{
    if (dePin_ >= 0)
    {
        digitalWrite(dePin_, enabled ? HIGH : LOW);
    }
}

bool BusproTransport::isAddressedToMe(const BusproFrame &f) const
{
    const bool addressMatch = (f.dstAddress == myAddress_) || (f.dstAddress == BusproAddr::BROADCAST_ADDRESS);
    return addressMatch; // && deviceMatch;
}

bool BusproTransport::poll(BusproFrame &outFrame)
{
    // return true;

    if (!serial_)
        return false;

    while (serial_->available() > 0)
    {
        const uint8_t b = static_cast<uint8_t>(serial_->read());
        const BusproDecodeResult result = decoder_.feed(b, outFrame);

        if (result == BusproDecodeResult::FRAME_READY)
        {

#ifdef BUSPRO_PROMISCUOUS_MODE
            return true; // return every valid frame
#else
            // Shared bus: many frames are not for us -- consume and ignore.
            if (isAddressedToMe(outFrame))
            {
                return true;
            }
#endif
        }
        else if (result == BusproDecodeResult::FRAME_INVALID)
        {
            // checksum or framing error -- decoder already resynced itself
            continue;
        }
        // IN_PROGRESS / NO_FRAME -> keep reading
    }
    return false;
}

void BusproTransport::send(const BusproFrame &frame)
{
    if (!serial_)
        return;

    uint8_t buf[BUSPRO_MAX_PAYLOAD + 8];
    const uint16_t len = busproEncodeFrame(frame, buf, sizeof(buf));
    if (len == 0)
        return; // encode failed (oversized payload etc.)

    setDriverEnabled(true);
    serial_->write(buf, len);
    serial_->flush(); // block until all bytes physically transmitted
    setDriverEnabled(false);
}
