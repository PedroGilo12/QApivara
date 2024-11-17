/**
 * @file  DSMatriz.cpp
 *
 * @brief Módulo responsável por gerenciar a matriz de dependência estrutural.
 *
 * @author Pedro Henrique Vieira Giló (phvg@ic.ufal.br)
 *
 */

#ifndef DSMATRIZMODULE_H
#define DSMATRIZMODULE_H

#include <clang-c/Index.h>
#include <clang/Lex/Preprocessor.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define DSMATRIZ_MODULE_MAX_FILE_DEPENDENCY_SIZE 100
#define DSMATRIZ_MODULE_MAX_FUNCTION_SIZE 100
#define DSMATRIZ_MODULE_MAX_FILE_NAME 100

typedef struct matriz_node matriz_node_t;

typedef struct {
    int index;
    std::unordered_map<std::string, int> function_map;
} dependencys_t;

struct matriz_node {
    char file_name[DSMATRIZ_MODULE_MAX_FILE_NAME];
    uint8_t function_list_size;
    std::unordered_map<std::string, int> function_map;
    std::unordered_map<std::string, dependencys_t> dependencies;
};

struct FunctionInfo {
    std::string name;
    std::string file;
    unsigned int line;
};

class DSMatrizModule
{
   private:
    const char* project_name_str;
    int module = 0;

   public:
    matriz_node_t* dsMatriz{};
    uint8_t size;

    explicit DSMatrizModule(const char* project_path);
    ~DSMatrizModule();

    void createMatrizNode(const char* file_name);
    static char** listFoldersAndFiles(const std::string& path, int& count);
    void setMatriz(const std::string& directory, const char* command_line_args[],
                   int command_line_size);

    void listFunctionCallsInFile(const char* filename, const char* command_line_args[],
                                 int num_args);

   private:
    static CXChildVisitResult visitor(CXCursor cursor, CXCursor parent,
                                      CXClientData client_data);
    static CXChildVisitResult callVisitor(CXCursor cursor, CXCursor parent,
                                          CXClientData client_data);
};

#endif  // DSMATRIZMODULE_H
