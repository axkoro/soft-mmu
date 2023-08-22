#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/random.h>


uint8_t hd_mem[4194304];
uint8_t ra_mem[65536];


struct seitentabellen_zeile {
	uint8_t present_bit;
	uint8_t dirty_bit;
	int8_t page_frame;
}seitentabelle[1024]; // 4194304 >> 12 = 1024


const uint16_t page_size = 4096;
uint8_t num_pages_loaded = 0;


uint16_t get_page_num(uint32_t virt_address) {
	/*
	* Extrahiert den 10-Bit-Teil der virtuellen Adresse, der die Seitennummer kodiert.
	*/

	return virt_address >> 12;
}

uint16_t ram_from_virt_address(uint32_t virt_address) {
	/**
	 * Wandelt eine virtuelle Adresse in eine physikalische Adresse um.
	 */

	uint16_t page_num = get_page_num(virt_address);
	uint8_t frame = seitentabelle[page_num].page_frame;
	
	if (frame < 0) { exit(-1); }

	uint16_t offset = virt_address & 4095; // 4095 = 0b0000111111111111

	return ((uint16_t) frame << 12) | offset;
}

int8_t check_present(uint32_t virt_address) {
	/**
	 * Wenn eine Seite im Arbeitsspeicher ist, gibt die Funktion "check_present" 1 zurück, sonst 0.
	 */

	return seitentabelle[get_page_num(virt_address)].present_bit;
}

int8_t is_mem_full() {
	/**
	 * Wenn der Speicher voll ist, gibt die Funktion 1 zurück.
	 */

	return (num_pages_loaded == 16);
}

void write_page_to_hd(uint16_t page_num) {
	/**
	 * Schreibt eine Seite zurück auf die HD.
	 */

	uint8_t frame = seitentabelle[page_num].page_frame; 

	for (int i = 0; i < page_size; i++) {
		hd_mem[page_num * page_size + i] = ra_mem[frame * page_size + i];
	}
}

void swap_page(uint16_t page_num) {
	/**
	 * Lagert eine Seite im Hauptspeicher in den Sekundärspeicher aus.
	 */

	if (seitentabelle[page_num].dirty_bit) {
		write_page_to_hd(page_num);
		seitentabelle[page_num].dirty_bit = 0;
	}

	seitentabelle[page_num].present_bit = 0;
}

void get_page_from_hd(uint32_t virt_address) {
	/**
	 * Lädt eine Seite von der Festplatte und speichert diese Daten im ra_mem (Arbeitsspeicher).
	 * Erstellt einen Seitentabelleneintrag.
	 * Wenn der Arbeitsspeicher voll ist, wird eine zufällige Seite ausgetauscht.
	 */

	uint16_t page_num = get_page_num(virt_address);
	uint8_t frame;
	
	if(is_mem_full()) {
		frame = rand() % 16; // Tauschstrategie: Random

		for (int i = 0; i < 1024; i++) { // Suche in Seitentabelle nach Page, die den Frame belegt und lagere sie aus
			if (seitentabelle[i].present_bit && (seitentabelle[i].page_frame == frame)) {
				swap_page(i);
				break;
			}
		}
	} else {
		frame = num_pages_loaded;
		num_pages_loaded++;
	}

	seitentabelle[page_num].page_frame = frame; // Tabelleneintrag aktualisieren
	seitentabelle[page_num].present_bit = 1;
	seitentabelle[page_num].dirty_bit = 0;

	for (int i = 0; i < page_size; i++) { // Page-Inhalt in Frame laden
		ra_mem[frame*page_size + i] = hd_mem[page_num*page_size + i];
	}
}

uint8_t get_data(uint32_t virt_address) {
	/**
	 * Gibt ein Byte aus dem Arbeitsspeicher zurück.
	 * Wenn die Seite nicht in dem Arbeitsspeicher vorhanden ist, wird sie zunächst geladen.
	 */

	if (!check_present(virt_address)) {
		get_page_from_hd(virt_address);
	}
	return ra_mem[ram_from_virt_address(virt_address)];
}

void set_data(uint32_t virt_address, uint8_t value) {
	/**
	 * Schreibt ein Byte in den Arbeitsspeicher.
	 * Wenn die Seite nicht in dem Arbeitsspeicher vorhanden ist, wird sie zunächst geladen.
	 */

	if (!check_present(virt_address)) {
		get_page_from_hd(virt_address);
	}
	ra_mem[ram_from_virt_address(virt_address)] = value;
	seitentabelle[get_page_num(virt_address)].dirty_bit = 1;
}


