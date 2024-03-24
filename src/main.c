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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "db.h"
#include "flashboy.h"

void print_usage() {
	printf("Usage: flashman rom.vb\n");
	printf("Optional parameters:\n");
	printf("-s: forces write to front of flash (used by some third-party games)\n");
	printf("-e: forces write to end of flash (used by Nintendo games and some third-party)\n");
	printf("By default, Flashman uses an internal database of VB games to choose which\n");
	printf("option to use.\n");
}

int db_lookup(uint8_t *rom_buff, int rom_len) {
	uint8_t *header = rom_buff + rom_len - HEADER_OFFSET;
	printf("%s\n", header);

	for (int i = 0; i < (sizeof(games) / sizeof(games[0])); i++) {
		if (!memcmp(header, games[i]->name, 20)) {
			printf("Game mode: %d\n", games[i]->flash_mode);
			return games[i]->flash_mode;
		}
	}

	// used by most homebrew games
	return MODE_START;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		print_usage();
		return -1;
	}

	int flash_mode = MODE_DB;
	char *filename = NULL;

	for (int i = 1; i < argc; i++) {
		switch (*(argv[i] + 1)) {
		case 's':
		case 'S':
			flash_mode = MODE_START;
			break;

		case 'e':
		case 'E':
			flash_mode = MODE_END;
			break;

		default:
			filename = argv[i];
			break;
		}
	}

	if (filename == NULL) {
		print_usage();
		return -1;
	}

	// open the rom
	FILE *rom_file = fopen(filename, "rb");
	if (rom_file == NULL) {
		printf("Error opening ROM file %s\n", filename);
		return -1;
	}
	fseek(rom_file, 0, SEEK_END);
	int rom_size = (int)ftell(rom_file);
	rewind(rom_file);
	uint8_t *rom_buff = malloc(rom_size);
	if (rom_buff == NULL) {
		printf("Out of memory!\n");
		return -1;
	}
	fread(rom_buff, sizeof(uint8_t), rom_size, rom_file);
	fclose(rom_file);

	// open the flashboy
	if (!fb_open()) {
		printf("Error opening Flashboy!\n");
		goto err_fb_open;
	}
	printf("Opened Flashboy!\n");

	// erase the rom
	printf("Erasing...\n");
	if (!fb_erase()) {
		printf("Error erasing Flashboy!\n");
		goto err_exit;
	}

	// write the rom
	if (flash_mode == MODE_DB) {
		flash_mode = db_lookup(rom_buff, rom_size);
	}
	printf("Writing (mode %d)...\n", flash_mode);
	if (flash_mode == 1) {
		if (!fb_write_start(rom_buff, rom_size)) {
			printf("Error writing!\n");
			goto err_exit;
		}
	}
	else {
		if (!fb_write_end(rom_buff, rom_size)) {
			printf("Error writing!\n");
			goto err_exit;
		}
	}

	printf("Successfully wrote ROM.\n");
	fb_close();
	free(rom_buff);
	return 0;

err_exit:
	fb_close();
err_fb_open:
	free(rom_buff);
	return -1;
}
