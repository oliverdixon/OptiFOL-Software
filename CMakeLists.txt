cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(OptiFOL)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX)
find_package(BISON)

if (NOT FLEX_FOUND)
    message(FATAL_ERROR "Flex is required but was not found.")
endif ()

if (NOT BISON_FOUND)
    message(FATAL_ERROR "Bison is required but was not found.")
endif ()

set(GENERATED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(GENERATED_SRC_DIR ${GENERATED_INCLUDE_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

bison_target(FOLParser Interpreter/FOLParser.y ${GENERATED_SRC_DIR}/BaseParser.cpp
        COMPILE_FLAGS "-t -Wcounterexamples")
flex_target(FOLLexer Interpreter/FOLLexer.l ${GENERATED_SRC_DIR}/FOLLexer.cpp)
add_flex_bison_dependency(FOLLexer FOLParser)

add_executable(OptiFOL
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
        Interpreter/FOLLexer.hpp
        Driver.cpp
        IR/ITermNode.hpp
        IR/ISentenceNode.hpp
        IR/VariableNode.hpp
        IR/ConstantNode.hpp
        IR/FunctionNode.hpp
        IR/PredicationNode.hpp
        IR/ConnectedSentenceNode.hpp
        IR/NegatedSentenceNode.hpp
        IR/IdentitySentenceNode.hpp
        IR/QuantifiedSentenceNode.hpp
        Interpreter/FOLParser.hpp
        Visitors/ImplicationPurgingVisitor.hpp
        Visitors/VisitorBase.hpp
        IR/QuantifiedSentenceNodeProxy.hpp
        IR/ConnectedSentenceNodeProxy.hpp
        Visitors/DMLVisitor.hpp
        IR/ConnectedSentenceNodeProxy.cpp
        IR/ConnectedSentenceNode.cpp
        IR/QuantifiedSentenceNode.cpp
        IR/QuantifiedSentenceNodeProxy.cpp
        IR/IdentitySentenceNode.cpp
        IR/PredicationNode.cpp
        IR/NegatedSentenceNode.cpp
)

target_include_directories(OptiFOL PRIVATE Interpreter/ ${GENERATED_INCLUDE_DIR})
