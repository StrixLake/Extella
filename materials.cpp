#include <materials.h>

Material material_factory(tinyobj::material_t &material)
{
    Material out = Material();

    out.material_name = material.name;

    out.material_constantsF1["shininess"] = material.shininess;
    out.material_constantsF1["refraction"] = material.ior;
    out.material_constantsF1["roughness"] = material.roughness;
    out.material_constantsF1["metallic"] = material.metallic;
    out.material_constantsF1["sheen"] = material.sheen;
    out.material_constantsF1["clearcoat_thickness"] = material.clearcoat_thickness;
    out.material_constantsF1["clearcoat_roughness"] = material.clearcoat_roughness;
    out.material_constantsF1["anisotropy"] = material.anisotropy;
    out.material_constantsF1["anisotropy_rotation"] = material.anisotropy_rotation;
    out.material_constantsF1["opacity"] = material.dissolve;
    out.material_constantsF1["illum"] = (float)material.illum;

    if(material.dissolve != 1) out.isTransparent = true;

    out.material_constantsF3["ambient"] = {material.ambient[0],
                                            material.ambient[1],
                                            material.ambient[2]};
    out.material_constantsF3["diffuse"] = {material.diffuse[0],
                                            material.diffuse[1],
                                            material.diffuse[2]};
    out.material_constantsF3["specular"] = {material.specular[0],
                                            material.specular[1],
                                            material.specular[2]};
    out.material_constantsF3["emission"] = {material.emission[0],
                                            material.emission[1],
                                            material.emission[2]};

    out.material_constantsF3["transmittance"] = {material.transmittance[0],
                                                    material.transmittance[1],
                                                    material.transmittance[2]};


    out.material_textures["ambient"] = material.ambient_texname.data();
    out.material_textures["diffuse"] = material.diffuse_texname.data();
    out.material_textures["specular"] = material.specular_texname.data();
    out.material_textures["specular_highlight"] = material.specular_highlight_texname.data();
    out.material_textures["bump"] = material.bump_texname.data();
    out.material_textures["displacement"] = material.displacement_texname.data();
    out.material_textures["alpha"] = material.alpha_texname.data();
    out.material_textures["reflection"] = material.reflection_texname.data();
    out.material_textures["roughness"] = material.roughness_texname.data();
    out.material_textures["metallic"] = material.metallic_texname.data();
    out.material_textures["sheen"] = material.sheen_texname.data();
    out.material_textures["emissive"] = material.emissive_texname.data();
    out.material_textures["normal"] = material.normal_texname.data();

    return out;
}