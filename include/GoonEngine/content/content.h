#pragma once
typedef struct geBgm geBgm;
typedef struct geFont geFont;
typedef struct geRichText geRichText;
typedef struct geImage geImage;

typedef enum geContentTypes {
	geContentTypeDefault = 0,
	geContentTypeBgm,
	geContentTypeFont,
	geContentTypeText,
	geContentTypeImage,
	geContentTypeMax,
} geContentTypes;

typedef struct geContent {
	geContentTypes Type;
	unsigned int RefCount;
	union {
		geBgm *Bgm;
		geFont *Font;
		geRichText *Text;
		geImage *Image;
	} Data;

} geContent;

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*ContentTypeCreateFunc)(geContent *content, void *data);
typedef void (*ContentTypeDeleteFunc)(geContent *content);
typedef void (*ContentTypeLoadFunc)(geContent *content);
typedef int (*ContentTypeCompareFunc)(const char *lhs, geContent *rhs);

void geContentInitializeAllContentTypes();
geContent *geGetLoadedContent(geContentTypes type, const char *path);
int geLoadAllContent();
int geUnloadAllContent();
int geUnloadContent(geContentTypes type, const char *data);
int geAddContent(geContentTypes type, void *data);
int geAddContentTypeFunctions(geContentTypes type, ContentTypeCreateFunc create,
							  ContentTypeDeleteFunc del,
							  ContentTypeLoadFunc load,
							  ContentTypeCompareFunc comp);

#ifdef __cplusplus
}
#endif