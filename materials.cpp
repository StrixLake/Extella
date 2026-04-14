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

    auto loadTexture = [&out](string name, string& texname)
    {
        if(texname != "")
        {
            out.material_textures[name] = texname.data();
        }
    };

    loadTexture("ambient", material.ambient_texname);
    loadTexture("diffuse", material.diffuse_texname);
    loadTexture("specular", material.specular_texname);
    loadTexture("specular_highlight", material.specular_highlight_texname);
    loadTexture("bump", material.bump_texname);
    loadTexture("displacement", material.displacement_texname);
    loadTexture("alpha", material.alpha_texname);
    loadTexture("reflection", material.reflection_texname);
    loadTexture("roughness", material.roughness_texname);
    loadTexture("metallic", material.metallic_texname);
    loadTexture("sheen", material.sheen_texname);
    loadTexture("emissive", material.emissive_texname);
    loadTexture("normal", material.normal_texname);

    return out;
}