/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/10/25.
//

#ifndef ITESTLISTENER_HPP
#define ITESTLISTENER_HPP

namespace optifol
{

class ITestListener
{
public:
    virtual ~ITestListener() = default;

protected:
    virtual void connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result) = 0;
};

} // namespace optifol

#endif // ITESTLISTENER_HPP
