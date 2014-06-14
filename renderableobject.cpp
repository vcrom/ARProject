#include "renderableobject.h"
#include <glm/glm.hpp>


RenderableObject::RenderableObject(void)
{

}


RenderableObject::~RenderableObject(void)
{
    Destroy();
}

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);
void RenderableObject::Init() {
    //setup vao and vbo stuff
    glGenBuffers(1, &vboVerticesID);
    glGenBuffers(1, &vboNormalsID);
    glGenBuffers(1, &vboIndicesID);

    //get total vertices and indices
    totalVertices = GetTotalVertices();
    totalIndices  = GetTotalIndices();
    primType      = GetPrimitiveType();

    GLuint* pIBuffer = (GLuint*) malloc (sizeof(GLuint)*totalIndices);
    //index
    FillIndexBuffer(pIBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*totalIndices, pIBuffer, GL_STATIC_DRAW);

    GLfloat* pBuffer = (GLfloat*) malloc (sizeof(glm::vec3)*totalVertices);
    //vertex
    FillVertexBuffer(pBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*totalVertices, pBuffer, GL_STATIC_DRAW);

    //normals
    FillNormalBuffer(pBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboNormalsID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::vec3)*totalVertices, pBuffer, GL_STATIC_DRAW);

    GL_CHECK_ERRORS
}

void RenderableObject::Destroy() {
    //Destroy vao and vbo
    glDeleteBuffers(1, &vboVerticesID);
    glDeleteBuffers(1, &vboIndicesID);
    glDeleteBuffers(1, &vboNormalsID);
}


void RenderableObject::Render() {
    //Vertex
    glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*) 0);
    glEnableClientState(GL_VERTEX_ARRAY);
    //Normals
    glBindBuffer(GL_ARRAY_BUFFER, vboNormalsID);
    glNormalPointer(GL_FLOAT, 0, (GLvoid*) 0);
    glEnableClientState(GL_NORMAL_ARRAY);
    //Index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);

    //Render
    glDrawElements(GetPrimitiveType(), totalIndices, GL_UNSIGNED_INT, (GLvoid*)0);
}
