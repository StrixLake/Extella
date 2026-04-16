#include <materials.h>

Material material_factory(tinyobj::material_t &material)
{
    Material out = Material();

    out.material_name = material.name;

    out.constMaterial.shininess = material.shininess;
    out.constMaterial.ior = material.ior;
    out.constMaterial.roughness = material.roughness;
    out.constMaterial.metallic = material.metallic;
    out.constMaterial.sheen = material.sheen;
    out.constMaterial.clearcoat_thickness = material.clearcoat_thickness;
    out.constMaterial.clearcoat_roughness = material.clearcoat_roughness;
    out.constMaterial.dissolve = material.dissolve;

    if(material.dissolve != 1) out.isTransparent = true;

    out.constMaterial.ambient = {material.ambient[0],
                                  material.ambient[1],
                                  material.ambient[2]};
    out.constMaterial.kdiffuse = {material.diffuse[0],
                                 material.diffuse[1],
                                 material.diffuse[2]};
    out.constMaterial.specular = {material.specular[0],
                                  material.specular[1],
                                  material.specular[2]};
    out.constMaterial.emission = {material.emission[0],
                                  material.emission[1],
                                  material.emission[2]};

    out.constMaterial.transmittance = {material.transmittance[0],
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