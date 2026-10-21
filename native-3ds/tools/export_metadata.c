/* Read only the recovered, portable 0.9.6 sprite metadata. */
#include "arena/assets.h"
#include <stdio.h>

int main(int argc,char **argv)
{
    const unsigned dex[]={25,448,6,9};
    unsigned n,a,i,limit=argc>1?asset_species_count:4;(void)argv;
    puts("[");
    for(n=0;n<limit;n++){
        const AssetSpecies *s;
        unsigned d=n<4?dex[n]:0;
        if(n>=4){unsigned j,k,index=4;for(j=1;j<=493;j++){int skip=0;for(k=0;k<4;k++)if(dex[k]==j)skip=1;if(!skip){if(index++==n){d=j;break;}}}}
        s=ArenaAssets_FindSpecies(d,0);
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
        printf("]}%s\n",n==limit-1?"":",");
    }
    puts("]");
    return 0;
}
