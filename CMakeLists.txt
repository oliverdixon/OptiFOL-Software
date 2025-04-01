# Copyright (c) All Rights Reserved
# 2025 Oliver Dixon <od641@york.ac.uk>

# File: CMakeLists.txt
# Brief: Build the OptiFOL system with CMake
# Author: Oliver Dixon
# Date: 2024-11-15
# Version: Development

# TODO: separate this out into multiple CMake files: Generated, Common, GUI, test, and Doxygen (where applicable).

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
include(FetchContent)
project(OptiFOL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)

# Strip release-like builds, except debuginfo release builds.
function(utils_strip TARGET)
    add_custom_command(
            TARGET "${TARGET}" POST_BUILD
            DEPENDS "${TARGET}"
            COMMAND $<$<OR:$<CONFIG:release>,$<CONFIG:minsizerel>>:${CMAKE_STRIP}>
            ARGS --strip-all $<TARGET_FILE:${TARGET}>
    )
endfunction()

######################################
## OptiFOL Generated Object Library ##
##                                  ##
## Requires:                        ##
##  - flex                          ##
##  - bison                         ##
######################################

# The "generated" object library controls the invocation of all non-GTK code-generators required for OptiFOL. Namely:
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

add_library(OptiFOLGenerated OBJECT
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
)

# Flex and Bison generate all sorts of suspect melting pots of C and C++. It is assumed safe and correct to suppress all
# compiler warnings produced by these programs, hence the grouping of all generated code into a distinct CMake target.
target_compile_options(OptiFOLGenerated PRIVATE -Wno-everything)
target_include_directories(OptiFOLGenerated PRIVATE Source/Interpreter ${GENERATED_INCLUDE_DIR})

set_source_files_properties(
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
        PROPERTIES GENERATED TRUE
)

###################################
## OptiFOL Common Object Library ##
##                               ##
## Requires:                     ##
##  - nlohmann::json >= 3.11.3   ##
##  - libpq                      ##
##  - libpqxx                    ##
##  - simdjson >= 3 (acquired)   ##
###################################

find_package(nlohmann_json 3.11.3 REQUIRED)
pkg_check_modules(pqxx REQUIRED libpqxx)
pkg_check_modules(pq REQUIRED libpq)
FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG tags/v3.12.2
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(simdjson)
get_target_property(SIMDJSON_INCLUDE_DIR simdjson INTERFACE_INCLUDE_DIRECTORIES)

add_library(OptiFOLCommon OBJECT
        Source/Interpreter/FOLLexer.hpp
        Source/IR/Terms/ITermNode.hpp
        Source/IR/Sentences/ISentenceNode.hpp
        Source/IR/Terms/VariableNode.hpp
        Source/IR/Terms/ConstantNode.hpp
        Source/IR/Terms/FunctionNode.hpp
        Source/IR/Sentences/PredicationNode.hpp
        Source/IR/Sentences/ConnectedSentenceNode.hpp
        Source/IR/Sentences/IdentitySentenceNode.hpp
        Source/IR/Sentences/NegatedSentenceNode.hpp
        Source/IR/Sentences/QuantifiedSentenceNode.hpp
        Source/Interpreter/FOLParser.hpp
        Source/Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.hpp
        Source/Visitors/Sentences/MutatingSentenceVisitorBase.hpp
        Source/IR/Sentences/NodeProxy.hpp
        Source/Visitors/Sentences/CNFNormalisers/DMLVisitor.hpp
        Source/IR/Sentences/ConnectedSentenceNode.cpp
        Source/IR/Sentences/NegatedSentenceNode.cpp
        Source/IR/Sentences/QuantifiedSentenceNode.cpp
        Source/IR/Sentences/NodeProxy.cpp
        Source/IR/Sentences/IdentitySentenceNode.cpp
        Source/IR/Sentences/PredicationNode.cpp
        Source/Visitors/Sentences/CNFNormalisers/UniversalEliminationVisitor.hpp
        Source/Visitors/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.hpp
        Source/Visitors/Sentences/CNFNormalisers/DisjunctionDistributionVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/UniversalEliminationVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/ImplicationEliminationVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/DMLVisitor.cpp
        Source/Visitors/Sentences/Serialisers/TextSerialiserVisitor.cpp
        Source/Visitors/Sentences/Serialisers/TextSerialiserVisitor.hpp
        Source/Visitors/Sentences/Serialisers/JSONSerialiserVisitor.cpp
        Source/Visitors/Sentences/Serialisers/JSONSerialiserVisitor.hpp
        Source/Visitors/Sentences/IObservingSentenceVisitor.hpp
        Source/Visitors/Sentences/CNFNormalisers/SymbolStandardisingVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/SymbolStandardisingVisitor.hpp
        Source/Exceptions/SemanticException.hpp
        Source/Visitors/Terms/MutatingTermVisitorBase.hpp
        Source/Visitors/Terms/TermResolutionVisitor.cpp
        Source/Visitors/Terms/TermResolutionVisitor.hpp
        Source/Visitors/Terms/MutatingTermVisitorBase.cpp
        Source/Visitors/Sentences/MutatingSentenceVisitorBase.cpp
        Source/Visitors/Sentences/CNFNormalisers/ExistentialShiftingVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/ExistentialShiftingVisitor.hpp
        Source/Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp
        Source/Exceptions/ExceptionBase.hpp
        Source/Exceptions/StorageExceptionBase.hpp
        Source/Exceptions/StorageConnectionException.hpp
        Source/Exceptions/BadStorageNotificationException.hpp
        Source/Exceptions/ExceptionPrefixes.cpp
        Source/LegacyWrappers.cpp
        Source/LegacyWrappers.hpp
        Source/OptiFOL.hpp
        Source/Logging.hpp
        Source/Logging.cpp
)

