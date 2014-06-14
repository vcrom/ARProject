#ifndef MESH_H
#define MESH_H


#include <glm/glm.hpp>
#include <iostream>
#include <vector>

//Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "renderableobject.h"
#include "bbox.h"

class Mesh : public RenderableObject
{
public:
    Mesh();
    ~Mesh();
    bool load(const std::string &filename);
    void free();
    CBBox getBBox();

protected:
    int GetTotalVertices();
    int GetTotalIndices();
    GLenum GetPrimitiveType();
    void FillVertexBuffer( GLfloat* pBuffer);
    void FillIndexBuffer( GLuint* pBuffer);
    void FillNormalBuffer( GLfloat* pBuffer);
    void FillColorBuffer(GLfloat* pBuffer);

private:
    void calcBBox();
    bool calc_box;
    CBBox mesh_bbox;
    void InitAdditionalAtributes(aiMesh *mesh);
    void InitVertexColorAtribute();

    GLuint vboColorID;
    int nVertices, nFaces;
    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec4> colours;
    std::vector<uint> vTable;

};

#endif // MESH_H
