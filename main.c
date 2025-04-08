#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mmu.h"

int main(void) {  // this was given by the tutors
  puts("test driver_");
  uint8_t hd_mem_expected[4194304];
  srand(1);
  fflush(stdout);
  for (int i = 0; i < 4194304; i++) {
    uint8_t val = (uint8_t)rand();
    hd_mem[i] = val;
    hd_mem_expected[i] = val;
  }

  for (uint32_t i = 0; i < 1024; i++) {
    page_table[i].dirty_bit = 0;
    page_table[i].page_frame = -1;
    page_table[i].present_bit = 0;
  }

  uint32_t random_address = 4192425;
  uint8_t value = get_data(random_address);

  if (hd_mem[random_address] != value) {
    printf("ERROR at Address %u, Value %u != %u!\n", random_address, hd_mem[random_address], value);
  }

  value = get_data(random_address);

  if (hd_mem[random_address] != value) {
    printf("ERROR at Address %u, Value %u != %u!\n", random_address, hd_mem[random_address], value);
  }

  fflush(stdout);

  srand(3);
  for (uint32_t i = 0; i <= 1000; i++) {
    uint32_t random_address = rand() % 4194304;
    uint8_t value = get_data(random_address);
    if (hd_mem[random_address] != value) {
      printf("ERROR at Address %u, Value %u != %u!\n", random_address, hd_mem[random_address],
             value);
      for (uint32_t j = 0; j <= 1023; j++) {
        if (page_table[j].present_bit) {
          printf("i: %u, page_table[j].page_frame: %d\n", j, page_table[j].page_frame);
          fflush(stdout);
        }
      }
      exit(1);
    }
    fflush(stdout);
  }

  srand(3);
  for (uint32_t i = 0; i <= 100; i++) {
    uint32_t random_address = (rand() % 4095) * 7;
    uint8_t value = (uint8_t)(random_address >> 1);
    set_data(random_address, value);
    hd_mem_expected[random_address] = value;
  }

  srand(4);
  for (uint32_t i = 0; i <= 16; i++) {
    uint32_t random_address = rand() % 4194304;
    uint8_t value = get_data(random_address);
    if (hd_mem_expected[random_address] != value) {
      for (uint32_t j = 0; j <= 1023; j++) {
        if (page_table[j].present_bit) {
          printf("i: %u, page_table[j].page_frame: %d\n", j, page_table[j].page_frame);
          fflush(stdout);
        }
      }
      exit(2);
    }
    fflush(stdout);
  }

  srand(3);
  for (uint32_t i = 0; i <= 2500; i++) {
    uint32_t random_address = rand() % (4095 * 5);
    uint8_t value = get_data(random_address);
    if (hd_mem_expected[random_address] != value) {
      printf("ERROR at Address %u, Value %u != %u!\n", random_address,
             hd_mem_expected[random_address], value);
      for (uint32_t j = 0; j <= 1023; j++) {
        if (page_table[j].present_bit) {
          printf("i: %u, page_table[j].page_frame: %d\n", j, page_table[j].page_frame);
          fflush(stdout);
        }
      }
      exit(3);
    }
    fflush(stdout);
  }

  puts("test end");
  fflush(stdout);
  return EXIT_SUCCESS;
}