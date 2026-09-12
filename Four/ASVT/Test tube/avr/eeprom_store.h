#ifndef EEPROM_STORE_H
#define EEPROM_STORE_H

#include <stdint.h>
#include <stdbool.h>

#include "config.h"

typedef union
{
    struct
    {
        uint8_t used     : 1;
        uint8_t reserved : 7;
    } bits;
    uint8_t raw;
} key_flags_t;

typedef struct __attribute__((packed))
{
    uint16_t a;
    uint16_t p;
    uint16_t x;
    uint8_t flags_raw;
} key_record_t;

typedef struct __attribute__((packed))
{
    uint16_t magic;
    uint16_t version;
    uint16_t block_size;
    uint8_t next_y_index;
    uint8_t reserved[3];
    key_record_t keys[MAX_KEYS];
} eeprom_layout_t;

void store_init(void);
uint16_t store_get_block_size(void);
void store_set_block_size(uint16_t block_size);

int8_t store_add_key(uint16_t a, uint16_t p, uint16_t x);
bool store_delete_key(uint8_t index);
int8_t store_find_key(uint16_t a, uint16_t p, key_record_t *out_record);
uint8_t store_list_keys(key_record_t *out_records, uint8_t max_records, uint8_t *out_indexes);
uint8_t store_get_next_y(void);

#endif /* EEPROM_STORE_H */
