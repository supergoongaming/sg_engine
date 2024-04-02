#include <GoonEngine/Camera.h>
#include <cglm/cglm.h>

geCamera *geCameraNew()
{
    geCamera *camera = calloc(1, sizeof(*camera));
    glm_vec3_zero(camera->CameraLocation);
    glm_mat4_identity(camera->CameraMatrix);
    geCameraUpdate(camera);
    return camera;
}

void geCameraUpdate(geCamera *camera)
{
    glm_mat4_identity(camera->CameraMatrix);
    glm_translate(camera->CameraMatrix, (vec3){-camera->CameraLocation[0], -camera->CameraLocation[1], 0.0f});
}

void geCameraFree(geCamera *camera)
{
    free(camera);
}