/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification and implementation for the Clause feature
 * @author Oliver Dixon
 * @date 2026-02-04
 * @version Development
 */

#ifndef OPTIFOL_FEATURE_HPP
#define OPTIFOL_FEATURE_HPP

#include <cassert>
#include <utility>

#include "../IHashable.hpp"

namespace optifol
{

/**
 * @class Feature
 * @brief A Feature is a metric of a Clause. It has a type and a magnitude, indicating the extent of the
 * feature type's representation within the Clause.
 */
class Feature : public IHashable
{
public:
    /**
     * @class FeatureType
     * @brief The four Clause feature types.
     */
    enum class FeatureType
    {
        MaxDepth,
        LiteralCount,
        FunctionCount,
        VariableCount
    };

    static constexpr std::array<FeatureType, 4> feature_types{FeatureType::MaxDepth,
            FeatureType::LiteralCount, FeatureType::FunctionCount, FeatureType::VariableCount};

    /**
     * @brief Construct a new feature type with zero magnitude.
     * @param feature_type The feature type to construct.
     */
    explicit Feature(const FeatureType feature_type) :
        feature_type(feature_type)
    {
    }

    /**
     * @brief Construct a new feature type with a specified magnitude.
     * @param feature_type The feature type to construct.
     * @param magnitude The initial magnitude of the feature.
     */
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

    /**
     * @brief Determine the feature's importance w.r.t. Resolution Potential.
     * @param other_feature The other feature to compare against.
     * @return Does this feature have a strictly lower RP than the other one?
     */
    [[nodiscard]] bool operator<(const Feature &other_feature) const noexcept
    {
        if (feature_type != other_feature.feature_type)
            return feature_type < other_feature.feature_type;

        return magnitude < other_feature.magnitude;
    }

    /**
     * @brief Determine the feature's importance w.r.t. Resolution Potential.
     * @param other_feature The other feature to compare against.
     * @return Does this feature have a strictly greater RP than the other one?
     */
    [[nodiscard]] bool operator>(const Feature &other_feature) const noexcept
    {
        if (feature_type != other_feature.feature_type)
            return feature_type > other_feature.feature_type;

        return magnitude > other_feature.magnitude;
    }

    /**
     * @brief Reset the feature's magnitude back to its initial value.
     */
    void reset() noexcept
    {
        magnitude = 0;
    }

    /**
     * @brief Increment the feature by a given amount
     * @param amount The extent by which the feature's magnitude should be incremented (default 1).
     */
    void increment(const unsigned int amount = 1) noexcept
    {
        magnitude += amount;
    }

    /**
     * @brief Maximise the feature with another magnitude.
     * @param candidate The candidate magnitude to maximise.
     */
    void maximise(const unsigned int candidate) noexcept
    {
        magnitude = std::max(magnitude, candidate);
    }

    /**
     * @brief Combine the given feature with this one.
     * @param new_feature The other feature with which to combine.
     * @pre The given feature must be of the same type.
     */
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

    /**
     * @brief Static helper to retrieve a feature of the specified type from a list of features.
     * @param features The list of features.
     * @param feature_type The feature type of retrieve.
     * @return The requested feature.
     */
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
