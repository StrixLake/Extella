#pragma once
#include <tiny_obj_loader.h>
#include <vectors.h>
#include <vector>
#include <fstream>
#include <string>
using std::ifstream;
using std::string;
using std::vector;


void load_stl(vector<Float3> &vertex, vector<uint32_t> &index, vector<Float3> &normal, const char* filename);

void load_obj(vector<Float3> &vertex, vector<int32_t> &index, vector<Float3> &normals, vector<Float2> &TexCoords, const char* filename);