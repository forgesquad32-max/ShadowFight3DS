#include <3ds.h>
#include <citro2d.h>
#include <cstdio>
#include "game.hpp"

static sf3ds::Game game;

static sf3ds::Input readInput() {
    sf3ds::Input in{};
    hidScanInput();
    const u32 down = hidKeysDown();
    const u32 held = hidKeysHeld();
    circlePosition pad{};
    hidCircleRead(&pad);
    in.move = pad.dx / 156.0f;
    if (in.move > 1.0f) in.move = 1.0f;
    if (in.move < -1.0f) in.move = -1.0f;
    in.jump = (down & KEY_UP) != 0;
    in.punch = (down & KEY_A) != 0;
    in.kick = (down & KEY_B) != 0;
    in.weapon = (down & KEY_X) != 0;
    in.block = (held & KEY_Y) != 0;
    in.dodge = (down & KEY_L) != 0;
    in.special = (down & KEY_R) != 0;
    in.start = (down & KEY_START) != 0;
    in.confirm = in.punch;

    if (held & KEY_TOUCH) {
        touchPosition touch{};
        hidTouchRead(&touch);
        const int x = touch.px;
        const int y = touch.py;
        if (y > 105) {
            if (x < 54) in.touchDodge = true;
            else if (x < 108) in.touchPunch = true;
            else if (x < 162) in.touchKick = true;
            else if (x < 216) in.touchWeapon = true;
            else if (x < 270) in.touchBlock = true;
            else in.touchSpecial = true;
        }
    }
    return in;
}

int main(int, char**) {
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, nullptr);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    game.init();

    while (aptMainLoop()) {
        const sf3ds::Input input = readInput();
        game.update(input);
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_SceneBegin(top);
        game.render();
        C3D_FrameEnd(0);

        consoleClear();
        std::printf("\x1b[1;1H SHADOW FIGHT 3DS");
        std::printf("\x1b[3;1H Circle Pad: Move | UP: Jump");
        std::printf("\x1b[4;1H A: Punch  B: Kick  X: Weapon");
        std::printf("\x1b[5;1H Y: Block   L: Dodge R: Special");
        std::printf("\x1b[8;1H Touch controls: six zones");
        std::printf("\x1b[10;1H Dodge | Punch | Kick | Weapon");
        std::printf("\x1b[11;1H Block | Special");
        std::printf("\x1b[14;1H START: return to mode menu");
    }

    C2D_Fini();
    C3D_Fini();
    gfxExit();
    return 0;
}
