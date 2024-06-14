#include <GoonEngine/content/bgm.h>
#include <GoonEngine/content/content.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/utils.h>
#include <SupergoonSound/include/sound.h>

#define BUFFER_SIZE 256

typedef struct geBgm {
	char *FilePath;
	// float BgmStart, BgmEnd;
	gsBgm *pBgm;
	int isBackground;

} geBgm;

static void geBgmFree(geBgm *bgm) {
	free(bgm->FilePath);
	if (bgm->pBgm)
		free(bgm->pBgm);
	free(bgm);
}

static void geBgmNewContent(geContent *content, void *data) {
	geBgm *bgm = (geBgm *)data;
	content->Data.Bgm = bgm;
}

static void geBgmDeleteContent(geContent *content) {
	geBgmFree(content->Data.Bgm);
}

static void geBgmLoadContent(geContent *content) {
	geBgm *bgm = content->Data.Bgm;
	if (bgm->pBgm || !bgm->FilePath)
		return;
	bgm->pBgm = gsLoadBgm(bgm->FilePath);
}

static int geBgmFindContent(const char *path, geContent *content) {
	return strcmp(path, content->Data.Bgm->FilePath) == 0;
}

void geInitializeBgmContentType() {
	geAddContentTypeFunctions(geContentTypeBgm, geBgmNewContent, geBgmDeleteContent, geBgmLoadContent, geBgmFindContent);
}

// #ifdef GN_PLATFORM_MACOS
// static const char *audioPath = "../Resources/assets/audio/%s.ogg";
// #else
static const char *audioPath = "assets/audio/%s.ogg";
// #endif

geBgm *geBgmNew(const char *bgmName) {
	char buffer[BUFFER_SIZE];
	sprintf(buffer, audioPath, bgmName);
	char buf[1000];
	GetLoadFilename(buf, sizeof(buf), buffer);
	geContent *loadedContent = geGetLoadedContent(geContentTypeBgm, buf);
	if (loadedContent) {
		return loadedContent->Data.Bgm;
	}
	geBgm *bgm = malloc(sizeof(*bgm));
	bgm->FilePath = strdup(buf);
	bgm->pBgm = NULL;
	bgm->isBackground = false;
	int result = geAddContent(geContentTypeBgm, bgm);
	if (!result) {
		LogCritical("Could not load BGM, something is wrong with content system.");
	}
	return bgm;
}
void geBgmLoad(geBgm *bgm) {
	bgm->pBgm = gsLoadBgm(bgm->FilePath);
}

void geBgmDelete(geBgm *bgm) {
	geUnloadContent(geContentTypeBgm, bgm->FilePath);
}

void geBgmSetBackground(geBgm *bgm, int isBackground) {
	bgm->isBackground = isBackground;
}

int geBgmPlay(geBgm *bgm, float volume, int loops) {
	// TODO this is here as if it in load, then it has issues when trying to play a bgm again.
	if (bgm->isBackground) {
		gsPreLoadBgm(bgm->pBgm, true);

	} else {
		gsPreLoadBgm(bgm->pBgm, false);
	}
	if (bgm->isBackground) {
		gsPlayBackgroundBgm(volume);
		if (loops == -1) {
			gsSetPlayerLoops(255);
		} else {
			gsSetPlayerLoops(loops);
		}
	} else {
		gsPlayBgm(volume);
		if (loops == -1) {
			gsSetPlayerLoops(255);
		} else {
			gsSetPlayerLoops(loops);
		}
	}
	return true;
}

int geBgmStop(geBgm *bgm) {
	if (bgm->isBackground) {
		gsStopBackgroundBgm();
	} else {
		gsStopBgm();
	}
	return 1;
}
