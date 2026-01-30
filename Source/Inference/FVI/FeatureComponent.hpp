/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#ifndef OPTIFOL_FEATURECOMPONENT_HPP
#define OPTIFOL_FEATURECOMPONENT_HPP

#include <algorithm>
#include <vector>

#include "FeatureComponentBuilder.hpp"
#include "OccurrenceCountFeature.hpp"

namespace optifol
{

class Clause;

struct FeatureComponent
{
    OccurrenceCountFeature feature_id;
    unsigned int magnitude;

    static std::vector<FeatureComponent> build(const Clause &clause)
    {
        FeatureComponentBuilder::OCMapType oc;

        for (const auto literal : clause) {
            const bool polarity = !literal->is_negative_polarity();
            const auto it = oc.find(polarity);

            if (it == oc.cend())
                oc.emplace(FeatureComponentBuilder::OCMapType::key_type(polarity), 1);
            else
                ++it->second;

            FeatureComponentBuilder builder(oc, polarity);
            literal->accept(builder);
        }

        std::vector<FeatureComponent> features;

        for (const auto& [feature, count] : oc)
            features.emplace_back(FeatureComponent{feature, count});

        std::ranges::sort(features, std::less{}, [](const FeatureComponent& component) { return component.feature_id; });
        return features;
    }
};

} // namespace optifol

#endif // OPTIFOL_FEATURECOMPONENT_HPP
