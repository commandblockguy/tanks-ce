#ifndef TANKS_MINE_DETECTOR_H
#define TANKS_MINE_DETECTOR_H

#include <stdbool.h>
#include "../physics/collision.h"
#include "physicsbody.h"
#include "../fwd.h"

class MineDetector: public PhysicsBody {
public:
    MineDetector(Mine *mine);

    // Whether the tank that placed this detector has left
    bool primed;

    void process();
    void render(uint8_t layer);

    void handle_collision(PhysicsBody *other);
    void collide(Tank *tank);
    void collide(Shell *shell);
    void collide(Mine *mine);
    void collide(MineDetector *detector);
};

#endif //TANKS_MINE_DETECTOR_H
