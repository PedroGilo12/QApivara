/**
 * @file  DSMatriz.cpp
 *
 * @brief Módulo responsável por gerenciar a matriz de dependência estrutural.
 *
 * @author Pedro Henrique Vieira Giló (phvg@ic.ufal.br)
 *
 */

#include "DSMatrizModule.h"

#include <clang-c/Index.h>
#include <clang/Lex/Preprocessor.h>

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

static std::unordered_map<std::string, FunctionInfo> functionMap;
static std::unordered_map<std::string, FunctionInfo> globalFunctionMap;
static std::unordered_map<std::string, dependencys_t> dependencyMap;
static int copy_of_size = 0;
static char copy_of_project_path[1000];

static void printFunctionMap(std::unordered_map<std::string, FunctionInfo> functionMap)
{
    std::cout << "Mapeamento de Funcoes:" << std::endl;
    for (const auto& entry : functionMap) {
        const auto& funcInfo = entry.second;
        std::cout << "Funcao: " << funcInfo.name << ", Arquivo: " << funcInfo.file
                  << ", Linha: " << funcInfo.line << std::endl;
    }
}

static void removeExtension(const char* filename)
{
    char* dot = strrchr(filename, '.');
    if (dot != NULL) {
        *dot = '\0';
    }
}


namespace fs = std::filesystem;

DSMatrizModule::DSMatrizModule(const char* project_path)
{
    dsMatriz = {nullptr};
    size     = 0;

    project_name_str = new char[strlen(project_path) + 1];
    strcpy(const_cast<char*>(project_name_str), project_path);
    strcpy(const_cast<char*>(copy_of_project_path), project_path);

    std::cout << "Diretorio: " << project_path << ":\n";
}

DSMatrizModule::~DSMatrizModule()
{
    delete[] project_name_str;
}

void DSMatrizModule::createMatrizNode(const char* file_name)
{
    auto* newDsMatriz = new matriz_node_t[size + 1];

    for (uint8_t i = 0; i < size; i++) {
        newDsMatriz[i] = dsMatriz[i];
    }

    matriz_node_t& newNode = newDsMatriz[size];
    strncpy(newNode.file_name, file_name, DSMATRIZ_MODULE_MAX_FILE_NAME - 1);
    newNode.file_name[DSMATRIZ_MODULE_MAX_FILE_NAME - 1] = '\0';

    delete[] dsMatriz;
    dsMatriz = newDsMatriz;

    size++;
    copy_of_size++;
}
char** DSMatrizModule::listFoldersAndFiles(const std::string& path, int& count)
{
    std::vector<std::string> names;

    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            names.push_back(entry.path().filename().string());
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return nullptr;
    }

    count         = names.size();
    char** result = new char*[count];

    for (int i = 0; i < count; ++i) {
        result[i] = new char[names[i].size() + 1];
        std::strcpy(result[i], names[i].c_str());
    }

    return result;
}

void DSMatrizModule::setMatriz(const std::string& directory,
                               const char* command_line_args[], int command_line_size)
{
    int count    = 0;
    char** files = listFoldersAndFiles(directory.c_str(), count);

    if (files == nullptr) {
        std::cerr << "Erro ao listar arquivos." << std::endl;
        return;
    }

    for (int i = 0; i < count; ++i) {
        std::string fullPath = directory + "/" + files[i];

        if (fs::is_regular_file(fullPath) && strstr(fullPath.c_str(), ".c")) {
            // Processa o arquivo .c como antes
            removeExtension(files[i]);
            createMatrizNode(files[i]);

            strcpy(copy_of_project_path, fullPath.c_str());

            listFunctionCallsInFile(fullPath.c_str(), command_line_args,
                                    command_line_size);

            for (auto& it : functionMap) {
                dsMatriz[module].function_map.insert({it.first.c_str(), 0});
            }

            std::cout << dsMatriz[module].file_name << " Depende: " << std::endl;

            dsMatriz[module].dependencies = dependencyMap;

            functionMap.clear();
            dependencyMap.clear();
            module++;
        } else if (fs::is_directory(fullPath)) {
            setMatriz(fullPath.c_str(), command_line_args, command_line_size);
        }

        delete[] files[i];
    }

    delete[] files;
}


void DSMatrizModule::listFunctionCallsInFile(const char* filename,
                                             const char* command_line_args[],
                                             int num_args)
{
    CXIndex index = clang_createIndex(0, 0);

    CXTranslationUnit translationUnit = clang_parseTranslationUnit(
        index, filename, command_line_args, num_args, nullptr, 0,
        CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_KeepGoing);

    if (translationUnit == nullptr) {
        std::cerr << "Falha ao carregar o arquivo: " << filename << std::endl;
        clang_disposeIndex(index);
        return;
    }

    CXCursor rootCursor = clang_getTranslationUnitCursor(translationUnit);

    clang_visitChildren(rootCursor, visitor, nullptr);

    clang_visitChildren(rootCursor, callVisitor, nullptr);

    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
}

CXChildVisitResult DSMatrizModule::visitor(CXCursor cursor, CXCursor parent,
                                           CXClientData client_data)
{
    CXFile file;
    unsigned int line, column, offset;

    CXSourceLocation location = clang_getCursorLocation(cursor);
    clang_getSpellingLocation(location, &file, &line, &column, &offset);

    CXString fileName       = clang_getFileName(file);
    const char* fileNameStr = clang_getCString(fileName);

    if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl && fileNameStr != nullptr
        && std::strlen(fileNameStr) > 0) {
        CXString functionName = clang_getCursorSpelling(cursor);
        globalFunctionMap[clang_getCString(functionName)] = {
            clang_getCString(functionName), fileNameStr, line};
        if (strstr(fileNameStr, copy_of_project_path) != nullptr) {
            functionMap[clang_getCString(functionName)] = {clang_getCString(functionName),
                                                           fileNameStr, line};
            clang_disposeString(functionName);
        }
    }

    clang_disposeString(fileName);
    return CXChildVisit_Recurse;
}


CXChildVisitResult DSMatrizModule::callVisitor(CXCursor cursor, CXCursor parent,
                                               CXClientData client_data)
{
    CXFile file;
    unsigned int line, column, offset;

    CXSourceLocation location = clang_getCursorLocation(cursor);
    clang_getSpellingLocation(location, &file, &line, &column, &offset);

    CXString fileName       = clang_getFileName(file);
    const char* fileNameStr = clang_getCString(fileName);

    if (clang_getCursorKind(cursor) == CXCursor_CallExpr && fileNameStr != nullptr
        && std::strlen(fileNameStr) > 0) {
        CXString functionName   = clang_getCursorSpelling(cursor);
        const char* funcNameStr = clang_getCString(functionName);

        // Verifica se a função chamada está no mapa
        auto it = globalFunctionMap.find(funcNameStr);
        if (it != globalFunctionMap.end()) {
            if (dependencyMap.find(it->second.file) == dependencyMap.end()) {
                dependencyMap[it->second.file.c_str()] = {
                    .index        = copy_of_size - 1,
                    .function_map = {},
                };
            }

            dependencyMap[it->second.file.c_str()].function_map.insert(
                {funcNameStr, line});
        }

        clang_disposeString(functionName);
    }

    clang_disposeString(fileName);
    return CXChildVisit_Recurse;
}