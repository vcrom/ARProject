#include "mesh.h"

Mesh::Mesh()
{
    calc_box = false;
}

Mesh::~Mesh()
{
    free();
}

//#define AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE 80.0f
bool Mesh::load(const std::string& filename)
{
    std::cout <<  "Importing: " << filename << std::endl;
    Assimp::Importer importer;
    //aiProcessPreset_TargetRealtime_Fast
    const aiScene *scene = importer.ReadFile(filename,
            aiProcess_Triangulate|
            aiProcess_FixInfacingNormals|
            aiProcess_FindInvalidData|
            aiProcess_ValidateDataStructure|
            aiProcess_ImproveCacheLocality|
            aiProcess_GenSmoothNormals|
            0);
    //check if importer failed
    if(!scene)
    {
        std::cout <<  importer.GetErrorString() << std::endl;
        return false;
    }

    aiMesh *mesh = scene->mMeshes[0];
    nVertices = mesh->mNumVertices;
    nFaces = mesh->mNumFaces;
    std::cout << "Loading mesh: " << filename << std::endl;
    std::cout << "#Vertices: " << nVertices <<  std::endl;
    std::cout << "#faces: " << nFaces << std::endl;

    //get vertices
    vertices = std::vector<glm::vec3> (nVertices);
    for(int i = 0; i < nVertices; ++i)
        vertices[i] = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

    //get normals
    normals = std::vector<glm::vec3> (nVertices);
    for(int i = 0; i < nVertices; ++i)
    {
        aiVector3D norm = mesh->mNormals[i];
        norm = norm.Normalize();
        normals[i] = glm::vec3(norm.x, norm.y, norm.z);
    }

    //get indices
    vTable = std::vector<uint> (nFaces * 3);
    for(int i = 0; i < nFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        assert(face.mNumIndices == 3);
        for(int j = 0; j < 3; ++j)
            vTable[i*3 + j] = face.mIndices[j];
    }

    //getVertexColors if have
    if(mesh->HasVertexColors(0)){
        colours = std::vector<glm::vec4> (nVertices);
        for( int i = 0 ; i<nVertices; i++)
        {
            aiColor4D color = mesh->mColors[0][i];
            colours[i] = glm::vec4(color.r, color.g, color.b, color.a);
        }
    }

    std::cout << "Mesh loaded" << std::endl;
    PreTransforms();
    Init();
    //InitAdditionalAtributes(mesh);

    //free();
    return true;
}

void Mesh::InitAdditionalAtributes(aiMesh *mesh)
{
    if(mesh->HasVertexColors(0)) InitVertexColorAtribute();
}


void Mesh::InitVertexColorAtribute()
{
    glGenBuffers(1, &vboColorID);
    GLfloat* pBuffer = (GLfloat*) malloc (sizeof(GLfloat)*totalVertices*3);
    //colors
    FillColorBuffer(pBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vboColorID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*totalVertices, pBuffer, GL_STATIC_DRAW);
}

void Mesh::PreTransforms()
{
    ScaleMeshToUnit();
}

void Mesh::ScaleMeshToUnit()
{
    calcBBox();
    glm::vec3 dims = mesh_bbox.limits[1] - mesh_bbox.limits[0];
    float max_dim = max(dims.x, dims.y);
    max_dim = max(max_dim, dims.z);
    for(int i = 0; i < nVertices; ++i)
        vertices[i] /= max_dim;
}

CBBox Mesh::getBBox()
{
    if(!calc_box) calcBBox();
    calc_box = true;
    return mesh_bbox;
}

void Mesh::calcBBox()
{
    mesh_bbox.limits[0] = vertices[0];
    mesh_bbox.limits[1] = vertices[0];
    for(int i=1; i < nVertices; ++i)
    {
        mesh_bbox.limits[0] = min(mesh_bbox.limits[0], vertices[i]);
        mesh_bbox.limits[1] = max(mesh_bbox.limits[1], vertices[i]);
    }
}

int Mesh::GetTotalVertices()
{
    return nVertices;
}

int Mesh::GetTotalIndices()
{
    return nFaces*3;
}

GLenum Mesh::GetPrimitiveType()
{
    return GL_TRIANGLES;
}

void Mesh::FillVertexBuffer(GLfloat* pBuffer) {
    glm::vec3* vert = (glm::vec3*)(pBuffer);
    for( int i = 0 ; i<nVertices; i++) {
        vert[i] = vertices[i];
    }

}

void Mesh::FillIndexBuffer(GLuint* pBuffer) {
    int* idx = (int*)(pBuffer);
    for(int i = 0; i < GetTotalIndices(); ++i)
        idx[i] = vTable[i];

}

void Mesh::FillNormalBuffer(GLfloat* pBuffer) {
    glm::vec3* norm = (glm::vec3*)(pBuffer);
    for( int i = 0 ; i<nVertices; i++) {
        norm[i] = normals[i];
    }
}

void Mesh::FillColorBuffer(GLfloat* pBuffer) {
    glm::vec4* col = (glm::vec4*)(pBuffer);
    for( int i = 0 ; i<nVertices; i++) {
        col[i] = colours[i];
    }
}

void Mesh::free()
{
    vertices.clear();
    normals.clear();
    vTable.clear();
}
