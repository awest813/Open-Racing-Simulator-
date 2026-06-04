/***************************************************************************
 * GltfLoader.h  --  glTF 2.0 model loader (tinygltf wrapper)
 *
 * Loads .gltf / .glb files and uploads their geometry + PBR textures into
 * GPU VBOs.  The resulting GltfModel holds a list of GltfPrimitive objects
 * each ready to draw with the standard oglgraph geometry shader.
 *
 * Usage:
 *   GltfLoader loader;
 *   GltfModel  model;
 *   if (loader.load("path/to/car.glb")) {   // parse only — no GL calls
 *       loader.buildGPU(model);              // upload to GPU (GL context req)
 *   }
 *   // Per frame:
 *   shader.use();
 *   model.draw();
 *   // On shutdown:
 *   model.destroy();
 ***************************************************************************/
#pragma once

#include "gl3.h"
#include <string>
#include <vector>

struct GltfPrimitive {
    GLuint  vao        = 0;
    GLuint  vbo        = 0;    // interleaved position/normal/uv
    GLuint  ibo        = 0;    // index buffer
    GLuint  albedoTex  = 0;    // base-colour texture
    GLuint  normalTex  = 0;    // normal map (may be 0)
    int     indexCount = 0;
    GLenum  indexType  = GL_UNSIGNED_INT;
};

struct GltfModel {
    std::vector<GltfPrimitive> primitives;
    /** Upload geometry to GPU and fill primitives. */
    bool build(class GltfLoader& loader);
    /** Draw all primitives using the currently bound shader. */
    void draw() const;
    /** Free all GPU resources. */
    void destroy();
};

class GltfLoader {
public:
    GltfLoader() = default;
    ~GltfLoader() = default;

    /**
     * Parse a .gltf or .glb file.  Returns true on success.
     * Call model.build(*this) afterwards to push data to the GPU.
     */
    bool load(const std::string& path);

    /**
     * Upload the parsed model to GPU, filling out the GltfModel structure.
     * Must be called from the render thread (GL context active).
     */
    bool buildGPU(GltfModel& out);

private:
    // Parsed tinygltf data is stored in the opaque pointer to avoid exposing
    // tinygltf.h here (it triggers heavy STL + stb headers in every TU that
    // includes GltfLoader.h).
    struct Impl;
    Impl* m_impl = nullptr;

    GLuint uploadTexture(int texIndex);
    GLuint makeWhiteTexture();
};
