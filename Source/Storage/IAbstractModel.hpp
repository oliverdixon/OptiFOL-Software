/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 4/5/25.
//

#ifndef IABSTRACTMODEL_HPP
#define IABSTRACTMODEL_HPP

namespace optifol
{

class IAbstractModel
{
public:
    IAbstractModel() = default;

    virtual ~IAbstractModel() = default;

    IAbstractModel(const IAbstractModel& model) = delete;

    IAbstractModel& operator=(const IAbstractModel& model) = delete;

    IAbstractModel(IAbstractModel&& model) = default;

    IAbstractModel& operator=(IAbstractModel&& model) = default;
};

}

#endif
