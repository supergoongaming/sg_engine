#include <GoonEngine/content/content.h>
#include <GoonEngine/debug.h>
#include <GoonEngine/gnpch.h>

// Content Types
#include <GoonEngine/content/bgm.h>

#define INITIAL_CONTENT_SIZE 0

typedef struct contentSizeCount {
	int Size, Count;
} contentSizeCount;
static contentSizeCount _loadedContentData[geContentTypeMax];
static geContent **_loadedContent[geContentTypeMax];

static ContentTypeCreateFunc _newContentFunctions[geContentTypeMax];
static ContentTypeDeleteFunc _deleteContentFunctions[geContentTypeMax];
static ContentTypeLoadFunc _loadContentFunctions[geContentTypeMax];
static ContentTypeCompareFunc _compareContentFunctions[geContentTypeMax];

static int findContentIndexByName(geContentTypes type, const char *name) {
	contentSizeCount *info = &_loadedContentData[type];
	for (size_t i = 0; i < info->Size; i++) {
		if (_compareContentFunctions[type](name, _loadedContent[type][i])) {
			return i;
		}
	}
	return -1;
}

static void expandArraySize(geContentTypes type) {
	contentSizeCount *info = &_loadedContentData[type];
	if (info->Count + 1 >= info->Size / 2) {
		size_t newSize = info->Size + 1 * 2;
		_loadedContent[type] =
			realloc(_loadedContent[type], newSize * sizeof(geContent *));
		if (!_loadedContent[type]) {
			LogCritical("Could not reallocate content array of type %d", type);
		}
		info->Size = newSize;
	}
}

int geAddContentTypeCreateFunc(geContentTypes type,
							   ContentTypeCreateFunc func) {
	if (!_newContentFunctions[type]) {
		_newContentFunctions[type] = func;
		return true;
	}
	return false;
}

geContent *geGetLoadedContent(geContentTypes type, const char *path) {
	int index = findContentIndexByName(type, path);
	if (index == -1) return NULL;
	geContent **array = _loadedContent[type];
	if (!array)
		LogCritical(
			"Loaded content type does not exist, can't get it properly");
	array[index]->RefCount++;
	return array[index];
}

int geLoadAllContent() {
	for (size_t i = 0; i < geContentTypeMax; i++) {
		contentSizeCount data = _loadedContentData[i];
		for (size_t j = 0; j < data.Count; j++) {
			_loadContentFunctions[i](_loadedContent[i][j]);
		}
	}
	return true;
}

int geUnloadAllContent() {
	for (size_t i = 0; i < geContentTypeMax; i++) {
		contentSizeCount data = _loadedContentData[i];
		for (size_t j = 0; j < data.Count; j++) {
			_deleteContentFunctions[i](_loadedContent[i][j]);
		}
	}
	return true;
}

int geUnloadContent(geContentTypes type, const char *data) {
	int loc = findContentIndexByName(type, data);
	if (loc == -1) {
		LogDebug("Did not find content with type %d with path %s", type, data);
		return false;
	}
	geContent *content = _loadedContent[type][loc];
	if (--content->RefCount == 0) {
		LogDebug("RefCount is 0, deleting type %d with name %s", type, data);
		if (!_deleteContentFunctions[type])
			LogCritical(
				"No delete function available for this type, please register!");
		_deleteContentFunctions[type](content);
		_loadedContent[type][loc] = NULL;
	}
	return true;
}

int geAddContent(geContentTypes type, void *data) {
	expandArraySize(type);
	contentSizeCount *info = &_loadedContentData[type];
	geContent *content = malloc(sizeof(*content));
	content->RefCount = 1;
	content->Type = type;
	if (!_newContentFunctions[type])
		LogCritical(
			"Content type is not available, please register this function "
			"properly!");
	_newContentFunctions[type](content, data);
	_loadedContent[type][info->Count++] = content;
	return true;
}
int geAddContentTypeFunctions(geContentTypes type, ContentTypeCreateFunc create,
							  ContentTypeDeleteFunc del,
							  ContentTypeLoadFunc load,
							  ContentTypeCompareFunc comp) {
	_newContentFunctions[type] = create;
	_deleteContentFunctions[type] = del;
	_loadContentFunctions[type] = load;
	_compareContentFunctions[type] = comp;
	return true;
}
