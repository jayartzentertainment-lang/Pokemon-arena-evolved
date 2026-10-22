#include "game.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>

static ArenaGame game;
static GameInput input(void){GameInput in={0};in.touchMove=-1;return in;}
static void Quiet(void)
{
    unsigned i;game.combat.aiEnabled=0;
    for(i=0;i<ARENA_OBSTACLES;i++)ArenaNav_SetObstacle(i,0);
}
static void Switching(void)
{
    unsigned original;uint16_t hp;
    Game_Start(&game,0);Quiet();original=game.combat.fighters[0].species;
    hp=game.combat.fighters[0].hp-=7;
    game.combat.fighters[0].pp[0]=2;game.combat.fighters[0].cooldown[0]=90;
    game.combat.fighters[0].stamina=31;
    assert(Game_Switch(&game,0));assert(Game_Switch(&game,0));
    assert(Game_Switch(&game,0));assert(Game_Switch(&game,0));
    assert(game.combat.fighters[0].species==original);
    assert(game.combat.fighters[0].hp==hp&&game.combat.fighters[0].pp[0]==2);
    assert(game.combat.fighters[0].cooldown[0]==90&&game.combat.fighters[0].stamina==31);
    Game_Start(&game,1);Quiet();assert(Game_Switch(&game,0));
    assert(game.binding[0]!=game.binding[2]);
}
static void AnalogAndDigital(void)
{
    GameInput in=input();int32_t full,half,digital;unsigned i;
    Game_Start(&game,0);Quiet();in.axisX=156;
    full=game.combat.fighters[0].x;
    for(i=0;i<12;i++)Game_Tick(&game,in);
    full=game.combat.fighters[0].x-full;
    Game_Start(&game,0);Quiet();in.axisX=87;
    half=game.combat.fighters[0].x;
    for(i=0;i<12;i++)Game_Tick(&game,in);
    half=game.combat.fighters[0].x-half;
    Game_Start(&game,0);Quiet();in.axisX=0;in.held=GAME_RIGHT;
    digital=game.combat.fighters[0].x;
    for(i=0;i<12;i++)Game_Tick(&game,in);
    digital=game.combat.fighters[0].x-digital;
    assert(full>0&&half>0&&half<full*3/4&&digital==full);
}
static void BuffAndPP(void)
{
    GameInput in=input();unsigned i,pp;
    Game_Start(&game,0);Quiet();assert(Game_Switch(&game,0));
    assert(game.combat.fighters[0].species==448);
    pp=game.combat.fighters[0].pp[3];in.touchMove=3;Game_Tick(&game,in);in.touchMove=-1;
    for(i=0;i<60;i++)Game_Tick(&game,in);
    assert(game.combat.fighters[0].stages[COMBAT_STAT_ATTACK]==2);
    assert(game.combat.fighters[0].pp[3]==pp-1);
}
static void ReplacementAndResult(void)
{
    GameInput in=input();unsigned i;
    Game_Start(&game,0);Quiet();game.combat.fighters[0].hp=0;Game_Tick(&game,in);
    assert(game.faintTicks[0]==1&&!game.combat.fighters[0].hp);
    for(i=0;i<20;i++)Game_Tick(&game,in);
    assert(game.combat.fighters[0].hp&&game.combat.fighters[0].species==448);
    assert(Game_Remaining(&game,0)==3);
    for(i=0;i<4;i++)game.party[1][i].hp=0;
    game.combat.fighters[1].hp=0;
    for(i=0;i<21;i++)Game_Tick(&game,in);
    assert(game.winner==1);
}
int main(void)
{
    Switching();AnalogAndDigital();BuffAndPP();ReplacementAndResult();
    assert(Game_TouchMove(10,100)==0&&Game_TouchMove(166,168)==3);
    assert(Game_TouchMove(160,100)==-1);
    puts("PASS native adapter: analog magnitude, D-pad, party preservation, no duplicate2v2 bindings, real buff/PP and battle result");
    return 0;
}
