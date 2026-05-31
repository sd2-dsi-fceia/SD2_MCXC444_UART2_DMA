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

/*==================[inclusions]=============================================*/

#include "fxls8974.h"
#include "fsl_i2c.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_clock.h"

/*==================[macros and definitions]=================================*/

#define FXLS8974_I2C_ADDRESS 0x18

#define INT1_PORT       PORTD
#define INT1_PORT_CLOCK kCLOCK_PortD
#define INT1_GPIO       GPIOD
#define INT1_PIN        1

#define FXLS8974_INT_STATUS_REG       0x00  // Interrupt and system status event flags
#define FXLS8974_TEMP_OUT_REG         0x01  // Temperature output data
#define FXLS8974_VECM_LSB_REG         0x02  // Vector magnitude LSB (12-bit vector magnitude)
#define FXLS8974_VECM_MSB_REG         0x03  // Vector magnitude MSB (12-bit vector magnitude)
#define FXLS8974_OUT_X_LSB_REG        0x04  // X-axis data LSB (or part, depending on endian)
#define FXLS8974_OUT_X_MSB_REG        0x05  // X-axis data MSB (or part)
#define FXLS8974_OUT_Y_LSB_REG        0x06  // Y-axis data LSB (or part)
#define FXLS8974_OUT_Y_MSB_REG        0x07  // Y-axis data MSB (or part)
#define FXLS8974_OUT_Z_LSB_REG        0x08  // Z-axis data LSB (or part)
#define FXLS8974_OUT_Z_MSB_REG        0x09  // Z-axis data MSB (or part)

#define FXLS8974_BUF_STATUS_REG       0x0B  // Buffer watermark/overflow status, sample count
#define FXLS8974_BUF_X_LSB_REG        0x0C  // Buffer X LSB (or part)
#define FXLS8974_BUF_X_MSB_REG        0x0D  // Buffer X MSB (or part)
#define FXLS8974_BUF_Y_LSB_REG        0x0E  // Buffer Y LSB (or part)
#define FXLS8974_BUF_Y_MSB_REG        0x0F  // Buffer Y MSB (or part)
#define FXLS8974_BUF_Z_LSB_REG        0x10  // Buffer Z LSB (or part)
#define FXLS8974_BUF_Z_MSB_REG        0x11  // Buffer Z MSB (or part)

#define FXLS8974_PROD_REV_REG         0x12  // Product revision (BCD)
#define FXLS8974_WHO_AM_I_REG         0x13  // Device ID (should read 0x84)

#define FXLS8974_SYS_MODE_REG         0x14  // System mode, buffer gate error flag/count

#define FXLS8974_SENS_CONFIG1_REG     0x15  // Full-scale, self-test, soft reset, Active enable
#define FXLS8974_SENS_CONFIG2_REG     0x16  // Wake/Sleep power modes, endian, auto-increment, fast-read
#define FXLS8974_SENS_CONFIG3_REG     0x17  // Wake/Sleep ODR and decimation selections
#define FXLS8974_SENS_CONFIG4_REG     0x18  // Auto-WAKE/SLEEP interrupt source enables, interrupt pin logic, ext trigger
#define FXLS8974_SENS_CONFIG5_REG     0x19  // Auto-increment disable bits for X/Y/Z, vector en, Hibernate enable

#define FXLS8974_WAKE_IDLE_LSB_REG    0x1A  // WAKE mode idle time LSB (12-bit)
#define FXLS8974_WAKE_IDLE_MSB_REG    0x1B  // WAKE mode idle time MSB (12-bit)
#define FXLS8974_SLEEP_IDLE_LSB_REG   0x1C  // SLEEP mode idle time LSB (12-bit)
#define FXLS8974_SLEEP_IDLE_MSB_REG   0x1D  // SLEEP mode idle time MSB (12-bit)
#define FXLS8974_ASLP_COUNT_LSB_REG   0x1E  // Auto-SLEEP inactivity timeout LSB (12-bit)
#define FXLS8974_ASLP_COUNT_MSB_REG   0x1F  // Auto-SLEEP inactivity timeout MSB (12-bit)

#define FXLS8974_INT_EN_REG           0x20  // Interrupt enable register
#define FXLS8974_INT_PIN_SEL_REG      0x21  // Interrupt routing (INT1/INT2)

#define FXLS8974_OFF_X_REG            0x22  // X-axis zero-g offset
#define FXLS8974_OFF_Y_REG            0x23  // Y-axis zero-g offset
#define FXLS8974_OFF_Z_REG            0x24  // Z-axis zero-g offset

