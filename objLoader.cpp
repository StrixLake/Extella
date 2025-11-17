#include <objLoader.h>


void load_stl(vector<Float3> &vertex, vector<uint32_t> &index, vector<Float3> &normal, const char* filename){
    // read the stl file
    std::ifstream vertexData(filename, std::ios::binary);
    vertexData.seekg(80);
    char num[4];
    vertexData.read(num, 4);
    int triangleCount = *(int*)num;

    unordered_map<Float3, uint32_t> vertex_; // hold the index of the vertex in the vector
    unordered_map<Float3, uint32_t> normals_; // hold the number of normals of that vertex
    
    for (int i = 0; i < triangleCount; ++i){
        Float3 triangle[4];
        vertexData.read((char*)&triangle, sizeof(triangle));
        vertexData.ignore(2);

        // all 3 vertex of the triangle assume the name
        // normal as its plane
        // if a vertex is not already in the vertex vector
        // then push_back it into the vector and 
        // make a key:value pair of the vertex and its
        // intex (its position in the vector) in the unordered_map
        // and also add the index in the index vector
        // the map can be used to check if the vertex is already
        // in the vector and if it is then take its position 
        // the in vector (from the map)
        // and put it in the index vector
        // the normal is averaged

        for(int j = 0; j < 3; ++j){
            if(vertex_.find(triangle[j+1]) == vertex_.end()){
                vertex.push_back(triangle[j+1]);
                vertex_[triangle[j+1]] = vertex.size() -1;
                index.push_back(vertex_[triangle[j+1]]);
                
                normal.push_back(triangle[0]);
                normals_[triangle[0]]++;
            }
            else{
                index.push_back(vertex_[triangle[j+1]]);
                normal[vertex_[triangle[j+1]]] = (triangle[0] + normal[vertex_[triangle[j+1]]]*normals_[triangle[0]])/(normals_[triangle[0]] +1);
                normals_[triangle[0]]++;
            }
        }
    }
    return;
}


template<typename T>
void parseIndex(tinyobj::mesh_t &mesh, tinyobj::attrib_t &attribute, vector<Float3> &vertices, vector<int32_t> &index, vector<Float3> &normals, vector<Float2> &TexCoords){
    
    unordered_map<T, int32_t> vertex_;

    for(tinyobj::index_t triangle : mesh.indices){
        T node;
        
        node.vertex = {attribute.vertices[3*triangle.vertex_index],
                    attribute.vertices[3*triangle.vertex_index+1],
                    attribute.vertices[3*triangle.vertex_index+2]};

        if constexpr(std::is_same_v<T, Float5>) {
            node.TexCord = {attribute.texcoords[2*triangle.texcoord_index],
                            attribute.texcoords[2*triangle.texcoord_index+1]};
        }
        else if constexpr(std::is_same_v<T, Float6>) {
            node.normal = {attribute.normals[3*triangle.normal_index],
                            attribute.normals[3*triangle.normal_index+1],
                            attribute.normals[3*triangle.normal_index+2]};
        }
        else if constexpr(std::is_same_v<T, Float8>) {
            node.normal = {attribute.normals[3*triangle.normal_index],
                            attribute.normals[3*triangle.normal_index+1],
                            attribute.normals[3*triangle.normal_index+2]};
            node.TexCord = {attribute.texcoords[2*triangle.texcoord_index],
                            attribute.texcoords[2*triangle.texcoord_index+1]};
        }

        if(vertex_.find(node) == vertex_.end()){
            vertices.push_back(node.vertex);
            if constexpr (std::is_same_v<T, Float5>) TexCoords.push_back(node.TexCord);
            
            else if constexpr (std::is_same_v<T, Float6>) normals.push_back(node.normal);

            else if constexpr (std::is_same_v<T, Float8>) {
                normals.push_back(node.normal);
                TexCoords.push_back(node.TexCord);
            }
            vertex_[node] = vertices.size() -1;
            index.push_back(vertex_[node]);
        }
        else{
            index.push_back(vertex_[node]);
        }

    }
}

void interpolateNormals();


void load_obj(vector<Float3> &vertex, vector<int32_t> &index, vector<Float3> &normals, vector<Float2> &TexCoords, const char* filename){
    string inputFile(filename);
    tinyobj::ObjReaderConfig config;
    config.vertex_color = false;

    tinyobj::ObjReader reader;
    reader.ParseFromFile(inputFile, config);
    
    // mesh object contains the indices in index_t
    // which contains the vertex, normal & texCord
    // index of each triangle
    vector<tinyobj::shape_t> shapes = reader.GetShapes();
    tinyobj::mesh_t mesh = shapes[0].mesh;

    // attribute object contains 3 vectors, each for
    // vector, normal and texCord which are indexed
    // by indices in mesh_t.indices
    tinyobj::attrib_t attribute = reader.GetAttrib();

    bool normalPresent = mesh.indices[0].normal_index != -1 ? true : false;
    bool texCordPresent = mesh.indices[0].texcoord_index != -1 ? true : false;

    if(normalPresent){
        if(texCordPresent){
            parseIndex<Float8>(mesh, attribute, vertex, index, normals, TexCoords);
        }
        else{
            parseIndex<Float6>(mesh, attribute, vertex, index, normals, TexCoords);
        }
    }
    else{
        if(texCordPresent){
            parseIndex<Float5>(mesh, attribute, vertex, index, normals, TexCoords);
        }
        else{
            parseIndex<Vertex>(mesh, attribute, vertex, index, normals, TexCoords);
        }
    }

    return;
}