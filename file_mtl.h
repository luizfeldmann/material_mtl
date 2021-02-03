#ifndef _FILE_MTL_H_
#define _FILE_MTL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct mtl_phong_material {
    char name[256];
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float shininess;
    float transparency;
} mtl_phong_material_t;

typedef struct mtl_file {
    uint8_t numMaterials;
    mtl_phong_material_t *materials;
} mtl_file_t;

static const mtl_phong_material_t defaultMaterial = (mtl_phong_material_t) {
    .ambient = {0.2f, 0.2f, 0.2f},
    .diffuse = {0.8f, 0.8f, 0.8f},
    .specular = {1.0f, 1.0f, 1.0f},
    .shininess = 0,
    .transparency = 0,
};

uint8_t mtl_file_save(const char* filename, mtl_file_t* mtl);
uint8_t mtl_file_read(const char* filename, mtl_file_t* mtl);
void mtl_file_free(mtl_file_t* mtl);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _FILE_MTL_H_
