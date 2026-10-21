/* Small original prototype cues; no commercial sound archive is bundled.
 * Events follow simulation releases/hits, not animation or display frames. */
#include "audio3ds.h"
#include <3ds.h>
#include <math.h>
#include <string.h>

#define SAMPLES 3300
#define RATE 22050
#define VOICES 4
static int ready;
static int16_t *samples[VOICES];
static ndspWaveBuf waves[VOICES];
static uint32_t releases[4],hits;

int Audio_Init(void)
{
    unsigned i;
    if(R_FAILED(ndspInit()))return 0;
    memset(waves,0,sizeof(waves));
    for(i=0;i<VOICES;i++){
        samples[i]=linearAlloc(SAMPLES*sizeof(int16_t));
        if(!samples[i]){Audio_Stop();return 0;}
        ndspChnSetInterp(i,NDSP_INTERP_LINEAR);ndspChnSetRate(i,RATE);
        ndspChnSetFormat(i,NDSP_FORMAT_MONO_PCM16);
    }
    ready=1;return 1;
}
static void Cue(unsigned seed,unsigned type)
{
    unsigned channel,i;float frequency=type==13?1100:type==11?240:type==10?130:420;
    for(channel=0;channel<VOICES;channel++)
        if(waves[channel].status!=NDSP_WBUF_PLAYING&&waves[channel].status!=NDSP_WBUF_QUEUED)break;
    if(channel==VOICES)return;
    for(i=0;i<SAMPLES;i++){
        float t=i/(float)RATE,envelope=(1-i/(float)SAMPLES);
        float sound;
        seed=seed*1664525u+1013904223u;
        sound=sinf(6.2831853f*(frequency*t+frequency*t*t*2));
        if(type==10||type==11)sound=.7f*((int)(seed>>16)-32768)/32768.0f+.3f*sound;
        else if(type==13)sound=.6f*sound+.4f*((int)(seed>>16)-32768)/32768.0f;
        samples[channel][i]=(int16_t)(sound*envelope*envelope*6500);
    }
    DSP_FlushDataCache(samples[channel],SAMPLES*sizeof(int16_t));
    memset(&waves[channel],0,sizeof(waves[channel]));waves[channel].data_vaddr=samples[channel];
    waves[channel].nsamples=SAMPLES;ndspChnWaveBufAdd(channel,&waves[channel]);
}
void Audio_Observe(const ArenaGame *g)
{
    unsigned i;if(!ready)return;
    for(i=0;i<g->combat.fighterCount;i++){
        const CombatFighter *f=&g->combat.fighters[i];
        if(releases[i]!=f->releaseSerial){
            const CombatMoveInfo *move=Combat_GetMoveInfo(f->releaseMove);
            releases[i]=f->releaseSerial;
            if(f->releaseSerial)Cue(f->releaseMove*7919u+g->combat.frame,move?move->type:0);
        }
    }
    if(hits!=g->combat.hits){hits=g->combat.hits;if(hits)Cue(hits+17,0);}
}
void Audio_Stop(void)
{
    unsigned i;
    for(i=0;i<VOICES;i++)if(samples[i]){
        ndspChnReset(i);linearFree(samples[i]);samples[i]=0;
    }
    ndspExit();ready=0;
}
