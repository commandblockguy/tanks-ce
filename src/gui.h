#ifndef TANKS_GUI_H
#define TANKS_GUI_H

//amount of time in milliseconds the mission start screen displays
#define MISSION_START_TIME 3000
//Font size
#define MISSION_NUMBER_TEXT 3
#define ENEMY_TANK_TEXT 2

void displayScores(void); //Display high scores

void displayKillCounts(void);

void missionStartScreen(uint8_t mission, uint8_t lives, uint8_t num_tanks); //Display the mission start screen

void updateGameKillCounter(uint8_t kills, bool force);
void displayGameKillCounter(void);
void displayGameBanner(uint8_t mission, uint8_t lives);

#endif //TANKS_GUI_H
