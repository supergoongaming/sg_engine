#pragma once

typedef struct gsBgm gsBgm;
typedef struct geBgm geBgm;
typedef struct geContent geContent;

#ifdef __cplusplus
extern "C"
{
#endif
    void geInitializeBgmContentType();
    geBgm *geBgmNew(const char *bgmName);
    void geBgmSetBackground(geBgm* bgm, int isBackground);
    void geBgmLoad(geBgm* bgm);
    void geBgmDelete(geBgm *bgm);
    int geBgmPlay(geBgm *bgm, float volume, int loops);
    int geBgmStop(geBgm *bgm);
#ifdef __cplusplus
}
#endif