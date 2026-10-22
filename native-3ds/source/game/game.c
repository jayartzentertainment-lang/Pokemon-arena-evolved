#include "game.h"
#include "adventure.h"
#include "arena/vendor/arena_navigation.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

static void Mobility(ArenaGame *g,unsigned actor)
{
    unsigned dex=g->combat.fighters[actor].species;
    if(dex&&dex<=493){const AdventureDex *d=&gAdventureDex[dex];Combat_ConfigureMobility(&g->combat,actor,d->stats[3],d->stats[0],d->stats[2]);Game_ConfigureMotion(g,actor);}
}
static unsigned Team(unsigned actor){return actor&1;}
static int Bound(const ArenaGame *g,unsigned team,unsigned slot,unsigned except)
{
    unsigned a;
    for(a=0;a<g->combat.fighterCount;a++)
        if(a!=except&&Team(a)==team&&g->binding[a]==slot)return 1;
    return 0;
}
static void Store(ArenaGame *g,unsigned actor)
{
    CombatFighter *f=&g->combat.fighters[actor];
    GamePartySlot *p=&g->party[Team(actor)][g->binding[actor]];
    p->hp=f->hp;p->stamina=f->stamina;p->status=Combat_GetNativeStatus(&g->combat,actor);
    memcpy(p->pp,f->pp,sizeof(p->pp));memcpy(p->cooldown,f->cooldown,sizeof(p->cooldown));
    p->naturalCooldown=f->naturalCooldown;p->storedAt=g->combat.frame;
}
static int Load(ArenaGame *g,unsigned actor,unsigned slot)
{
    if(slot>=g->partyCount[Team(actor)])return 0;
    GamePartySlot *p=&g->party[Team(actor)][slot];
    unsigned elapsed=g->combat.frame-p->storedAt,i;
    if(!p->hp||Bound(g,Team(actor),slot,actor))return 0;
    if(!Combat_ReplaceFighter(&g->combat,actor,&g->configs[Team(actor)][slot]))return 0;
    Mobility(g,actor);g->binding[actor]=(uint8_t)slot;
    g->faintTicks[actor]=0;g->deployTicks[actor]=12;
    g->combat.fighters[actor].hp=p->hp;g->combat.fighters[actor].naturalCooldown=p->naturalCooldown>elapsed?(uint16_t)(p->naturalCooldown-elapsed):0;
    g->combat.fighters[actor].stamina=(uint8_t)(p->stamina+elapsed/6*2>g->combat.fighters[actor].staminaMax?g->combat.fighters[actor].staminaMax:p->stamina+elapsed/6*2);
    Combat_SetMovePP(&g->combat,actor,p->pp);
    Combat_SetNativeStatus(&g->combat,actor,p->status);
    for(i=0;i<4;i++)g->combat.fighters[actor].cooldown[i]=
        p->cooldown[i]>elapsed?(uint16_t)(p->cooldown[i]-elapsed):0;
    g->combat.winner=0;g->switches++;if(Team(actor)==0)g->participated|=(uint8_t)(1u<<slot);
    return 1;
}
const ArenaSpecies *Game_ActorSpecies(const ArenaGame *g,unsigned actor)
{
    return g&&actor<g->combat.fighterCount?ArenaSpecies_Find(g->combat.fighters[actor].species):0;
}
unsigned Game_Remaining(const ArenaGame *g,unsigned team)
{
    unsigned i,n=0;if(!g||team>1)return 0;
    for(i=0;i<g->partyCount[team];i++)if(g->party[team][i].hp)n++;
    return n;
}
int Game_Switch(ArenaGame *g,unsigned actor)
{
    unsigned i,start;
    if(!g||g->winner||actor>=g->combat.fighterCount)return 0;
    Store(g,actor);start=g->binding[actor];
    for(i=1;i<=g->partyCount[Team(actor)];i++){
        unsigned slot=(start+i)%g->partyCount[Team(actor)];
        if(slot!=start&&Load(g,actor,slot))return 1;
    }
    return 0;
}
void Game_Start(ArenaGame *g,int doubles)
{
    CombatFighterConfig configs[4];unsigned team,slot,m,a;
    static const unsigned single[4]={0,3,1,2};
    memset(g,0,sizeof(*g));g->doubles=(uint8_t)!!doubles;
    g->partyCount[0]=g->partyCount[1]=4;
    for(team=0;team<2;team++)for(slot=0;slot<4;slot++){
        g->configs[team][slot]=gSpecies[slot].config;g->levels[team][slot]=50;
        GamePartySlot *p=&g->party[team][slot];
        p->hp=gSpecies[slot].config.hp;p->stamina=100;
        for(m=0;m<4;m++)p->pp[m]=Combat_GetMoveInfo(gSpecies[slot].config.moves[m])->pp;
    }
    for(a=0;a<4;a++){
        g->binding[a]=(uint8_t)single[a];configs[a]=gSpecies[single[a]].config;
    }
    if(doubles)Combat_InitTeams(&g->combat,configs);else Combat_Init(&g->combat,configs);
    for(a=0;a<g->combat.fighterCount;a++)Mobility(g,a);
    g->combat.fighters[0].selected=0;g->combat.lockAim=1;
    g->combat.growingBeams=1;
    g->aim.mode=GAME_AIM_LOCK;g->aim.target=255;g->aim.freeX=256;
    {GameInput none={0};none.touchMove=-1;Game_AimTick(g,none);}
    /* One arena's authored destructible cover and navigation are retained. */
}
static CombatInput Translate(ArenaGame *g,GameInput in)
{
    CombatInput out={0,0};
    unsigned i;uint16_t direction=in.held&15;
    static const uint16_t buttons[4]={GAME_UP,GAME_DOWN,GAME_LEFT,GAME_RIGHT};
    static const uint16_t commands[4]={COMBAT_UP,COMBAT_DOWN,COMBAT_LEFT,COMBAT_RIGHT};
    int magnitude=(int)sqrt((double)in.axisX*in.axisX+(double)in.axisY*in.axisY);
    if(magnitude>18){
        int ax=in.axisX,ay=in.axisY;
        direction=0;
        if(ax>18&&abs(ax)*100>=abs(ay)*42)direction|=GAME_RIGHT;
        if(ax< -18&&abs(ax)*100>=abs(ay)*42)direction|=GAME_LEFT;
        if(ay>18&&abs(ay)*100>=abs(ax)*42)direction|=GAME_DOWN;
        if(ay< -18&&abs(ay)*100>=abs(ax)*42)direction|=GAME_UP;
        Combat_SetAnalogMagnitude(&g->combat,(unsigned)(magnitude>156?256:(magnitude-18)*256/138),1);
    }else Combat_SetAnalogMagnitude(&g->combat,256,0);
    for(i=0;i<4;i++){
        if(direction&buttons[i])out.held|=commands[i];
        if(in.pressed&buttons[i])out.pressed|=commands[i];
    }
    if(in.held&GAME_B)out.held|=COMBAT_B;
    if(in.pressed&GAME_B)out.pressed|=COMBAT_B;
    if(in.held&GAME_L){
        if(in.pressed&GAME_Y)out.pressed|=COMBAT_CYCLE_LIGHT;
        if(in.pressed&GAME_A)out.pressed|=COMBAT_CYCLE_HEAVY;
        if(in.pressed&GAME_X)out.pressed|=COMBAT_R;
    }else{
        if(!(in.held&GAME_R)){
            if(in.held&GAME_Y)out.held|=COMBAT_L;
            if(in.pressed&GAME_Y)out.pressed|=COMBAT_L;
        }
        if(in.held&GAME_A)out.held|=COMBAT_A;
        if(in.pressed&GAME_A)out.pressed|=COMBAT_A;
    }
    if(in.held&GAME_R){
        if(g->rAge<255)g->rAge++;
        if(in.pressed&GAME_X){out.pressed|=COMBAT_COMMAND;g->rUsed=1;}
    }else{
        if(g->rAge&&g->rAge<=12&&!g->rUsed)Game_Switch(g,g->combat.controlActor);
        g->rAge=g->rUsed=0;
        if(!(in.held&GAME_L)){
            if(in.held&GAME_X)out.held|=COMBAT_ALT;
            if(in.pressed&GAME_X)out.pressed|=COMBAT_ALT;
        }
    }
    return out;
}
void Game_Tick(ArenaGame *g,GameInput in)
{
    unsigned a,transition=0;
    CombatInput combat;
    if(!g||g->winner)return;
    if(in.switchPressed)Game_Switch(g,g->combat.controlActor);
    if(in.touchMove>=0&&in.touchMove<4)
        g->combat.fighters[g->combat.controlActor].selected=(uint8_t)in.touchMove;
    Game_AimTick(g,in);
    combat=Translate(g,in);
    if(in.touchMove>=0&&in.touchMove<4)
        Combat_QueueMove(&g->combat,g->combat.controlActor,(unsigned)in.touchMove);
    Combat_Tick(&g->combat,combat);g->ticks++;
    for(a=0;a<g->combat.fighterCount;a++)Store(g,a);
    for(a=0;a<g->combat.fighterCount;a++){
        if(g->deployTicks[a])g->deployTicks[a]--;
        if(!g->combat.fighters[a].hp){
            if(g->faintTicks[a]<20){g->faintTicks[a]++;transition=1;}
            else Game_Switch(g,a);
        }
    }
    if(!Game_Remaining(g,0)&&!transition)g->winner=2;
    else if(!Game_Remaining(g,1)&&!transition)g->winner=1;
    else g->combat.winner=0;
}
int Game_TouchMove(unsigned x,unsigned y)
{
    unsigned slot;
    for(slot=0;slot<4;slot++){
        int left,top,width,height;Game_MoveRect(slot,&left,&top,&width,&height);
        if(x>=(unsigned)left&&x<(unsigned)(left+width)&&y>=(unsigned)top&&y<(unsigned)(top+height))return (int)slot;
    }
    return -1;
}
void Game_MoveRect(unsigned slot,int *x,int *y,int *width,int *height)
{*x=8+(int)(slot&1)*156;*y=96+(int)(slot/2)*48;*width=148;*height=40;}
void Game_CommandRect(unsigned command,int *x,int *y,int *width,int *height)
{
    if(command<3){*x=8+(int)command*104;*y=200;*width=96;*height=32;}
    else if(command==3){*x=216;*y=8;*width=96;*height=30;}
    else {*x=8;*y=8;*width=200;*height=30;}
}
int Game_TouchCommand(unsigned x,unsigned y)
{
    unsigned command;
    for(command=0;command<5;command++){
        int left,top,width,height;Game_CommandRect(command,&left,&top,&width,&height);
        if(x>=(unsigned)left&&x<(unsigned)(left+width)&&y>=(unsigned)top&&y<(unsigned)(top+height))return (int)command;
    }
    return -1;
}

