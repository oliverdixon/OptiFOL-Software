/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Optifol GUI entry point definition
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#include <log4cxx/basicconfigurator.h>
#include "../IR/Sentences/PredicationNode.hpp"
#include "../IR/Terms/FunctionNode.hpp"
#include "../IR/Terms/VariableNode.hpp"
#include "Application.hpp"

int main(const int argc, char **argv)
{
    log4cxx::BasicConfigurator::configure();

    std::vector<std::unique_ptr<optifol::ITermNode>> generic_vector;
    generic_vector.emplace_back(std::make_unique<optifol::VariableNode>("x"));
    generic_vector.emplace_back(std::make_unique<optifol::FunctionNode>("John",
        std::vector<std::unique_ptr<optifol::ITermNode>>()));

    std::vector<std::unique_ptr<optifol::ITermNode>> specialised_vector;
    specialised_vector.emplace_back(std::make_unique<optifol::FunctionNode>("Jane",
        std::vector<std::unique_ptr<optifol::ITermNode>>()));
    specialised_vector.emplace_back(std::make_unique<optifol::FunctionNode>("John",
        std::vector<std::unique_ptr<optifol::ITermNode>>()));

    optifol::PredicationNode john_knows_x("Knows", std::move(generic_vector));
    optifol::PredicationNode john_knows_jane("Knows", std::move(specialised_vector));

    const auto& subs = john_knows_x.unify(john_knows_jane);

    const auto app = optifol::Application::create();
    return app->run(argc, argv);
}
