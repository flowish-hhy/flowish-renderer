#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "FlowishModel.h"

#include <stdexcept>
#include <glm/glm.hpp>

FlowishModel::FlowishModel(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error("failed to load obj: " + warn + err);
    }

    const bool hasNormals = !attrib.normals.empty();

    _vertices.reserve(attrib.vertices.size() / 3);
    for (size_t i = 0; i < attrib.vertices.size() / 3; ++i) {
        _vertices.push_back({
            .pos   = { attrib.vertices[3*i+0], attrib.vertices[3*i+1], attrib.vertices[3*i+2] },
            .color = { 0.0f, 0.0f, 0.0f },
        });
    }

    std::vector<glm::vec3> accum(_vertices.size(), glm::vec3(0.0f));

    for (const auto& shape : shapes) {
        const auto& mesh = shape.mesh;
        for (size_t f = 0; f < mesh.indices.size(); f += 3) {
            uint32_t i0 = mesh.indices[f+0].vertex_index;
            uint32_t i1 = mesh.indices[f+1].vertex_index;
            uint32_t i2 = mesh.indices[f+2].vertex_index;
            _indices.push_back(i0);
            _indices.push_back(i1);
            _indices.push_back(i2);

            if (!hasNormals) {
                glm::vec3 e1 = _vertices[i1].pos - _vertices[i0].pos;
                glm::vec3 e2 = _vertices[i2].pos - _vertices[i0].pos;
                glm::vec3 n = glm::cross(e1, e2);
                accum[i0] += n;
                accum[i1] += n;
                accum[i2] += n;
            } else {
                int n0 = mesh.indices[f+0].normal_index;
                int n1 = mesh.indices[f+1].normal_index;
                int n2 = mesh.indices[f+2].normal_index;
                if (n0 >= 0) accum[i0] += glm::vec3(attrib.normals[3*n0+0], attrib.normals[3*n0+1], attrib.normals[3*n0+2]);
                if (n1 >= 0) accum[i1] += glm::vec3(attrib.normals[3*n1+0], attrib.normals[3*n1+1], attrib.normals[3*n1+2]);
                if (n2 >= 0) accum[i2] += glm::vec3(attrib.normals[3*n2+0], attrib.normals[3*n2+1], attrib.normals[3*n2+2]);
            }
        }
    }

    for (size_t i = 0; i < _vertices.size(); ++i) {
        glm::vec3 n = glm::length(accum[i]) > 0.0f ? glm::normalize(accum[i]) : glm::vec3(0.0f, 0.0f, 1.0f);
        _vertices[i].color = n * 0.5f + 0.5f;
    }
}
