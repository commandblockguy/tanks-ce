#ifndef TANKS_GUI_H
#define TANKS_GUI_H

#include <cstdint>

#include <string.h>

//amount of time in milliseconds the mission start screen displays
#define MISSION_START_TIME 3000
//Font size
#define MISSION_NUMBER_TEXT 3
#define ENEMY_TANK_TEXT 2

void display_scores(); //Display high scores

void display_kill_counts();

void mission_start_screen(uint8_t mission, uint8_t lives, uint8_t num_tanks);

void extra_life_screen(uint8_t old_lives);

void update_game_kill_counter(uint8_t kills, bool force);

void display_game_kill_counter();

void display_game_banner(uint8_t mission, uint8_t lives);

uint8_t pause_menu();

void draw_aim_dots();

[[noreturn]] void error_screen(const char *error, const char *file, uint24_t line);

#define ERROR(msg) error_screen(msg, __FILE_NAME__, __LINE__)

#endif //TANKS_GUI_H
