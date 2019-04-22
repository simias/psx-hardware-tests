#include "utils.h"
#include "bios.h"
#include "irq.h"
#include "joy_mc.h"

/*
 * Get Memory Card ID Command
 *
 * Send Reply Comment
 * 81h  N/A   Memory Card Access (unlike 01h=Controller access), dummy response
 * 53h  FLAG  Send Get ID Command (ASCII "S"), Receive FLAG Byte
 * 00h  5Ah   Receive Memory Card ID1
 * 00h  5Dh   Receive Memory Card ID2
 * 00h  5Ch   Receive Command Acknowledge 1
 * 00h  5Dh   Receive Command Acknowledge 2
 * 00h  04h   Receive 04h
 * 00h  00h   Receive 00h
 * 00h  00h   Receive 00h
 * 00h  80h   Receive 80h
 */
static const uint8_t command[] = {
  0x81,
  0x53,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
};

void delay(void) {
  int i;

  for (i = 0; i < 10000; i++) {
    nop();
  }
}


uint8_t send_byte(uint8_t byte, bool expect_ack) {
  uint8_t r;

  bios_printf("Send %02x\n", byte);

  joy_mc_tx(byte);

  joy_mc_set_ctrl(0x1013);

  if (expect_ack) {
    joy_mc_wait_for_ack();

    bios_printf("Got ack\n");
  }

  r = joy_mc_wait_for_rx();

  bios_printf("Rx: %02x\n", r);

  return r;
}

int main() {
  unsigned nbytes = ARRAY_SIZE(command);
  unsigned i;

  bios_printf("Attempt to read kernel from PocketStation in slot 1\n");

  /* Mask all interrupts */
  irq_set_mask(0);

  joy_mc_reset();

  joy_mc_set_baudrate_div(0x88);

  joy_mc_set_mode(0x000d);

  joy_mc_set_ctrl(0x0000);
  delay();

  joy_mc_rx();

  //joy_mc_txen(true);


  for (i = 0; i < nbytes; i++) {
    send_byte(command[i], i < (nbytes - 1));
  }

  return 0;
}
