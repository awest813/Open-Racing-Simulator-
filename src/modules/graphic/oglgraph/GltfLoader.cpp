/***************************************************************************
 * GltfLoader.cpp  --  glTF 2.0 model loader implementation
 *
 * Dependencies (header-only, already placed in src/libs/thirdparty/):
 *   tinygltf/tiny_gltf.h   (includes stb_image internally)
 *   nlohmann/json.hpp       (used by tinygltf)
 ***************************************************************************/

// tinygltf requires these defines in exactly one .cpp
#define TINYGLTF_IMPLEMENTATION
// stb_image is already compiled in tgfclient/img.cpp — defining
// STB_IMAGE_IMPLEMENTATION again would cause duplicate symbol errors.
// TINYGLTF_NO_STB_IMAGE tells tinygltf to skip the stb_image body.
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
// Still need stb_image_write declarations (tinygltf references them).
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "GltfLoader.h"

// Silence "conversion from double to float" warnings from tinygltf
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4267 4996)
#endif

#include "tiny_gltf.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <tgf.h>   // GfOut / GfError
#include <cstring>
#include <cstdio>

// =========================================================================
// Pimpl implementation struct
// =========================================================================

struct GltfLoader::Impl {
    tinygltf::Model    model;
    tinygltf::TinyGLTF loader;
    bool               loaded = false;
};

// =========================================================================
// GltfLoader
// =========================================================================

bool GltfLoader::load(const std::string& path)
{
    delete m_impl;
    m_impl = new Impl();

    std::string err, warn;
    bool ok;

    if (path.size() >= 4 &&
        (path.substr(path.size() - 4) == ".glb" ||
         path.substr(path.size() - 4) == ".GLB")) {
        ok = m_impl->loader.LoadBinaryFromFile(&m_impl->model, &err, &warn, path);
    } else {
        ok = m_impl->loader.LoadASCIIFromFile(&m_impl->model, &err, &warn, path);
    }

    if (!warn.empty()) GfOut("GltfLoader: warning: %s\n", warn.c_str());
    if (!err.empty())  GfOut("GltfLoader: error: %s\n",   err.c_str());

    if (!ok) {
        GfOut("GltfLoader: failed to load '%s'\n", path.c_str());
        return false;
    }

    m_impl->loaded = true;
    GfOut("GltfLoader: loaded '%s' (%zu meshes, %zu textures)\n",
          path.c_str(),
          m_impl->model.meshes.size(),
          m_impl->model.textures.size());
    return true;
}

// =========================================================================
// GPU upload helpers
// =========================================================================

