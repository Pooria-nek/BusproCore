#include "BusproTransport.h"
extern "C"
{

    void USART1_IRQHandler(void)
    {
        if (BusproTransport::instance)
            BusproTransport::instance->handleIrq();
    }

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *h)
    {
        if (h->Instance == USART1 && BusproTransport::instance)
            BusproTransport::instance->onRxCplt();
    }

    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *h)
    {
        if (h->Instance == USART1 && BusproTransport::instance)
            BusproTransport::instance->onTxDone();
    }

    void HAL_UART_ErrorCallback(UART_HandleTypeDef *h)
    {
        if (h->Instance == USART1 && BusproTransport::instance)
            BusproTransport::instance->onError();
    }

    extern "C" void HAL_UART_MspInit(UART_HandleTypeDef *h)
    {
        if (h->Instance != USART1)
            return;

        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef g{};

#if defined(STM32F1xx)
        // F1: TX = AF push-pull, RX = floating/pull-up input, no AF number
        g.Pin = GPIO_PIN_9;
        g.Mode = GPIO_MODE_AF_PP;
        g.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &g);

        g.Pin = GPIO_PIN_10;
        g.Mode = GPIO_MODE_INPUT;
        g.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOA, &g);
#else
        // F4, L4, G4, ...
        g.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        g.Mode = GPIO_MODE_AF_PP;
        g.Pull = GPIO_PULLUP;
        g.Speed = GPIO_SPEED_FREQ_HIGH;
        g.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &g);
#endif

        HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }

} // extern "C"

BusproTransport *BusproTransport::instance = nullptr;

BusproTransport::BusproTransport(int8_t dePin) : dePin_(dePin) {}

void BusproTransport::begin(uint32_t baud)
{
    instance = this;

    huart_.Instance = USART1;
    huart_.Init.BaudRate = baud;
    huart_.Init.WordLength = UART_WORDLENGTH_9B; // 8 data + parity bit
    huart_.Init.StopBits = UART_STOPBITS_1;
    huart_.Init.Parity = UART_PARITY_ODD; // 8O1
    huart_.Init.Mode = UART_MODE_TX_RX;
    huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart_.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart_); // calls HAL_UART_MspInit

    if (dePin_ >= 0)
    {
        pinMode(dePin_, OUTPUT);
        setDriverEnabled(false);
    }
    HAL_UART_Receive_IT(&huart_, &rxByte_, 1); // arm first byte
}

void BusproTransport::setDriverEnabled(bool en)
{
    if (dePin_ >= 0)
        digitalWrite(dePin_, en ? HIGH : LOW);
}

// ---- ISR side (keep it tiny) ----
void BusproTransport::onRxByte(uint8_t b)
{
    dbgRx++;
    lastRxMs_ = millis();

    uint16_t next = (rxHead_ + 1) & (RX_BUF_SIZE - 1);
    if (next != rxTail_)
    { // drop on overflow
        rxBuf_[rxHead_] = b;
        rxHead_ = next;
    }
    HAL_UART_Receive_IT(&huart_, &rxByte_, 1); // re-arm
}

void BusproTransport::onTxDone()
{
    dbgTx++;
    setDriverEnabled(false); // release bus right after last bit
    txBusy_ = false;
}

void BusproTransport::onError()
{
    dbgErr++;
    __HAL_UART_CLEAR_OREFLAG(&huart_);
    __HAL_UART_CLEAR_FEFLAG(&huart_);
    __HAL_UART_CLEAR_PEFLAG(&huart_);
    __HAL_UART_CLEAR_NEFLAG(&huart_);

    if (txBusy_)
    { // aborted TX: release the bus
        txBusy_ = false;
        setDriverEnabled(false);
    }
    HAL_UART_Receive_IT(&huart_, &rxByte_, 1);
}

// ---- main-loop side ----
bool BusproTransport::poll(BusproFrame &outFrame)
{
    // 1. internal loopback first
    if (lqTail_ != lqHead_)
    {
        outFrame = loopQ_[lqTail_];
        lqTail_ = (lqTail_ + 1) & (LOOP_Q - 1);
        return true;
    }

    // 2. then bus bytes
    while (rxTail_ != rxHead_)
    {
        uint8_t b = rxBuf_[rxTail_];
        rxTail_ = (rxTail_ + 1) & (RX_BUF_SIZE - 1);
        if (decoder_.feed(b, outFrame) == BusproDecodeResult::FRAME_READY)
            return true;
    }
    return false;
}

void BusproTransport::send(const BusproFrame &frame)
{
    // wait for previous TX (unchanged)
    uint32_t t0 = millis();
    while (txBusy_ && (millis() - t0) < 100)
    {
    }
    if (txBusy_)
    {
        HAL_UART_AbortTransmit_IT(&huart_);
        txBusy_ = false;
        setDriverEnabled(false);
    }

    const uint16_t len = busproEncodeFrame(frame, txBuf_, sizeof(txBuf_));
    if (len == 0)
        return;

    // loopback = what a receiver would decode from these exact bytes
    BusproFrame looped{};
    for (uint16_t i = 0; i < len; i++)
    {
        if (loopDecoder_.feed(txBuf_[i], looped) == BusproDecodeResult::FRAME_READY)
        {
            uint8_t nq = (lqHead_ + 1) & (LOOP_Q - 1);
            if (nq != lqTail_)
            {
                loopQ_[lqHead_] = looped;
                lqHead_ = nq;
            }
            break;
        }
    }

    while ((millis() - lastRxMs_) < IDLE_GAP_MS)
    {
    }

    txBusy_ = true;
    setDriverEnabled(true);
    if (HAL_UART_Transmit_IT(&huart_, txBuf_, len) != HAL_OK)
    {
        txBusy_ = false;
        setDriverEnabled(false);
    }
}