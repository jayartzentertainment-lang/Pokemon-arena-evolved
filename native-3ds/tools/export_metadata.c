/* Read only the recovered, portable 0.9.6 sprite metadata. */
#include "arena/assets.h"
#include <stdio.h>

int main(void)
{
    const unsigned dex[]={25,448,6,9};
    unsigned n,a,i;
    puts("[");
    for(n=0;n<4;n++){
        const AssetSpecies *s=ArenaAssets_FindSpecies(dex[n],0);
        if(!s)return 1;
        printf("{\"dex\":%u,\"name\":\"%s\",\"palette\":[",s->dex,s->name);
        for(i=0;i<16;i++)printf("%s%u",i?",":"",s->palette[i]);
        printf("],\"animations\":[");
        for(a=0;a<5;a++){
            const AssetAnimation *p=&s->animations[a];
            printf("%s{\"offset\":%u,\"bytes\":%u,\"frames\":%u,\"total_ticks\":%u,\"hit_tick\":%u,\"durations\":[",
                   a?",":"",p->offset,p->size,p->frames,p->total_ticks,p->hit_tick);
            for(i=0;i<p->frames;i++)printf("%s%u",i?",":"",p->durations[i]);
            printf("],\"ground\":[");
            for(i=0;i<8;i++)printf("%s%d",i?",":"",p->ground[i]);
            printf("]}");
        }
        printf("]}%s\n",n==3?"":",");
    }
    puts("]");
    return 0;
}
