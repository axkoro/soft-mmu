#pragma once

#include <stdint.h>

extern uint8_t hd_mem[4194304];
extern uint8_t ra_mem[65536];

typedef struct {
  uint8_t present_bit;
  uint8_t dirty_bit;
  int8_t page_frame;
} page_table_entry;

extern page_table_entry page_table[1024];

extern const uint16_t page_size;
extern uint8_t num_pages_loaded;

uint16_t get_page_num(uint32_t virt_address);
uint16_t get_phys_address(uint32_t virt_address);
int8_t is_present(uint32_t virt_address);
int8_t is_mem_full();
void write_page_to_hd(uint16_t page_num);
void swap_page(uint16_t page_num);
void load_page_from_hd(uint32_t virt_address);
uint8_t get_data(uint32_t virt_address);
void set_data(uint32_t virt_address, uint8_t value);