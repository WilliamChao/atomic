#ifndef __atomic_Graphics_CreateModelData_h__
#define __atomic_Graphics_CreateModelData_h__
namespace atomic {

    class ParticleSet;

    void CreateSphere(VertexArray& va, VertexBufferObject& vbo, IndexBufferObject& ibo, float32 radius, uint32 div_xz, uint32 div_y);
    void CreateScreenQuad(VertexArray& va, VertexBufferObject& vbo);
    void CreateBloomLuminanceQuads(VertexArray& va, VertexBufferObject& vbo);
    void CreateBloomBlurQuads(VertexArray& va, VertexBufferObject& vbo);
    void CreateBloomCompositeQuad(VertexArray& va, VertexBufferObject& vbo);
    void CreateCube(VertexArray& va, VertexBufferObject& vbo, float32 len);

    void CreateCubeParticleSet(ParticleSet& ps, float32 len);
    void CreateSphereParticleSet(ParticleSet& ps, float32 radius);

} // namespace atomic
#endif // __atomic_Graphics_ModelData_h__
