// MIT License

// Copyright (c) 2024 phonght32

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __HD44780_H__
#define __HD44780_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "err_code.h"

#define HD77480_I2C_ADDR_PCF8574  		0x27
#define HD77480_I2C_ADDR_PCF8574A  		0x3F

typedef err_code_t (*hd44780_func_set_gpio)(uint8_t level);
typedef err_code_t (*hd44780_func_i2c_send)(uint8_t *buf_send, uint16_t len);
typedef void (*hd44780_func_delay)(uint32_t time_ms);

/**
 * @brief   Handle structure.
 */
typedef struct hd44780 *hd44780_handle_t;

/**
 * @brief   LCD size.
 */
typedef enum {
	HD44780_SIZE_16_2 = 0,							/*!< LCD size 16x2 */
	HD44780_SIZE_16_4,								/*!< LCD size 16x4 */
	HD44780_SIZE_20_4,								/*!< LCD size 20x4 */
	HD44780_SIZE_MAX,
} hd44780_size_t;

/**
 * @brief   Communication mode.
 */
typedef enum {
	HD44780_COMM_MODE_4BIT = 0,						/*!< 4 bit data mode */
	HD44780_COMM_MODE_8BIT,							/*!< 8 bit data mode */
	HD44780_COMM_MODE_I2C,							/*!< I2C communication mode */
	HD44780_COMM_MODE_MAX,
} hd44780_comm_mode_t;

typedef struct {
	hd44780_size_t 				size;				/*!< LCD size */
	hd44780_comm_mode_t 		comm_mode;			/*!< LCD communicate mode */
	hd44780_func_set_gpio 		set_rs;				/*!< Function set RS */
	hd44780_func_set_gpio 		set_rw;				/*!< Function set RW */
	hd44780_func_set_gpio 		set_en;				/*!< Function set EN */
	hd44780_func_set_gpio 		set_d0;				/*!< Function set D0 */
	hd44780_func_set_gpio 		set_d1;				/*!< Function set D1 */
	hd44780_func_set_gpio 		set_d2;				/*!< Function set D2 */
	hd44780_func_set_gpio 		set_d3;				/*!< Function set D3 */
	hd44780_func_set_gpio 		set_d4;				/*!< Function set D4 */
	hd44780_func_set_gpio 		set_d5;				/*!< Function set D5 */
	hd44780_func_set_gpio 		set_d6;				/*!< Function set D6 */
	hd44780_func_set_gpio 		set_d7;				/*!< Function set D7 */
	hd44780_func_i2c_send		i2c_send;			/*!< Function send I2C data */
	hd44780_func_delay 			delay;				/*!< Function delay in ms */
} hd44780_cfg_t;

/*
 * @brief   Initialize HD44780 with default parameters.
 *
 * @note    This function must be called first.
 *
 * @param   None.
 *
 * @return
 *      - Handle structure: Success.
 *      - Others:           Fail.
 */
hd44780_handle_t hd44780_init(void);

/*
 * @brief   Set configuration parameters.
 *
 * @param 	handle Handle structure.
 * @param   config Configuration structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_set_config(hd44780_handle_t handle, hd44780_cfg_t config);

/*
 * @brief   Configure HD44780 to run.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_config(hd44780_handle_t handle);

/*
 * @brief   Clear LCD screen.
 *
 * @param   handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_clear(hd44780_handle_t handle);

/*
 * @brief   Set LCD cursor to home.
 *
 * @param   handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_home(hd44780_handle_t handle);

/*
 * @brief   Display a character.
 *
 * @param   handle Handle structure.
 *
 * @param 	char Character.
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_write_char(hd44780_handle_t handle, uint8_t chr);

/*
 * @brief   Display string.
 *
 * @param   handle Handle structure.
 * @param 	str String display.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_write_string(hd44780_handle_t handle, uint8_t *str);

/*
 * @brief   Display integer.
 *
 * @param   handle Handle structure.
 * @param 	number Number as integer format.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_write_int(hd44780_handle_t handle, int number);

/*
 * @brief   Display float.
 *
 * @param   handle Handle structure.
 * @param 	number Number as float format.
 * @param 	precision Number of digit after decimal.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_write_float(hd44780_handle_t handle, float number, uint8_t precision);

/*
 * @brief 	Move LCD's cursor to cordinate (x,y).
 *
 * @param 	row Row position.
 * @param   col Column position.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_gotoxy(hd44780_handle_t handle, uint8_t row, uint8_t col);

/*
 * @brief   Shift cursor forward.
 *
 * @param   handle Handle structure.
 * @param   step Number of step.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_shift_cursor_forward(hd44780_handle_t handle, uint8_t step);

/*
 * @brief   Shift cursor backward.
 *
 * @param   handle Handle structure.
 * @param   step Number of step.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t hd44780_shift_cursor_backward(hd44780_handle_t handle, uint8_t step);


#ifdef __cplusplus
}
#endif

#endif /* __HD44780_H__ */
