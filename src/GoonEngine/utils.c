#include <GoonEngine/clock.h>
#include <GoonEngine/content/textv2.h>
#include <GoonEngine/gnpch.h>
#include <GoonEngine/point.h>
#include <GoonEngine/utils.h>

const char* _systemDefaultFont = "Roboto-Regular";

void geUtilsDrawDebugText(const char* text) {
	Point bounds;
	bounds.x = 0;
	bounds.y = 0;
	geRichText* t = geRichTextNew(text, _systemDefaultFont, 32, &bounds);
	geRectangle r;
	r.x = r.y = 0;
	r.w = bounds.x;
	r.h = bounds.y;
	geRichTextSetDrawRect(t, &r);
	geRichTextLoad(t);
	geRichTextDraw(t);
}

void geUtilsDrawFps() {
	char str[50];
	sprintf(str, "%f", geClockGetTps());
	geUtilsDrawDebugText(str);
}
