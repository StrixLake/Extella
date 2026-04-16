#pragma once
#include <vectors.h>
#include <tiny_obj_loader.h>
#include <boost/unordered_map.hpp>
#ifdef BOOST
#include <boost/container/string.hpp>
using string = boost::container::basic_string<char>;
#else
#include <string>
using string = std::string;
#endif
using boost::unordered_map;

class Material
{
public:
    string material_name = "default";
    bool isTransparent = false;
    
    // <semantic, texture name>
    // semantic is the name of the texture referenced in the shader
    // since differnt textures may have the same semantic
    // or different semantic for same texture
    unordered_map<string, string> material_textures;
    
    unordered_map<string, float> material_constantsF1;
    unordered_map<string, Float3> material_constantsF3;

    struct Mat
    {
        Float3 ambient;
        float pad;
        Float3 kdiffuse;
        float pad1;
        Float3 specular;
        float pad2;
        Float3 transmittance;
        float pad3;
        Float3 emission;
        float shininess, ior, dissolve;
        float roughness,metallic,sheen;
        float clearcoat_thickness,clearcoat_roughness;
        float pad4[5];
    };

    Mat constMaterial;
};


// create a material from tinyobj::material_t
Material material_factory(tinyobj::material_t &material);