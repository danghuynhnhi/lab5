/*
 * auto.c
 *
 *  Created on: 2 thg 12, 2024
 *      Author: DELL
 */

#include "auto.h"
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

 int system_state = STATE_INIT;
 float adc_current_value = 0.0;
uint8_t rx_buffer_byte;
uint8_t rx_buffer[BUFFER_MAX_SIZE] = {0};
uint8_t rx_buffer_index = 0;
uint8_t rx_buffer_flag = 0;

static uint8_t command_flag = STATE_INIT;
static uint8_t command_data[CMD_MAX_SIZE] = {0};
static uint8_t command_index = 0;

static int compare_command(const uint8_t *input, const char *reference) {
    return strncmp((const char *)input, reference, CMD_MAX_SIZE) == 0;
} //so sánh


void process_command_fsm() {
    switch (system_state) {
    case STATE_INIT:
        if (rx_buffer_byte == '!') {
            system_state = STATE_READ;
        }
        break;

    case STATE_READ:
        if (rx_buffer_byte != '!' && rx_buffer_byte != '#') {
            command_data[command_index++] = rx_buffer_byte;
            if (command_index >= CMD_MAX_SIZE) {
                command_index = 0;
            }
        }

        if (rx_buffer_byte == '#') {
                   system_state = STATE_STOP;
                   command_index = 0;
               }
               break;

           case STATE_STOP:
               if (compare_command(command_data, "RST")) {
                   command_flag = CMD_RESET;
                   setTimer1(1);
               } else if (compare_command(command_data, "OK")) {
                   command_flag = CMD_CONFIRM;
               }
               memset(command_data, 0, CMD_MAX_SIZE);
               system_state = STATE_INIT;
               break;

           default:
               break;
           }
}
    void communication_fsm() {
        static char message_buffer[50] = {0};
        static float adc_value_cached = 0.0;
        static int adc_locked = 0;

        char formatted_string[50] = {0};

        switch (command_flag) {
        case CMD_RESET:
            if (!adc_locked) {
                adc_value_cached = HAL_ADC_GetValue(&hadc1) * 5.0f / 4096.0f;
                adc_locked = 1;
            }

            if (timer1_flag == 1) {
                int integer_part = (int)adc_value_cached;
                int fractional_part = (int)((adc_value_cached - integer_part) * 100);
                snprintf(message_buffer, sizeof(message_buffer), "!ADC=%d.%02d#\r\n", integer_part, fractional_part);
                HAL_UART_Transmit(&huart2, (uint8_t *)message_buffer, strlen(message_buffer), 500);
                setTimer1(300);
                timer1_flag = 0;
            }
            break;

        case CMD_CONFIRM:
            adc_locked = 0;
            HAL_UART_Transmit(&huart2, (uint8_t *)"STOP#\r\n", strlen("STOP#\r\n"), 500);
            command_flag = STATE_INIT;
            break;

           default:
               break;
           }
       }
