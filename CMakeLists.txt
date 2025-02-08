# OptiFOL CMakeLists
# Copyright (C) 2025 Oliver Dixon

cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
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
######################################

# The "generated" object library controls the invocation of all code-generators required for OptiFOL. Namely: Flex
# (lexer, generates C++), Bison (parser, generates C++), and glib-compile-resources (GResources blob, generates C). All
# generated source translation units are placed in ${GENERATED_SRC_DIR}, and headers are placed in
# ${GENERATED_INCLUDE_DIR}. Note that glib-compile-resources requires GTKMM for the headers only.

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

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

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(RESOURCE_XML ${RESOURCE_DIR}/optifol.gresource.xml)
set(RESOURCE_OUT ${GENERATED_SRC_DIR}/resources.c)
set(RESOURCE_DEP ${GENERATED_SRC_DIR}/optifol.gresource.d)

# The below command generates the 'resources.c' pseudo-blob with 'glib-compile-resources', containing octal
# representations of GTK resources. This is later linked into the generated object target only. A Make-style depfile is
# also created and retained in the generated sources root, using paths relative to the current binary directory, or
# absolute paths, as required by the Ninja backend.
add_custom_command(
        OUTPUT ${RESOURCE_OUT}
        MAIN_DEPENDENCY ${RESOURCE_XML}
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

add_library(OptiFOLGenerated OBJECT
        ${RESOURCE_OUT}
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
)

# Flex, Bison, and glib-compile-resources generate all sorts of suspect melting pots of C and C++. It is assumed safe
# and correct to suppress all compiler warnings produced by these programs, hence the grouping of all generated code
# into a distinct CMake target.
target_compile_options(OptiFOLGenerated PRIVATE -Wno-everything)
target_include_directories(OptiFOLGenerated PRIVATE Source/Interpreter ${GENERATED_INCLUDE_DIR} ${GTKMM_INCLUDE_DIRS})

###################################
## OptiFOL Common Object Library ##
###################################

find_package(nlohmann_json 3.11.3 REQUIRED)

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
        Source/SemanticException.hpp
        Source/Visitors/Terms/MutatingTermVisitorBase.hpp
        Source/Visitors/Terms/TermResolutionVisitor.cpp
        Source/Visitors/Terms/TermResolutionVisitor.hpp
        Source/Visitors/Terms/MutatingTermVisitorBase.cpp
        Source/Visitors/Sentences/MutatingSentenceVisitorBase.cpp
        Source/Visitors/Sentences/CNFNormalisers/ExistentialShiftingVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/ExistentialShiftingVisitor.hpp
        Source/Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.cpp
        Source/Visitors/Sentences/CNFNormalisers/SkolemIntroducingVisitor.hpp
        Source/IR/Meta/Solution.cpp
        Source/IR/Meta/Solution.hpp
        Source/IR/Meta/Project.cpp
        Source/IR/Meta/Project.hpp
        Source/IR/Meta/Group.cpp
        Source/IR/Meta/Group.hpp
        Source/IR/Meta/Requirement.cpp
        Source/IR/Meta/Requirement.hpp
        Source/Visitors/Meta/IObservingMetaVisitor.hpp
)

target_link_libraries(OptiFOLCommon PRIVATE nlohmann_json::nlohmann_json)

########################################
## OptiFOL GUI Application Executable ##
########################################

add_executable(OptiFOLGUI
        Source/GUI/GUIDriver.cpp
        Source/GUI/MainWindow.cpp
        Source/GUI/MainWindow.hpp
        Source/GUI/Application.cpp
        Source/GUI/Application.hpp
        Source/GUI/GTKHelpers.hpp
)

target_compile_definitions(OptiFOLGUI PRIVATE OPTIFOL_GUI)
target_link_libraries(OptiFOLGUI PRIVATE OptiFOLCommon OptiFOLGenerated ${GTKMM_LIBRARIES})
target_include_directories(OptiFOLGUI PRIVATE Source/Interpreter/)
target_include_directories(OptiFOLGUI SYSTEM PRIVATE ${GTKMM_INCLUDE_DIRS})
target_compile_options(OptiFOLGUI PRIVATE
        -Weverything
        -Wno-unknown-pragmas # Support some IntelliJ IDEA in-line directives
        -Wno-c++98-compat
        -Wno-padded
        -Wno-c++98-compat-pedantic
        -Wno-weak-vtables
        -Wno-switch-default
        -Wno-shadow-field-in-constructor
)

utils_strip(OptiFOLGUI)

###########################################
## OptiFOL Google Test Suite Executable  ##
## TODO: test suite needs updating       ##
###########################################

include(FetchContent)
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
        Tests/FOLParserTest.cpp
)

target_include_directories(OptiFOLTesting PRIVATE Source/Interpreter/)
target_include_directories(OptiFOLTesting SYSTEM PRIVATE ${GENERATED_INCLUDE_DIR})
target_link_libraries(OptiFOLTesting PRIVATE OptiFOLCommon OptiFOLGenerated GTest::gtest_main)
target_compile_options(OptiFOLTesting PRIVATE -Weverything)
gtest_discover_tests(OptiFOLTesting)
