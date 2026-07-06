#pragma once
#include <string>
#include <vector>
#include "../Core/Vertex.h"

class FlowishModel {
public:
    explicit FlowishModel(const std::string& path);

    [[nodiscard]] const std::vector<Vertex>& vertices() const { return _vertices; }
    [[nodiscard]] const std::vector<uint32_t>& indices() const { return _indices; }

private:
    std::vector<Vertex> _vertices;
    std::vector<uint32_t> _indices;
};
