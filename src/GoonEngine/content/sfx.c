#include <GoonEngine/content/content.h>
#include <GoonEngine/content/sfx.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/utils.h>
#include <SupergoonSound/include/sound.h>

#define BUFFER_SIZE 256
static const char *audioPath = "assets/audio/%s.ogg";

typedef struct geSfx {
	char *FilePath;
	gsSfx *pSfx;

} geSfx;

static void geSfxFree(geSfx *s) {
	LogWarn("Freeing Sfx %s", s->FilePath);
	free(s->FilePath);
	if (s->pSfx)
		gsUnloadSfx(s->pSfx);
	free(s);
}

static void geSfxNewContent(geContent *content, void *data) {
	geSfx *s = (geSfx *)data;
	content->Data.Sfx = s;
}

static void geSfxDeleteContent(geContent *content) {
	geSfxFree(content->Data.Sfx);
}

static void geSfxLoadContent(geContent *content) {
	geSfx *s = content->Data.Sfx;
	if (s->pSfx || !s->FilePath)
		return;
	gsLoadSfx(s->pSfx);
}

static int geSfxFindContent(const char *path, geContent *content) {
	return strcmp(path, content->Data.Sfx->FilePath) == 0;
}

void geInitializeSfxContentType() {
	geAddContentTypeFunctions(geContentTypeSfx, geSfxNewContent, geSfxDeleteContent, geSfxLoadContent, geSfxFindContent);
}

geSfx *geSfxNew(const char *n) {
	char buffer[BUFFER_SIZE];
	sprintf(buffer, audioPath, n);
	char buf[BUFFER_SIZE];
	geGetLoadFilename(buf, sizeof(buf), buffer);
	geContent *loadedContent = geGetLoadedContent(geContentTypeBgm, buf);
	if (loadedContent) {
		return loadedContent->Data.Sfx;
	}
	geSfx *s = malloc(sizeof(*s));
	s->FilePath = strdup(buf);
	s->pSfx = gsNewSfx(buf);
	int result = geAddContent(geContentTypeSfx, (void *)s);
	if (!result) {
		LogCritical("Could not load sfx, something is wrong with content system.");
	}
	return s;
}
void geSfxLoad(geSfx *s) {
	if (s->pSfx) {
		return;
	}
	gsLoadSfx(s->pSfx);
}
void geSfxDelete(geSfx *s) {
	geUnloadContent(geContentTypeSfx, s->FilePath);
}

int geSfxPlay(geSfx *s, float volume) {
	gsPlaySfxOneShot(s->pSfx, volume);
	return true;
}