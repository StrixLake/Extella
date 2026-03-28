#include <mesh.h>
#include <materials.h>
#include <tiny_obj_loader.h>

std::pair<vector<Mesh1>, vector<Material>> load_obj(string filename);