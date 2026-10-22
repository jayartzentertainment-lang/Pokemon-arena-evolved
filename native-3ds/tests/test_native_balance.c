#include "game.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
static ArenaGame g;
static GameInput None(void){GameInput in={0};in.touchMove=-1;return in;}
static void Setup(void){unsigned i;Game_Start(&g,0);g.combat.aiEnabled=0;for(i=0;i<ARENA_OBSTACLES;i++)ArenaNav_SetObstacle(i,0);g.combat.fighters[0].x=100*256;g.combat.fighters[0].y=150*256;g.combat.fighters[1].x=300*256;g.combat.fighters[1].y=150*256;}
int main(void)
{
 unsigned fast,slow,i,serial;int32_t x,y;CombatFighter *f;GameInput in=None();
 Setup();fast=g.combat.fighters[0].moveSpeed;assert(g.combat.nativeBalance&&fast<450);
 assert(Game_Switch(&g,0)&&Game_Switch(&g,0)&&Game_Switch(&g,0));f=&g.combat.fighters[0];assert(f->species==9);slow=f->moveSpeed;assert(slow<fast&&slow>=266*105/100&&slow<=266*115/100&&f->staminaMax!=100);
 Setup();f=&g.combat.fighters[0];in.held=GAME_Y;in.pressed=GAME_Y;Game_Tick(&g,in);assert(f->naturalCooldown>0);serial=f->actionSerial;
 in.pressed=0;
 while(f->phase!=COMBAT_READY)Game_Tick(&g,in);
 assert(f->naturalCooldown);for(i=0;i<6;i++)Game_Tick(&g,in);assert(f->actionSerial==serial);
 Setup();f=&g.combat.fighters[0];f->moves[0]=MOVE_SCRATCH;f->pp[0]=35;x=f->x;y=f->y;in=None();in.touchMove=0;Game_Tick(&g,in);in=None();
 for(i=0;i<160;i++){Game_Tick(&g,in);}
 assert(f->x==x&&f->y==y); /* A short strike never becomes a generic rush. */
 Setup();f=&g.combat.fighters[0];f->moves[0]=MOVE_TACKLE;f->pp[0]=35;x=f->x;in=None();in.touchMove=0;Game_Tick(&g,in);in=None();for(i=0;i<160;i++)Game_Tick(&g,in);assert(f->x>x&&f->cooldown[0]==0);
 Setup();f=&g.combat.fighters[0];f->moves[0]=MOVE_HYPER_BEAM;f->pp[0]=5;in=None();in.touchMove=0;Game_Tick(&g,in);assert(f->cooldown[0]>150&&f->stamina<f->staminaMax);
 puts("PASS native balance: slower stat-based Blastoise, species stamina, natural spam cooldown, stationary Scratch, genuine Tackle travel and high-power move cost/cooldown");return 0;
}
