#include "mdec.h"
#include "bios.h"
#include "utils.h"

#define MDEC_BASE        0x1f801820
#define MDEC_COMMAND     (MDEC_BASE + 0x0)
#define MDEC_DATA        (MDEC_BASE + 0x0)
#define MDEC_CONTROL     (MDEC_BASE + 0x4)
#define MDEC_STATUS      (MDEC_BASE + 0x4)

void mdec_reset(void) {
  write32(MDEC_CONTROL, 1UL << 31);
}

uint32_t mdec_status(void) {
  uint32_t status = read32(MDEC_STATUS);

  bios_printf("status: 0x%08x\n", status);

  return status;
}

void mdec_set_control(uint32_t ctrl) {
  write32(MDEC_CONTROL, ctrl);
}

bool mdec_data_fifo_empty(void) {
  return !!(mdec_status() & 1UL << 31);
}

bool mdec_cmd_fifo_full(void) {
  return !!(mdec_status() & 1UL << 30);
}

void mdec_write_command(uint32_t cmd) {
  while (mdec_cmd_fifo_full()) {
    ;
  }

  bios_printf("mdec_write_command(0x%08x)\n", cmd);

  write32(MDEC_COMMAND, cmd);
}

uint32_t mdec_read_data() {
  while (mdec_data_fifo_empty()) {
    ;
  }

  return read32(MDEC_DATA);
}

void mdec_set_quant_table(const uint8_t *table, bool with_color) {
  uint32_t cmd = 2U << 29;
  unsigned nwords;
  unsigned i;
  cmd |= (uint32_t)with_color;

  mdec_write_command(cmd);

  if (with_color) {
    nwords = 32;
  } else {
    nwords = 16;
  }

  for (i = 0; i < nwords; i++) {
    uint32_t q = 0;

    q |= (uint32_t)table[i * 4];
    q |= ((uint32_t)table[i * 4 + 1]) << 8;
    q |= ((uint32_t)table[i * 4 + 2]) << 16;
    q |= ((uint32_t)table[i * 4 + 3]) << 24;

    mdec_write_command(q);
  }
}

void mdec_set_idct_table(const int16_t *table) {
  uint32_t cmd = 3U << 29;
  unsigned i;

  mdec_write_command(cmd);

  for (i = 0; i < 32; i++) {
    uint32_t f = 0;

    f |= ((uint32_t)table[i * 2]) & 0xffff;
    f |= ((uint32_t)table[i * 2 + 1]) << 16;

    mdec_write_command(f);
  }
}

void mdec_decode(const uint16_t *data,
		 size_t len,
		 unsigned depth,
		 bool is_signed,
		 bool bit15set) {
  size_t len_words = (len + 1) / 2;
  uint32_t cmd = 1U << 29;

  cmd |= depth << 27;
  cmd |= ((uint32_t)is_signed) << 26;
  cmd |= ((uint32_t)bit15set) << 25;
  cmd |= len_words & 0xffff;

  mdec_write_command(cmd);

  while (len_words) {
    uint32_t d = *(data++);
    len_words--;

    if (len) {
      d |= ((uint32_t)(*(data++))) << 16;
    } else {
      // pad with "end of block"
      d |= 0xfe00U << 16;
    }

    mdec_write_command(d);
  }
}