target_link_libraries(OptiFOLCommon PRIVATE nlohmann_json::nlohmann_json pqxx pq simdjson)
target_include_directories(OptiFOLCommon SYSTEM PRIVATE ${SIMDJSON_INCLUDE_DIR})

########################################
## OptiFOL GUI Application Executable ##
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
        ${RESOURCE_DIR}/UI/MainWindow/EditorPane.ui
        ${RESOURCE_DIR}/UI/MainWindow/ProjectPane.ui
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

add_executable(OptiFOLGUI
        ${RESOURCE_OUT}
        Source/Driver.cpp
        Source/GUI/GUIDriver.cpp
        Source/GUI/MainWindow.cpp
        Source/GUI/MainWindow.hpp
        Source/GUI/Application.cpp
        Source/GUI/Application.hpp
        Source/GUI/GTKHelpers.hpp
        Source/Storage/Project/Project.cpp
        Source/Storage/Project/Project.hpp
        Source/Storage/PGDatabaseController.cpp
        Source/Storage/PGDatabaseController.hpp
        Source/Storage/IStorageController.hpp
        Source/Storage/Subsystem/Subsystem.cpp
        Source/Storage/Subsystem/Subsystem.hpp
        Source/Storage/IStorageObject.hpp
        Source/Storage/PGUpdateNotification.hpp
        Source/Storage/Project/PGProjectModel.hpp
        Source/Storage/Project/PGProjectModel.cpp
        Source/Storage/PGChronoType.hpp
        Source/Storage/PGChronoType.cpp
        Source/Storage/StorageHashFunctor.hpp
        Source/Storage/Subsystem/PGSubsystemModel.cpp
        Source/Storage/Subsystem/PGSubsystemModel.hpp
        Source/Storage/PGStorableObjectModelBase.hpp
        Source/Storage/StorageEqualityFunctor.hpp
        Source/Storage/Requirement/PGRequirementModel.cpp
        Source/Storage/Requirement/PGRequirementModel.hpp
        Source/Storage/Requirement/Requirement.cpp
        Source/Storage/Requirement/Requirement.hpp
        Source/GUI/Panels/ProjectHierarchyPane.cpp
        Source/GUI/Panels/ProjectHierarchyPane.hpp
        Source/GUI/Panels/RequirementsIndexArea.cpp
        Source/GUI/Panels/RequirementsIndexArea.hpp
        Source/Storage/Project/IProjectModel.hpp
        Source/Storage/Subsystem/ISubsystemModel.hpp
        Source/Storage/Requirement/IRequirementModel.hpp
        Source/Storage/PGStorableObjectModelBase.cpp
        Source/Storage/Project/ProjectHierarchicalModel.cpp
        Source/Storage/Project/ProjectHierarchicalModel.hpp
        Source/Storage/ICacheableStorableObjectModel.hpp
        Source/Storage/Subsystem/SubsystemHierarchicalModel.cpp
        Source/Storage/Subsystem/SubsystemHierarchicalModel.hpp
        Source/Storage/Requirement/RequirementHierarchicalModel.cpp
        Source/Storage/Requirement/RequirementHierarchicalModel.hpp
)

set(COMMON_COMPILER_OPTIONS
        -Weverything
        -Wno-unknown-pragmas # Support some IntelliJ IDEA in-line directives
        -Wno-c++98-compat
        -Wno-padded
        -Wno-c++98-compat-pedantic
        -Wno-switch-default
        -Wno-shadow-field-in-constructor
        -Wno-weak-vtables
        -Wno-shadow-field
)

if (FALSE)
    # Disable the GUI? This is for testing the application logic that is unrelated to the GUI e.g. FOL resolution.
    target_compile_definitions(OptiFOLGUI PRIVATE OPTIFOL_DISABLE_GUI)
endif ()

target_compile_options(OptiFOLGUI PRIVATE ${COMMON_COMPILER_OPTIONS})

target_link_libraries(OptiFOLGUI PRIVATE
        OptiFOLCommon
        OptiFOLGenerated
        log4cxx
        ${GTKMM_LIBRARIES}
        ${LOG4CXX_LIBRARIES}
)

target_include_directories(OptiFOLGUI PRIVATE
        Source/Interpreter/
        ${GENERATED_INCLUDE_DIR}
)

target_include_directories(OptiFOLGUI SYSTEM PRIVATE
        ${GTKMM_INCLUDE_DIRS}
        ${SIMDJSON_INCLUDE_DIR}
)

utils_strip(OptiFOLGUI)

##########################################
## OptiFOL Google Test Suite Executable ##
##                                      ##
## Requires:                            ##
##  - Google Test (acquired)            ##
##########################################

include(GoogleTest)
cmake_policy(SET CMP0135 NEW)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(OptiFOLTesting
        Source/Tests/FOLParserTest.cpp
)

target_include_directories(OptiFOLTesting PRIVATE Source/Interpreter/)
target_include_directories(OptiFOLTesting SYSTEM PRIVATE ${GENERATED_INCLUDE_DIR})
target_link_libraries(OptiFOLTesting PRIVATE OptiFOLCommon OptiFOLGenerated log4cxx GTest::gtest_main)
target_compile_options(OptiFOLTesting PRIVATE
        ${COMMON_COMPILER_OPTIONS}
        -Wno-global-constructors # GTest is known to generate declarations which require global constructors
)

gtest_discover_tests(OptiFOLTesting)

# TODO: add a Doxygen target like https://github.com/halide/Halide/blob/main/doc/CMakeLists.txt
