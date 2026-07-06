#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "FlowishModel.h"

#include <stdexcept>
#include <unordered_map>
#include <glm/glm.hpp>

namespace {
    struct VertexKey {
        int v, n, t;
        bool operator==(const VertexKey& o) const { return v == o.v && n == o.n && t == o.t; }
    };
    struct VertexKeyHash {
        size_t operator()(const VertexKey& k) const {
            return (size_t)k.v * 73856093u ^ (size_t)k.n * 19349663u ^ (size_t)k.t * 83492791u;
        }
    };
}

FlowishModel::FlowishModel(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error("failed to load obj: " + warn + err);
    }

    std::unordered_map<VertexKey, uint32_t, VertexKeyHash> cache;

    for (const auto& shape : shapes) {
        for (const auto& idx : shape.mesh.indices) {
            VertexKey key{idx.vertex_index, idx.normal_index, idx.texcoord_index};
            auto it = cache.find(key);
            if (it != cache.end()) {
                _indices.push_back(it->second);
                continue;
            }

            Vertex v = {};
            v.pos = {
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2],
            };
            if (idx.normal_index >= 0) {
                v.normal = {
                    attrib.normals[3 * idx.normal_index + 0],
                    attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2],
                };
            }
            if (idx.texcoord_index >= 0) {
                v.texCoords = {
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * idx.texcoord_index + 1],
                };
            }

            uint32_t out = static_cast<uint32_t>(_vertices.size());
            _vertices.push_back(v);
            cache[key] = out;
            _indices.push_back(out);
        }
    }

    if (attrib.normals.empty()) {
        std::vector<glm::vec3> accum(_vertices.size(), glm::vec3(0.0f));
        for (size_t i = 0; i + 2 < _indices.size(); i += 3) {
            uint32_t i0 = _indices[i], i1 = _indices[i + 1], i2 = _indices[i + 2];
            glm::vec3 n = glm::cross(_vertices[i1].pos - _vertices[i0].pos,
                                     _vertices[i2].pos - _vertices[i0].pos);
            accum[i0] += n;
            accum[i1] += n;
            accum[i2] += n;
        }
        for (size_t i = 0; i < _vertices.size(); ++i) {
            _vertices[i].normal = glm::length(accum[i]) > 0.0f ? glm::normalize(accum[i]) : glm::vec3(0, 0, 1);
        }
    }
}
