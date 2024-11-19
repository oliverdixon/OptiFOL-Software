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

set(GENERATED_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_SRC_DIR})

bison_target(FOLParser Interpreter/FOLParser.y ${GENERATED_SRC_DIR}/FOLParser.cpp
        DEFINES_FILE ${GENERATED_SRC_DIR}/FOLParser.hpp)
flex_target(FOLLexer Interpreter/FOLLexer.l ${GENERATED_SRC_DIR}/FOLLexer.cpp)
add_flex_bison_dependency(FOLLexer FOLParser)

add_executable(OptiFOL
        ${BISON_FOLParser_OUTPUTS}
        ${FLEX_FOLLexer_OUTPUTS}
        Interpreter/FOLLexer.hpp
        Interpreter/FOLInterpreter.cpp
        Interpreter/AST/ITermASTNode.hpp
        Interpreter/AST/IFormulaASTNode.hpp
        Interpreter/AST/VariableASTNode.hpp
        Interpreter/AST/ConstantASTNode.hpp
        Interpreter/AST/FunctionASTNode.hpp
)

target_include_directories(OptiFOL PRIVATE Interpreter/ ${GENERATED_SRC_DIR})