#define FXLS8974_BUF_CONFIG1_REG      0x26  // Buffer operating mode, trigger source, data collection mode
#define FXLS8974_BUF_CONFIG2_REG      0x27  // Buffer flush, gate control, watermark level

#define FXLS8974_ORIENT_STATUS_REG    0x28  // Orientation event status
#define FXLS8974_ORIENT_CONFIG_REG    0x29  // Orientation detection configuration
#define FXLS8974_ORIENT_DBCOUNT_REG   0x2A  // Orientation debounce count
#define FXLS8974_ORIENT_BF_ZCOMP_REG  0x2B  // Orientation back/front Z-compensation
#define FXLS8974_ORIENT_THS_REG       0x2C  // Orientation threshold & hysteresis

#define FXLS8974_SDCD_INT_SRC1_REG    0x2D  // Sensor Data Change Detection OUT-OF-THRESH flags
#define FXLS8974_SDCD_INT_SRC2_REG    0x2E  // Sensor Data Change Detection WITHIN-THRESH flags
#define FXLS8974_SDCD_CONFIG1_REG     0x2F  // SDCD configuration 1 (axis enables, latch)
#define FXLS8974_SDCD_CONFIG2_REG     0x30  // SDCD configuration 2 (enable, ref update, mode, debounce)
#define FXLS8974_SDCD_OT_DBCNT_REG    0x31  // SDCD outside-threshold debounce count
#define FXLS8974_SDCD_WT_DBCNT_REG    0x32  // SDCD within-threshold debounce count
#define FXLS8974_SDCD_LTHS_LSB_REG    0x33  // SDCD lower-threshold LSB (12-bit)
#define FXLS8974_SDCD_LTHS_MSB_REG    0x34  // SDCD lower-threshold MSB (12-bit)
#define FXLS8974_SDCD_UTHS_LSB_REG    0x35  // SDCD upper-threshold LSB (12-bit)
#define FXLS8974_SDCD_UTHS_MSB_REG    0x36  // SDCD upper-threshold MSB (12-bit)

#define FXLS8974_SELF_TEST_CONFIG1_REG 0x37 // Self-test idle duration
#define FXLS8974_SELF_TEST_CONFIG2_REG 0x38 // Self-test measurement decimation

typedef union {
    struct {
        unsigned SRC_BOOT   : 1;  // bit 0
        unsigned SRC_ASLP   : 1;  // bit 1 Auto-WAKE/SLEEP event flag
        unsigned SRC_ORIENT : 1;  // bit 2 Orientation event flag
        unsigned SRC_SDCD_WT: 1;  // bit 3 SDCD within-threshold event flag
        unsigned SRC_SDCD_OT: 1;  // bit 4 SDCD outside-threshold event flag
        unsigned SRC_BUF    : 1;  // bit 5 Buffer event flag (SRC_BUF as per datasheet)
        unsigned SRC_OVF    : 1;  // bit 6 Output data overflow event flag
        unsigned SRC_DRDY   : 1;  // bit 7 Data-ready event flag
    } bits;
    uint8_t data;
} INT_STATUS_t;

typedef union {
    struct {
        unsigned BUF_CNT : 6;  // bits 0–5: sample count
        unsigned BUF_OVF : 1;  // bit 6: overflow flag
        unsigned BUF_WMRK: 1;  // bit 7: watermark event flag
    } bits;
    uint8_t data;
} BUF_STATUS_t;

typedef union {
    struct {
        unsigned SYS_MODE     : 2;  // bit 0-1: system operating mode
        unsigned BUF_GATE_CNT : 5;  // bit 2-6: number of OOR periods that have elapsed since the BUF _GATE_ERR flag was asserted
        unsigned BUF_GATE_ERR : 1;  // bit 7: buffer gate error flag
    } bits;
    uint8_t data;
} SYS_MODE_t;

typedef union {
    struct {
        unsigned ACTIVE      : 1;  // bit 0: standby/active mode selection
        unsigned FSR         : 2;  // bit 1-2: full-scale measurement range
        unsigned SPI_M       : 1;  // bits 3: SPI interface mode selection
        unsigned ST_POL      : 1;  // bit 4: self-test displacement polarity
        unsigned ST_AXIS_SEL : 2;  // bits 5-6: selft-test axis selection
        unsigned RST         : 1;  // bit 7: soft reset
    } bits;
    uint8_t data;
} SENS_CONFIG1_t;

