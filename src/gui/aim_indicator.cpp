#include <cstdint>
#include "aim_indicator.h"
#include "../util/trig.h"
#include "../game.h"
#include "../util/profiler.h"
#include "../data/gfx/aim_dot.h"
#include "../graphics/graphics.h"
#include "../graphics/partial_redraw.h"

void draw_aim_dots() {
    profiler_start(aim_indicator);
    const uint8_t NUM_DOTS = 6;
    struct line_seg line;
    angle_t angle = game.player->barrel_rot;

    profiler_add(raycast);
    raycast(game.player->center_x(), game.player->center_y(), angle, game.tiles, &line);
    profiler_end(raycast);

    int dx = (line.x2 - line.x1) / (NUM_DOTS - 1);
    int dy = (line.y2 - line.y1) / (NUM_DOTS - 1);

    int x = line.x1;
    int y = line.y1;

    for(uint8_t dot = 0; dot < NUM_DOTS; dot++) {
        pdraw_TransparentSprite_NoClip(aim_dot, SCREEN_X(x) - aim_dot_width / 2, SCREEN_Y(y) - aim_dot_height / 2 - 10);
        x += dx;
        y += dy;
    }
    profiler_end(aim_indicator);
}
