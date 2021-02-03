#ifndef _MTL_FILE_H_
#define _MTL_FILE_H_

#include <map>
#include <wx/colour.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#ifndef ALWAYS_INLINE
    #define ALWAYS_INLINE inline __attribute__((always_inline))
#endif

struct PhongColor
{
private:
    float color[3]; // RGBA normalized in 0.0-1.0 range

public:
    /// OPERATORS
    PhongColor& operator=(const PhongColor& other) = default;
    friend std::ostream& operator<<(std::ostream& os, const PhongColor& c);

    /// CONSTRUCTORS
    // build from float rgb in 0.0-1.0 range
    ALWAYS_INLINE PhongColor(const float& r, const float& g, const float& b) noexcept :
        color{r, g, b}
    {
    }

    // from bytes in 0-255 range
    ALWAYS_INLINE PhongColor(const unsigned char& r, const unsigned char& g, const unsigned char& b) noexcept :
        color{float(r)/255.0f, float(g)/255.0f, float(b)/255.0f}
    {
    }

    // from wxColor
    ALWAYS_INLINE PhongColor(const wxColour& c) noexcept :
        PhongColor((unsigned char)c.Red(), (unsigned char)c.Green(), (unsigned char)c.Blue())
    {
    }

    // from vector
    ALWAYS_INLINE PhongColor(const glm::vec3& c3) noexcept :
        PhongColor(c3.x, c3.y, c3.z)
    {
    }

    ALWAYS_INLINE PhongColor(const glm::vec4& c4) noexcept :
        PhongColor(c4.x, c4.y, c4.z)
    {
    }

    /// ACCESSORS
    ALWAYS_INLINE float r() const noexcept {return color[0];}
    ALWAYS_INLINE float g() const noexcept {return color[1];}
    ALWAYS_INLINE float b() const noexcept {return color[2];}

    ALWAYS_INLINE glm::vec3 rgb() const noexcept {
        return glm::vec3(color[0], color[1], color[2]);
    }


    ALWAYS_INLINE unsigned char R() const noexcept {return static_cast<unsigned char>(255.0f*color[0]);}
    ALWAYS_INLINE unsigned char G() const noexcept {return static_cast<unsigned char>(255.0f*color[1]);}
    ALWAYS_INLINE unsigned char B() const noexcept {return static_cast<unsigned char>(255.0f*color[2]);}

    ALWAYS_INLINE wxColour RGB() const noexcept {
        return wxColour(R(), G(), B());
    }
};

// make sure this has has minimal size...
static_assert(sizeof(PhongColor) <= 12, "class PhongColor size overhead");

inline std::ostream& operator<<(std::ostream& os, const PhongColor& c)
{
    os << c.color[0] << " " << c.color[1] << " " << c.color[2];
    return os;
}

class PhongMaterial
{
public:
    PhongColor ambient  = PhongColor(0.2f, 0.2f, 0.2f);
    PhongColor diffuse  = PhongColor(0.8f, 0.8f, 0.8f);
    PhongColor specular = PhongColor(1.0f, 1.0f, 1.0f);
    float shininess = 0;
    float transparency = 0;

    PhongMaterial() = default;
};

class MtlFile
{
private:
    std::map<wxString, PhongMaterial> materials;                // maps materials to their names
    using       iterator = decltype(materials)::iterator;
    using const_iterator = decltype(materials)::const_iterator;

    bool bOk = false;               // was this filed properly loaded ?
    std::string savedPath;          // used in "save" when given path is omitted; if not omitted, this gets updated

public:
    MtlFile(const char* path) noexcept;

    // tests if this file was loaded properly
    ALWAYS_INLINE bool IsOk() const noexcept {
        return bOk;
    }

    // get's the file path associated to this MTL file
    ALWAYS_INLINE const std::string& GetFilePath() const noexcept {
        return savedPath;
    }

    // saves to a given file; if no path is specified, original path is assumed
    bool Save(const char* path = NULL) const noexcept;

    // adds new material to list
    iterator insert(const wxString& name, const PhongMaterial& material);

    // allows iterating over the materials in the file (relays to underlying map)
    ALWAYS_INLINE iterator begin() noexcept {
        return materials.begin();
    }

    ALWAYS_INLINE iterator end() noexcept {
        return materials.end();
    }

    ALWAYS_INLINE const_iterator cbegin() const noexcept {
        return materials.cbegin();
    }
    ALWAYS_INLINE const_iterator cend() const noexcept {
        return materials.cend();
    }

    ALWAYS_INLINE iterator find(const wxString& name) noexcept {
        return materials.find(name);
    }

    ALWAYS_INLINE const_iterator find(const wxString& name) const noexcept {
        return materials.find(name);
    }

    ALWAYS_INLINE PhongMaterial& at(const wxString& name) {
        return materials.at(name);
    }

    ALWAYS_INLINE const PhongMaterial& at(const wxString& name) const {
        return materials.at(name);
    }

    ALWAYS_INLINE size_t size() {
        return materials.size();
    }
};



#endif // _MTL_FILE_H_
