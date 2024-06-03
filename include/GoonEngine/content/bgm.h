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
    void geBgmDelete(geBgm *bgm);
    int geBgmPlay(geBgm *bgm, float volume, int loops);
#ifdef __cplusplus
}
#endif