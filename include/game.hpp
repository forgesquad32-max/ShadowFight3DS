#pragma once
#include <3ds.h>
#include <cstdint>
namespace sf3ds {
struct Input { float move=0; bool jump=false,punch=false,kick=false,weapon=false,block=false,dodge=false,special=false,start=false,confirm=false; bool touchPunch=false,touchKick=false,touchBlock=false,touchWeapon=false,touchDodge=false,touchSpecial=false; };
class Game {
public: void init(); void update(const Input&); void render() const; void renderControls() const;
private:
 struct Fighter { float x=0,y=205,vy=0; int hp=100,energy=0,facing=1,attackTimer=0,cooldown=0,stun=0,invuln=0,attackType=0,combo=0,comboTimer=0; bool blocking=false,grounded=true; };
 struct Particle { float x=0,y=0,vx=0,vy=0; int life=0; u32 color=0; };
 enum class Screen:uint8_t{Title,Select,Fight,Result};
 Screen screen_=Screen::Title; Fighter player_{},rival_{}; Particle particles_[28]{};
 int selected_=0,round_=1,frame_=0,resultTimer_=0,shake_=0,hitFlash_=0; bool won_=false;
 void beginFight(); void startAttack(Fighter&,int); void updateFight(const Input&); void drawFighter(const Fighter&,bool) const; void spawnImpact(float,float,u32);
};
}