void Game_StartParty(ArenaGame *g,const CombatFighterConfig configs[2][6],const uint8_t counts[2],const GamePartySlot state[2][6],int doubles)
{
    CombatFighterConfig active[4];unsigned a,t,s,used[2]={0,0};
    memset(g,0,sizeof(*g));
    for(t=0;t<2;t++){
        g->partyCount[t]=counts[t]>6?6:counts[t];
        for(s=0;s<g->partyCount[t];s++){
            g->configs[t][s]=configs[t][s];g->party[t][s]=state[t][s];
        }
    }
    g->doubles=(uint8_t)(doubles&&counts[0]>=2&&counts[1]>=2);
    for(a=0;a<(g->doubles?4u:2u);a++){
        t=Team(a);
        for(s=used[t];s<g->partyCount[t]&&!g->party[t][s].hp;s++);
        if(s==g->partyCount[t])s=0;
        used[t]=s+1;g->binding[a]=(uint8_t)s;active[a]=configs[t][s];
    }
    if(g->doubles)Combat_InitTeams(&g->combat,active);else Combat_Init(&g->combat,active);
    for(a=0;a<g->combat.fighterCount;a++){
        GamePartySlot *p=&g->party[Team(a)][g->binding[a]];Mobility(g,a);
        g->combat.fighters[a].hp=p->hp;g->combat.fighters[a].stamina=p->stamina>g->combat.fighters[a].staminaMax?g->combat.fighters[a].staminaMax:p->stamina;
        Combat_SetMovePP(&g->combat,a,p->pp);Combat_SetNativeStatus(&g->combat,a,p->status);
    }
    g->participated=(uint8_t)((1u<<g->binding[0])|(g->doubles?(1u<<g->binding[2]):0));
    g->combat.lockAim=1;g->combat.growingBeams=1;g->aim.mode=GAME_AIM_LOCK;
    g->aim.target=255;g->aim.freeX=256;
    {GameInput none={0};none.touchMove=-1;Game_AimTick(g,none);}
}
