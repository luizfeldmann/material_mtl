#include "FileMtl.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define tag_material  "newmtl"
#define tag_ambient   "Ka"
#define tag_diffuse   "Kd"
#define tag_specular  "Ks"
#define tag_transpart "Tr"
#define tag_invtransp "d"
#define tag_shininess "Ns"

MtlFile::MtlFile(const char* path) noexcept
{
    if (!path)
    {
        std::cerr << "MtlFile::MtlFile received NULL pointer" << std::endl;
        return;
    }
    else
        savedPath = path;

    std::ifstream fp(path, std::ios::in | std::ios::binary);
    if (!fp.is_open())
    {
        std::cerr << "Failed to open file '" << path << "' for reading" << std::endl;
        return;
    }

    std::string line;
    PhongMaterial* currentMaterial = NULL;

    while (std::getline(fp, line))
    {
        // ignore blanks and comments
        if (line.empty())
            continue;

        if (line[0] == '#')
            continue;

        float data[3];                    // receives entry parameters
        char dump[256];                   // newmtl name dump
        const char* lptr = line.c_str();  // gets the line as /0 terminated C-string

        if (sscanf(lptr, tag_material " %s", dump) == 1)
        {
            auto result = materials.emplace( std::pair<wxString, PhongMaterial>{ wxString(dump), PhongMaterial() } );
            if (result.second == true)
                currentMaterial = &(result.first->second);
        }
        else if (currentMaterial != NULL)
        {
            if (sscanf(lptr, tag_ambient " %f %f %f", &data[0], &data[1], &data[2]) == 3) {
                currentMaterial->ambient = PhongColor(data[0], data[1], data[2]);
            }
            else if (sscanf(lptr, tag_diffuse " %f %f %f", &data[0], &data[1], &data[2]) == 3) {
                currentMaterial->diffuse = PhongColor(data[0], data[1], data[2]);
            }
            else if (sscanf(lptr, tag_specular " %f %f %f", &data[0], &data[1], &data[2]) == 3) {
                currentMaterial->specular = PhongColor(data[0], data[1], data[2]);
            }
            else if (sscanf(lptr, tag_transpart " %f", &data[0]) == 1) {
                currentMaterial->transparency = data[0];
            }
            else if (sscanf(lptr, tag_invtransp " %f", &data[0]) == 1) {
                currentMaterial->transparency = 1.0f - data[0];
            }
            else if (sscanf(lptr, tag_shininess " %f", &data[0]) == 1) {
                currentMaterial->shininess = data[0];
            }
        }
    }

    bOk = true;
    fp.close();
}

bool MtlFile::Save(const char* _path) const noexcept
{
    const std::string filename = (_path != NULL) ? _path : savedPath;

    if (filename.empty())
        return false;

    std::ofstream fp(filename, std::ios::out | std::ios::binary);
    if (!fp.is_open())
    {
        std::cerr << "Failed to open file '" << filename << "' for writing" << std::endl;
        return false;
    }

    for (const std::pair<wxString, PhongMaterial>& mat : materials)
    {
        fp << "newmtl "   << mat.first << std::endl;
        fp << tag_ambient " " << mat.second.ambient << std::endl;
        fp << tag_diffuse " " << mat.second.diffuse << std::endl;
        fp << tag_specular " " << mat.second.specular << std::endl;
        fp << tag_shininess " " << mat.second.shininess << std::endl;
        fp << tag_transpart " " << mat.second.transparency << std::endl;
        fp << std::endl;
    }

    fp.close();
    return true;
}

MtlFile::iterator MtlFile::insert(const wxString& name, const PhongMaterial& material)
{
    auto result = materials.emplace( std::pair<wxString, PhongMaterial>{ name, material } );
    if (result.second == true)
        return result.first;
    else
        return materials.end();
}
