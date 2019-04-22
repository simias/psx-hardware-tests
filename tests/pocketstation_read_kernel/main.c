#include "utils.h"
#include "bios.h"
#include "irq.h"
#include "joy_mc.h"

void delay(void) {
  int i;

  for (i = 0; i < 10000; i++) {
    nop();
  }
}


uint8_t send_byte(uint8_t byte, bool expect_ack) {
  uint8_t r;

  joy_mc_tx(byte);

  joy_mc_set_ctrl(0x1013);

  if (expect_ack) {
    joy_mc_wait_for_ack();
  }

  r = joy_mc_wait_for_rx();

  return r;
}

int main() {
  unsigned block;

  bios_printf("Attempt to read kernel from PocketStation in slot 1\n");

  /* Mask all interrupts */
  irq_set_mask(0);

  joy_mc_reset();

  joy_mc_set_baudrate_div(0x88);

  joy_mc_set_mode(0x000d);

  joy_mc_set_ctrl(0x0000);
  delay();

  joy_mc_rx();

  /* We read 16k as 128 * 128B */
  for (block = 0; block < 128; block++) {
    uint32_t addr = 0x04000000UL + block * 128;
    unsigned i;

    joy_mc_set_ctrl(0x1003);
    delay();

    joy_mc_set_ctrl(0x0000);
    delay();


    send_byte(0x81, true); /* Memory card access */
    send_byte(0x5b, true); /* Execute function and transfer data */
    send_byte(0x01, true); /* Function Get or Set memory block */
    send_byte(0x00, true); /* Get params (5) */

    send_byte(addr,       true); /* Addr [7:0]   */
    send_byte(addr >> 8,  true); /* Addr [15:8]  */
    send_byte(addr >> 16, true); /* Addr [23:16] */
    send_byte(addr >> 24, true); /* Addr [32:24] */
    send_byte(0x80, true);       /* Len: 128 */

    send_byte(0x00, true); /* Get len (0x80) */

    for (i = 0; i < 0x80; i++) {
      uint8_t b = send_byte(0x00, i < 0x7f);

      bios_printf("%02x", b);
    }
    bios_printf("\n");
  }

  return 0;
}
