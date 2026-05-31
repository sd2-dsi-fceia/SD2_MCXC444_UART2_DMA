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

#ifndef UART2_DRV_H_
#define UART2_DRV_H_

/*==================[inclusions]=============================================*/

#include "stdint.h"
#include "stdbool.h"
#include "fsl_uart.h"
#include "fsl_uart_dma.h"
#include <PERI_DMAMUX.h>
#include <PERI_UART.h>
#include <PERI_DMA.h>

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

typedef struct 
{
    DMAMUX_Type*            dmaMux_type;
    DMA_Type*               dma_type;
    uint8_t                 channel;
    dma_request_source_t    txRequest;
    // dma_request_source_t    rxRequest;   No vamos usarlo
} dma_init_t;

typedef void (*uart2_rx_callback_t)(uint8_t *data, uint16_t len);

typedef struct 
{
    uart_config_t   cfg;
    uart_transfer_t xfer;
    uart_transfer_t sendXfer;
    uart_transfer_t receiveXfer;
    UART_Type*      uart_type;
} uart2_cfg_t;

/**
 * @brief   Configuracion para inicializar el uart 2 con 
 *          transmision por DMA y recepcion con interrupcion.
 * 
 * @param   cbRxInterrupt   Funcion de callback para la recepcion de datos
 * @param   cbTxDMA         Funcion de callback para la transmision de datos
 * @param   uart_cfg        Configuraciones del uart 2
 * @param   dma_cfg         Configuraciones del dma
 */
typedef struct 
{
    uart2_rx_callback_t cbRxInterrupt;
    uart_dma_transfer_callback_t    cbTxDMA;
    uart2_cfg_t         uart_cfg;
    dma_init_t          dma_cfg;
} uart2_drv_init_t;

/*==================[external data declaration]==============================*/

/*==================[external functions definition]==========================*/

void uart2_drv_init(uart2_drv_init_t *dev);

/** \brief envía datos por puerto serie vía DMA
 **
 ** \param[inout] pBuf buffer a donde estan los datos a enviar
 ** \param[in] size tamaño del buffer
 ** \return cantidad de bytes enviados
 **/
int32_t uart2_drv_envDatos(uint8_t *pBuf, int32_t size);

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* UART2_DRV_H_ */
