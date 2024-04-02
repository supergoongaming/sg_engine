#pragma once
#include <cglm/mat4.h>
#include <cglm/vec3.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct geCamera
    {
        mat4 CameraMatrix;
        vec3 CameraLocation;
        float CameraScale;

    } geCamera;

    geCamera *geCameraNew();
    inline void geCameraAddX(geCamera *camera, float x) { camera->CameraLocation[0] += x; }
    inline void geCameraSetX(geCamera *camera, float x) { camera->CameraLocation[0] = x; }
    inline void geCameraAddY(geCamera *camera, float y) { camera->CameraLocation[1] += y; }
    inline void geCameraSetY(geCamera *camera, float y) { camera->CameraLocation[1] = y; }
    void geCameraUpdate(geCamera *camera);
    void geCameraFree(geCamera *camera);

#ifdef __cplusplus
}
#endif