# Copyright (c) All Rights Reserved
# 2025 Oliver Dixon <od641@york.ac.uk>

# File: CMakeLists.txt
# Brief: Build the Optifol system with CMake
# Author: Oliver Dixon
# Date: 2024-11-15
# Version: Development

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
include(FetchContent)
project(Optifol)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)

# Strip release-like builds, except debuginfo release builds.
function(utils_strip TARGET)
    add_custom_command(
            TARGET "${TARGET}" POST_BUILD
            COMMAND $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:${CMAKE_STRIP}>
            ARGS --strip-all $<TARGET_FILE:${TARGET}>
    )
endfunction()

######################################
## Optifol Generated Object Library ##
##                                  ##
## Requires:                        ##
##  - flex                          ##
##  - bison                         ##
######################################

# The "generated" object library controls the invocation of all non-GTK code-generators required for Optifol. Namely:
# Flex (lexer, generates C++) and Bison (parser, generates C++). All generated source translation units are placed in
# ${GENERATED_SRC_DIR}, and headers are placed in ${GENERATED_INCLUDE_DIR}.

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(GENERATED_SRC_DIR ${GENERATED_INCLUDE_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

bison_target(FOLParser
        Source/Interpreter/FOLParser.y
        ${GENERATED_SRC_DIR}/BaseParser.cpp
        COMPILE_FLAGS "-t -Wcounterexamples"
)

flex_target(FOLLexer
        Source/Interpreter/FOLLexer.l
        ${GENERATED_SRC_DIR}/FOLLexer.cpp
)

add_flex_bison_dependency(FOLLexer FOLParser)

add_library(OptifolGenerated OBJECT
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
)

# Flex and Bison generate all sorts of suspect melting pots of C and C++. It is assumed safe and correct to suppress all
# compiler warnings produced by these programs, hence the grouping of all generated code into a distinct CMake target.
target_compile_options(OptifolGenerated PRIVATE -Wno-everything)
target_include_directories(OptifolGenerated PRIVATE Source/Interpreter ${GENERATED_INCLUDE_DIR})

set_source_files_properties(
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
        PROPERTIES GENERATED TRUE
)

###################################
## Optifol Common Object Library ##
##                               ##
## Requires:                     ##
##  - nlohmann::json >= 3.11.3   ##
##  - simdjson >= 3 (acquired)   ##
###################################

find_package(nlohmann_json 3.11.3 REQUIRED)
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG tags/v3.12.2
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(simdjson)
get_target_property(SIMDJSON_INCLUDE_DIR simdjson INTERFACE_INCLUDE_DIRECTORIES)

add_library(OptifolCommon OBJECT
        Source/Interpreter/FOLLexer.hpp
        Source/IR/MutableVariants/Terms/IMutableTerm.hpp
        Source/IR/MutableVariants/Sentences/IMutableSentence.hpp
        Source/IR/MutableVariants/Terms/MutableVariable.hpp
        Source/IR/MutableVariants/Terms/MutableConstant.hpp
        Source/IR/MutableVariants/Terms/MutableFunction.hpp
        Source/IR/MutableVariants/Sentences/MutablePredicate.hpp
        Source/IR/MutableVariants/Sentences/MutableBinaryConnected.hpp
        Source/IR/MutableVariants/Sentences/MutableIdentity.hpp
        Source/IR/MutableVariants/Sentences/MutableQuantified.hpp
        Source/Interpreter/FOLParser.hpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp
        Source/Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/DMLVisitor.hpp
        Source/IR/MutableVariants/Sentences/MutableBinaryConnected.cpp
        Source/IR/MutableVariants/Sentences/MutableQuantified.cpp
        Source/IR/MutableVariants/Sentences/MutableIdentity.cpp
        Source/IR/MutableVariants/Sentences/MutablePredicate.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/UniversalEliminationVisitor.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/ImplicationEliminationVisitor.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/DMLVisitor.cpp
        Source/Visitors/MutableTargets/Observers/TextSerialiserVisitor.cpp
        Source/Visitors/MutableTargets/Observers/TextSerialiserVisitor.hpp
        Source/Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/SymbolStandardisingVisitor.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp
        Source/Exceptions/SemanticException.hpp
        Source/Visitors/MutableTargets/Terms/MutatingTermVisitorBase.hpp
        Source/Visitors/MutableTargets/Terms/TermResolutionVisitor.cpp
        Source/Visitors/MutableTargets/Terms/TermResolutionVisitor.hpp
        Source/Visitors/MutableTargets/Terms/MutatingTermVisitorBase.cpp
        Source/Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/QuantifierExtractingVisitor.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/QuantifierExtractingVisitor.hpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/SkolemIntroducingVisitor.cpp
        Source/Visitors/MutableTargets/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp
        Source/LegacyWrappers.cpp
        Source/LegacyWrappers.hpp
        Source/Optifol.hpp
        Source/Logging.hpp
        Source/Logging.cpp
        Source/IR/MutableVariants/Sentences/MutableSentenceRoot.hpp
        Source/IR/MutableVariants/Sentences/MutableSentenceRoot.cpp
        Source/IR/MutableVariants/Terms/MutableConstant.cpp
        Source/IR/MutableVariants/Terms/MutableFunction.cpp
        Source/IR/MutableVariants/Terms/MutableVariable.cpp
        Source/IR/MutableVariants/Terms/MutableSkolemFunction.cpp
        Source/IR/MutableVariants/Terms/MutableSkolemFunction.hpp
        Source/Visitors/MutableTargets/Terms/ScopedTermResolutionVisitor.cpp
        Source/Visitors/MutableTargets/Terms/ScopedTermResolutionVisitor.hpp
        Source/Exceptions/ParseError.hpp
        Source/IHashable.hpp
        Source/IR/Substitution.cpp
        Source/IR/Substitution.hpp
        Source/Visitors/RegularTargets/Unification/UnificationVisitor.cpp
        Source/Visitors/RegularTargets/Unification/UnificationVisitor.hpp
        Source/IR/MutableVariants/OwningBuildable.hpp
        Source/IR/SymbolRepository.cpp
        Source/IR/SymbolRepository.hpp
        Source/IR/Sentences/ISentence.hpp
        Source/IR/Terms/IProcessedTerm.hpp
        Source/IR/Sentences/Predicate.cpp
        Source/IR/Sentences/Predicate.hpp
        Source/IR/Terms/Variable.cpp
        Source/IR/Terms/Variable.hpp
        Source/IR/Terms/Function.cpp
        Source/IR/Terms/Function.hpp
        Source/IR/Terms/IProcessedTerm.cpp
        Source/IR/Terms/IProcessedTerm.hpp
        Source/CompositeSerialisationHelpers.hpp
        Source/IR/Sentences/BinaryConnected.cpp
        Source/IR/Sentences/BinaryConnected.hpp
        Source/IR/Sentences/Quantified.cpp
        Source/IR/Sentences/Quantified.hpp
        Source/IR/Sentences/Identity.cpp
        Source/IR/Sentences/Identity.hpp
        Source/IR/Terms/Constant.cpp
        Source/IR/Terms/Constant.hpp
        Source/Visitors/MutableTargets/RepositoryBuildingVisitor.cpp
        Source/Visitors/MutableTargets/RepositoryBuildingVisitor.hpp
        Source/IR/Sentences/SentenceRoot.cpp
        Source/IR/Sentences/SentenceRoot.hpp
        Source/HashableEqualityFunctor.hpp
        Source/Visitors/MutableTargets/Observers/LaTeXSerialisationVisitor.cpp
        Source/Visitors/MutableTargets/Observers/LaTeXSerialisationVisitor.hpp
        Source/IR/Sentences/IProcessedSentence.hpp
)

target_link_libraries(OptifolCommon PRIVATE nlohmann_json::nlohmann_json simdjson)
target_include_directories(OptifolCommon SYSTEM PRIVATE ${SIMDJSON_INCLUDE_DIR})

########################################
## Optifol GUI Application Executable ##
##                                    ##
## Requires:                          ##
##  - glib-compile-resources          ##
##  - awk                             ##
##  - gtkmm >= 4.0                    ##
########################################

find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
find_package(log4cxx REQUIRED)

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(RESOURCE_XML_IN ${RESOURCE_DIR}/optifol.gresource.xml.in)
set(RESOURCE_XML ${GENERATED_SRC_DIR}/optifol.gresource.xml)
set(RESOURCE_OUT ${GENERATED_SRC_DIR}/resources.c)
set(RESOURCE_DEP ${GENERATED_SRC_DIR}/optifol.gresource.d)

set(AWK_SUBSTITUTION_SCRIPT ${RESOURCE_DIR}/substitute.awk)

set(MAINWINDOW_RESOURCE_INPUT ${RESOURCE_DIR}/UI/MainWindow/MainWindow.ui.in)
set(MAINWINDOW_RESOURCE_OUTPUT ${GENERATED_SRC_DIR}/MainWindow.ui)
set(MAINWINDOW_RESOURCE_COMPONENTS
        ${RESOURCE_DIR}/UI/MainWindow/ProjectPane/ProjectPane.ui
        ${RESOURCE_DIR}/UI/MainWindow/ProjectPane/NewProjectPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/ProjectPane/NewSubsystemPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/ProjectPane/EditStructurePopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/ProjectPane/DeleteStructurePopover.ui

        ${RESOURCE_DIR}/UI/MainWindow/RequirementsIndexArea/NewRequirementPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/RequirementsIndexArea/DeleteRequirementPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/RequirementsIndexArea/EditRequirementPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/RequirementsIndexArea/DuplicateRequirementPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/RequirementsIndexArea/RequirementsIndexView.ui

        ${RESOURCE_DIR}/UI/MainWindow/AnalysisArea/AnalysisView.ui
        ${RESOURCE_DIR}/UI/MainWindow/AnalysisArea/NewAnalysisGroupPopover.ui
        ${RESOURCE_DIR}/UI/MainWindow/AnalysisArea/DeleteAnalysisGroupPopover.ui

        ${RESOURCE_DIR}/UI/MainWindow/TestingArea/TestingView.ui

        ${RESOURCE_DIR}/UI/MainWindow/ReportsArea/ReportsArea.ui
        ${RESOURCE_DIR}/UI/MainWindow/ReportsArea/GenerateLaTeXPopover.ui

        ${RESOURCE_DIR}/UI/MainWindow/replacements.txt
)

# Generate the Main Window UI specification file using the given Awk script. The Awk script nominally takes a number of
# key-value pairs, defined in ./replacements.txt (relative to the location of the UI files), and replaces all instances
# of the key with the contents of the file whose path is detained by the value.
add_custom_command(
        OUTPUT ${MAINWINDOW_RESOURCE_OUTPUT}
        MAIN_DEPENDENCY ${MAINWINDOW_RESOURCE_INPUT}
        DEPENDS ${MAINWINDOW_RESOURCE_COMPONENTS} ${AWK_SUBSTITUTION_SCRIPT}
        WORKING_DIRECTORY ${RESOURCE_DIR}/UI/MainWindow
        COMMAND awk -f ${AWK_SUBSTITUTION_SCRIPT} > ${MAINWINDOW_RESOURCE_OUTPUT}
        ARGS ${MAINWINDOW_RESOURCE_INPUT}
)

# Performs any CMake variable substitutions in the GResources manifest. This is required due to some included resource
# files being generated dynamically, hence existing in an implementation-defined generated source directory.
configure_file(
        ${RESOURCE_XML_IN}
        ${RESOURCE_XML}
)

# The below command generates the 'resources.c' pseudo-blob with 'glib-compile-resources', containing octal
# representations of GTK resources. This is later linked into the GUI target only. A Make-style depfile is also created
# and retained in the generated sources root, using paths relative to the current binary directory, or absolute paths,
# as required by the Ninja backend.
add_custom_command(
        OUTPUT ${RESOURCE_OUT}
        MAIN_DEPENDENCY ${RESOURCE_XML}
        DEPENDS ${MAINWINDOW_RESOURCE_OUTPUT}
        DEPFILE ${RESOURCE_DEP}
        VERBATIM
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} # Encourage glib-compile-resources to emit relative paths
        BYPRODUCTS ${RESOURCE_DEP} # Don't let Ninja absorb the depfile into its internal database
        COMMAND ${GLIB_COMPILE_RESOURCES}
        ARGS
            --generate-source
            --generate-phony-targets # Individual resource files are not generated
            --dependency-file=${RESOURCE_DEP}
            --target=${RESOURCE_OUT}
            --sourcedir=${RESOURCE_DIR}
            ${RESOURCE_XML}
        COMMAND sed -i "s|${CMAKE_CURRENT_BINARY_DIR}/||g" ${RESOURCE_DEP} # Clean up any paths that should be relative
)

# Suppress compiler warnings known to be harmlessly invoked by glib-compile-resources translation units.
set_source_files_properties(
        ${RESOURCE_OUT}
        PROPERTIES COMPILE_OPTIONS
        "-Wno-overlength-strings;-Wno-reserved-macro-identifier;-Wno-unused-macros"
)

add_executable(OptifolGUI
        ${RESOURCE_OUT}
        Source/GUI/GUIDriver.cpp
        Source/GUI/MainWindow.cpp
        Source/GUI/MainWindow.hpp
        Source/GUI/Application.cpp
        Source/GUI/Application.hpp
        Source/GUI/GTKHelpers.hpp
        Source/Storage/TreeNode.cpp
        Source/Storage/TreeNode.hpp
        Source/Storage/Project.cpp
        Source/Storage/Project.hpp
        Source/Storage/Subsystem.cpp
        Source/Storage/Subsystem.hpp
        Source/Storage/StorageObjectBase.hpp
        Source/GUI/Panels/ProjectHierarchyPane.cpp
        Source/GUI/Panels/ProjectHierarchyPane.hpp
        Source/GUI/Panels/RequirementsIndexArea.cpp
        Source/GUI/Panels/RequirementsIndexArea.hpp
        Source/GUI/ContextButtonCorrespondence.cpp
        Source/GUI/ContextButtonCorrespondence.hpp
        Source/Storage/Requirement.cpp
        Source/Storage/Requirement.hpp
        Source/Storage/StorageObjectBase.cpp
        Source/GUI/Panels/AnalysisArea.cpp
        Source/GUI/Panels/AnalysisArea.hpp
        Source/Storage/AnalysisGroup.cpp
        Source/Storage/AnalysisGroup.hpp
        Source/GUI/ReportsArea/ReportsArea.cpp
        Source/GUI/ReportsArea/ReportsArea.hpp
        Source/GUI/ReportsArea/ReportsAreaGenerateLaTeXPopover.cpp
        Source/GUI/ReportsArea/ReportsAreaGenerateLaTeXPopover.hpp
)

set(COMMON_COMPILER_OPTIONS
        -Weverything
        -Wno-exit-time-destructors
        -Wno-unknown-pragmas # Support some IntelliJ IDEA in-line directives
        -Wno-c++98-compat
        -Wno-padded
        -Wno-c++98-compat-pedantic
        -Wno-switch-default
        -Wno-shadow-field-in-constructor
        -Wno-weak-vtables
        -Wno-shadow-field
        -Wno-global-constructors
)

target_compile_options(OptifolGUI PRIVATE ${COMMON_COMPILER_OPTIONS})

target_link_libraries(OptifolGUI PRIVATE
        OptifolCommon
        OptifolGenerated
        log4cxx
        ${GTKMM_LIBRARIES}
        ${LOG4CXX_LIBRARIES}
)

target_include_directories(OptifolGUI PRIVATE
        Source/Interpreter/
        ${GENERATED_INCLUDE_DIR}
)

target_include_directories(OptifolGUI SYSTEM PRIVATE
        ${GTKMM_INCLUDE_DIRS}
        ${SIMDJSON_INCLUDE_DIR}
)

utils_strip(OptifolGUI)

##########################################
## Optifol Google Test Suite Executable ##
##                                      ##
## Requires:                            ##
##  - Google Test (acquired)            ##
##########################################

include(GoogleTest)
cmake_policy(SET CMP0135 NEW)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # v.1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(OptifolTesting
        Source/Tests/FOLParserTest.cpp
        Source/Tests/CNFNormalisationTest.cpp
        Source/Tests/UnificationTest.cpp
        Source/Tests/GoogleTestSupport.hpp
)

