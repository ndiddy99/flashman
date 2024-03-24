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
#include <string.h>
#include <wchar.h>

#include "flashboy.h"
#include "hidapi.h"

typedef struct hid_device_info hid_device_info;

#define FLASHBOY_VID (0x1781)
#define FLASHBOY_PID (0x09a2)

#define CMD_SIZE (65)
#define CMD_ERASE (0xA1)
#define CMD_DEVUNLOCK (0xA2)
#define CMD_STARTWRITE (0xB0)
#define CMD_WRITE1K (0xB4)

static hid_device *flashboy = NULL;

int fb_open(void) {
	if (hid_init() < 0) {
        return 0;
    }

	// open the flashboy
	flashboy = hid_open(FLASHBOY_VID, FLASHBOY_PID, NULL);
	if (flashboy == NULL) {
        const wchar_t *error = hid_error(flashboy);
        if (error) { printf("Error: %ls\n", error); }
		return 0;
	}

	return 1;
}

void fb_close(void) {
	hid_close(flashboy);
}

int fb_erase(void) {
	uint8_t cmd[CMD_SIZE] = {0};
	uint8_t response;

	cmd[1] = CMD_ERASE;
	hid_write(flashboy, cmd, CMD_SIZE);
	hid_read(flashboy, &response, 1);
	return response == CMD_ERASE;
}

int fb_write_start(uint8_t *rom_buff, int rom_size) {
	// tell the flashboy to start writing
	uint8_t cmd[CMD_SIZE] = {0};
	cmd[1] = CMD_STARTWRITE;
	hid_write(flashboy, cmd, CMD_SIZE);

	// write the rom in 1K chunks
	for (int i = 0; i < FLASHBOY_SIZE; i += CHUNK_SIZE) {
		memset(cmd, 0, sizeof(cmd));
		cmd[1] = CMD_WRITE1K;
		hid_write(flashboy, cmd, CMD_SIZE);

		// write vector table
		if (i == (FLASHBOY_SIZE - CHUNK_SIZE)) {
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				memcpy(cmd + 1, &rom_buff[rom_size - CHUNK_SIZE + j], 64);
				hid_write(flashboy, cmd, CMD_SIZE);
			}
		}
		// write FF's (fastest byte to write)
		else if (i > rom_size) {
			memset(cmd + 1, 0xFF, 64);
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				hid_write(flashboy, cmd, CMD_SIZE);
			}
		}
		// write ROM data
		else {
			for (int j = i; j < (i + CHUNK_SIZE); j += 64) {
				memcpy(cmd + 1, &rom_buff[j], 64);
				hid_write(flashboy, cmd, CMD_SIZE);
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
	uint8_t cmd[CMD_SIZE] = {0};
	cmd[1] = CMD_STARTWRITE;
	hid_write(flashboy, cmd, CMD_SIZE);

	// write the rom in 1K chunks
	int rom_start = FLASHBOY_SIZE - rom_size;
	for (int i = 0; i < FLASHBOY_SIZE; i += CHUNK_SIZE) {
		memset(cmd, 0, CMD_SIZE);
		cmd[1] = CMD_WRITE1K;
		hid_write(flashboy, cmd, CMD_SIZE);

		// write FF's (fastest byte to write)
		if (i < rom_start) {
			memset(cmd + 1, 0xFF, 64);
			for (int j = 0; j < CHUNK_SIZE; j += 64) {
				hid_write(flashboy, cmd, CMD_SIZE);
			}
		}
		// write ROM data
		else {
			for (int j = i; j < (i + CHUNK_SIZE); j += 64) {
				memcpy(cmd + 1, &rom_buff[j - rom_start], 64);
				hid_write(flashboy, cmd, CMD_SIZE);
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
