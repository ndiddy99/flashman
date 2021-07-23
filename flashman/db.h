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

#ifndef DB_H
#define DB_H
#include <stdint.h>

#define HEADER_OFFSET (0x220)

typedef enum {
	MODE_DB = 0,
	MODE_START,
	MODE_END,
} FLASH_MODE;

typedef struct {
	uint8_t name[20];
	int flash_mode;
} DB_ENTRY;

DB_ENTRY three_d_tetris = {
	"POLYGOBLOCK         ",
	MODE_END
};

DB_ENTRY bound_high = {
	"BOUND HIGH          ",
	MODE_START
};

DB_ENTRY galactic_pinball = {
	"GALACTIC PINBALL    ",
	MODE_END
};

DB_ENTRY golf = {
	"GOLF                ",
	MODE_END
};

DB_ENTRY insmouse = {
	{0xB2, 0xDD, 0xBD, 0xCF, 0xB3, 0xBD, 0x20, 0xC9, 0x20, 0xD4, 0xB6, 0xC0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
	MODE_START
};

DB_ENTRY jack_bros = {
	"JACK BROS           ",
	MODE_START
};

DB_ENTRY mario_clash = {
	"MARIO CLASH         ",
	MODE_END
};

DB_ENTRY mario_tennis = {
	"Mario's Tennis      ",
	MODE_END
};

DB_ENTRY nester_bowling = {
	"NESTER'S FUNKY BOWLI",
	MODE_END
};

DB_ENTRY panic_bomber = {
	"Panic Bomber        ",
	MODE_START
};

DB_ENTRY panic_bomber_jap = {
	{0xC4, 0xCB, 0xDE, 0xC0, 0xDE, 0xBE, 0x21, 0xCA, 0xDF, 0xC6, 0xCE, 0xDE, 0xDD, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
	MODE_START
};

DB_ENTRY red_alarm = {
	"REDALARM            ",
	MODE_START
};

DB_ENTRY sd_gundam = {
	{0x56, 0x42, 0x20, 0x53, 0x44, 0xB6, 0xDE, 0xDD, 0xC0, 0xDE, 0xD1, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
	MODE_END
};

DB_ENTRY space_invaders = {
	"SPACE INVADERS V.C. ",
	MODE_END
};

DB_ENTRY space_squash = {
	"SPACE SQUASH        ",
	MODE_START
};

DB_ENTRY teleroboxer = {
	"TELEROBOXER         ",
	MODE_END
};

DB_ENTRY vertical_force = {
	"VERTICAL FORCE      ",
	MODE_START
};

DB_ENTRY virtual_bowling = {
	"VIRTUAL BOWLING     ",
	MODE_END
};

DB_ENTRY virtual_fishing = {
	"VIRTUAL FISHING JPN ",
	MODE_START
};

DB_ENTRY virtual_lab = {
	{0xCA, 0xDE, 0x2D, 0xC1, 0xAC, 0xD9, 0x4C, 0x41, 0x42, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
	MODE_START
};

DB_ENTRY virtual_league = {
	"VIRTUAL LEAGUE BB   ",
	MODE_START
};

DB_ENTRY virtual_league_jap = {
	{0xCA, 0xDE, 0xB0, 0xC1, 0xAC, 0xD9, 0xCC, 0xDF, 0xDB, 0xD4, 0xB7, 0xAD, 0xB3, 0x27, 0x39, 0x35, 0x20, 0x20, 0x20, 0x20},
	MODE_START
};

// also works for nikochan battle because they forgot to change the header
DB_ENTRY v_tetris = {
	{0x56, 0xA5, 0x54, 0x45, 0x54, 0x52, 0x49, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
	MODE_END
};

DB_ENTRY wario = {
	{0x56, 0x42, 0x20, 0xDC, 0xD8, 0xB5, 0xD7, 0xDD, 0xC4, 0xDE, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},
	MODE_END
};

DB_ENTRY waterworld = {
	"WATERWORLD          ",
	MODE_START
};

DB_ENTRY *games[] = {
	&three_d_tetris,
	&bound_high,
	&galactic_pinball,
	&golf,
	&insmouse,
	&jack_bros,
	&mario_clash,
	&mario_tennis,
	&nester_bowling,
	&panic_bomber,
	&panic_bomber_jap,
	&red_alarm,
	&sd_gundam,
	&space_invaders,
	&space_squash,
	&teleroboxer,
	&vertical_force,
	&virtual_bowling,
	&virtual_fishing,
	&virtual_lab,
	&virtual_league,
	&virtual_league_jap,
	&v_tetris,
	&wario,
	&waterworld
};

#endif
