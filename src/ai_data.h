#ifndef H_AI_DATA
#define H_AI_DATA

//AI tasks:
//Spin randomly and shoot if it can hit the player
struct ai_fire_random {
	bool clockwise;
};
//Aim at the player's current position
struct ai_fire_current {
	uint8_t dummy;
};
//Aim at the player or at a point that can hit the player with a reflection
struct ai_fire_reflect {
	bool scan_dir; //0 = X, 1 = Y
	uint8_t scan_pos;
};
//Aim at the future position of a player, or a position that will reflect there
struct ai_fire_future {
	uint8_t dummy;
};

union ai_fire {
	struct ai_fire_random random;
	struct ai_fire_current current;
	struct ai_fire_reflect reflect;
	struct ai_fire_future future;
};



//Stay stationary
//struct ai_move_none {
//	uint8_t dummy;
//};
//Move randomly
struct ai_move_random {
	uint8_t cur_dir;
};
//Move towards the player
struct ai_move_toward {
	uint8_t dummy;
};
//Stay away from the player
struct ai_move_away {
	ufix_t target_x;
	ufix_t target_y;
};

union ai_move {
	//struct ai_move_none none;
	struct ai_move_random random;
	struct ai_move_toward toward;
	struct ai_move_away away;
};

//Move out of the path of bullets
//Stay away from mines
struct ai_general {
	uint8_t dummy;
};

#endif
