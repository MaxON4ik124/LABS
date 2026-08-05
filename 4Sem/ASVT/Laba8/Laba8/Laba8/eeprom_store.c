#include "eeprom_store.h"

#include <avr/eeprom.h>
#include <string.h>

static eeprom_layout_t EEMEM g_store;

static uint16_t clamp_block_size(uint16_t size)
{
    if (size < MIN_BLOCK_SIZE)
    {
        return MIN_BLOCK_SIZE;
    }
    if (size > MAX_BLOCK_SIZE)
    {
        return MAX_BLOCK_SIZE;
    }
    return size;
}

static void write_default_layout(void)
{
    eeprom_layout_t layout;
    uint8_t i;

    memset(&layout, 0, sizeof(layout));
    layout.magic = EEPROM_MAGIC;
    layout.version = EEPROM_VERSION;
    layout.block_size = DEFAULT_BLOCK_SIZE;
    layout.next_y_index = 0u;

    for (i = 0u; i < MAX_KEYS; ++i)
    {
        layout.keys[i].flags_raw = 0u;
    }

    eeprom_update_block((const void *)&layout, (void *)&g_store, sizeof(layout));
}

void store_init(void)
{
    uint16_t magic = eeprom_read_word(&g_store.magic);
    uint16_t version = eeprom_read_word(&g_store.version);
    uint16_t block_size;

    if ((magic != EEPROM_MAGIC) || (version != EEPROM_VERSION))
    {
        write_default_layout();
        return;
    }

    block_size = eeprom_read_word(&g_store.block_size);
    block_size = clamp_block_size(block_size);
    eeprom_update_word(&g_store.block_size, block_size);
}

uint16_t store_get_block_size(void)
{
    return clamp_block_size(eeprom_read_word(&g_store.block_size));
}

void store_set_block_size(uint16_t block_size)
{
    block_size = clamp_block_size(block_size);
    eeprom_update_word(&g_store.block_size, block_size);
}

int8_t store_add_key(uint16_t a, uint16_t p, uint16_t x)
{
    uint8_t i;
    key_record_t record;
    key_flags_t flags;

    /* Replace existing record with the same (a, p). */
    for (i = 0u; i < MAX_KEYS; ++i)
    {
        eeprom_read_block((void *)&record, (const void *)&g_store.keys[i], sizeof(record));
        flags.raw = record.flags_raw;
        if ((flags.bits.used != 0u) && (record.a == a) && (record.p == p))
        {
            record.x = x;
            eeprom_update_block((const void *)&record, (void *)&g_store.keys[i], sizeof(record));
            return (int8_t)i;
        }
    }

    for (i = 0u; i < MAX_KEYS; ++i)
    {
        eeprom_read_block((void *)&record, (const void *)&g_store.keys[i], sizeof(record));
        flags.raw = record.flags_raw;
        if (flags.bits.used == 0u)
        {
            record.a = a;
            record.p = p;
            record.x = x;
            flags.raw = 0u;
            flags.bits.used = 1u;
            record.flags_raw = flags.raw;
            eeprom_update_block((const void *)&record, (void *)&g_store.keys[i], sizeof(record));
            return (int8_t)i;
        }
    }

    return -1;
}

bool store_delete_key(uint8_t index)
{
    key_record_t record;

    if (index >= MAX_KEYS)
    {
        return false;
    }

    memset(&record, 0, sizeof(record));
    eeprom_update_block((const void *)&record, (void *)&g_store.keys[index], sizeof(record));
    return true;
}

int8_t store_find_key(uint16_t a, uint16_t p, key_record_t *out_record)
{
    uint8_t i;
    key_record_t record;
    key_flags_t flags;

    for (i = 0u; i < MAX_KEYS; ++i)
    {
        eeprom_read_block((void *)&record, (const void *)&g_store.keys[i], sizeof(record));
        flags.raw = record.flags_raw;
        if ((flags.bits.used != 0u) && (record.a == a) && (record.p == p))
        {
            if (out_record != NULL)
            {
                *out_record = record;
            }
            return (int8_t)i;
        }
    }

    return -1;
}

uint8_t store_list_keys(key_record_t *out_records, uint8_t max_records, uint8_t *out_indexes)
{
    uint8_t i;
    uint8_t count = 0u;
    key_record_t record;
    key_flags_t flags;

    for (i = 0u; (i < MAX_KEYS) && (count < max_records); ++i)
    {
        eeprom_read_block((void *)&record, (const void *)&g_store.keys[i], sizeof(record));
        flags.raw = record.flags_raw;
        if (flags.bits.used != 0u)
        {
            out_records[count] = record;
            if (out_indexes != NULL)
            {
                out_indexes[count] = i;
            }
            ++count;
        }
    }

    return count;
}

uint8_t store_get_next_y(void)
{
    static const uint8_t k_values[] = { 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u };
    uint8_t index = eeprom_read_byte(&g_store.next_y_index);
    uint8_t value = k_values[index & (uint8_t)(sizeof(k_values) - 1u)];

    index = (uint8_t)((index + 1u) & (uint8_t)(sizeof(k_values) - 1u));
    eeprom_update_byte(&g_store.next_y_index, index);
    return value;
}
