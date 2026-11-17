#include "game.hpp"
#include <citro2d.h>
#include <algorithm>
#include <cmath>

namespace sf3ds {
namespace {
constexpr float FLOOR = 205.0f;
constexpr u32 INK = C2D_Color32(5, 5, 12, 255);
constexpr u32 RED = C2D_Color32(220, 42, 55, 255);
constexpr u32 GOLD = C2D_Color32(226, 177, 76, 255);
constexpr u32 GREY = C2D_Color32(70, 70, 86, 255);
constexpr u32 PALE = C2D_Color32(235, 225, 207, 255);
float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }
void rect(float x, float y, float w, float h, u32 color) {
    C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}
void circle(float x, float y, float radius, u32 color) {
    C2D_DrawCircleSolid(x, y, 0.5f, radius, color);
}
}

void Game::init() {
    screen_ = Screen::Title;
    round_ = 1;
    frame_ = 0;
}
void Game::beginFight() {
    player_ = Fighter{};
    rival_ = Fighter{};
    player_.x = 105.0f;
    rival_.x = 295.0f;
    rival_.hp = std::min(100 + round_ * 5, 160);
    screen_ = Screen::Fight;
}
void Game::startAttack(Fighter& f, int type) {
    if (f.cooldown || f.attackTimer || f.stun) return;
    if (type == 4 && f.energy < 50) return;
    f.attackType = type;
    f.attackTimer = type == 4 ? 22 : (type == 3 ? 16 : 12);
    f.cooldown = type == 4 ? 38 : 19;
    if (type == 4) f.energy -= 50;
}
void Game::update(const Input& in) {
    ++frame_;
    if (screen_ == Screen::Title) {
        if (in.confirm || in.start) screen_ = Screen::Select;
        return;
    }
    if (screen_ == Screen::Select) {
        if (in.move < -0.4f) selected_ = (selected_ + 4) % 5;
        if (in.move > 0.4f) selected_ = (selected_ + 1) % 5;
        if (in.confirm || in.start) beginFight();
        return;
    }
    if (in.start) {
        screen_ = Screen::Select;
        return;
    }
    if (screen_ == Screen::Result) {
        if (resultTimer_ > 0) --resultTimer_;
        if (in.confirm || resultTimer_ == 0) {
            if (won_) ++round_;
            screen_ = Screen::Select;
        }
        return;
    }
    updateFight(in);
}
void Game::updateFight(const Input& in) {
    auto tick = [](Fighter& f) {
        if (f.cooldown > 0) --f.cooldown;
        if (f.attackTimer > 0) --f.attackTimer;
        if (f.attackTimer == 0) f.attackType = 0;
        if (f.stun > 0) --f.stun;
        if (f.invuln > 0) --f.invuln;
        if (!f.grounded) {
            f.vy += 0.42f;
            f.y += f.vy;
            if (f.y >= FLOOR) { f.y = FLOOR; f.vy = 0; f.grounded = true; }
        }
    };
    tick(player_);
    tick(rival_);
    player_.blocking = in.block || in.touchBlock;
    if (!player_.stun && !player_.blocking) player_.x += in.move * 2.5f;
    player_.x = clampf(player_.x, 28.0f, 372.0f);
    if (player_.grounded && in.jump) { player_.vy = -7.0f; player_.grounded = false; }
    if ((in.dodge || in.touchDodge) && player_.invuln == 0) {
        player_.x = clampf(player_.x - player_.facing * 28.0f, 28.0f, 372.0f);
        player_.invuln = 18;
    }
    if (in.punch || in.touchPunch) startAttack(player_, 1);
    if (in.kick || in.touchKick) startAttack(player_, 2);
    if (in.weapon || in.touchWeapon) startAttack(player_, 3);
    if (in.special || in.touchSpecial) startAttack(player_, 4);

    player_.facing = player_.x < rival_.x ? 1 : -1;
    rival_.facing = rival_.x < player_.x ? 1 : -1;
    const float distance = std::fabs(player_.x - rival_.x);
    if (!rival_.stun) {
        if (distance > 70.0f) rival_.x += rival_.facing * 1.0f;
        else if (!rival_.cooldown && !rival_.attackTimer)
            startAttack(rival_, ((frame_ / 41) % 3) == 0 ? 3 : (((frame_ / 41) % 3) == 1 ? 2 : 1));
    }
    rival_.x = clampf(rival_.x, 28.0f, 372.0f);

    auto hit = [](Fighter& a, Fighter& b) {
        if (a.attackTimer < 5 || a.attackTimer > 11 || !a.attackType) return;
        const float range = a.attackType == 1 ? 43.0f : (a.attackType == 2 ? 58.0f : (a.attackType == 3 ? 72.0f : 82.0f));
        if (std::fabs(a.x - b.x) > range || b.invuln) return;
        if (a.facing != (b.x > a.x ? 1 : -1)) return;
        int damage = a.attackType == 1 ? 7 : (a.attackType == 2 ? 11 : (a.attackType == 3 ? 14 : 24));
        if (b.blocking) damage = std::max(1, damage / 4);
        b.hp = std::max(0, b.hp - damage);
        b.stun = b.blocking ? 3 : 9;
        b.x = clampf(b.x + a.facing * 9.0f, 28.0f, 372.0f);
        a.energy = std::min(100, a.energy + 13);
        a.attackTimer = 4;
    };
    hit(player_, rival_);
    hit(rival_, player_);
    if (player_.hp <= 0 || rival_.hp <= 0) {
        won_ = rival_.hp <= 0;
        resultTimer_ = 150;
        screen_ = Screen::Result;
    }
}
void Game::drawFighter(const Fighter& f) const {
    const u32 color = f.invuln && (frame_ % 4 < 2) ? PALE : INK;
    circle(f.x, f.y - 67.0f, 10.0f, color);
    rect(f.x - 8, f.y - 57, 16, 34, color);
    if (f.blocking) {
        rect(f.x + f.facing * 10, f.y - 52, 8, 27, color);
        rect(f.x - f.facing * 10, f.y - 49, 8, 24, color);
    } else if (f.attackTimer > 4) {
        rect(f.x + f.facing * 10, f.y - 51, 29, 7, color);
    } else {
        rect(f.x + f.facing * 9, f.y - 51, 22, 6, color);
        rect(f.x - f.facing * 14, f.y - 49, 20, 6, color);
    }
    rect(f.x - 10, f.y - 24, 7, 24, color);
    rect(f.x + 3, f.y - 24, 7, 24, color);
}
void Game::render() const {
    rect(0, 0, 400, 240, C2D_Color32(18, 17, 31, 255));
    rect(0, 120, 400, 84, C2D_Color32(28, 25, 42, 255));
    for (int i = 0; i < 8; ++i) {
        const float x = i * 58.0f - 12.0f;
        rect(x, 88 + (i % 3) * 8, 34, 116, C2D_Color32(11, 11, 22, 255));
    }
    rect(0, 205, 400, 35, C2D_Color32(44, 33, 36, 255));
    rect(0, 203, 400, 2, GOLD);
    if (screen_ == Screen::Title) {
        circle(200, 83, 25, INK);
        rect(187, 106, 26, 65, INK);
        rect(150, 132, 100, 8, INK);
        rect(55, 185, 290, 3, RED);
        return;
    }
    if (screen_ == Screen::Select) {
        for (int i = 0; i < 5; ++i) {
            rect(82, 42 + i * 31, 236, 22, i == selected_ ? RED : GREY);
            rect(94, 50 + i * 31, 100, 5, PALE);
        }
        return;
    }
    rect(18, 14, 164, 9, GREY);
    rect(18, 14, 164.0f * player_.hp / 100.0f, 9, RED);
    rect(218, 14, 164, 9, GREY);
    rect(218, 14, 164.0f * rival_.hp / 160.0f, 9, RED);
    rect(18, 27, player_.energy * 1.2f, 3, GOLD);
    drawFighter(player_);
    drawFighter(rival_);
    if (screen_ == Screen::Result) {
        rect(65, 82, 270, 65, C2D_Color32(12, 8, 12, 230));
        rect(90, 104, won_ ? 220 : 150, 5, won_ ? GOLD : RED);
    }
}
}
