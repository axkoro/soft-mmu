#include "mmu.h"

#include <stdint.h>
#include <stdlib.h>
#include <sys/random.h>

/*
Specifications of the simulated system:

- Virtual Memory Size: 4,194,304 bytes (4 MB)
- RAM (Physical Memory) Size: 65,536 bytes (64 KB)
- Page Size: 4,096 bytes
- Number of Virtual Pages: 4,194,304 / 4,096 = 1,024 pages
- Number of Physical Frames: 65,536 / 4,096 = 16 frames
- Page Table: Contains 1,024 entries, each with a present bit, a dirty bit, and a frame number
- Swapping Strategy: Random replacement when the physical memory is full
*/

uint8_t hd_mem[4194304];
uint8_t ra_mem[65536];

page_table_entry page_table[1024];

const uint16_t page_size = 4096;
uint8_t num_pages_loaded = 0;

uint16_t get_page_num(
    uint32_t virt_address) {  // Extracts the 10-bit page number from the virtual address.
  return virt_address >> 12;
}

uint16_t get_page_offset(uint32_t virt_address) {
  return (uint16_t)virt_address & 4095;  // mask for offset: 0b0000111111111111
}

int8_t is_present(uint32_t virt_address) {
  return page_table[get_page_num(virt_address)].present_bit;
}

int8_t is_mem_full() { return (num_pages_loaded == 16); }

uint16_t get_phys_address(uint32_t virt_address) {
  if (!is_present(virt_address)) exit(-1);
  uint16_t page_num = get_page_num(virt_address);
  uint8_t frame = page_table[page_num].page_frame;
  uint16_t offset = get_page_offset(virt_address);
  return ((uint16_t)frame << 12) | offset;
}

void write_page_to_hd(uint16_t page_num) {
  uint8_t frame = page_table[page_num].page_frame;
  for (int i = 0; i < page_size; i++) {
    hd_mem[page_num * page_size + i] = ra_mem[frame * page_size + i];
  }
}

void swap_page(uint16_t page_num) {
  if (page_table[page_num].dirty_bit) {
    write_page_to_hd(page_num);
    page_table[page_num].dirty_bit = 0;
  }
  page_table[page_num].present_bit = 0;
}

void load_page_from_hd(uint32_t virt_address) {
  uint16_t page_num = get_page_num(virt_address);
  uint8_t frame;

  if (is_mem_full()) {
    frame = rand() % 16;              // Random swap strategy
    for (int i = 0; i < 1024; i++) {  // Find the page occupying the chosen frame and swap it out
      if (page_table[i].present_bit && (page_table[i].page_frame == frame)) {
        swap_page(i);
        break;
      }
    }
  } else {
    frame = num_pages_loaded;
    num_pages_loaded++;
  }

  page_table[page_num].page_frame = frame;
  page_table[page_num].present_bit = 1;
  page_table[page_num].dirty_bit = 0;

  for (int i = 0; i < page_size; i++) {
    ra_mem[frame * page_size + i] = hd_mem[page_num * page_size + i];
  }
}

uint8_t get_data(uint32_t virt_address) {
  if (!is_present(virt_address)) {
    load_page_from_hd(virt_address);
  }
  return ra_mem[get_phys_address(virt_address)];
}

void set_data(uint32_t virt_address, uint8_t value) {
  if (!is_present(virt_address)) {
    load_page_from_hd(virt_address);
  }
  ra_mem[get_phys_address(virt_address)] = value;
  page_table[get_page_num(virt_address)].dirty_bit = 1;
}