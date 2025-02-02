cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

include(FetchContent)
include(GoogleTest)

project(OptiFOL)
find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

set(GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(GENERATED_SRC_DIR ${GENERATED_INCLUDE_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

bison_target(FOLParser Source/Interpreter/FOLParser.y ${GENERATED_SRC_DIR}/BaseParser.cpp
        COMPILE_FLAGS "-t -Wcounterexamples")
flex_target(FOLLexer Source/Interpreter/FOLLexer.l ${GENERATED_SRC_DIR}/FOLLexer.cpp)
add_flex_bison_dependency(FOLLexer FOLParser)

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
set(RESOURCE_XML ${RESOURCE_DIR}/optifol.gresource.xml)
set(RESOURCE_OUT ${GENERATED_SRC_DIR}/resources.c)

add_custom_command(
        OUTPUT ${RESOURCE_OUT}
        WORKING_DIRECTORY ${RESOURCE_DIR}
        COMMAND ${GLIB_COMPILE_RESOURCES}
        ARGS
            --generate-source
            --target=${RESOURCE_OUT}
            ${RESOURCE_XML}
        VERBATIM
        MAIN_DEPENDENCY ${RESOURCE_XML}
)

add_library(OptiFOLGenerated
        OBJECT
        ${RESOURCE_OUT}
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS})

target_include_directories(OptiFOLGenerated PRIVATE Source/Interpreter ${GENERATED_INCLUDE_DIR} ${GTKMM_INCLUDE_DIRS})
target_compile_options(OptiFOLGenerated PRIVATE -Wno-everything)

set(COMMON_SOURCES
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
)

add_executable(OptiFOL
        ${COMMON_SOURCES}
        Source/Driver.cpp
        Source/GUI/GUIDriver.cpp
        Source/GUI/MainWindow.cpp
        Source/GUI/MainWindow.hpp
        Source/GUI/Application.cpp
        Source/GUI/Application.hpp
)

target_compile_definitions(OptiFOL PRIVATE OPTIFOL_GUI)
target_link_libraries(OptiFOL PRIVATE OptiFOLGenerated ${GTKMM_LIBRARIES})
target_include_directories(OptiFOL PRIVATE Source/Interpreter/)
target_include_directories(OptiFOL SYSTEM PRIVATE ${GENERATED_INCLUDE_DIR} ${GTKMM_INCLUDE_DIRS})
target_compile_options(OptiFOL PRIVATE
        -Weverything
        -Wno-unknown-pragmas
        -Wno-c++98-compat
        -Wno-padded
        -Wno-c++98-compat-pedantic
        -Wno-weak-vtables
        -Wno-switch-default
        -Wno-shadow-field-in-constructor
)

cmake_policy(SET CMP0135 NEW)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(OptiFOLTesting
        ${COMMON_SOURCES}
        Tests/FOLParserTest.cpp
)

target_include_directories(OptiFOLTesting PRIVATE Source/Interpreter/)
target_include_directories(OptiFOLTesting SYSTEM PRIVATE ${GENERATED_INCLUDE_DIR})
target_link_libraries(OptiFOLTesting PRIVATE GTest::gtest_main OptiFOLGenerated)
target_compile_options(OptiFOLTesting PRIVATE -Weverything)
gtest_discover_tests(OptiFOLTesting)
