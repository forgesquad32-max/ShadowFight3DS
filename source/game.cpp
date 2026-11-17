#include "game.hpp"
#include <citro2d.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
namespace sf3ds { namespace {
constexpr float FLOOR=204;
constexpr u32 BG=C2D_Color32(12,16,31,255),PANEL=C2D_Color32(26,33,53,255),PANEL2=C2D_Color32(38,47,70,255),INK=C2D_Color32(8,10,18,255),RED=C2D_Color32(224,62,72,255),GOLD=C2D_Color32(255,198,91,255),CYAN=C2D_Color32(83,210,221,255),PALE=C2D_Color32(244,236,220,255),GREEN=C2D_Color32(100,203,139,255),MUTED=C2D_Color32(150,160,181,255);
C2D_TextBuf textBuf=nullptr;
float clampf(float v,float a,float b){return std::max(a,std::min(b,v));}
void rect(float x,float y,float w,float h,u32 c){C2D_DrawRectSolid(x,y,.5f,w,h,c);}
void circle(float x,float y,float r,u32 c){C2D_DrawCircleSolid(x,y,.5f,r,c);}
void bar(float x,float y,float w,float h,int v,int m,u32 c){rect(x,y,w,h,C2D_Color32(48,52,67,255));rect(x+2,y+2,(w-4)*clampf((float)v/(m?m:1),0,1),h-4,c);}
const char* weapons[]={"Training Blade","Moonsteel Katana","Stormbreaker","Dragon Fang","Void Reaper"};
const int weaponCost[]={0,350,900,1800,35};
const char* armors[]={"Rookie Wraps","Ronin Armor","Iron Guard","Dragon Scale","Wraith Suit"};
const int armorCost[]={0,300,800,1600,30};
const char* stages[]={"Ashen Courtyard","Bamboo Crossing","Sunken Temple","Crimson Pass","The Iron Citadel","Frost Monastery","Ember Fortress","Warlord's Keep","Nightfall Bridge","The Shadow Throne"};
}
void Game::drawText(const char*s,float x,float y,float scale,u32 color)const{
 if(!textBuf)return;C2D_Text t{};C2D_TextParse(&t,textBuf,s);C2D_DrawText(&t,C2D_WithColor,x,y,0,scale,scale,color);
}
void Game::init(){screen_=Screen::Title;frame_=0;level_=1;xp_=0;coins_=500;gems_=25;wins_=0;weapon_=armor_=0;if(!textBuf)textBuf=C2D_TextBufNew(4096);}
void Game::beginFight(Battle b){
 battle_=b;player_=Fighter{};rival_=Fighter{};player_.x=108;rival_.x=292;
 int hp=100+level_*7+(b==Battle::Boss?100:0);rival_.hp=rival_.maxHp=hp;
 player_.maxHp=100+armor_*8;player_.hp=player_.maxHp;screen_=Screen::Fight;
 for(auto&p:particles_)p.life=0;
}
void Game::startAttack(Fighter&f,int t){if(f.cooldown||f.attackTimer||f.stun||f.blocking)return;if(t==4&&f.energy<50)return;f.attackType=t;f.attackTimer=t==4?25:(t==3?18:(t==2?15:12));f.cooldown=t==4?42:(t==3?25:(t==2?21:15));if(t==4)f.energy-=50;}
void Game::spawnImpact(float x,float y,u32 c){int s=0;for(int i=0;i<24;i++)if(!particles_[i].life){s=i;break;}float a=(float)((frame_*37+s*71)%360)*.0174533f;particles_[s]={x,y,std::cos(a)*(1+s%3),std::sin(a)*(1+s%3)-1,13,c};}
void Game::handleMenuTap(const Input&in){
 if(!in.touchTap)return;int x=in.touchX,y=in.touchY;
 if(screen_==Screen::Hub){if(y>177){if(x<64)hubChoice_=0;else if(x<128)hubChoice_=1;else if(x<192)hubChoice_=2;else if(x<256)hubChoice_=3;else hubChoice_=4;
  if(hubChoice_==0)beginFight(Battle::Story);else if(hubChoice_==1)beginFight(Battle::Duel);else if(hubChoice_==2)beginFight(Battle::Boss);else if(hubChoice_==3){shopChoice_=0;screen_=Screen::Shop;}else{gearChoice_=0;screen_=Screen::Gear;}return;}
  if(y>92&&y<166){if(x<160)beginFight(Battle::Story);else beginFight(Battle::Duel);}
 }else if(screen_==Screen::Shop){if(y>216){screen_=Screen::Hub;return;}if(y<96||y>211)return;shopChoice_=clampf((y-96)/23,0,4);int i=shopChoice_;if(i>0&&!weaponOwned_[i]){if(i==4){if(gems_>=weaponCost[i]){gems_-=weaponCost[i];weaponOwned_[i]=true;}}else if(coins_>=weaponCost[i]){coins_-=weaponCost[i];weaponOwned_[i]=true;}}else if(weaponOwned_[i])weapon_=i;
 }else if(screen_==Screen::Gear){if(y>216){screen_=Screen::Hub;return;}if(y<96||y>211)return;gearChoice_=clampf((y-96)/23,0,4);int i=gearChoice_;if(armorOwned_[i])armor_=i;else if(i==4){if(gems_>=armorCost[i]){gems_-=armorCost[i];armorOwned_[i]=true;armor_=i;}}else if(coins_>=armorCost[i]){coins_-=armorCost[i];armorOwned_[i]=true;armor_=i;}}
}
void Game::update(const Input&in){
 ++frame_;if(shake_)--shake_;if(hitFlash_)--hitFlash_;
 for(auto&p:particles_)if(p.life){p.x+=p.vx;p.y+=p.vy;p.vy+=.12f;--p.life;}
 if(screen_==Screen::Title){if(in.confirm||in.start||in.touchTap)screen_=Screen::Hub;return;}
 if(screen_==Screen::Hub||screen_==Screen::Shop||screen_==Screen::Gear){handleMenuTap(in);if(screen_==Screen::Hub&&in.confirm){if(hubChoice_==0)beginFight(Battle::Story);else if(hubChoice_==1)beginFight(Battle::Duel);else if(hubChoice_==2)beginFight(Battle::Boss);else if(hubChoice_==3)screen_=Screen::Shop;else screen_=Screen::Gear;}if(in.start)screen_=Screen::Hub;return;}
 if(screen_==Screen::Result){if(resultTimer_)--resultTimer_;if(in.confirm||in.touchTap||!resultTimer_)screen_=Screen::Hub;return;}
 if(in.start){screen_=Screen::Hub;return;}updateFight(in);
}
void Game::updateFight(const Input&in){
 auto tick=[](Fighter&f){if(f.cooldown)--f.cooldown;if(f.attackTimer)--f.attackTimer;if(!f.attackTimer)f.attackType=0;if(f.stun)--f.stun;if(f.invuln)--f.invuln;if(f.comboTimer)--f.comboTimer;else f.combo=0;if(!f.grounded){f.vy+=.38f;f.y+=f.vy;if(f.y>=FLOOR){f.y=FLOOR;f.vy=0;f.grounded=true;}}};tick(player_);tick(rival_);
 player_.blocking=(in.block||in.touchBlock)&&player_.grounded;if(!player_.stun&&!player_.blocking)player_.x+=in.move*(player_.attackTimer?1:3.15f);player_.x=clampf(player_.x,27,373);
 if(player_.grounded&&in.jump&&!player_.stun){player_.vy=-7.6f;player_.grounded=false;}
 if((in.dodge||in.touchDodge)&&!player_.invuln&&!player_.stun){player_.x=clampf(player_.x-player_.facing*35,27,373);player_.invuln=22;}
 if(in.punch||in.touchPunch)startAttack(player_,1);if(in.kick||in.touchKick)startAttack(player_,2);if(in.weapon||in.touchWeapon)startAttack(player_,3);if(in.special||in.touchSpecial)startAttack(player_,4);
 player_.facing=player_.x<rival_.x?1:-1;rival_.facing=rival_.x<player_.x?1:-1;float d=std::fabs(player_.x-rival_.x);
 if(!rival_.stun){if(d>78)rival_.x+=rival_.facing*(battle_==Battle::Boss?1.55f:1.1f);else if(!rival_.cooldown&&!rival_.attackTimer){int p=(frame_/31+level_)%5;if(d<48&&p==0)rival_.blocking=true;else{rival_.blocking=false;startAttack(rival_,p==1?3:(p==2?2:1));}}}
 if(battle_==Battle::Boss&&frame_%155<18&&d<100)rival_.blocking=true;else if(frame_%155>=18)rival_.blocking=false;
 rival_.x=clampf(rival_.x,27,373);
 auto hit=[this](Fighter&a,Fighter&b,bool human){if(a.attackTimer<5||a.attackTimer>(a.attackType==4?21:13)||!a.attackType)return;float range=a.attackType==1?49:(a.attackType==2?66:(a.attackType==3?79:94));if(std::fabs(a.x-b.x)>range||b.invuln||a.facing!=(b.x>a.x?1:-1))return;int dmg=a.attackType==1?8:(a.attackType==2?13:(a.attackType==3?17:29));if(human)dmg+=weapon_*3;else dmg=std::max(1,dmg-armor_*2);if(b.blocking)dmg=std::max(1,dmg/5);b.hp=std::max(0,b.hp-dmg);b.stun=b.blocking?4:11;b.x=clampf(b.x+a.facing*(b.blocking?4:12),27,373);a.energy=std::min(100,a.energy+(a.attackType==4?5:15));if(human){++a.combo;a.comboTimer=90;}else a.combo=0;spawnImpact((a.x+b.x)*.5f,b.y-43,b.blocking?CYAN:GOLD);shake_=b.blocking?2:5;hitFlash_=3;a.attackTimer=3;};
 hit(player_,rival_,true);hit(rival_,player_,false);
 if(player_.hp<=0||rival_.hp<=0){won_=rival_.hp<=0;rewardCoins_=won_?(battle_==Battle::Boss?350: battle_==Battle::Duel?90:65):20;rewardGems_=won_&&battle_==Battle::Boss?3:0;if(won_){coins_+=rewardCoins_;gems_+=rewardGems_;xp_+=battle_==Battle::Boss?150:60;++wins_;if(xp_>=level_*100){xp_-=level_*100;++level_;}}resultTimer_=200;screen_=Screen::Result;}
}
void Game::drawFighter(const Fighter&f,bool enemy)const{
 float x=f.x,y=f.y,face=(float)f.facing,bob=f.grounded?std::sin(frame_*.12f)*1.3f:0;
 u32 cloth=enemy?C2D_Color32(112,31,48,255):C2D_Color32(28,76,105,255);
 u32 plate=enemy?C2D_Color32(178,70,55,255):C2D_Color32(183,193,205,255);
 u32 trim=enemy?C2D_Color32(250,155,69,255):C2D_Color32(91,220,222,255);
 if(armor_>=2&&!enemy)plate=armor_==4?C2D_Color32(99,79,142,255):C2D_Color32(195,155,85,255);
 float lean=f.attackTimer>5?face*5:0;
 circle(x,y+3,20,C2D_Color32(4,5,10,110));
 // boots, layered trousers and split tunic
 rect(x-13+lean,y-24+bob,11,22,INK);rect(x+3+lean,y-24+bob,11,22,INK);
 rect(x-14+lean,y-7+bob,13,7,plate);rect(x+2+lean,y-7+bob,13,7,plate);
 rect(x-11+lean,y-48+bob,22,27,cloth);
 rect(x-16+lean,y-39+bob,13,19,cloth);rect(x+3+lean,y-39+bob,13,19,cloth);
 rect(x-13+lean,y-25+bob,26,5,trim);
 // armored chest, collar, shoulders, forearms
 rect(x-11+lean,y-58+bob,22,28,plate);rect(x-7+lean,y-55+bob,14,20,cloth);
 rect(x-8+lean,y-60+bob,16,5,trim);
 circle(x-face*12+lean,y-54+bob,7,plate);circle(x+face*12+lean,y-54+bob,7,plate);
 rect(x+face*8+lean,y-51+bob,7,22,cloth);rect(x+face*12+lean,y-33+bob,8,7,plate);
 rect(x-face*14+lean,y-50+bob,7,20,cloth);rect(x-face*17+lean,y-34+bob,8,7,plate);
 // head, mask, headband and tied hair
 circle(x+lean,y-72+bob,10,PALE);rect(x-10+lean,y-77+bob,20,6,cloth);
 rect(x-8+lean,y-72+bob,16,7,INK);rect(x+face*3+lean,y-70+bob,3,2,GOLD);
 rect(x-face*7+lean,y-77+bob,12,3,trim);rect(x-face*15+lean,y-79+bob,11,3,trim);
 // equipped weapon is visibly held during weapon/special attacks
 if(f.attackTimer>4&&f.attackType>=3){rect(x+face*17,y-60,35,4,GOLD);rect(x+face*49,y-64,4,12,trim);rect(x+face*15,y-63,7,11,INK);}
 if(f.attackTimer>4&&f.attackType==4){circle(x+face*38,y-49,20,C2D_Color32(255,188,73,90));circle(x+face*38,y-49,11,GOLD);}
 if(f.blocking){rect(x+face*15,y-58,8,29,plate);rect(x+face*12,y-63,14,7,trim);}
}
void Game::render()const{
 if(textBuf)C2D_TextBufClear(textBuf);
 rect(0,0,400,240,BG);circle(325,48,29,C2D_Color32(224,208,173,255));circle(325,48,24,C2D_Color32(187,192,195,255));
 rect(0,100,400,100,C2D_Color32(24,30,48,255));for(int i=0;i<8;i++){int x=i*57-5;rect(x,115+(i%3)*6,35,85,C2D_Color32(15,20,35,255));rect(x+9,104+(i%3)*6,17,12,C2D_Color32(38,43,62,255));}
 rect(0,198,400,42,C2D_Color32(46,35,38,255));rect(0,196,400,2,GOLD);
 if(screen_==Screen::Title){drawText("SHADOW FIGHT",87,61,.75f,GOLD);drawText("CHRONICLES",125,94,.55f,PALE);drawText("A NEW LEGEND BEGINS",103,166,.38f,CYAN);drawText("PRESS A OR TOUCH TO START",83,207,.34f,PALE);return;}
 if(screen_==Screen::Fight||screen_==Screen::Result){
  rect(12,9,178,18,INK);rect(210,9,178,18,INK);bar(17,14,168,8,player_.hp,player_.maxHp,RED);bar(215,14,168,8,rival_.hp,rival_.maxHp,RED);bar(17,28,94,5,player_.energy,100,GOLD);
  drawText("HERO",18,1,.28f,PALE);drawText(battle_==Battle::Boss?"BOSS":"RIVAL",311,1,.28f,PALE);
  drawFighter(player_,false);drawFighter(rival_,true);for(const auto&p:particles_)if(p.life)circle(p.x,p.y,2+(p.life%3),p.color);
  if(player_.combo>1){char c[24];std::snprintf(c,sizeof(c),"%d HIT COMBO",player_.combo);drawText(c,20,44,.42f,GOLD);}
  if(screen_==Screen::Result){rect(45,69,310,105,C2D_Color32(5,8,18,235));drawText(won_?"VICTORY":"DEFEAT",145,83,.7f,won_?GOLD:RED);char s[64];std::snprintf(s,sizeof(s),"+%d COINS    +%d GEMS",rewardCoins_,rewardGems_);drawText(s,90,119,.38f,PALE);drawText("TAP OR PRESS A TO CONTINUE",82,151,.31f,CYAN);}
  if(hitFlash_)rect(0,0,400,240,C2D_Color32(255,255,255,18));return;
 }
 drawText("CHRONICLES OF THE VEIL",76,20,.48f,GOLD);
 drawText("LEVEL",18,59,.32f,MUTED);char s[64];std::snprintf(s,sizeof(s),"%d",level_);drawText(s,18,78,.58f,PALE);
 std::snprintf(s,sizeof(s),"COINS %d",coins_);drawText(s,104,59,.36f,GOLD);std::snprintf(s,sizeof(s),"GEMS %d",gems_);drawText(s,260,59,.36f,CYAN);
 bar(18,105,364,8,xp_,level_*100,CYAN);drawText("EXPERIENCE",18,119,.27f,MUTED);
 if(screen_==Screen::Hub){drawText("CAMPAIGN",30,147,.45f,PALE);drawText(stages[(level_-1)%10],30,169,.33f,CYAN);drawText("DUEL",250,147,.45f,PALE);drawText("TEST YOUR SKILL",230,169,.27f,MUTED);drawText("SELECT A MODE BELOW",113,211,.31f,PALE);}
 else if(screen_==Screen::Shop){drawText("WEAPON FORGE",26,53,.47f,GOLD);for(int i=0;i<5;i++){int y=84+i*27;rect(18,y-3,364,23,i==shopChoice_?PANEL2:PANEL);drawText(weapons[i],27,y,.31f,PALE);if(weaponOwned_[i])drawText(weapon_==i?"EQUIPPED":"OWNED",294,y,.27f,GREEN);else{std::snprintf(s,sizeof(s),"%d %s",weaponCost[i],i==4?"GEMS":"COINS");drawText(s,280,y,.27f,GOLD);}}drawText("TAP ITEM TO BUY / EQUIP",75,222,.29f,CYAN);}
 else if(screen_==Screen::Gear){drawText("ARMOR & EQUIPMENT",26,53,.47f,GOLD);for(int i=0;i<5;i++){int y=84+i*27;rect(18,y-3,364,23,i==gearChoice_?PANEL2:PANEL);drawText(armors[i],27,y,.31f,PALE);if(armorOwned_[i])drawText(armor_==i?"EQUIPPED":"OWNED",294,y,.27f,GREEN);else{std::snprintf(s,sizeof(s),"%d %s",armorCost[i],i==4?"GEMS":"COINS");drawText(s,280,y,.27f,GOLD);}}drawText("TAP ITEM TO BUY / EQUIP",75,222,.29f,CYAN);}
}
void Game::renderControls()const{
 if(textBuf)C2D_TextBufClear(textBuf);rect(0,0,320,240,BG);rect(0,0,320,5,GOLD);
 char s[64];std::snprintf(s,sizeof(s),"LV %d",level_);drawText(s,13,12,.35f,GOLD);std::snprintf(s,sizeof(s),"%d",coins_);drawText(s,112,12,.35f,GOLD);std::snprintf(s,sizeof(s),"%d",gems_);drawText(s,246,12,.35f,CYAN);
 if(screen_==Screen::Hub||screen_==Screen::Title){
  drawText("YOUR JOURNEY",15,42,.48f,PALE);drawText("Choose your next challenge",15,67,.31f,MUTED);
  rect(12,91,296,70,PANEL);drawText("STORY CAMPAIGN",24,101,.4f,GOLD);drawText("Explore stages and defeat rivals",24,128,.29f,PALE);
  drawText("DUELS",15,174,.34f,PALE);drawText("BOSSES",83,174,.34f,PALE);drawText("SHOP",157,174,.34f,PALE);drawText("GEAR",229,174,.34f,PALE);
  const char* tabs[]={"STORY","DUEL","BOSS","SHOP","GEAR"};for(int i=0;i<5;i++){int x=i*64;rect(x+2,199,60,34,i==hubChoice_?RED:PANEL2);drawText(tabs[i],x+7,208,.27f,PALE);}
 }else if(screen_==Screen::Shop){
  drawText("FORGE & MERCHANT",15,47,.48f,GOLD);drawText("Weapons change your attack power",15,76,.29f,MUTED);
  for(int i=0;i<5;i++){int y=100+i*23;rect(12,y,296,20,i==shopChoice_?PANEL2:PANEL);drawText(weapons[i],20,y+1,.27f,PALE);}
  rect(12,218,296,18,RED);drawText("BACK TO HALL",105,218,.27f,PALE);
 }else if(screen_==Screen::Gear){
  drawText("ARMORY",15,47,.48f,GOLD);drawText("Armor increases maximum health",15,76,.29f,MUTED);
  for(int i=0;i<5;i++){int y=100+i*23;rect(12,y,296,20,i==gearChoice_?PANEL2:PANEL);drawText(armors[i],20,y+1,.27f,PALE);}
  rect(12,218,296,18,RED);drawText("BACK TO HALL",105,218,.27f,PALE);
 }else{
  drawText("ACTIVE LOADOUT",15,47,.48f,GOLD);drawText("Your fighter is in the arena",15,76,.3f,MUTED);
  rect(12,100,296,47,PANEL);drawText("WEAPON",22,105,.26f,CYAN);drawText(weapons[weapon_],22,123,.34f,PALE);
  rect(12,154,296,47,PANEL);drawText("ARMOR",22,159,.26f,GOLD);drawText(armors[armor_],22,177,.34f,PALE);
  drawText("PAUSE WITH START",91,218,.27f,MUTED);
 }
}
}