typedef union {
    struct {
    	unsigned F_READ     : 1;  // bit 0: fast-read mode
        unsigned AINC_TEMP  : 1;  // bit 1: Auto-increment temperature enable
        unsigned            : 1;  // bit 2: Reserved
        unsigned LE_BE      : 1;  // bit 3: endian selection
        unsigned SLEEP_PM   : 2;  // bits 4-5: sleep power mode selection
        unsigned WAKE_PM    : 2;  // bits 6-7: wake power mode selection
    } bits;
    uint8_t data;
} SENS_CONFIG2_t;

typedef union {
    struct {
        unsigned SLEEP_ODR : 4;  // bit 0-3: output data rate in sleep mode
        unsigned WAKE_ODR  : 4;  // bit 4-7: output data rate in sleep mode
    } bits;
    uint8_t data;
} SENS_CONFIG3_t;

typedef union {
    struct {
    	unsigned INT_POL      : 1;  // bit 0: interrupt logic polarity
        unsigned INT_PP_OD    : 1;  // bit 1: interrupt pins output driver selection
        unsigned INT2_FUNC    : 1;  // bit 2: INT2/EXT_TRG pin function selection
        unsigned DRDY_PUL     : 1;  // bit 3: pulse generation option for DRDY event
        unsigned WAKE_ORIENT  : 1;  // bit 4: orientation change event Auto-WAKE/SLEEP transition source enable
        unsigned WAKE_SDCD_OT : 1;  // bit 5: SDCD outside of thresholds event Auto-WAKE/SLEEP transition source enable
        unsigned WAKE_SDCD_WT : 1;  // bit 6: SDCD within thresholds event Auto-WAKE/SLEEP transition source enable
        unsigned EXT_TRG_M    : 1;  // bit 7: external trigger function acquisition mode
    } bits;
    uint8_t data;
} SENS_CONFIG4_t;

typedef union {
    struct {
        unsigned HIBERNATE_EN : 1;  // bit 0: Hibernate enable
        unsigned Z_DIS        : 1;  // bit 1: Z-axis auto-increment disable
        unsigned Y_DIS        : 1;  // bit 2: Y-axis auto-increment disable
        unsigned X_DIS        : 1;  // bit 3: X-axis auto-increment disable
        unsigned VECM_EN      : 1;  // bit 4: enable vector magnitude calculation
        unsigned RESERVED     : 3;  // bits 5-7
    } bits;
    uint8_t data;
} SENS_CONFIG5_t;

typedef union {
    struct {
        unsigned WAKE_OUT_EN : 1;  // bit 0: WAKE power state output enable
        unsigned BOOT_DIS    : 1;  // bit 1: Boot interrupt output disable
        unsigned ASLP_EN     : 1;  // bit 2: Auto-WAKE/SLEEP interrupt output enable
        unsigned ORIENT_EN   : 1;  // bit 3: Orientation interrupt output enable
        unsigned SDCD_WT_EN  : 1;  // bit 4: SDCD within thresholds interrupt output enable
        unsigned SDCD_OT_EN  : 1;  // bit 5: SDCD outside of thresholds interrupt output enable
        unsigned BUF_EN      : 1;  // bit 6: Output data buffer interrupt output enable
        unsigned DRDY_EN     : 1;  // bit 7: Data-ready interrupt output enable
    } bits;
    uint8_t data;
} INT_EN_t;

typedef union {
    struct {
        unsigned WAKE_OUT_INT2   : 1;  // bit 0: WAKE power state interrupt routing
        unsigned BOOT_INT2       : 1;  // bit 1: BOOT event interrupt routing
        unsigned ASLP_INT2       : 1;  // bit 2: Auto-WAKE/SLEEP event interrupt routing
        unsigned ORIENT_INT2     : 1;  // bit 3: ORIENT event interrupt routing
        unsigned SDCD_WT_INT2    : 1;  // bit 4: SDCD within thresholds event interrupt routing
        unsigned SDCD_OT_INT2    : 1;  // bit 5: SDCD outside of thresholds event interrupt routing
        unsigned BUF_INT2        : 1;  // bit 6: Output buffer interrupt routing
        unsigned DRDY_INT2       : 1;  // bit 7: Data-ready interrupt routing
    } bits;
    uint8_t data;
} INT_PIN_SEL_t;

