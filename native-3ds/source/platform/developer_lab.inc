/* Optional developer-only scenarios. They never alter a campaign encounter. */
typedef struct {unsigned dex[2],moves[2][4],mask,ai;} DeveloperLab;
static int DeveloperLabRead(DeveloperLab *out)
{
    char line[192];unsigned team,a,b,c,d;FILE *f;
    memset(out,0,sizeof(*out));out->ai=1;
    f=fopen("sdmc:/3ds/platinum-arena-preview/developer-mode","r");if(!f)return 0;
    while(fgets(line,sizeof(line),f)){
        if(sscanf(line,"actors %u %u",&a,&b)==2&&a>=1&&a<=493&&b>=1&&b<=493){out->dex[0]=a;out->dex[1]=b;}
        else if(sscanf(line,"moves%u %u %u %u %u",&team,&a,&b,&c,&d)==5&&team<2&&a<=467&&b<=467&&c<=467&&d<=467){
            out->moves[team][0]=a;out->moves[team][1]=b;out->moves[team][2]=c;out->moves[team][3]=d;out->mask|=1u<<team;
        }else if(sscanf(line,"ai %u",&a)==1&&a<2)out->ai=a;
    }
    fclose(f);return 1;
}
static void DeveloperLabApply(Adventure *a,FILE *log)
{
    DeveloperLab dev;CombatFighterConfig configs[2][6];GamePartySlot state[2][6];
    uint8_t counts[2],levels[2][6];unsigned doubles=a->battle.doubles,t,m;
    if(a->battleKind!=ADV_LAB_BATTLE||!DeveloperLabRead(&dev))return;
    if(!dev.dex[0]&&!dev.mask){a->battle.combat.aiEnabled=(uint8_t)dev.ai;return;}
    memcpy(configs,a->battle.configs,sizeof(configs));memcpy(state,a->battle.party,sizeof(state));
    memcpy(counts,a->battle.partyCount,sizeof(counts));memcpy(levels,a->battle.levels,sizeof(levels));
    for(t=0;t<2;t++){
        if(dev.dex[t]){configs[t][0]=ArenaSpecies_Find(dev.dex[t])->config;a->labDex[t]=(uint16_t)dev.dex[t];levels[t][0]=50;}
        if(dev.mask&(1u<<t))for(m=0;m<4;m++)configs[t][0].moves[m]=(uint16_t)dev.moves[t][m];
        state[t][0].hp=configs[t][0].hp;state[t][0].status=0;state[t][0].stamina=100;
        for(m=0;m<4;m++){state[t][0].pp[m]=Combat_GetMoveInfo(configs[t][0].moves[m])->pp;state[t][0].cooldown[m]=0;}
    }
    Game_StartParty(&a->battle,configs,counts,state,doubles);memcpy(a->battle.levels,levels,sizeof(levels));
    a->battle.combat.aiEnabled=(uint8_t)dev.ai;a->battle.combat.fighters[0].selected=0;
    Combat_SetClashPractice(&a->battle.combat,a->labClash);
    if(log){fprintf(log,"developer_lab actors=%u,%u moves=%u,%u,%u,%u ai=%u doubles=%u\n",configs[0][0].species,configs[1][0].species,configs[0][0].moves[0],configs[0][0].moves[1],configs[0][0].moves[2],configs[0][0].moves[3],dev.ai,doubles);fflush(log);}
}
static void DeveloperLabTrace(const Adventure *a,FILE *log)
{
    const Combat *c=&a->battle.combat;
    if(!log||a->battleKind!=ADV_LAB_BATTLE||c->frame%6)return;
    for(unsigned i=0;i<c->fighterCount;i++){
        const CombatFighter *f=&c->fighters[i];
        fprintf(log,"motion t=%lu actor=%u dex=%u xy=%ld,%ld vel=%ld,%ld dir=%u gait=%lu phase=%u anim=%u hurt=%u hp=%u dash=%u hits=%lu\n",
                (unsigned long)c->frame,i,f->species,(long)f->x,(long)f->y,(long)f->vx,(long)f->vy,
                f->direction,(unsigned long)f->gaitPhase,f->phase,f->animation,f->hurtTimer,f->hp,f->dashTimer,(unsigned long)c->hits);
    }
    fflush(log);
}
