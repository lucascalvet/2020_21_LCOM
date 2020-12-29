#include "rtc.h"
#include "utils.h"

int hook_id_rtc;

int(rtc_subscribe_int)(uint8_t *bit_no) {
  hook_id_rtc = 3;
  *bit_no = hook_id_rtc;
  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_rtc) != OK)
    return 1;
  return 0;
}

int(rtc_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id_rtc) != OK)
    return 1;
  return 0;
}

int(rtc_enable_update_int)() {
  uint8_t reg_b;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &reg_b) != OK) return 1;
  reg_b |= RTC_REG_B_UIE;
  if (sys_outb(RTC_DATA_REG, reg_b) != OK) return 1;
  return 0;
}

void (rtc_ih)() {
  uint8_t reg_c;
  sys_outb(RTC_ADDR_REG, RTC_REG_C);
  util_sys_inb(RTC_DATA_REG, &reg_c);
}

/**
 * @brief Gets the RTC current time and saves it to a rtc_time struct, in binary and 24 hour format
 * 
 * @param time pointer to a rtc_time struct
 * @return 0 upon success and non-zero otherwise
 */
int(rtc_get_time)(rtc_time *time) {
  uint8_t data;
  rtc_conf config;
  if (rtc_get_config(&config) != OK) return 1;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_SECONDS) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  if (config.reg_b & RTC_REG_B_DM) time->seconds = data;
  else {
    time->seconds = (data >> 4) * 10 + (data & 0x0f);
  }
  if (sys_outb(RTC_ADDR_REG, RTC_REG_MINUTES) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  if (config.reg_b & RTC_REG_B_DM) time->minutes = data;
  else {
    time->minutes = (data >> 4) * 10 + (data & 0x0f);
  }
  if (sys_outb(RTC_ADDR_REG, RTC_REG_HOURS) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  bool hour_pm = false;
  if (!(config.reg_b & RTC_REG_B_24_RANGE) && data & RTC_HOUR_PM) {
    hour_pm = true;
    data &= 0x7f;
  }
  if (config.reg_b & RTC_REG_B_DM) time->hours = data;
  else {
    time->hours = (data >> 4) * 10 + (data & 0x0f);
  }
  if (hour_pm && time->hours != 12) { //TODO: Haven't tested if it works correctly in all cases.
    time->hours += 12;
  }
  if (!hour_pm && !(config.reg_b & RTC_REG_B_24_RANGE) && time->hours == 12) time->hours = 0;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_WEEK_DAY) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  if (config.reg_b & RTC_REG_B_DM) time->week_day = data;
  else {
    time->week_day = (data >> 4) * 10 + (data & 0x0f);
  }
  if (sys_outb(RTC_ADDR_REG, RTC_REG_MONTH_DAY) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  if (config.reg_b & RTC_REG_B_DM) time->month_day = data;
  else {
    time->month_day = (data >> 4) * 10 + (data & 0x0f);
  }
  if (sys_outb(RTC_ADDR_REG, RTC_REG_MONTH) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  if (config.reg_b & RTC_REG_B_DM) time->month = data;
  else {
    time->month = (data >> 4) * 10 + (data & 0x0f);
  }
  if (sys_outb(RTC_ADDR_REG, RTC_REG_YEAR) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  if (config.reg_b & RTC_REG_B_DM) time->year = data;
  else {
    time->year = (data >> 4) * 10 + (data & 0x0f);
  }
  return 0;
}

/**
 * @brief Print a rtc_time struct in a readable way
 * 
 * @param time the rtc_time struct with the time to be printed
 */
void(rtc_print_time)(rtc_time time) {
  printf("%d:%d:%d %d/%d/%d\n", time.hours, time.minutes, time.seconds, time.month_day, time.month, time.year);
}

/**
 * @brief Gets the RTC configuration and saves it to a rtc_conf struct
 * 
 * @param config pointer to a rtc_conf struct
 * @return 0 upon success and non-zero otherwise
 */
int(rtc_get_config)(rtc_conf *config) {
  uint8_t data;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_A) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  config->reg_a = data;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  config->reg_b = data;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_C) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  config->reg_c = data;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_D) != OK) return 1;
  if (util_sys_inb(RTC_DATA_REG, &data) != OK) return 1;
  config->reg_d = data;
  return 0;
}

/**
 * @brief Sets the RTC configuration
 * 
 * @param config a rtc_conf struct with the configuration to be set
 * @return 0 upon success and non-zero otherwise
 */
int(rtc_set_config)(rtc_conf config) {
  if (sys_outb(RTC_ADDR_REG, RTC_REG_A) != OK) return 1;
  if (sys_outb(RTC_DATA_REG, config.reg_a) != OK) return 1;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK) return 1;
  if (sys_outb(RTC_DATA_REG, config.reg_b) != OK) return 1;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_C) != OK) return 1;
  if (sys_outb(RTC_DATA_REG, config.reg_c) != OK) return 1;
  if (sys_outb(RTC_ADDR_REG, RTC_REG_D) != OK) return 1;
  if (sys_outb(RTC_DATA_REG, config.reg_d) != OK) return 1;
  return 0;
}