target_include_directories(OptifolTesting PRIVATE Source/Interpreter/)
target_include_directories(OptifolTesting SYSTEM PRIVATE ${GENERATED_INCLUDE_DIR})
target_link_libraries(OptifolTesting PRIVATE OptifolCommon OptifolGenerated log4cxx GTest::gtest_main)
target_compile_options(OptifolTesting PRIVATE
        ${COMMON_COMPILER_OPTIONS}
        -Wno-global-constructors # GTest is known to generate declarations which require global constructors
)

gtest_discover_tests(OptifolTesting)

######################################
## Optifol Documentation Generation ##
##                                  ##
## Requires:                        ##
##  - Doxygen                       ##
##  - wget                          ##
######################################

# TODO 1: wrap external tagfile support into a function
# TODO 2: i.a.w. the above, use to get tags for cppreference.com and externally GTKmm Doxygen, plus anything else of use

set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/GeneratedDocumentation)
set(CPPREFERENCE_DOXYGEN_WEB_TAG ${DOXYGEN_OUTPUT_DIR}/cppreference-doxygen-web.tag.xml)
set(DOXYGEN_SENTINEL ${DOXYGEN_OUTPUT_DIR}/.doxygen_cmake_sentinel)

# Collate declared source files from all eligible targets containing Doxygen directives.
set(DOXYGEN_TARGETS OptifolCommon OptifolGenerated OptifolTesting OptifolGUI)
set(DOXYGEN_SOURCES)
foreach (target IN LISTS DOXYGEN_TARGETS)
    get_target_property(target_sources ${target} SOURCES)
    if (target_sources)
        list(APPEND DOXYGEN_SOURCES ${target_sources})
    endif ()
endforeach ()

# Download a known version of the cppreference.com Doxygen web tag XML file.
add_custom_command(
        OUTPUT ${CPPREFERENCE_DOXYGEN_WEB_TAG}
        WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIR}
        VERBATIM
        COMMAND wget https://www-users.york.ac.uk/~od641/cppreference-doxygen-web.tag.xml
)

add_custom_command(
        OUTPUT ${DOXYGEN_SENTINEL}
        MAIN_DEPENDENCY Doxyfile
        DEPENDS ${DOXYGEN_SOURCES} ${CPPREFERENCE_DOXYGEN_WEB_TAG}
        VERBATIM
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND doxygen
        COMMAND touch ${DOXYGEN_SENTINEL}
)

add_custom_target(OptifolHTMLDocumentation DEPENDS
        ${CPPREFERENCE_DOXYGEN_WEB_TAG}
        ${DOXYGEN_SENTINEL}
)
