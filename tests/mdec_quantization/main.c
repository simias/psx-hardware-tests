#include "mdec.h"
#include "bios.h"

static const uint8_t quant_table[64] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
};

static const int16_t idct_table[64] = {
  0x4000, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
};

static const uint16_t macroblock[] = {
  0x0001,
  0xfe00
};

int main() {
  unsigned i;

  mdec_reset();

  mdec_set_quant_table(quant_table, false);
  mdec_set_idct_table(idct_table);
  mdec_decode(macroblock,
  	      ARRAY_SIZE(macroblock),
  	      MDEC_DEPTH_8BPP,
  	      true,
  	      false);

  for (i = 0; i < 32; i++) {
    bios_printf("%02d: 0x%08x\n", i, mdec_read_data());
  }

  bios_printf("Done!\n");
}
