/* Copyright 2026, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2026, Agustin M. Zuliani
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

#include "board.h"
#include "pin_mux.h"
#include "uart2_drv.h"
#include "fsl_debug_console.h"
#include <MCXC444_COMMON.h>
#include <system_MCXC444.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

static volatile uint8_t g_lastByte  = 0;
static volatile bool    g_byteReady = false;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void onRxByte(uint8_t *data, uint16_t len)
{
    g_lastByte  = data[0];
    g_byteReady = true;
}

static void onTxDone(uint8_t *data, uint16_t len)
{
    /* Se llama cuando el DMA termina de transmitir los 'len' bytes apuntados
     * por 'data'. Aquí se puede liberar el buffer, señalizar una tarea, etc. */
}

/*==================[external functions definition]==========================*/

int main(void)
{
    uart2_drv_init_t uartDev =
    {
        .cbRxInterrupt        = onRxByte,
        .cbTxDMA              = onTxDone,
        .uart_cfg =
        {
            .cfg.baudRate_Bps = 115200,
            .cfg.enableTx     = true,
            .cfg.enableRx     = true,
            .uart_type        = UART2,
        },
        .dma_cfg =
        {
            .dmaMux_type      = DMAMUX0,
            .dma_type         = DMA0,
            .channel          = 0,
            .txRequest        = kDmaRequestMux0UART2Tx,
        },
    };

    static uint8_t testMsg[] = "UART2 TX OK\r\n";

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    uart2_drv_init(&uartDev);

    PRINTF("Esperando datos por UART2 (115200 8N1)...\r\n");
    uart2_drv_envDatos(testMsg, sizeof(testMsg) - 1);

    while (1)
    {
        if (g_byteReady)
        {
            g_byteReady = false;
            PRINTF("ECO: '%c' (0x%02X)\r\n", g_lastByte, g_lastByte);
        }
    }
}

/*==================[end of file]============================================*/
