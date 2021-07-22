/* Flashman: Alternative FlashBoy client
* Copyright(C) 2021 Nathan Misner
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
* GNU General Public License for more details.
*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>

#include "hidapi.h"
typedef struct hid_device_info hid_device_info;

#define FLASHBOY_VID (0x1781)
#define FLASHBOY_PID (0x09a2)

#define FLASHBOY_SIZE (0x200000)
#define CHUNK_SIZE (0x400)

#define CMD_ERASE (0xA1)
#define CMD_STARTWRITE (0xB0)
#define CMD_WRITE1K (0xB4)

void clear_cmd(uint8_t *cmd) {
	for (int i = 0; i < 65; i++) {
		cmd[i] = 0;
	}
}

int erase(hid_device *flashboy) {
	uint8_t cmd[65];
	uint8_t response;

	clear_cmd(cmd);
	cmd[1] = CMD_ERASE;
	hid_write(flashboy, cmd, 65);
	hid_read(flashboy, &response, 1);
	return response == CMD_ERASE;
}

int write(hid_device *flashboy, FILE *rom_file) {
	// load the rom into memory
	fseek(rom_file, 0, SEEK_END);
	size_t rom_size = ftell(rom_file);
	rewind(rom_file);
	uint8_t *rom_buff = malloc(rom_size);
	if (rom_buff == NULL) {
		printf("Out of memory!\n");
		return 0;
	}
	fread(rom_buff, sizeof(uint8_t), rom_size, rom_file);

	// tell the flashboy to start writing
	uint8_t cmd[65];
	clear_cmd(cmd);
	cmd[1] = CMD_STARTWRITE;
	hid_write(flashboy, cmd, 65);

	// write the rom in 1K chunks
	for (int i = 0; i < FLASHBOY_SIZE; i += CHUNK_SIZE) {
		clear_cmd(cmd);
		cmd[1] = CMD_WRITE1K;
		hid_write(flashboy, cmd, 65);

		// write vector table
		if (i == (FLASHBOY_SIZE - CHUNK_SIZE)) {
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				memcpy(cmd + 1, &rom_buff[rom_size - CHUNK_SIZE + j], 64);
				hid_write(flashboy, cmd, 65);
			}
		}
		// write FF's (faster than mirroring)
		else if (i > (int)rom_size) {
			memset(cmd + 1, 0xFF, 64);
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				hid_write(flashboy, cmd, 65);
			}
		}
		else {
			for (int j = i; j < (i + CHUNK_SIZE); j += 64) {
				memcpy(cmd + 1, &rom_buff[j], 64);
				hid_write(flashboy, cmd, 65);
			}
		}

		uint8_t response;
		hid_read(flashboy, &response, 1);
		if (response != CMD_WRITE1K) {
			printf("Error writing!\n");
			free(rom_buff);
			return 0;
		}
		else {
			printf("%d\n", i);
		}

	}


	free(rom_buff);
	return 1;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: flashman rom.vb\n");
		return -1;
	}

	hid_init();

	hid_device_info *info = hid_enumerate(FLASHBOY_VID, FLASHBOY_PID);
	if (info == NULL) {
		printf("Flashboy not found.\n");
		return -1;
	}

	setlocale(LC_ALL, ""); // needed because product_string is a wchar_t

	wchar_t *serial = NULL;
	while (info) {
		printf("%ls\n", info->product_string);
		if (wcscmp(info->product_string, L"FlashBoy") == 0) {
			serial = info->serial_number;
			break;
		}
		info = info->next;
	}

	if (serial == NULL) {
		printf("Flashboy not found.\n");
		return -1;
	}

	printf("Flashboy found!\n");

	hid_device *flashboy = hid_open(FLASHBOY_VID, FLASHBOY_PID, serial);
	if (flashboy == NULL) {
		printf("Error opening Flashboy!\n");
		return -1;
	}

	FILE *rom_file = fopen(argv[1], "rb");
	if (rom_file == NULL) {
		printf("Error opening ROM file %s\n", argv[1]);
		return -1;
	}

	printf("Erasing, please wait...\n");
	if (!erase(flashboy)) {
		printf("Error erasing!\n");
		fclose(rom_file);
		hid_close(flashboy);
		return -1;
	}
	printf("Successfully erased Flashboy.\n");

	printf("Writing rom...\n");
	if (!write(flashboy, rom_file)) {
		printf("Error writing!\n");
		fclose(rom_file);
		hid_close(flashboy);
		return -1;
	}
	printf("Successfully wrote rom\n");

	hid_close(flashboy);
	return 0;
}