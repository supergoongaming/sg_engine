#include <GoonEngine/gnpch.h>
#include <SupergoonSound/include/sound.h>
#include <GoonEngine/content/bgm.h>
#include <GoonEngine/content/content.h>

#define BUFFER_SIZE 256

typedef struct geBgm
{
    char *FilePath;
    float BgmStart, BgmEnd;
    gsBgm *pBgm;

} geBgm;

static void geBgmFree(geBgm *bgm)
{
    LogWarn("Deleted bgm %s", bgm->FilePath);
    free(bgm->FilePath);
    if (bgm->pBgm)
        free(bgm->pBgm);
    free(bgm);
}

static void geBgmNewContent(geContent *content, void *data)
{
    geBgm *bgm = (geBgm *)data;
    content->Data.Bgm = bgm;
}

static void geBgmDeleteContent(geContent *content)
{
    geBgmFree(content->Data.Bgm);
}

static void geBgmLoadContent(geContent *content)
{
    geBgm *bgm = content->Data.Bgm;
    // already loaded
    if (bgm->pBgm)
        return;
    bgm->pBgm = gsLoadBgm(bgm->FilePath);
    bgm->pBgm->loop_begin = bgm->BgmStart;
    bgm->pBgm->loop_end = bgm->BgmEnd;
}

static int geBgmFindContent(const char *path, geContent *content)
{
    return strcmp(path, content->Data.Bgm->FilePath) == 0;
}

void geInitializeBgmContentType()
{
    geAddContentTypeFunctions(geContentTypeBgm, geBgmNewContent, geBgmDeleteContent, geBgmLoadContent, geBgmFindContent);
}

geBgm *geBgmNew(const char *bgmName, float bgmStart, float bgmEnd)
{
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "assets/audio/%s.ogg", bgmName);
    geContent *loadedContent = geGetLoadedContent(geContentTypeBgm, buffer);
    if (loadedContent)
    {
        return loadedContent->Data.Bgm;
    }
    geBgm *bgm = malloc(sizeof(*bgm));
    bgm->BgmEnd = bgmEnd;
    bgm->BgmStart = bgmStart;
    bgm->FilePath = strdup(buffer);
    int result = geAddContent(geContentTypeBgm, bgm);
    if (!result)
    {
        LogCritical("Could not load BGM, something is wrong with content system.");
    }
    return bgm;
}

void geBgmDelete(geBgm *bgm)
{
    geUnloadContent(geContentTypeBgm, bgm->FilePath);
}

int geBgmPlay(geBgm *bgm, float volume, int loops)
{
    gsPreLoadBgm(bgm->pBgm);
    gsPlayBgm(volume);
    if (loops == -1)
    {
        gsSetPlayerLoops(255);
    }
    else
    {
        gsSetPlayerLoops(loops);
    }
    return true;
}
