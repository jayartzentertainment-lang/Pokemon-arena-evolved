/* Portable original-content checkpoint, with separate appearance/party payload.
 * Only quiescent field states are serializable; no raw pointers or VM stacks. */
#include "platinum.h"
#include <stdio.h>
#include <string.h>
#define CAPACITY 12288
static uint32_t Crc(const uint8_t *s,size_t n){uint32_t c=~0u;unsigned b;while(n--){c^=*s++;for(b=0;b<8;b++)c=(c>>1)^(0xedb88320u&-(c&1));}return ~c;}
static void Put(uint8_t *s,unsigned v){s[0]=(uint8_t)v;s[1]=(uint8_t)(v>>8);}
static void Put32(uint8_t *s,uint32_t v){unsigned i;for(i=0;i<4;i++)s[i]=(uint8_t)(v>>(8*i));}
static uint32_t Get32(const uint8_t *s){uint32_t v=0;unsigned i;for(i=0;i<4;i++)v|=(uint32_t)s[i]<<(8*i);return v;}
static unsigned Get(const uint8_t *s){return s[0]|(unsigned)s[1]<<8;}
static size_t Encode(const Platinum *p,const uint8_t *profile,size_t size,uint8_t out[CAPACITY])
{
 size_t at=16,i;uint32_t crc;
 if(!p||!profile||size>4096||p->intro||p->blocked||p->pendingTransition||p->cameraActive||p->pc!=PT_NO_PC||p->waiting||p->actors[PT_ACTORS].ticks||!Platinum_Map(p->header))return 0;
 for(i=0;i<=PT_ACTORS;i++)if(p->actors[i].pc!=PT_NO_PC||p->actors[i].ticks)return 0;
 memset(out,0,CAPACITY);memcpy(out,"PAPT",4);out[4]=1;
 Put(out+at,p->header);at+=2;Put(out+at,p->x);at+=2;Put(out+at,p->z);at+=2;out[at++]=p->facing;out[at++]=p->creatorDone;for(i=0;i<4;i++)out[at++]=(uint8_t)(p->random>>(i*8));Put(out+at,p->starter);at+=2;out[at++]=p->lastBattleWon;out[at++]=p->gender;out[at++]=p->partner;out[at++]=p->runningShoes;
 for(i=0;i<PT_VARS;i++){Put(out+at,p->vars[i]);at+=2;}
 memcpy(out+at,p->flags,PT_FLAG_BYTES);at+=PT_FLAG_BYTES;
 for(i=0;i<=PT_ACTORS;i++){const PtActor *a=&p->actors[i];Put32(out+at,(uint32_t)a->x);at+=4;Put32(out+at,(uint32_t)a->z);at+=4;out[at++]=a->dir;out[at++]=a->visible;Put(out+at,a->movementType);at+=2;}
 Put(out+at,(unsigned)size);at+=2;memcpy(out+at,profile,size);at+=size;
 Put(out+6,(unsigned)at);crc=Crc(out+16,at-16);for(i=0;i<4;i++)out[8+i]=(uint8_t)(crc>>(i*8));return at;
}
static int ReadOne(const char *path,Platinum *p,uint8_t *profile,size_t cap,size_t *size)
{
 uint8_t bytes[CAPACITY+1];FILE *f=fopen(path,"rb");size_t n,at=16,i,profileSize;unsigned header,x,z,dir,creator;uint32_t crc=0,random=0;unsigned starter,won,gender,partner,shoes;Platinum temp;PtActor actors[PT_ACTORS+1];
 if(!f){return 0;}
 n=fread(bytes,1,sizeof(bytes),f);if(ferror(f)){fclose(f);return 0;}fclose(f);
 if(n<16+18+PT_VARS*2+PT_FLAG_BYTES+(PT_ACTORS+1)*12+2||n>CAPACITY||memcmp(bytes,"PAPT",4)||bytes[4]!=1||bytes[5]||Get(bytes+6)!=n||bytes[12]||bytes[13]||bytes[14]||bytes[15])return 0;
 for(i=0;i<4;i++){crc|=(uint32_t)bytes[8+i]<<(i*8);}
 if(crc!=Crc(bytes+16,n-16))return 0;
 header=Get(bytes+at);at+=2;x=Get(bytes+at);at+=2;z=Get(bytes+at);at+=2;dir=bytes[at++];creator=bytes[at++];for(i=0;i<4;i++)random|=(uint32_t)bytes[at++]<<(i*8);starter=Get(bytes+at);at+=2;won=bytes[at++];gender=bytes[at++];partner=bytes[at++];shoes=bytes[at++];
 if(dir>3||creator!=1||starter>493||won>1||gender>1||partner>1||shoes>1||!Platinum_Map(header))return 0;
 memset(&temp,0,sizeof(temp));temp.active=1;temp.creatorDone=1;temp.random=random;temp.starter=(uint16_t)starter;temp.lastBattleWon=(uint8_t)won;temp.gender=(uint8_t)gender;temp.partner=(uint8_t)partner;temp.runningShoes=(uint8_t)shoes;temp.pc=PT_NO_PC;
 for(i=0;i<PT_VARS;i++){temp.vars[i]=(uint16_t)Get(bytes+at);at+=2;}
 memcpy(temp.flags,bytes+at,PT_FLAG_BYTES);at+=PT_FLAG_BYTES;
 memset(actors,0,sizeof(actors));
 for(i=0;i<=PT_ACTORS;i++){
  PtActor *a=&actors[i];a->x=a->fromX=(int32_t)Get32(bytes+at);at+=4;a->z=a->fromZ=(int32_t)Get32(bytes+at);at+=4;a->dir=bytes[at++];a->visible=bytes[at++];a->movementType=(uint16_t)Get(bytes+at);at+=2;a->pc=PT_NO_PC;
  if(a->x<0||a->z<0||a->x>4096*4096||a->z>4096*4096||a->dir>3||a->visible>1)return 0;
 }
 profileSize=Get(bytes+at);at+=2;
 if(profileSize>cap||profileSize!=n-at||!Platinum_Enter(&temp,header,x,z,dir))return 0;
 if(actors[PT_ACTORS].x/4096!=(int)x||actors[PT_ACTORS].z/4096!=(int)z)return 0;
 memcpy(temp.actors,actors,sizeof(actors));memcpy(profile,bytes+at,profileSize);*p=temp;if(size)*size=profileSize;return 1;
}
int Platinum_SaveRead(const char *path,Platinum *p,uint8_t *profile,size_t cap,size_t *size)
{
 char back[512];if(!path||strlen(path)>500)return 0;
 if(ReadOne(path,p,profile,cap,size)){return 1;}
 snprintf(back,sizeof(back),"%s.bak",path);return ReadOne(back,p,profile,cap,size)?2:0;
}
int Platinum_SaveWrite(const char *path,const Platinum *p,const uint8_t *profile,size_t size)
{
 uint8_t out[CAPACITY],check[4096];size_t n;char tmp[512],bak[512];FILE *f;int good,old;Platinum state;
 if(!path||strlen(path)>500||(n=Encode(p,profile,size,out))==0)return 0;
 snprintf(tmp,sizeof(tmp),"%s.tmp",path);snprintf(bak,sizeof(bak),"%s.bak",path);f=fopen(tmp,"wb");if(!f)return 0;
 good=fwrite(out,1,n,f)==n;if(fflush(f))good=0;if(fclose(f))good=0;
 if(!good||!ReadOne(tmp,&state,check,sizeof(check),NULL)){remove(tmp);return 0;}
 old=ReadOne(path,&state,check,sizeof(check),NULL);
 if(old){remove(bak);if(rename(path,bak)){remove(tmp);return 0;}}else remove(path);
 if(rename(tmp,path)){if(old)rename(bak,path);remove(tmp);return 0;}return 1;
}
