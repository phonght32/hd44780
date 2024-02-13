#include "stdlib.h"
#include "stdio.h"
#include "hd44780.h"

typedef err_code_t (*hd44780_func_write)(hd44780_handle_t handle, uint8_t data);
typedef void (*hd44780_func_wait)(hd44780_handle_t handle);

typedef struct hd44780 {
	hd44780_size_t 				size;				/*!< LCD size */
	hd44780_comm_mode_t 		comm_mode;			/*!< LCD communicate mode */
	hd44780_func_set_rs 		set_rs;				/*!< Function set RS */
	hd44780_func_set_rw 		set_rw;				/*!< Function set RW */
	hd44780_func_set_en 		set_en;				/*!< Function set EN */
	hd44780_func_set_d0 		set_d0;				/*!< Function set D0 */
	hd44780_func_set_d1 		set_d1;				/*!< Function set D1 */
	hd44780_func_set_d2 		set_d2;				/*!< Function set D2 */
	hd44780_func_set_d3 		set_d3;				/*!< Function set D3 */
	hd44780_func_set_d4 		set_d4;				/*!< Function set D4 */
	hd44780_func_set_d5 		set_d5;				/*!< Function set D5 */
	hd44780_func_set_d6 		set_d6;				/*!< Function set D6 */
	hd44780_func_set_d7 		set_d7;				/*!< Function set D7 */
	hd44780_func_i2c_send		i2c_send;			/*!< Function send I2C data */
	hd44780_func_delay 			delay;				/*!< Function delay in ms */
	hd44780_func_write			write_cmd;			/*!< Function write command */
	hd44780_func_write			write_data;			/*!< Function write data */
	hd44780_func_wait 			wait;				/*!< Function wait */
} hd44780_t;

static void hd44780_wait(hd44780_handle_t handle)
{
	handle->delay(2);
}

static err_code_t hd44780_write_cmd_4bit(hd44780_handle_t handle, uint8_t cmd)
{
	uint8_t bit_data;
	uint8_t nibble_h = cmd >> 4 & 0x0F;
	uint8_t nibble_l = cmd & 0x0F;

	/* Set hw_info RS to write to command register */
	handle->set_rs(0);

	if (handle->set_rw != NULL) {
		handle->set_rw(0);
	}

	bit_data = (nibble_h >> 0) & 0x01;
	handle->set_d4(bit_data);
	bit_data = (nibble_h >> 1) & 0x01;
	handle->set_d5(bit_data);
	bit_data = (nibble_h >> 2) & 0x01;
	handle->set_d6(bit_data);
	bit_data = (nibble_h >> 3) & 0x01;
	handle->set_d7(bit_data);

	handle->set_en(1);
	handle->delay(1);
	handle->set_en(0);
	handle->delay(1);

	bit_data = (nibble_l >> 0) & 0x01;
	handle->set_d4(bit_data);
	bit_data = (nibble_l >> 1) & 0x01;
	handle->set_d5(bit_data);
	bit_data = (nibble_l >> 2) & 0x01;
	handle->set_d6(bit_data);
	bit_data = (nibble_l >> 3) & 0x01;
	handle->set_d7(bit_data);

	handle->set_en(1);
	handle->delay(1);
	handle->set_en(0);
	handle->delay(1);

	return ERR_CODE_SUCCESS;
}

static err_code_t hd44780_write_cmd_8bit(hd44780_handle_t handle, uint8_t cmd)
{
	/* 8 bit currently is not supported */

	return ERR_CODE_SUCCESS;
}

static err_code_t hd44780_write_cmd_serial(hd44780_handle_t handle, uint8_t cmd)
{
	uint8_t buf_send[4];
	buf_send[0] = (cmd & 0xF0) | 0x04;
	buf_send[1] = (cmd & 0xF0);
	buf_send[2] = ((cmd << 4) & 0xF0) | 0x04;
	buf_send[3] = ((cmd << 4) & 0xF0) | 0x08;

	handle->i2c_send(buf_send, 4, 1000);

	return ERR_CODE_SUCCESS;
}

