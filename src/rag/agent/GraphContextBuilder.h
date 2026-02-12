#pragma once

#include <vector>
#include <nlohmann/json.hpp>
#include "GraphContext.h"

class GraphContextBuilder {
public:
    static GraphContext extractGraphContext(
        const std::vector<nlohmann::json>& paths
    );
};
