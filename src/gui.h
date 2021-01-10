#ifndef TANKS_GUI_H
#define TANKS_GUI_H

//amount of time in milliseconds the mission start screen displays
#define MISSION_START_TIME 3000
//Font size
#define MISSION_NUMBER_TEXT 3
#define ENEMY_TANK_TEXT 2

void display_scores(void); //Display high scores

void display_kill_counts(void);

void mission_start_screen(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void update_game_kill_counter(uint8_t kills, bool force);

void display_game_kill_counter(void);

void display_game_banner(uint8_t mission, uint8_t lives);

void bang(void);

#endif //TANKS_GUI_H
