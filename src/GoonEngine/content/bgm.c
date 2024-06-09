#include <GoonEngine/gnpch.h>
#include <SupergoonSound/include/sound.h>
#include <GoonEngine/content/bgm.h>
#include <GoonEngine/content/content.h>
#include <GoonEngine/utils.h>

#define BUFFER_SIZE 256

typedef struct geBgm
{
    char *FilePath;
    // float BgmStart, BgmEnd;
    gsBgm *pBgm;

} geBgm;

static void geBgmFree(geBgm *bgm)
{
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
    if (bgm->pBgm || !bgm->FilePath)
        return;
    bgm->pBgm = gsLoadBgm(bgm->FilePath);
}

static int geBgmFindContent(const char *path, geContent *content)
{
    return strcmp(path, content->Data.Bgm->FilePath) == 0;
}

void geInitializeBgmContentType()
{
    geAddContentTypeFunctions(geContentTypeBgm, geBgmNewContent, geBgmDeleteContent, geBgmLoadContent, geBgmFindContent);
}

#ifdef GN_PLATFORM_MACOS
static const char* audioPath = "../Resources/assets/audio/%s.ogg";
#else
static const char* audioPath = "assets/audio/%s.ogg";
#endif

geBgm *geBgmNew(const char *bgmName)
{
    char buffer[BUFFER_SIZE];
    sprintf(buffer, audioPath, bgmName);
	char buf[1000];
	GetLoadFilename(buf, sizeof(buf), buffer);
    geContent *loadedContent = geGetLoadedContent(geContentTypeBgm, buf);
    if (loadedContent)
    {
        return loadedContent->Data.Bgm;
    }
    geBgm *bgm = malloc(sizeof(*bgm));
    bgm->FilePath = strdup(buf);
    bgm->pBgm = NULL;
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
