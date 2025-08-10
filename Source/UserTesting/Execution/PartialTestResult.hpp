/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 8/10/25.
//

#ifndef OPTIFOL_PARTIALTESTRESULT_HPP
#define OPTIFOL_PARTIALTESTRESULT_HPP

#include "../../Storage/StorageObjectBase.hpp"
#include "../Modelling/ITestModelNode.hpp"

namespace optifol
{

class PartialTestResult : public StorageObjectBase,
                          public ITestModelNode
{
public:
    PartialTestResult(std::string file, std::size_t line, const std::string &message);

    PartialTestResult(std::string file, std::size_t line, const std::string &message, BaseObjectType *cobject,
            const Glib::RefPtr<Gtk::Builder> &builder);

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tests_tree() const noexcept override;

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_results_tree() const noexcept override;

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_file();

    [[nodiscard]] Glib::PropertyProxy<guint> property_line();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_file() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<guint> property_line() const;

private:
    Glib::Property<Glib::ustring> file;
    Glib::Property<guint> line;
};

} // namespace optifol

#endif // OPTIFOL_PARTIALTESTRESULT_HPP