GLuint GltfLoader::makeWhiteTexture()
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    unsigned char white[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

GLuint GltfLoader::uploadTexture(int texIndex)
{
    if (!m_impl || texIndex < 0 || texIndex >= (int)m_impl->model.textures.size())
        return makeWhiteTexture();

    const tinygltf::Texture& gltfTex = m_impl->model.textures[texIndex];

    // Guard: source index must be valid
    if (gltfTex.source < 0 || gltfTex.source >= (int)m_impl->model.images.size())
        return makeWhiteTexture();

    const tinygltf::Image& img = m_impl->model.images[gltfTex.source];

    if (img.image.empty() || img.width <= 0 || img.height <= 0)
        return makeWhiteTexture();

    GLenum fmt = (img.component == 4) ? GL_RGBA :
                 (img.component == 3) ? GL_RGB  : GL_RED;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, (GLint)fmt,
                 img.width, img.height, 0,
                 fmt, GL_UNSIGNED_BYTE,
                 img.image.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

// =========================================================================
// Build GPU buffers from parsed glTF
// =========================================================================

bool GltfLoader::buildGPU(GltfModel& out)
{
    if (!m_impl || !m_impl->loaded) return false;

    tinygltf::Model& model = m_impl->model;

    for (const tinygltf::Mesh& mesh : model.meshes) {
        for (const tinygltf::Primitive& prim : mesh.primitives) {
            if (prim.mode != TINYGLTF_MODE_TRIANGLES) continue;

            GltfPrimitive gprim;

            // ---- Accessors ----
            auto hasAttr = [&](const char* name) {
                return prim.attributes.count(name) > 0;
            };
            if (!hasAttr("POSITION")) continue;

            const tinygltf::Accessor& posAcc =
                model.accessors[prim.attributes.at("POSITION")];
            const tinygltf::BufferView& posBV  = model.bufferViews[posAcc.bufferView];
            const unsigned char* posData =
                model.buffers[posBV.buffer].data.data() + posBV.byteOffset + posAcc.byteOffset;
            size_t vertCount = posAcc.count;

            // Build interleaved CPU buffer: pos(3) + normal(3) + uv(2) = 8 floats
            std::vector<float> verts(vertCount * 8, 0.f);

            // Positions
            {
                size_t stride = posBV.byteStride ? posBV.byteStride : sizeof(float)*3;
                for (size_t i = 0; i < vertCount; ++i) {
                    const float* p = reinterpret_cast<const float*>(posData + i * stride);
                    verts[i*8+0] = p[0];
                    verts[i*8+1] = p[1];
                    verts[i*8+2] = p[2];
                }
            }

            // Normals
            if (hasAttr("NORMAL")) {
                const tinygltf::Accessor& normAcc =
                    model.accessors[prim.attributes.at("NORMAL")];
                const tinygltf::BufferView& normBV = model.bufferViews[normAcc.bufferView];
                const unsigned char* normData =
                    model.buffers[normBV.buffer].data.data() +
                    normBV.byteOffset + normAcc.byteOffset;
                size_t stride = normBV.byteStride ? normBV.byteStride : sizeof(float)*3;
                for (size_t i = 0; i < vertCount; ++i) {
                    const float* n = reinterpret_cast<const float*>(normData + i*stride);
                    verts[i*8+3] = n[0];
                    verts[i*8+4] = n[1];
                    verts[i*8+5] = n[2];
                }
            }

            // Texture coords
            if (hasAttr("TEXCOORD_0")) {
                const tinygltf::Accessor& uvAcc =
                    model.accessors[prim.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView& uvBV = model.bufferViews[uvAcc.bufferView];
                const unsigned char* uvData =
                    model.buffers[uvBV.buffer].data.data() +
                    uvBV.byteOffset + uvAcc.byteOffset;
                size_t stride = uvBV.byteStride ? uvBV.byteStride : sizeof(float)*2;
                for (size_t i = 0; i < vertCount; ++i) {
                    const float* uv = reinterpret_cast<const float*>(uvData + i*stride);
                    verts[i*8+6] = uv[0];
                    verts[i*8+7] = uv[1];
                }
            }

            // ---- Upload VBO ----
            glGenBuffers(1, &gprim.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, gprim.vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         (GLsizeiptr)(verts.size() * sizeof(float)),
                         verts.data(), GL_STATIC_DRAW);

            // ---- Index buffer ----
            if (prim.indices >= 0) {
                const tinygltf::Accessor&   idxAcc = model.accessors[prim.indices];
                const tinygltf::BufferView& idxBV  = model.bufferViews[idxAcc.bufferView];
                const unsigned char* idxData =
                    model.buffers[idxBV.buffer].data.data() +
                    idxBV.byteOffset + idxAcc.byteOffset;
                size_t idxBytes = idxAcc.count *
                    tinygltf::GetComponentSizeInBytes(idxAcc.componentType);

                glGenBuffers(1, &gprim.ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gprim.ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                             (GLsizeiptr)idxBytes, idxData, GL_STATIC_DRAW);
                gprim.indexCount = (int)idxAcc.count;
                // Map tinygltf component type to GL type
                switch (idxAcc.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    gprim.indexType = GL_UNSIGNED_BYTE;  break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    gprim.indexType = GL_UNSIGNED_SHORT; break;
                default:
                    gprim.indexType = GL_UNSIGNED_INT;   break;
                }
            } else {
                // No index buffer — draw as arrays
                gprim.indexCount = -(int)vertCount;
            }

            // ---- VAO setup ----
            glGenVertexArrays(1, &gprim.vao);
            glBindVertexArray(gprim.vao);
            glBindBuffer(GL_ARRAY_BUFFER, gprim.vbo);
            if (gprim.ibo) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gprim.ibo);

            // attr 0: position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
                                  (void*)(0));
            glEnableVertexAttribArray(0);
            // attr 1: normal
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
                                  (void*)(3*sizeof(float)));
            glEnableVertexAttribArray(1);
            // attr 2: uv
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),
                                  (void*)(6*sizeof(float)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // ---- Material textures ----
            if (prim.material >= 0) {
                const tinygltf::Material& mat = model.materials[prim.material];
                int albIdx = mat.pbrMetallicRoughness.baseColorTexture.index;
                gprim.albedoTex = uploadTexture(albIdx);

                int normIdx = mat.normalTexture.index;
                gprim.normalTex = uploadTexture(normIdx);
            } else {
                gprim.albedoTex = makeWhiteTexture();
                gprim.normalTex = 0;
            }

            out.primitives.push_back(gprim);
        }
    }

    GfOut("GltfLoader::buildGPU: %zu primitives uploaded\n", out.primitives.size());
    return !out.primitives.empty();
}

// =========================================================================
// GltfModel
// =========================================================================

void GltfModel::draw() const
{
    for (const GltfPrimitive& p : primitives) {
        // Bind albedo texture to unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, p.albedoTex ? p.albedoTex : 0);
        // Bind normal map to unit 1
        if (p.normalTex) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, p.normalTex);
        }

        glBindVertexArray(p.vao);
        if (p.indexCount > 0) {
            glDrawElements(GL_TRIANGLES, p.indexCount, p.indexType, nullptr);
        } else if (p.indexCount < 0) {
            glDrawArrays(GL_TRIANGLES, 0, -p.indexCount);
        }
        glBindVertexArray(0);
    }
}

void GltfModel::destroy()
{
    for (GltfPrimitive& p : primitives) {
        if (p.vao)       glDeleteVertexArrays(1, &p.vao);
        if (p.vbo)       glDeleteBuffers(1, &p.vbo);
        if (p.ibo)       glDeleteBuffers(1, &p.ibo);
        if (p.albedoTex) glDeleteTextures(1, &p.albedoTex);
        if (p.normalTex) glDeleteTextures(1, &p.normalTex);
        p = {};
    }
    primitives.clear();
}

bool GltfModel::build(GltfLoader& loader)
{
    return loader.buildGPU(*this);
}
