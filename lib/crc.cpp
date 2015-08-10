#include <stddef.h>
#include <stdint.h>

uint16_t crc_16(const uint8_t * data, size_t length, uint16_t crc, uint16_t poly) {
    for (size_t ibyte = 0; ibyte < length; ibyte++) {

        crc ^= ((uint16_t)data[ibyte]) << 8;

        for (uint8_t ibit=0; ibit<8; ibit++) {
            if(crc & 0x8000) {
                crc = (crc << 1) ^ poly;
            }else{
                crc <<= 1;
            }
        }

    }
    return crc;

}

