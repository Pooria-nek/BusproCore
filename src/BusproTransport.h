#pragma once
#include <Arduino.h>
#include "BusproFrame.h"

class BusproTransport
{
public:
    BusproTransport(int8_t dePin = -1);

    void begin(uint32_t baud = 9600);
    bool poll(BusproFrame &outFrame);
    void send(const BusproFrame &frame);

    void send(uint16_t src, uint16_t devType, uint16_t opCode, uint16_t dst,
              const uint8_t *payload, uint8_t payloadLen)
    {
        BusproFrame frame;
        frame.srcAddress = src;
        frame.devType = devType;
        frame.opCode = opCode;
        frame.dstAddress = dst;
        if (payloadLen > BUSPRO_MAX_PAYLOAD)
            payloadLen = BUSPRO_MAX_PAYLOAD;
        frame.payloadLen = payloadLen;
        if (payload && payloadLen)
            memcpy(frame.payload, payload, payloadLen);
        send(frame);
    }

volatile uint32_t dbgRx = 0, dbgTx = 0, dbgErr = 0;
    
// Called from the C callbacks / ISR
    void handleIrq() { HAL_UART_IRQHandler(&huart_); }
    void onRxCplt() { onRxByte(rxByte_); }
    void onRxByte(uint8_t b);
    void onTxDone();
    void onError();

    static BusproTransport *instance;

private:
    void setDriverEnabled(bool enabled);

    UART_HandleTypeDef huart_{};
    int8_t dePin_;
    BusproFrameDecoder decoder_;
    BusproFrameDecoder loopDecoder_;

    static constexpr uint8_t LOOP_Q = 4; // power of 2
    BusproFrame loopQ_[LOOP_Q];
    uint8_t lqHead_ = 0, lqTail_ = 0;

    volatile uint32_t lastRxMs_ = 0;
    static constexpr uint32_t IDLE_GAP_MS = 10;

    static constexpr uint16_t RX_BUF_SIZE = 256; // power of 2
    volatile uint8_t rxBuf_[RX_BUF_SIZE];
    volatile uint16_t rxHead_ = 0, rxTail_ = 0;
    uint8_t rxByte_ = 0;
    volatile bool txBusy_ = false;
    uint8_t txBuf_[BUSPRO_MAX_PAYLOAD + 16];
};