int main(void) {
		puts("test driver_");
	static uint8_t hd_mem_expected[4194304];
	srand(1);
	fflush(stdout);
	for(int i = 0; i < 4194304; i++) {
		//printf("%d\n",i);
		uint8_t val = (uint8_t)rand();
		hd_mem[i] = val;
		hd_mem_expected[i] = val;
	}

	for (uint32_t i = 0; i < 1024;i++) {
//		printf("%d\n",i);
		seitentabelle[i].dirty_bit = 0;
		seitentabelle[i].page_frame = -1;
		seitentabelle[i].present_bit = 0;
	}


	uint32_t zufallsadresse = 4192425;
	uint8_t value = get_data(zufallsadresse);
//	printf("value: %d\n", value);

	if(hd_mem[zufallsadresse] != value) {
		printf("ERROR_ at Address %d, Value %d =! %d!\n",zufallsadresse, hd_mem[zufallsadresse], value);
	}

	value = get_data(zufallsadresse);

	if(hd_mem[zufallsadresse] != value) {
			printf("ERROR_ at Address %d, Value %d =! %d!\n",zufallsadresse, hd_mem[zufallsadresse], value);

	}

//		printf("Address %d, Value %d =! %d!\n",zufallsadresse, hd_mem[zufallsadresse], value);


	srand(3);

	for (uint32_t i = 0; i <= 1000;i++) {
		uint32_t zufallsadresse = rand() % 4194304;//i * 4095 + 1;//rand() % 4194303
		uint8_t value = get_data(zufallsadresse);
		if(hd_mem[zufallsadresse] != value) {
			printf("ERROR_ at Address %d, Value %d =! %d!\n",zufallsadresse, hd_mem[zufallsadresse], value);
			for (uint32_t i = 0; i <= 1023;i++) {
				//printf("%d,%d-",i,seitentabelle[i].present_bit);
				if(seitentabelle[i].present_bit) {
					printf("i: %d, seitentabelle[i].page_frame %d\n", i, seitentabelle[i].page_frame);
				    fflush(stdout);
				}
			}
			exit(1);
		}
//		printf("i: %d data @ %u: %d hd value: %d\n",i,zufallsadresse, value, hd_mem[zufallsadresse]);
		fflush(stdout);
	}


	srand(3);

	for (uint32_t i = 0; i <= 100;i++) {
		uint32_t zufallsadresse = rand() % 4095 *7;
		uint8_t value = (uint8_t)zufallsadresse >> 1;
		set_data(zufallsadresse, value);
		hd_mem_expected[zufallsadresse] = value;
//		printf("i : %d set_data address: %d - %d value at ram: %d\n",i,zufallsadresse,(uint8_t)value, ra_mem[virt_2_ram_address(zufallsadresse)]);
	}



	srand(4);
	for (uint32_t i = 0; i <= 16;i++) {
		uint32_t zufallsadresse = rand() % 4194304;//i * 4095 + 1;//rand() % 4194303
		uint8_t value = get_data(zufallsadresse);
		if(hd_mem_expected[zufallsadresse] != value) {
//			printf("ERROR_ at Address %d, Value %d =! %d!\n",zufallsadresse, hd_mem[zufallsadresse], value);
			for (uint32_t i = 0; i <= 1023;i++) {
				//printf("%d,%d-",i,seitentabelle[i].present_bit);
				if(seitentabelle[i].present_bit) {
					printf("i: %d, seitentabelle[i].page_frame %d\n", i, seitentabelle[i].page_frame);
				    fflush(stdout);
				}
			}

			exit(2);
		}
//		printf("i: %d data @ %u: %d hd value: %d\n",i,zufallsadresse, value, hd_mem[zufallsadresse]);
		fflush(stdout);
	}

	srand(3);
	for (uint32_t i = 0; i <= 2500;i++) {
		uint32_t zufallsadresse = rand() % (4095 *5);//i * 4095 + 1;//rand() % 4194303
		uint8_t value = get_data(zufallsadresse);
		if(hd_mem_expected[zufallsadresse] != value ) {
			printf("ERROR_ at Address %d, Value %d =! %d!\n",zufallsadresse, hd_mem_expected[zufallsadresse], value);
			for (uint32_t i = 0; i <= 1023;i++) {
				//printf("%d,%d-",i,seitentabelle[i].present_bit);
				if(seitentabelle[i].present_bit) {
					printf("i: %d, seitentabelle[i].page_frame %d\n", i, seitentabelle[i].page_frame);
				    fflush(stdout);
				}
			}
			exit(3);
		}
//		printf("i: %d data @ %u: %d hd value: %d\n",i,zufallsadresse, value, hd_mem_expected[zufallsadresse]);
		fflush(stdout);
	}

	puts("test end");
	fflush(stdout);
	return EXIT_SUCCESS;
}