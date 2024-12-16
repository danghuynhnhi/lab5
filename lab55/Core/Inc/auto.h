/*
 * auto.h
 *
 *  Created on: 2 thg 12, 2024
 *      Author: DELL
 */

#ifndef INC_AUTO_H_
#define INC_AUTO_H_

#include "main.h"
#include "software_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATE_INIT 0

#define STATE_READ 1

#define STATE_STOP 2


#define CMD_RESET 8
#define CMD_CONFIRM 9

#define CMD_MAX_SIZE 3
#define BUFFER_MAX_SIZE 30

extern uint8_t rx_buffer_byte;
extern uint8_t rx_buffer[BUFFER_MAX_SIZE];
extern uint8_t rx_buffer_index;
extern uint8_t rx_buffer_flag;

void process_command_fsm();
void communication_fsm();

#endif /* INC_AUTO_H_ * */
