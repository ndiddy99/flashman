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

#ifndef FLASHBOY_H
#define FLASHBOY_H
#include <stdint.h>

#define FLASHBOY_SIZE (0x200000)
#define CHUNK_SIZE (0x400)

// opens the connected flashboy's usb hid connection.
// returns 1 for success, 0 for failure
int fb_open(void);

// closes the connected flashboy's usb connection.
void fb_close(void);

// erases the flashboy.
// returns 1 for success, 0 for failure
int fb_erase(void);

// writes the rom to the start of the flashboy's memory, adds the vector
// table to the last 1kb of memory. returns 1 for success, 0 for failure
int fb_write_start(uint8_t *rom_buff, int rom_size);

// writes the rom such that it's aligned with the end of the flashboy's
// memory. returns 1 for success, 0 for failure
int fb_write_end(uint8_t *rom_buff, int rom_size);

#endif