typedef union {
    struct {
        unsigned TRIG_ORIENT   : 1;  // bit 0: Orientation change event buffer trigger enable
        unsigned               : 1;  // bit 1
        unsigned TRIG_SDCD_OT  : 1;  // bit 2: SDCD outside of thresholds event buffer trigger enable
        unsigned TRIG_SDCD_WT  : 1;  // bit 3: SDCD within thresholds event buffer trigger enable
        unsigned BUF_GATE      : 1;  // bit 4: Output data buffer gate enable
        unsigned BUF_MODE      : 2;  // bits 5-6: Output data buffer gate enable
        unsigned BUF_TYPE      : 1;  // bit 7: Buffer data read out order
    } bits;
    uint8_t data;
} BUF_CONFIG1_t;

typedef union {
    struct {
        unsigned BUF_WMRK     : 6;  // bits 0–5: Buffer sample count watermark
        unsigned WAKE_SRC_BUF : 1;  // bit 6: Buffer WAKE-to-SLEEP transition source enable
        unsigned BUF_FLUSH    : 1;  // bit 7: buffer flush enable
    } bits;
    uint8_t data;
} BUF_CONFIG2_t;

typedef union {
    struct {
        unsigned BAFRO      : 1;  // bit 0: Back or front orientation
        unsigned LAPO       : 2;  // bits 1-2: Landscape/Portrait orientation
        unsigned            : 3;  // bits 3-5
        unsigned LO         : 1;  // bit 6: Z-tilt angle lockout
        unsigned NEW_ORIENT : 1;  // bit 7: Orientation status change flag
    } bits;
    uint8_t data;
} ORIENT_STATUS_t;

typedef union {
    struct {
    	unsigned               : 6;  // bits 0-5
        unsigned ORIENT_ENABLE : 1;  // bit 6: orientation detection function enable
        unsigned ORIENT_DBCNTM : 3;  // bit 7: orientation debounce counter mode selection
    } bits;
    uint8_t data;
} ORIENT_CONFIG_t;

typedef union {
    struct {
        unsigned ORIENT_ZLOCK : 3;  // bits 0-2: Z-lockout angle threshold
        unsigned              : 3;  // bits 3-5
        unsigned ORIENT_BKFR  : 3;  // bits 6-7: Back-up/Front-up trip angle threshold
    } bits;
    uint8_t data;
} ORIENT_BF_ZCOMP_t;

typedef union {
    struct {
        unsigned HYST    : 4;  // bit 0–3: hysteresis angle
        unsigned THRESH  : 4;  // bit 4–7: threshold angle
    } bits;
    uint8_t data;
} ORIENT_THS_t;

typedef union {
    struct {
    	unsigned Z_OT_POL : 1;  // bit 0: Z-axis outside-of-thresholds polarity flag
    	unsigned Z_OT_EF  : 1;  // bit 1: Z-axis data or delta-outside-of-upper-and-lower-thresholds event flag
    	unsigned Y_OT_POL : 1;  // bit 2: Y-axis outside-of-thresholds polarity flag
    	unsigned Y_OT_EF  : 1;  // bit 3: Y-axis data or delta-outside-of-upper-and-lower-thresholds event flag
    	unsigned X_OT_POL : 1;  // bit 4: X-axis outside-of-thresholds polarity flag
    	unsigned X_OT_EF  : 1;  // bit 5: X-axis data or delta-outside-of-upper-and-lower-thresholds event flag
        unsigned          : 1;  // bit 6
        unsigned OT_EA    : 1;  // bit 7: SDCD outside-of-thresholds event active flag
    } bits;
    uint8_t data;
} SDCD_INT_SRC1_t;

typedef union {
    struct {
    	unsigned         : 1;  // bit 0
        unsigned Z_WT_EF : 1;  // bit 1: Z-axis data or delta inside of upper and lower thresholds event flag
        unsigned         : 1;  // bit 2
		unsigned Y_WT_EF : 1;  // bit 3: Y-axis data or delta inside of upper and lower thresholds event flag
		unsigned         : 1;  // bit 4
		unsigned X_WT_EF : 1;  // bit 5: X-axis data or delta inside of upper and lower thresholds event flag
		unsigned         : 1;  // bit 6
		unsigned WT_EA   : 1;  // bit 7: SDCD within-thresholds event active flag
    } bits;
    uint8_t data;
} SDCD_INT_SRC2_t;

