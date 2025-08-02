/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Test-level storage object
 * @author Oliver Dixon
 * @date 2025-07-17
 * @version Development
 */

#ifndef TEST_HPP
#define TEST_HPP

#include "../../Storage/StorageObjectBase.hpp"
#include "../Discovery/TestSpecificationEntry.hpp"
#include "../Execution/TargetTestExecutableBase.hpp"
#include "../Execution/TestResult.hpp"
#include "ITestModelNode.hpp"

namespace optifol
{

/**
 * @class Test
 * @brief The Test storage object denotes a single unit test to be executed against a testable target executable.
 */
class Test : public StorageObjectBase,
             public ITestModelNode
{
public:
    explicit Test(std::shared_ptr<TestSpecificationEntry> template_specification);

    Test(std::shared_ptr<TestSpecificationEntry> template_specification, BaseObjectType *cobject,
            const Glib::RefPtr<Gtk::Builder> &builder);

    [[nodiscard]] Glib::RefPtr<Gtk::TreeListModel> get_tree() const noexcept override;

    /**
     * @brief Accept a shared TestResult object to indicate the last-known result of the Test.
     * @param test_result The TestResult to share.
     * @throws SemanticException if the TestResult was not appropriate or relevant to the Test.
     */
    void emplace_result(std::shared_ptr<TestResult> test_result);

    void share_test_executable(std::shared_ptr<TargetTestExecutableBase> shared_exe);

    [[nodiscard]] const TargetTestExecutableBase *observe_test_executable() const noexcept;

    [[nodiscard]] Glib::PropertyProxy<Glib::ustring> property_fixture();

    [[nodiscard]] Glib::PropertyProxy<std::shared_ptr<TestResult>> property_result();

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_target_executable_name() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<Glib::ustring> property_fixture() const;

    [[nodiscard]] Glib::PropertyProxy_ReadOnly<std::shared_ptr<TestResult>> property_result() const;

private:
    void instantiate_from_specification(std::shared_ptr<TestSpecificationEntry> spec);

    std::shared_ptr<TargetTestExecutableBase> target_executable;
    Glib::Property<Glib::ustring> target_executable_name;
    Glib::Property<Glib::ustring> fixture;
    Glib::Property<std::shared_ptr<TestResult>> result;
};

} // namespace optifol

#endif // TEST_HPP
