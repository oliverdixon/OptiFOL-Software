/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 2/4/26.
//

#ifndef OPTIFOL_FEATURE_HPP
#define OPTIFOL_FEATURE_HPP

#include <assert.h>
#include <utility>

#include "../IHashable.hpp"

namespace optifol
{

class Feature : public IHashable
{
public:
    enum class FeatureType
    {
        MaxDepth,
        LiteralCount,
        FunctionCount,
        VariableCount
    };

    static constexpr std::array<FeatureType, 4> feature_types{FeatureType::MaxDepth,
            FeatureType::LiteralCount, FeatureType::FunctionCount, FeatureType::VariableCount};

    explicit Feature(const FeatureType feature_type) :
        feature_type(feature_type)
    {
    }

    Feature(const FeatureType feature_type, const unsigned int magnitude) :
        feature_type(feature_type),
        magnitude(magnitude)
    {
    }

    [[nodiscard]] std::size_t hash() const noexcept override
    {
        return hash_combine(
                std::hash<std::underlying_type_t<FeatureType>>{}(std::to_underlying(feature_type)),
                std::hash<unsigned int>{}(magnitude));
    }

    [[nodiscard]] bool operator<(const Feature &other_feature) const noexcept
    {
        if (feature_type != other_feature.feature_type)
            return feature_type < other_feature.feature_type;

        return magnitude < other_feature.magnitude;
    }

    [[nodiscard]] bool operator>(const Feature &other_feature) const noexcept
    {
        if (feature_type != other_feature.feature_type)
            return feature_type > other_feature.feature_type;

        return magnitude > other_feature.magnitude;
    }

    void reset() noexcept
    {
        magnitude = 0;
    }

    void increment(const unsigned int amount = 1) noexcept
    {
        magnitude += amount;
    }

    void maximise(const unsigned int candidate) noexcept
    {
        magnitude = std::max(magnitude, candidate);
    }

    void join(const Feature &new_feature) noexcept
    {
        assert(feature_type == new_feature.feature_type);

        switch (feature_type) {
        case FeatureType::MaxDepth:
            maximise(new_feature.magnitude);
            break;
        case FeatureType::FunctionCount:
        case FeatureType::LiteralCount:
        case FeatureType::VariableCount:
            increment(new_feature.magnitude);
            break;
        }
    }

    [[nodiscard]] unsigned int get_magnitude() const noexcept
    {
        return magnitude;
    }

    [[nodiscard]] FeatureType get_feature_type() const noexcept
    {
        return feature_type;
    }

    static Feature &get(std::vector<Feature> &features, const FeatureType feature_type)
    {
        const auto index = static_cast<std::size_t>(std::to_underlying(feature_type));
        assert(index < features.size());

        auto &candidate = features[index];
        assert(candidate.feature_type == feature_type);

        return candidate;
    }

private:
    const FeatureType feature_type;
    unsigned int magnitude = 0;
};

} // namespace optifol

#endif // OPTIFOL_FEATURE_HPP
