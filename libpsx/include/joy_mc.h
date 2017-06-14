#ifndef _JOY_MC_H_
#define _JOY_MC_H_

#include "utils.h"

void joy_mc_reset(void);
void joy_mc_set_baudrate_div(uint16_t);
void joy_mc_set_mode(uint16_t);
void joy_mc_set_ctrl(uint16_t);
uint16_t joy_mc_ctrl(void);
uint16_t joy_mc_stat(void);

void joy_mc_select(bool);
void joy_mc_txen(bool);
void joy_mc_tx(uint8_t);
uint8_t joy_mc_rx(void);

uint8_t joy_mc_wait_for_rx(void);

void joy_mc_wait_for_ack(void);

#endif /* _JOY_MC_H_ */