static err_code_t hd44780_write_data_4bit(hd44780_handle_t handle, uint8_t data)
{
	uint8_t bit_data;
	uint8_t nibble_h = data >> 4 & 0x0F;
	uint8_t nibble_l = data & 0x0F;

	/* Set hw_info RS to high to write to data register */
	handle->set_rs(1);

	if (handle->set_rw != NULL) {
		handle->set_rw(0);
	}

	bit_data = (nibble_h >> 0) & 0x01;
	handle->set_d4(bit_data);
	bit_data = (nibble_h >> 1) & 0x01;
	handle->set_d5(bit_data);
	bit_data = (nibble_h >> 2) & 0x01;
	handle->set_d6(bit_data);
	bit_data = (nibble_h >> 3) & 0x01;
	handle->set_d7(bit_data);

	handle->set_en(1);
	handle->delay(1);
	handle->set_en(0);
	handle->delay(1);

	bit_data = (nibble_l >> 0) & 0x01;
	handle->set_d4(bit_data);
	bit_data = (nibble_l >> 1) & 0x01;
	handle->set_d5(bit_data);
	bit_data = (nibble_l >> 2) & 0x01;
	handle->set_d6(bit_data);
	bit_data = (nibble_l >> 3) & 0x01;
	handle->set_d7(bit_data);

	handle->set_en(1);
	handle->delay(1);
	handle->set_en(0);
	handle->delay(1);

	return ERR_CODE_SUCCESS;
}

static err_code_t hd44780_write_data_8bit(hd44780_handle_t handle, uint8_t data)
{
	/* 8 bit currently is not supported */

	return ERR_CODE_SUCCESS;
}

static err_code_t hd44780_write_data_serial(hd44780_handle_t handle, uint8_t data)
{
	uint8_t buf_send[4];
	buf_send[0] = (data & 0xF0) | 0x0D;
	buf_send[1] = (data & 0xF0) | 0x09;
	buf_send[2] = ((data << 4) & 0xF0) | 0x0D;
	buf_send[3] = ((data << 4) & 0xF0) | 0x09;

	handle->i2c_send(buf_send, 4, 1000);

	return ERR_CODE_SUCCESS;
}

hd44780_handle_t hd44780_init(void)
{
	hd44780_handle_t handle = calloc(1, sizeof(hd44780_t));
	if (handle == NULL)
	{
		return NULL;
	}

	return handle;
}

