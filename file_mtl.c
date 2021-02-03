#include "file_mtl.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "getdelim.h"

uint8_t mtl_file_read(const char* filename, mtl_file_t* mtl)
{
    // sanity check
    if (filename == NULL || mtl == NULL)
    {
        fprintf(stderr, "\nmtl_file_read received NULL pointer");
        return 0;
    }

    if (mtl->numMaterials != 0 || mtl->materials != NULL)
    {
        fprintf(stderr, "\nmtl_file_read received struct not initialized to be empty");
        mtl->materials = NULL;
        mtl->numMaterials = 0;
    }

    // open file
    FILE* fp = fopen(filename, "rb+");

    if (fp == NULL)
    {
        fprintf(stderr, "\nfailed to open file '%s': %d %s", filename, errno, strerror(errno));
        return 0;
    }

    // read line by line
    char* line = NULL;
    size_t line_alloc_size = 0;
    ssize_t line_read;

    mtl_phong_material_t* currentMaterial = 0;

    while ((line_read = getline(&line, &line_alloc_size, fp)) != -1)
    {
        char dump[256];

        // skip comments
        if (line[0] == '#')
            continue;

        #define match_keyword(keyword) (sscanf(line, (keyword " %[^\t\r\n]"), dump) == 1)

        if match_keyword("newmtl") // new material will be defined inside the file
        {
            // create new material in the file array
            mtl_phong_material_t* new_array = (mtl_phong_material_t*)realloc(mtl->materials, sizeof(mtl_phong_material_t) * (mtl->numMaterials + 1));
            if (new_array == NULL)
            {
                fprintf(stderr, "\nfailed to alloc memory for additional material");
                goto bail;
            }

            mtl->materials = new_array;
            currentMaterial = &mtl->materials[mtl->numMaterials++];

            // assign name and defaults
            *currentMaterial = defaultMaterial;
            strncpy(currentMaterial->name, dump, sizeof(currentMaterial->name));
        }
        else if (currentMaterial != NULL) // there is a material being defined
        {
            #define sscanf3float(array) sscanf(dump, "%f %f %f", &array[0], &array[1], &array[2])
            // read parameters
            if match_keyword("Ka")
            {
                sscanf3float(currentMaterial->ambient);
            }
            else if match_keyword("Kd")
            {
                sscanf3float(currentMaterial->diffuse);
            }
            else if match_keyword("Ks")
            {
                sscanf3float(currentMaterial->specular);
            }
            else if match_keyword("d")
            {
                sscanf(dump, "%f", &currentMaterial->transparency);
                currentMaterial->transparency = 1 - currentMaterial->transparency;
            }
            else if match_keyword("Tr")
            {
                sscanf(dump, "%f", &currentMaterial->transparency);
            }
            else if match_keyword("Ns")
            {
                sscanf(dump, "%f", &currentMaterial->shininess);
            }
        }
    }

    bail:
    fclose(fp);
    return mtl->numMaterials;
}

uint8_t mtl_file_save(const char* filename, mtl_file_t* mtl)
{
    uint8_t countSaved = 0;

    if (filename == NULL || mtl == NULL)
    {
        err_null:
        fprintf(stderr, "\nmtl_file_save received NULL pointer");
        return 0;
    }

    if (mtl->materials == NULL || mtl->numMaterials == 0)
        goto err_null;

    // open file
    FILE* fp = fopen(filename, "wb+");

    if (fp == NULL)
    {
        fprintf(stderr, "\nfailed to write file '%s': %d %s", filename, errno, strerror(errno));
        return 0;
    }

    for (uint8_t i = 0; i < mtl->numMaterials; i++)
    {
        mtl_phong_material_t* mat = &mtl->materials[i];

        char buffer[512];

        int len = sprintf(buffer,
                "newmtl %s\n"
                "Ka %f %f %f\n"
                "Kd %f %f %f\n"
                "Ks %f %f %f\n"
                "Ns %f\n"
                "Tr %f\n\n",
                mat->name,
                mat->ambient[0], mat->ambient[1], mat->ambient[2],
                mat->diffuse[0], mat->diffuse[1], mat->diffuse[2],
                mat->specular[0], mat->specular[1], mat->specular[2],
                mat->shininess,
                mat->transparency
                );

        if (fwrite(buffer, sizeof(char), len, fp) != len) // some error...
            break;
    }

    fclose(fp);
    return countSaved;
}

void mtl_file_free(mtl_file_t* mtl)
{
    // sanity check
    if (mtl == NULL)
        return;

    free(mtl->materials);

    mtl->numMaterials = 0;
    mtl->materials = NULL;
}
