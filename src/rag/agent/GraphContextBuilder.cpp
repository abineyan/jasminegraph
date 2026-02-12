#include "GraphContextBuilder.h"

#include <algorithm>
#include <unordered_map>

#include "../../util/logger/Logger.h"


using json = nlohmann::json;

Logger graph_context_builder_logger;

GraphContext GraphContextBuilder::extractGraphContext(const std::vector<json>& paths) {
    GraphContext ctx;

    std::unordered_map<std::string, int> labelFreq;
    std::unordered_map<std::string, float> biasAccum;

    int maxHop = 0;

    for (const auto& p : paths) {
        float score = p.value("score", 0.0f);
        int hop = p.value("hop", 0);

        maxHop = std::max(maxHop, hop);

        if (!p.contains("pathObj"))
            continue;
        const auto& pathObj = p["pathObj"];

        // ---------- Nodes ----------
        if (pathObj.contains("pathNodes")) {
            for (const auto& node : pathObj["pathNodes"]) {
                std::string label = node.value("label", "");
                std::string name = node.value("name", "");

                if (label.empty())
                    continue;

                labelFreq[label]++;
                ctx.nodeTypes.push_back(label);

                if (!name.empty()) {
                    biasAccum[label + ":" + name] += score;
                }
            }
        }

        // ---------- Relationships ----------
        if (pathObj.contains("pathRels")) {
            for (const auto& rel : pathObj["pathRels"]) {
                std::string type = rel.value("type", "");
                if (!type.empty()) {
                    ctx.edgeTypes.push_back(type);
                }
            }
        }
    }

    // ---------- Entry label selection ----------
    // Labels that repeatedly appear are good anchors
    for (const auto& [label, freq] : labelFreq) {
        if (freq >= 2) { 
            ctx.entryLabels.push_back(label);
        }
    }

    // ---------- Normalize bias ----------
    // (No hard normalization — planner can rank relatively)
    for (const auto& [key, val] : biasAccum) {
        ctx.semanticBias[key] = val;
    }

    ctx.maxHop = maxHop;

    // ---------- Deduplication ----------
    auto dedup = [](auto& v) {
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
    };

    dedup(ctx.entryLabels);
    dedup(ctx.nodeTypes);
    dedup(ctx.edgeTypes);

    return ctx;
}