err_code_t hd44780_set_config(hd44780_handle_t handle, hd44780_cfg_t config)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	hd44780_func_write write_cmd;
	hd44780_func_write write_data;

	if (config.comm_mode == HD44780_COMM_MODE_4BIT)
	{
		write_cmd = hd44780_write_cmd_4bit;
		write_data = hd44780_write_data_4bit;
	}
	else if (config.comm_mode == HD44780_COMM_MODE_8BIT)
	{
		write_cmd = hd44780_write_cmd_8bit;
		write_data = hd44780_write_data_8bit;
	}
	else
	{
		write_cmd = hd44780_write_cmd_serial;
		write_data = hd44780_write_data_serial;
	}

	handle->size = config.size;
	handle->comm_mode = config.comm_mode;
	handle->set_rs = config.set_rs;
	handle->set_rw = config.set_rw;
	handle->set_en = config.set_en;
	handle->set_d0 = config.set_d0;
	handle->set_d1 = config.set_d1;
	handle->set_d2 = config.set_d2;
	handle->set_d3 = config.set_d3;
	handle->set_d4 = config.set_d4;
	handle->set_d5 = config.set_d5;
	handle->set_d6 = config.set_d6;
	handle->set_d7 = config.set_d7;
	handle->i2c_send = config.i2c_send;
	handle->delay = config.delay;
	handle->write_cmd = write_cmd;
	handle->write_data = write_data;
	handle->wait = hd44780_wait;

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_config(hd44780_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	if (handle->comm_mode == HD44780_COMM_MODE_4BIT)
	{
		handle->set_rs(0);
		if (handle->set_rw != NULL)
		{
			handle->set_rw(0);
		}
		handle->set_en(0);
		handle->set_d4(0);
		handle->set_d5(0);
		handle->set_d6(0);
		handle->set_d7(0);
	}
	else if (handle->comm_mode == HD44780_COMM_MODE_8BIT)
	{
		/* 8 bit currently is not supported */
	}
	else
	{
		/* Nothing to do */
	}

	handle->write_cmd(handle, 0x02);
	handle->delay(100);
	handle->write_cmd(handle, 0x28);
	handle->delay(100);
	handle->write_cmd(handle, 0x06);
	handle->delay(100);
	handle->write_cmd(handle, 0x0C);
	handle->delay(100);
	handle->write_cmd(handle, 0x01);
	handle->delay(100);

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_clear(hd44780_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	ret = handle->write_cmd(handle, 0x01);
	if (ret != ERR_CODE_SUCCESS)
	{
		return ret;
	}

	handle->wait(handle);

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_home(hd44780_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	ret = handle->write_cmd(handle, 0x02);
	if (ret != ERR_CODE_SUCCESS)
	{
		return ret;
	}

	handle->wait(handle);

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_write_char(hd44780_handle_t handle, uint8_t chr)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	ret = handle->write_data(handle, chr);
	if (ret != ERR_CODE_SUCCESS)
	{
		return ret;
	}

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_write_string(hd44780_handle_t handle, uint8_t *str)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	while (*str) {
		ret = handle->write_data(handle, *str);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
		str++;
	}

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_write_int(hd44780_handle_t handle, int number)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	if (number < 0) {
		ret = handle->write_data(handle, '-');
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
		number *= -1;
	}

	int num_digit = 1;
	int temp = number;

	while (temp > 9) {
		num_digit++;
		temp /= 10;
	}

	uint8_t buf[num_digit];
	sprintf((char*)buf, "%d", number);

	for (int i = 0; i < num_digit; i++) {
		ret = handle->write_data(handle, buf[i]);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_write_float(hd44780_handle_t handle, float number, uint8_t precision)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	if (number < 0) {
		ret = handle->write_data(handle, '-');
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
		number *= -1;
	}

	int num_digit = 1;
	int temp = (int)number;

	while (temp > 9) {
		num_digit++;
		temp /= 10;
	}

	uint8_t buf[num_digit + 1 + precision];
	uint8_t float_format[7];

	sprintf((char*)float_format, "%%.%df", precision);
	sprintf((char*)buf, (const char*)float_format, number);

	for (int i = 0; i < (num_digit + 1 + precision); i++) {
		ret = handle->write_data(handle, buf[i]);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_gotoxy(hd44780_handle_t handle, uint8_t col, uint8_t row)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;
	if (row == 0) {
		ret = handle->write_cmd(handle, 0x80 + col);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}
	else if (row == 1) {
		ret = handle->write_cmd(handle, 0xC0 + col);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}
	else if (row == 2) {
		ret = handle->write_cmd(handle, 0x94 + col);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}
	else {
		ret = handle->write_cmd(handle, 0xD4 + col);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_shift_cursor_forward(hd44780_handle_t handle, uint8_t step)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	for (uint8_t i = 0; i < step; i++)
	{
		ret = handle->write_cmd(handle, 0x14);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}

	return ERR_CODE_SUCCESS;
}

err_code_t hd44780_shift_cursor_backward(hd44780_handle_t handle, uint8_t step)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	err_code_t ret;

	for (uint8_t i = 0; i < step; i++)
	{
		ret = handle->write_cmd(handle, 0x10);
		if (ret != ERR_CODE_SUCCESS)
		{
			return ret;
		}
	}

	return ERR_CODE_SUCCESS;
}