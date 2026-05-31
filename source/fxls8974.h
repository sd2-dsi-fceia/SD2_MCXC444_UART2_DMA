/* Copyright 2025, DSI FCEIA UNR - Sistemas Digitales 2
 *    DSI: http://www.dsi.fceia.unr.edu.ar/
 * Copyright 2025, Guido Cicconi
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

#ifndef FXLS8974_H_
#define FXLS8974_H_

/*==================[inclusions]=============================================*/
#include "stdint.h"
#include "stdbool.h"

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

typedef enum
{
	DR_3200hz = 0b0000,
	DR_1600hz = 0b0001,
    DR_800hz = 0b0010,
    DR_400hz = 0b0011,
    DR_200hz = 0b0100,
    DR_100hz = 0b0101,
    DR_50hz = 0b0110,
	DR_25hz = 0b0111,
    DR_12p5hz = 0b1000,
	DR_6p25hz = 0b1001,
    DR_3p125hz = 0b1010,
    DR_1p563hz = 0b1011,
	DR_0p781hz = 0b1100,
}DR_enum;

/** \brief puerto I2C utilizado en el acelerómetro  */
#define FXLS8974_I2C     I2C0

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions definition]==========================*/

/** \brief configura acelerometro FXLS8974
 **	
 **/
void fxls8974_init(void);

/** \brief Lee lectura del acelerometro en el eje X
 **
 ** \return Lectura del acelerometro en centecimas de g
 **/
int16_t fxls8974_getAcX(void);

void fxls8974_setDataRate(DR_enum rate);

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* FXLS8974_H_ */
