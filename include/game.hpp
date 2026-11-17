#pragma once
#include <3ds.h>
#include <cstdint>

namespace sf3ds {
struct Input {
    float move = 0.0f;
    bool jump = false, punch = false, kick = false;
    bool weapon = false, block = false, dodge = false, special = false;
    bool start = false, confirm = false;
    bool touchPunch = false, touchKick = false, touchBlock = false;
    bool touchWeapon = false, touchDodge = false, touchSpecial = false;
};

class Game {
public:
    void init();
    void update(const Input& input);
    void render() const;
private:
    struct Fighter {
        float x = 0.0f, y = 205.0f, vy = 0.0f;
        int hp = 100, energy = 0, facing = 1;
        int attackTimer = 0, cooldown = 0, stun = 0, invuln = 0;
        int attackType = 0;
        bool blocking = false, grounded = true;
    };
    enum class Screen : uint8_t { Title, Select, Fight, Result };
    Screen screen_ = Screen::Title;
    Fighter player_{}, rival_{};
    int selected_ = 0, round_ = 1, frame_ = 0, resultTimer_ = 0;
    bool won_ = false;
    void beginFight();
    void startAttack(Fighter& fighter, int type);
    void updateFight(const Input& input);
    void drawFighter(const Fighter& fighter) const;
};
}
