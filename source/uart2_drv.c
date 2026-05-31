/* Copyright 2022, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2022, 2019, 2018, Gustavo Muro - Daniel Márquez
 * Copyright 2026 Agustin M. Zuliani
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inclusions]=============================================*/

#include "uart2_drv.h"
#include <MCXC444_COMMON.h>

// Standard C Included Files
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Project Included Files
#include "SD2_board.h"
#include "fsl_lpuart.h"
#include "fsl_port.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"
#include "fsl_uart_dma.h"

/*==================[macros and definitions]=================================*/

#define UART2_DRV_TX_DMA_CHANNEL        0U
#define UART2_DRV_TX_BUFFER_DMA_SIZE    32
#define UART2_DRV_RX_BUFFER_DMA_SIZE    32

/*==================[internal data declaration]==============================*/

static uart2_drv_init_t  g_Uart2_drv_init;
static uart_dma_handle_t g_uart2DmaHandle;
static dma_handle_t      g_uart2TxDmaHandle;
static uint8_t           g_txBuffer[UART2_DRV_TX_BUFFER_DMA_SIZE]   = {0};
static volatile bool     txOnGoing                                  = false;

/*==================[internal functions declaration]=========================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/* UART user callback */
static void UART_UserCallback(UART_Type *base, uart_dma_handle_t *handle, status_t status, void *userData)
{
    userData = userData;

    if (kStatus_UART_TxIdle == status)
    {
        txOnGoing = false;
    }

    if (g_Uart2_drv_init.cbTxDMA != NULL)
    {
        g_Uart2_drv_init.cbTxDMA(base, handle, status, userData);
    }

    return;
}

void UART2_FLEXIO_IRQHandler(void)
{
    uint8_t data;

    if (UART_GetStatusFlags(g_Uart2_drv_init.uart_cfg.uart_type) & kUART_RxDataRegFullFlag)
    {
        data = UART_ReadByte(g_Uart2_drv_init.uart_cfg.uart_type);

        if (g_Uart2_drv_init.cbRxInterrupt != NULL)
        {
            g_Uart2_drv_init.cbRxInterrupt(&data, 1);
        }
    }

    /* El SDK habilita kUART_TransmissionCompleteInterruptEnable al terminar el DMA TX.
     * Hay que llamar al handler interno para que limpie ese flag y dispare el callback. */
    UART_TransferDMAHandleIRQ(g_Uart2_drv_init.uart_cfg.uart_type, &g_uart2DmaHandle);
}

/*==================[external functions definition]==========================*/

void uart2_drv_init(uart2_drv_init_t *dev)
{
    uart_config_t   config;

    /* Verificamos la entrada */
    if (dev == NULL)    {return;}

    /* Cargamos los datos del uart */
    g_Uart2_drv_init.cbRxInterrupt  = dev->cbRxInterrupt;
    g_Uart2_drv_init.cbTxDMA        = dev->cbTxDMA;
    g_Uart2_drv_init.uart_cfg       = dev->uart_cfg;
    g_Uart2_drv_init.dma_cfg        = dev->dma_cfg;

    /* Configuramos los pines PTD2 (RX) y PTD3 (TX) como UART2 */
    CLOCK_EnableClock(kCLOCK_PortD);
    PORT_SetPinMux(PORTD, 2U, kPORT_MuxAlt3);
    PORT_SetPinMux(PORTD, 3U, kPORT_MuxAlt3);

    /* Inicializamos el modulo UART */
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = g_Uart2_drv_init.uart_cfg.cfg.baudRate_Bps;
    config.enableTx     = g_Uart2_drv_init.uart_cfg.cfg.enableTx;
    config.enableRx     = g_Uart2_drv_init.uart_cfg.cfg.enableRx;

    UART_Init(g_Uart2_drv_init.uart_cfg.uart_type, &config, CLOCK_GetFreq(kCLOCK_BusClk));

    /* Inicializamos el DMA */
    DMAMUX_Init(g_Uart2_drv_init.dma_cfg.dmaMux_type);

    DMAMUX_SetSource(g_Uart2_drv_init.dma_cfg.dmaMux_type, g_Uart2_drv_init.dma_cfg.channel, g_Uart2_drv_init.dma_cfg.txRequest);
    DMAMUX_EnableChannel(g_Uart2_drv_init.dma_cfg.dmaMux_type, g_Uart2_drv_init.dma_cfg.channel);

    DMA_Init(g_Uart2_drv_init.dma_cfg.dma_type);
    DMA_CreateHandle(&g_uart2TxDmaHandle, g_Uart2_drv_init.dma_cfg.dma_type, g_Uart2_drv_init.dma_cfg.channel);

    UART_TransferCreateHandleDMA(g_Uart2_drv_init.uart_cfg.uart_type, &g_uart2DmaHandle, UART_UserCallback, NULL, &g_uart2TxDmaHandle, NULL);

    /* Habilitamos la interrupcion por recepcion en el periferico y en el NVIC */
    UART_EnableInterrupts(g_Uart2_drv_init.uart_cfg.uart_type, kUART_RxDataRegFullInterruptEnable);
    EnableIRQ(UART2_FLEXIO_IRQn);

    return;
}

/** \brief envía datos por puerto serie accediendo a memoria RAM
 **
 ** \param[inout] pBuf buffer a donde estan los datos a enviar
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes enviados
 **/
int32_t uart2_drv_envDatos(uint8_t *pBuf, int32_t size)
{
    uart_transfer_t xfer;

    if (txOnGoing)
    {
        size = 0;
    }
    else
    {
        // Limita size
        if (size > UART2_DRV_TX_BUFFER_DMA_SIZE)
        {
            size = UART2_DRV_TX_BUFFER_DMA_SIZE;
        }

        /* Copia el buffer recibido al buffer interno del driver antes de
         * disparar el DMA. Garantiza que la función que invoca pueda
         * reutilizar su buffer sin pisar datos que el DMA todavía está leyendo.
         */
        memcpy(g_txBuffer, pBuf, size);

        xfer.data = g_txBuffer;
        xfer.dataSize = size;

        txOnGoing = true;

        UART_TransferSendDMA(g_Uart2_drv_init.uart_cfg.uart_type, &g_uart2DmaHandle, &xfer);

        UART_EnableInterrupts(g_Uart2_drv_init.uart_cfg.uart_type, kUART_TransmissionCompleteInterruptEnable);
    }

    return size;
}

/*==================[end of file]============================================*/
