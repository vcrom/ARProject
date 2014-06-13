#pragma once
#include <GL/glew.h>

class RenderableObject
{
public:
    RenderableObject(void);
    virtual ~RenderableObject(void);
    void Render();

    virtual int GetTotalVertices()=0;
    virtual int GetTotalIndices()=0;
    virtual GLenum GetPrimitiveType() =0;

    virtual void FillVertexBuffer(GLfloat* pBuffer)=0;
    virtual void FillIndexBuffer(GLuint* pBuffer)=0;
    virtual void FillNormalBuffer(GLfloat* pBuffer)=0;

    void Init();
    void Destroy();

protected:
    GLuint vaoID;
    GLuint vboVerticesID;
    GLuint vboIndicesID;
    GLuint vboNormalsID;

    GLenum primType;
    int totalVertices, totalIndices;
};
