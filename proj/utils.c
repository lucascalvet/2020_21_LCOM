#include <lcom/lcf.h>
#include <stdint.h>
#include "utils.h"

/**
 * @brief Get the least significant byte of a uint16_t
 * @param val the input value
 * @param lsb pointer to a uint8_t to be assigned the result
 * @return zero upon success, non zero otherwise
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) val;
  return 0;
}

/**
 * @brief Get the most significant byte of a uint16_t
 * @param val the input value
 * @param lsb pointer to a uint8_t to be assigned the result
 * @return zero upon success, non zero otherwise
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  val >>= 8;
  *msb = (uint8_t) val;
  return 0;
}

/**
 * @brief Call sys_inb with a uint8_t value
 * @param port the port to read from
 * @param value pointer to a uint8_t to be assigned the result
 * @return zero upon success, non zero otherwise
 */
int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t val;
  int ret = sys_inb(port, &val);
  *value = (uint8_t) val;
  count++;
  return ret;
}
