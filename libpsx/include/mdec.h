#ifndef _MDEC_H_
#define _MDEC_H_

#include "utils.h"

uint32_t mdec_status(void);
void mdec_reset(void);
bool mdec_data_fifo_empty(void);
bool mdec_cmd_fifo_full(void);
void mdec_write_command(uint32_t cmd);
uint32_t mdec_read_data();
void mdec_set_quant_table(const uint8_t *table, bool with_color);
void mdec_set_idct_table(const int16_t *table);

#define MDEC_DEPTH_4BPP  0
#define MDEC_DEPTH_8BPP  1
#define MDEC_DEPTH_24BPP 2
#define MDEC_DEPTH_15BPP 3

void mdec_decode(const uint16_t *data,
		 size_t len,
		 unsigned depth,
		 bool is_signed,
		 bool bit15set);

#define MDEC_CONTROL_DATA_IN_REQ_EN   (1UL << 30)
#define MDEC_CONTROL_DATA_OUT_REQ_EN  (1UL << 29)

void mdec_set_control(uint32_t ctrl);


#endif /* _MDEC_H_ */
