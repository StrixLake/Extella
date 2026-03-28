#include <vertexLoader.h>

Mesh1 construct_mesh(tinyobj::shape_t shape, tinyobj::attrib_t attribute);

std::pair<vector<Mesh1>, vector<Material>> load_obj(string filename)
{
    std::string inputFile(filename);
    tinyobj::ObjReaderConfig config;
    config.vertex_color = false;

    tinyobj::ObjReader reader;
    reader.ParseFromFile(inputFile, config);
    
    // first we get the vector of all shapes in the obj file
    vector<tinyobj::shape_t> shapes = reader.GetShapes();
    // sort the shapes(meshes) by their material
    // so all the same material meshes are together
    std::sort(shapes.begin(), shapes.end(), [](tinyobj::shape_t& a, tinyobj::shape_t& b)->bool
    {
        return a.mesh.material_ids < b.mesh.material_ids;
    });

    // then we get the materials data
    vector<tinyobj::material_t> materials = reader.GetMaterials();

    // and then the attribute data that
    // contains the per vertex data
    tinyobj::attrib_t attributes = reader.GetAttrib();

    // the material factory function can construct a material
    // directly from tinyobj::material_t
    vector<Material> out_material;
    for(auto material : materials)
    {
        out_material.push_back(material_factory(material));
    }

    // before we convert the shapes to mesh1 instances,
    // we merge the meshes with same material ids

    auto merge_mesh = [](){};

    vector<Mesh1> out_mesh;
    for(auto shape : shapes)
    {
        out_mesh.push_back(construct_mesh(shape, attributes));
    }

    return {out_mesh, out_material};

}