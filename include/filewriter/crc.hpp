/*
**  CRC.H - header file for SNIPPETS CRC and checksum functions
*/

#ifndef CRC__HPP
#define CRC__HPP

#include <mlboard/core.hpp>

int crc32file(char *name, uint32_t *crc, long *charcnt);
uint32_t crc32buf(const char *buf, size_t len);
uint32_t masked_crc32c(const char *buf, size_t len);

#include "crc_impl.hpp"

#endif /* CRC__H */
