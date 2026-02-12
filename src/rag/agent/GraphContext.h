#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

struct GraphContext {
    std::vector<std::string> entryLabels;
    std::vector<std::string> nodeTypes;
    std::vector<std::string> edgeTypes;
    std::unordered_map<std::string, float> semanticBias;
    int maxHop = 4;

    nlohmann::json toJson() const {
        return {
            {"entry_labels", entryLabels},
            {"node_types", nodeTypes},
            {"edge_types", edgeTypes},
            {"semantic_bias", semanticBias},
            {"max_hop", maxHop}
        };
    }
};