typedef union {
    struct {
        unsigned Z_WT_EN : 1;  // bit 0: SDCD function Z-axis within-thresholds condition
        unsigned Y_WT_EN : 1;  // bit 1: SDCD function Y-axis within-thresholds condition
        unsigned X_WT_EN : 1;  // bit 2: SDCD function X-axis within-thresholds condition
        unsigned Z_OT_EN : 1;  // bit 3: SDCD function Z-axis outside-of-thresholds condition
        unsigned Y_OT_EN : 1;  // bit 4: SDCD function Y-axis outside-of-thresholds condition
        unsigned X_OT_EN : 1;  // bit 5: SDCD function X-axis outside-of-thresholds condition
        unsigned WT_ELE  : 1;  // bit 6: SDCD within-thresholds event latch
        unsigned OT_ELE  : 1;  // bit 7: SDCD outside-of-thresholds event latch
    } bits;
    uint8_t data;
} SDCD_CONFIG1_t;

typedef union {
    struct {
        unsigned REF_UPD    : 1;  // bit 0: SDCD synchronous X/Y/Z reference values update bit
        unsigned MODE       : 1;  // bit 1: SDCD input data mode
        unsigned WT_LOG_SEL : 1;  // bit 2: SDCD within-thresholds event logic selection
        unsigned WT_DBCTM   : 1;  // bit 3: SDCD within-thresholds event debounce counter behavior
        unsigned OT_DBCTM   : 1;  // bit 4: SDCD outside-of-threshold event debounce counter behavior
        unsigned REF_UPDM   : 2;  // bits 5-6: SDCD internal reference values update mode
        unsigned SDCD_EN    : 1;  // bit 7: SDCD function
    } bits;
    uint8_t data;
} SDCD_CONFIG2_t;

/*==================[internal data declaration]==============================*/

static int16_t readX;
static volatile bool booted = false;

/*==================[internal functions declaration]=========================*/

static uint8_t fxls8974_read_reg(uint8_t addr)
{
	i2c_master_transfer_t masterXfer;
    uint8_t ret;

	memset(&masterXfer, 0, sizeof(masterXfer));
	masterXfer.slaveAddress = FXLS8974_I2C_ADDRESS;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &ret;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferBlocking(FXLS8974_I2C, &masterXfer);

	return ret;
}

static void fxls8974_write_reg(uint8_t addr, uint8_t data)
{
	i2c_master_transfer_t masterXfer;

    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.slaveAddress = FXLS8974_I2C_ADDRESS;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = addr;
	masterXfer.subaddressSize = 1;
	masterXfer.data = &data;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

    I2C_MasterTransferBlocking(FXLS8974_I2C, &masterXfer);
}

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void config_port_int1(void)
{
	// Habilitación de clock
	CLOCK_EnableClock(INT1_PORT_CLOCK);

	const port_pin_config_t port_int1_config = {
			/* Internal pull-up/down resistor is disabled */
		.pullSelect = kPORT_PullDisable,
		/* Slow slew rate is configured */
		.slewRate = kPORT_SlowSlewRate,
		/* Passive filter is disabled */
		.passiveFilterEnable = kPORT_PassiveFilterDisable,
		/* Low drive strength is configured */
		.driveStrength = kPORT_LowDriveStrength,
		/* Pin is configured as GPIO */
		.mux = kPORT_MuxAsGpio,
	};
	const gpio_pin_config_t gpio_int1_config = {
		.pinDirection = kGPIO_DigitalInput,
		.outputLogic = 0U
	};

	PORT_SetPinConfig(INT1_PORT, INT1_PIN, &port_int1_config);
	GPIO_PinInit(INT1_GPIO, INT1_PIN, &gpio_int1_config);

	/* Interrupt polarity active high, or active low. Default value: 1.
	   0: Active low; 1: Active high. */
	PORT_SetPinInterruptConfig(INT1_PORT, INT1_PIN, kPORT_InterruptRisingEdge);

	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	NVIC_SetPriority(PORTC_PORTD_IRQn, 0);
}

/*==================[external functions definition]==========================*/

