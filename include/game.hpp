#pragma once
#include <3ds.h>
#include <cstdint>
namespace sf3ds {
struct Input {
 float move=0; bool jump=false,punch=false,kick=false,weapon=false,block=false,dodge=false,special=false,start=false,confirm=false;
 bool touchPunch=false,touchKick=false,touchBlock=false,touchWeapon=false,touchDodge=false,touchSpecial=false;
 bool touchTap=false; int touchX=0,touchY=0;
};
class Game {
public: void init(); void update(const Input&); void render() const; void renderControls() const;
private:
 struct Fighter { float x=0,y=204,vy=0; int hp=100,maxHp=100,energy=0,facing=1,attackTimer=0,cooldown=0,stun=0,invuln=0,attackType=0,combo=0,comboTimer=0; bool blocking=false,grounded=true; };
 struct Particle { float x=0,y=0,vx=0,vy=0; int life=0; u32 color=0; };
 enum class Screen:uint8_t { Title,Hub,Fight,Result,Shop,Gear };
 enum class Battle:uint8_t { Story,Duel,Boss };
 Screen screen_=Screen::Title; Battle battle_=Battle::Story;
 Fighter player_{},rival_{}; Particle particles_[24]{};
 int frame_=0,resultTimer_=0,shake_=0,hitFlash_=0,level_=1,xp_=0,wins_=0,coins_=500,gems_=25,hubChoice_=0,shopChoice_=0,gearChoice_=0;
 int weapon_=0,armor_=0; bool weaponOwned_[5]{true,false,false,false,false}; bool armorOwned_[5]{true,false,false,false,false};
 bool won_=false; int rewardCoins_=0,rewardGems_=0;
 void beginFight(Battle); void startAttack(Fighter&,int); void updateFight(const Input&); void drawFighter(const Fighter&,bool) const; void spawnImpact(float,float,u32);
 void drawText(const char*,float,float,float,u32) const; void handleMenuTap(const Input&);
};
}