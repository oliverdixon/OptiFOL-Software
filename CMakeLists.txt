cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

include(FetchContent)
include(GoogleTest)

project(OptiFOL)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(GENERATED_SRC_DIR ${GENERATED_INCLUDE_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

bison_target(FOLParser Source/Interpreter/FOLParser.y ${GENERATED_SRC_DIR}/BaseParser.cpp
        COMPILE_FLAGS "-t -Wcounterexamples")
flex_target(FOLLexer Source/Interpreter/FOLLexer.l ${GENERATED_SRC_DIR}/FOLLexer.cpp)
add_flex_bison_dependency(FOLLexer FOLParser)

set(COMMON_SOURCES
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
        Source/Interpreter/FOLLexer.hpp
        Source/IR/ITermNode.hpp
        Source/IR/ISentenceNode.hpp
        Source/IR/VariableNode.hpp
        Source/IR/ConstantNode.hpp
        Source/IR/FunctionNode.hpp
        Source/IR/PredicationNode.hpp
        Source/IR/ConnectedSentenceNode.hpp
        Source/IR/IdentitySentenceNode.hpp
        Source/IR/NegatedSentenceNode.hpp
        Source/IR/QuantifiedSentenceNode.hpp
        Source/Interpreter/FOLParser.hpp
        Source/Visitors/CNFNormalisers/ImplicationEliminationVisitor.hpp
        Source/Visitors/MutatingVisitorBase.hpp
        Source/IR/NodeProxy.hpp
        Source/Visitors/CNFNormalisers/DMLVisitor.hpp
        Source/IR/ConnectedSentenceNode.cpp
        Source/IR/NegatedSentenceNode.cpp
        Source/IR/QuantifiedSentenceNode.cpp
        Source/IR/NodeProxy.cpp
        Source/IR/IdentitySentenceNode.cpp
        Source/IR/PredicationNode.cpp
        Source/Visitors/CNFNormalisers/UniversalEliminationVisitor.hpp
        Source/Visitors/CNFNormalisers/DisjunctionDistributionVisitor.hpp
        Source/Visitors/CNFNormalisers/DisjunctionDistributionVisitor.cpp
        Source/Visitors/CNFNormalisers/UniversalEliminationVisitor.cpp
        Source/Visitors/CNFNormalisers/ImplicationEliminationVisitor.cpp
        Source/Visitors/CNFNormalisers/DMLVisitor.cpp
        Source/Visitors/Serialisers/TextSerialiserVisitor.cpp
        Source/Visitors/Serialisers/TextSerialiserVisitor.hpp
        Source/Visitors/Serialisers/JSONSerialiserVisitor.cpp
        Source/Visitors/Serialisers/JSONSerialiserVisitor.hpp
        Source/Visitors/IObservingVisitor.hpp
)

add_executable(OptiFOL
        ${COMMON_SOURCES}
        Source/Driver.cpp
)

target_include_directories(OptiFOL PRIVATE Source/Interpreter/ ${GENERATED_INCLUDE_DIR})

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

target_include_directories(OptiFOLTesting PRIVATE Source/Interpreter/ ${GENERATED_INCLUDE_DIR})
target_link_libraries(OptiFOLTesting GTest::gtest_main)
gtest_discover_tests(OptiFOLTesting)
