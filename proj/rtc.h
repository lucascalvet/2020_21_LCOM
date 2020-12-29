#ifndef RTC_H
#define RTC_H

#include <stdbool.h>
#include <stdint.h>

#define RTC_IRQ 8
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_SECONDS 0x00
#define RTC_REG_MINUTES 0x02
#define RTC_REG_HOURS 0x04
#define RTC_REG_WEEK_DAY 0x06
#define RTC_REG_MONTH_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_C 0x0C
#define RTC_REG_D 0x0D
#define RTC_REG_B_UIE BIT(4)
#define RTC_REG_B_DM BIT(2)
#define RTC_REG_B_24_RANGE BIT(1)
#define RTC_HOUR_PM BIT(7)

typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t week_day;
  uint8_t month_day;
  uint8_t month;
  uint8_t year;
} rtc_time;

typedef struct {
  uint8_t reg_a;
  uint8_t reg_b;
  uint8_t reg_c;
  uint8_t reg_d;
} rtc_conf;

int(rtc_subscribe_int)(uint8_t *bit_no);

int(rtc_unsubscribe_int)();

int(rtc_enable_update_int)();

void (rtc_ih)();

int(rtc_get_time)(rtc_time *time);

void (rtc_print_time)(rtc_time time);

int(rtc_get_config)(rtc_conf *config);

int(rtc_set_config)(rtc_conf config);

void(rtc_ih)();
#endif
