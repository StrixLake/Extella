rewrite plan for the project

struct mesh is created from tinyobjloader

struct Mesh
{
    vector<int> index;
    vector<float> vertices;
    vector<float> normals;
    vector<float> texcoords;
    vector<float> instanceData;
};

the instance data is optional and contains per instance data for instanced draw
mesh is only a struct now and not a class and so it does not contain any member functions

pipeline setup is managed by a pipeline class object
the pipeline class instance is not created directly from its constructor, rather a description struct is used with a factory function
to setup the immutable pipeline instance

struct PipelineStateDescription
{
    D3D11_BLEND_STATE blend = DEFAULT; // here DEFAULT is just a macro to provide the default blend state
    D3D11_Raster_State = RSDEFAULT;
    wstring VertexShader, HullShader, DomainShader, GeometryShader, PixelShader;
    array<string, 8> renderTargets;
    array<DXGI_FORMAT, 8> renderTargetFormat;
    string depthStencil;
    D3D11_PRIMITIVE_TOPOLOGY topology;
};

class Pipeline{};

Pipeline* createPipeline(PipelineStateDescription desc);

here the factory function does a lot of things.
- It loads all the shaders whose filename is given in the description struct, that's why they were wstring.
- It creates the rtv's for the render targets using the format specified in the formats array. The resources it uses to create those
rtv's are copied from the resource manager that holds the texture of the name. Hence it was also a string.
- it creates the blend state and the raster state of the specified description.
- it gets the depth stencil from the resource manager and create a DSV.

it stores all those pointers in the pipeline object. The pipeline object has a render/draw method (haven't decided on the name) that sets up all the driver state using the
pointers it holds. The destructor of the pipeline object is responsible for releasing all those com objects.
In terms of textures, the pipeline class is only responsible for defining all the outputs of the render pass. The inputs of the render pass are defined by its material.

class Material{};

similar to pipeline class, material is also created using a factory and a description struct

struct Material_Description
{
    vector<string> textures;
    vector<string> vsTextures, hsTextures, dsTextures, gsTextures, psTextures; // optional array containing the textures for each render stage
    vector<flaot> materialConstants;
    D3D11_Sampler_Desc samplerDesc = DEFAULT_SAMPLER;
};

the factory takes this description, creates srv's for the textures that are identified by string handles and managed by resource manager.
Of course, the textures vector does not need to contain actual textures loaded from a file. It can just contain things like "renderTarget" which is the texture on which rendering happens.
It can be in the material texture as in one render pass, we might render to a gbuffer and in another pass, that gbuffer becomes the input.
It also creates/gets a constant buffer to hold all the material constants and sets that in the pixel shader slot 7. So slot 7 in pixel shader is always reserved for material constants.

all these classes are used in a render pass, So we have a class for that too
class RenderPass
{
    Mesh* mesh;
    Pipeline* pipeline;
    Material* material;
}

its destructor does not frees its memebers since multiple render passes can use the same pipeline but different material or mesh and so on.

The actual object construction is held in a vector and the render pass class data members are pointers into that vector elements. This allows for hot reloading since those objects just need to be constructed in place again on hot reload.

Now, for the input layout object.
We do not hard code it. Instead, we use shader reflection to build the input layout. Since all the inputs to the vertex shader come from the mesh object and we get the semantic names for the input params of the vertex shader, we create another mesh class that contains the vertex buffers along with the semantic names.

class GPUMesh
{
    vector<ID3D11Buffer*> vertexBuffers;
    vector<string> bufferSemantics;
};

So after getting the semantic names using reflection from the vertex shader, we simply check if those buffers with those semantics are in the mesh and we set those in the IA slots.

The render pass class is changed to hold GPUMesh instead of Mesh.

Similarly, we use shader reflection to get the constant buffers in all shaders and the variable names in the constant buffer names "Host Variables". All the host variables, like camera position or time, are in an unordered_map<wstring, float> like before. Previously, I was passing the names of the variables to the unified shader class to build a corresponding constant buffer. Now we use shader reflection to get the appropriate variable names that each shader uses from host.

All this setup can be done by a render pass constructor factory or pipeline factory.