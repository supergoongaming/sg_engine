#pragma once

typedef struct gsSfx gsSfx;
typedef struct geSfx geSfx;
typedef struct geContent geContent;

#ifdef __cplusplus
extern "C" {
#endif
void geInitializeSfxContentType();
geSfx *geSfxNew(const char *n);
void geSfxLoad(geSfx *s);
void geSfxDelete(geSfx *s);
int geSfxPlay(geSfx *s, float volume);
#ifdef __cplusplus
}
#endif