void fxls8974_init(void)
{
	SENS_CONFIG1_t sens_config_1_reg;
	SENS_CONFIG3_t sens_config_3_reg;
	SENS_CONFIG4_t sens_config_4_reg;
	INT_EN_t int_en_reg;
	INT_PIN_SEL_t int_pin_sel_reg;

	// GPIO INT1 config
	config_port_int1();

    // Software reset
    sens_config_1_reg.data = 0;
    sens_config_1_reg.bits.RST = 1;
    fxls8974_write_reg(FXLS8974_SENS_CONFIG1_REG, sens_config_1_reg.data);

    // Esperar a que se complete el reset
    while(!booted);

	// Salgo de modo active
	sens_config_1_reg.data = fxls8974_read_reg(FXLS8974_SENS_CONFIG1_REG);
	sens_config_1_reg.bits.ACTIVE = 0;
    fxls8974_write_reg(FXLS8974_SENS_CONFIG1_REG, sens_config_1_reg.data);

    // Config register 1
    sens_config_1_reg.data = 0;
    sens_config_1_reg.bits.FSR = 0b00; // +-2g full-scale
    fxls8974_write_reg(FXLS8974_SENS_CONFIG1_REG, sens_config_1_reg.data);

    // Config register 3
    sens_config_3_reg.data = 0;
    sens_config_3_reg.bits.WAKE_ODR = DR_100hz;  // Wake up mode output data rate
    sens_config_3_reg.bits.SLEEP_ODR = DR_100hz; // Sleep mode output data rate
    fxls8974_write_reg(FXLS8974_SENS_CONFIG3_REG, sens_config_3_reg.data);

    // Config register 4
    sens_config_4_reg.data = 0;
    sens_config_4_reg.bits.DRDY_PUL = 0;  // Interrupciones por DRDY active high/low (no pulsos)
    sens_config_4_reg.bits.INT_POL = 1;   // Interrupciones active high
    sens_config_4_reg.bits.INT_PP_OD = 0; // Pines de interrupción push-pull
    fxls8974_write_reg(FXLS8974_SENS_CONFIG4_REG, sens_config_4_reg.data);

    // Data ready interrupt enable and boot interrupt disable
    int_en_reg.data = 0;
    int_en_reg.bits.DRDY_EN = 1;
	fxls8974_write_reg(FXLS8974_INT_EN_REG, int_en_reg.data);

	// Interrupciones ruteadas a INT1
	int_pin_sel_reg.data = 0;
	fxls8974_write_reg(FXLS8974_INT_PIN_SEL_REG, int_pin_sel_reg.data);

	// Paso a modo active
	sens_config_1_reg.data = fxls8974_read_reg(FXLS8974_SENS_CONFIG1_REG);
	sens_config_1_reg.bits.ACTIVE = 1;
    fxls8974_write_reg(FXLS8974_SENS_CONFIG1_REG, sens_config_1_reg.data);
}

void fxls8974_setDataRate(DR_enum rate)
{
	SENS_CONFIG1_t sens_config_1_reg;
	SENS_CONFIG3_t sens_config_3_reg;
	bool estAct;

	// Salgo de modo active
	sens_config_1_reg.data = fxls8974_read_reg(FXLS8974_SENS_CONFIG1_REG);
	estAct = sens_config_1_reg.bits.ACTIVE; 	// Guardo el valor que tenía active
	sens_config_1_reg.bits.ACTIVE = 0;
    fxls8974_write_reg(FXLS8974_SENS_CONFIG1_REG, sens_config_1_reg.data);

    // Actualizo output data rate
    sens_config_3_reg.data = 0;
    sens_config_3_reg.bits.WAKE_ODR = rate; // Wake up mode output data rate
    sens_config_3_reg.bits.SLEEP_ODR = rate; // Sleep mode output data rate
    fxls8974_write_reg(FXLS8974_SENS_CONFIG3_REG, sens_config_3_reg.data);

	// Restauro el modo
	sens_config_1_reg.data = fxls8974_read_reg(FXLS8974_SENS_CONFIG1_REG);
	sens_config_1_reg.bits.ACTIVE = estAct;
    fxls8974_write_reg(FXLS8974_SENS_CONFIG1_REG, sens_config_1_reg.data);
}

int16_t fxls8974_getAcX(void)
{
	return (int16_t)(((int32_t)readX * 100 * 4) / (int32_t)4096);
}

void PORTC_PORTD_IRQHandler(void)
{
    int16_t readG;
    INT_STATUS_t intStatus;
    SDCD_INT_SRC2_t sdcdIntSrc;
    uint32_t time_us;
    uint8_t reg;

    intStatus.data = fxls8974_read_reg(FXLS8974_INT_STATUS_REG);

    if (intStatus.bits.SRC_DRDY)
    {
        // Se lee eje X
		reg = fxls8974_read_reg(FXLS8974_OUT_X_MSB_REG);
		readG   = (int16_t)reg<<8;
		reg = fxls8974_read_reg(FXLS8974_OUT_X_LSB_REG);
		readG  |= reg;
		readX = readG;
    }

    if(intStatus.bits.SRC_BOOT)
    {
    	booted = true;
    }

    PORT_ClearPinsInterruptFlags(INT1_PORT, 1 << INT1_PIN);
}

/*==================[end of file]============================================*/
