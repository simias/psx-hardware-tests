#include "joy_mc.h"

#define JOY_MC_BASE      0x1f801040
#define JOY_MC_TX_DATA   (JOY_MC_BASE + 0x0)
#define JOY_MC_RX_DATA   (JOY_MC_BASE + 0x0)
#define JOY_MC_STAT      (JOY_MC_BASE + 0x4)
#define JOY_MC_MODE      (JOY_MC_BASE + 0x8)
#define JOY_MC_CTRL      (JOY_MC_BASE + 0xa)
#define JOY_MC_BAUD      (JOY_MC_BASE + 0xe)

void joy_mc_reset(void) {
  write16(JOY_MC_CTRL, 0x40);
}

void joy_mc_set_baudrate_div(uint16_t div) {
  write16(JOY_MC_BAUD, div);
}

void joy_mc_set_mode(uint16_t mode) {
  write16(JOY_MC_MODE, mode);
}

void joy_mc_set_ctrl(uint16_t ctrl) {
  write16(JOY_MC_CTRL, ctrl);
}

uint16_t joy_mc_ctrl(void) {
  return read16(JOY_MC_CTRL);
}

uint16_t joy_mc_stat(void) {
  return read16(JOY_MC_STAT);
}

void joy_mc_select(bool slot2) {
  uint16_t ctrl = joy_mc_ctrl();

  ctrl &= ~(1U << 13);
  ctrl |= (uint16_t)slot2 << 13;

  ctrl |= 2;

  joy_mc_set_ctrl(ctrl);
}

void joy_mc_txen(bool en) {
  uint16_t ctrl = joy_mc_ctrl();

  ctrl &= ~1U;
  ctrl |= (uint16_t)en;

  joy_mc_set_ctrl(ctrl);
}

void joy_mc_tx(uint8_t data) {
  write8(JOY_MC_TX_DATA, data);
}

uint8_t joy_mc_rx(void) {
  return read8(JOY_MC_RX_DATA);
}

uint8_t joy_mc_wait_for_rx(void) {
  while ((joy_mc_stat() & 2) == 0) {
    ;
  }

  return joy_mc_rx();
}

void joy_mc_wait_for_ack(void) {
  //while ((joy_mc_stat() & 0x80) == 0) {
  while ((joy_mc_stat() & 0x200) == 0) {
    ;
  }
}
