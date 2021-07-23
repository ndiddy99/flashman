/* Flashman: Alternative FlashBoy client
*  Copyright (C) 2021  Nathan Misner
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "flashboy.h"
#include "hidapi.h"

typedef struct hid_device_info hid_device_info;

#define FLASHBOY_VID (0x1781)
#define FLASHBOY_PID (0x09a2)

#define CMD_ERASE (0xA1)
#define CMD_DEVUNLOCK (0xA2)
#define CMD_STARTWRITE (0xB0)
#define CMD_WRITE1K (0xB4)

static hid_device *flashboy = NULL;

int fb_open(void) {
	hid_device_info *info = NULL;

	hid_init();
	info = hid_enumerate(FLASHBOY_VID, FLASHBOY_PID);
	if (info == NULL) {
		return 0;
	}
	setlocale(LC_ALL, ""); // needed because product_string is a wchar_t

	// search for the flashboy in the connected devices with the given ID
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
		return 0;
	}

	// open the flashboy
	flashboy = hid_open(FLASHBOY_VID, FLASHBOY_PID, serial);
	if (flashboy == NULL) {
		return 0;
	}

	return 1;
}

void fb_close() {
	hid_close(flashboy);
}

inline void clear_cmd(uint8_t *cmd) {
	for (int i = 0; i < 65; i++) {
		cmd[i] = 0;
	}
}

int fb_erase() {
	uint8_t cmd[65];
	uint8_t response;

	clear_cmd(cmd);
	cmd[1] = CMD_ERASE;
	hid_write(flashboy, cmd, 65);
	hid_read(flashboy, &response, 1);
	return response == CMD_ERASE;
}

int fb_write_start(uint8_t *rom_buff, int rom_size) {
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
		// write FF's (fastest byte to write)
		else if (i > rom_size) {
			memset(cmd + 1, 0xFF, 64);
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				hid_write(flashboy, cmd, 65);
			}
		}
		// write ROM data
		else {
			for (int j = i; j < (i + CHUNK_SIZE); j += 64) {
				memcpy(cmd + 1, &rom_buff[j], 64);
				hid_write(flashboy, cmd, 65);
			}
		}

		// check if sector wrote correctly
		uint8_t response;
		hid_read(flashboy, &response, 1);
		if (response != CMD_WRITE1K) {
			printf("Error writing!\n");
			return 0;
		}
		else {
			printf("%d\n", i);
		}

	}
	return 1;
}

int fb_write_end(uint8_t *rom_buff, int rom_size) {
	// tell the flashboy to start writing
	uint8_t cmd[65];
	clear_cmd(cmd);
	cmd[1] = CMD_STARTWRITE;
	hid_write(flashboy, cmd, 65);

	// write the rom in 1K chunks
	int rom_start = FLASHBOY_SIZE - rom_size;
	for (int i = 0; i < FLASHBOY_SIZE; i += CHUNK_SIZE) {
		clear_cmd(cmd);
		cmd[1] = CMD_WRITE1K;
		hid_write(flashboy, cmd, 65);

		// write FF's (fastest byte to write)
		if (i < rom_start) {
			memset(cmd + 1, 0xFF, 64);
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				hid_write(flashboy, cmd, 65);
			}
		}
		// write ROM data
		else {
			for (int j = i; j < (i + CHUNK_SIZE); j += 64) {
				memcpy(cmd + 1, &rom_buff[j - rom_start], 64);
				hid_write(flashboy, cmd, 65);
			}
		}

		// check if sector wrote correctly
		uint8_t response;
		hid_read(flashboy, &response, 1);
		if (response != CMD_WRITE1K) {
			printf("Error writing!\n");
			return 0;
		}
		else {
			printf("%d\n", i);
		}

	}
	return 1;
}