#include "ComplianceChecker.h"

#include <clang-c/Index.h>

#include <cstring>
#include <iostream>
#include <regex>
#include <unordered_map>

// Mapa estático global para armazenar os objetos que não passaram na verificação
static std::unordered_map<std::string, InvalidObjectInfo> invalidObjects;

// Definições do YAML e padrões de nomenclatura (não alterados)
static YAML::Node namingPatterns;
static std::string project_path;

static const std::unordered_map<std::string, std::string> defaultNamingPatterns = {
    {"snake_case", "^[a-z0-9]+(_[a-z0-9]+)*$"},
    {"camel_case", "^[a-z]+([A-Z][a-z0-9]*)*$"},
    {"pascal_case", "^[A-Z][a-z0-9]*(?:[A-Z][a-z0-9]*)*$"},
    {"upper_case", "^[A-Z0-9]+(_[A-Z0-9]+)*$"},
    {"kebab_case", "^[a-z0-9]+(-[a-z0-9]+)*$"},
    {"valid_identifiers", "^[a-zA-Z_][a-zA9A0-9_]*$"}};

// Função para verificar se a chave existe nos padrões de nomenclatura
bool keyExists(const std::string& key)
{
    auto it = defaultNamingPatterns.find(key);
    return it != defaultNamingPatterns.end();
}

// Função para verificar se o nome corresponde ao padrão
bool matchesPattern(const std::string& name, const std::string& pattern)
{
    std::regex regex(pattern);
    return std::regex_match(name, regex);
}

// Função que será chamada para visitar os nós do código fonte
CXChildVisitResult namingVisitor(CXCursor cursor, CXCursor parent,
                                 CXClientData client_data)
{
    CXString name       = clang_getCursorSpelling(cursor);
    const char* nameStr = clang_getCString(name);
    std::string pattern;

    // Obtém a localização do cursor para pegar o arquivo
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned int line, column;
    clang_getSpellingLocation(location, &file, &line, &column, nullptr);
    CXString fileName        = clang_getFileName(file);
    const char* fileNameCStr = clang_getCString(fileName);

    if (fileNameCStr == nullptr) {
        clang_disposeString(name);
        return CXChildVisit_Recurse;
    }
    std::string fileNameStr(fileNameCStr);  // Converte para std::string

    // Verifica se o arquivo está no projeto
    size_t found = fileNameStr.find(project_path);
    if (found != std::string::npos) {
        std::string objName(nameStr);

        // Verifica o tipo do objeto (variável, função, macro)
        if (clang_getCursorKind(cursor) == CXCursor_VarDecl) {
            if (keyExists(namingPatterns["specifies_naming_patterns"]["variable_naming"]
                              .as<std::string>())) {
                pattern = defaultNamingPatterns.at(
                    namingPatterns["specifies_naming_patterns"]["variable_naming"]
                        .as<std::string>());
            } else {
                pattern = namingPatterns["specifies_naming_patterns"]["variable_naming"]
                              .as<std::string>();
            }

        } else if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl) {
            if (keyExists(namingPatterns["specifies_naming_patterns"]["function_naming"]
                              .as<std::string>())) {
                pattern = defaultNamingPatterns.at(
                    namingPatterns["specifies_naming_patterns"]["function_naming"]
                        .as<std::string>());
            } else {
                pattern = namingPatterns["specifies_naming_patterns"]["function_naming"]
                              .as<std::string>();
            }
        } else if (clang_getCursorKind(cursor) == CXCursor_MacroDefinition) {
            if (keyExists(namingPatterns["specifies_naming_patterns"]["macro_naming"]
                              .as<std::string>())) {
                pattern = defaultNamingPatterns.at(
                    namingPatterns["specifies_naming_patterns"]["macro_naming"]
                        .as<std::string>());
            } else {
                pattern = namingPatterns["specifies_naming_patterns"]["macro_naming"]
                              .as<std::string>();
            }
        } else {
            clang_disposeString(name);
            return CXChildVisit_Recurse;
        }

        // Se o nome não corresponder ao padrão, adiciona ao mapa
        if (!matchesPattern(objName, pattern)) {
            // Verifica se a chave já existe no mapa
            if (invalidObjects.find(objName) == invalidObjects.end()) {
                invalidObjects[objName] = {fileNameStr, line, column};
            }
        }
    }

    clang_disposeString(name);
    return CXChildVisit_Recurse;
}

// Função para imprimir o conteúdo do unordered_map
void printInvalidObjects()
{
    std::cout << "\nObjetos invalidos encontrados:\n";
    for (const auto& entry : invalidObjects) {
        std::cout << "Objeto: " << entry.first << " | Arquivo: " << entry.second.fileName
                  << " | Linha: " << entry.second.line
                  << " | Coluna: " << entry.second.column << std::endl;
    }
}

void checkNamingPatterns(const char* filename, const char* command_line_args[],
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

    clang_visitChildren(rootCursor, namingVisitor, nullptr);

    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
}

ComplianceChecker::ComplianceChecker(YAML::Node config)
{
    namingPatterns = config;

    // Imprime o conteúdo do YAML de forma legível
    std::cout << "Configuração YAML carregada:\n";
}

void ComplianceChecker::checkFile(std::string fileName)
{
    checkNamingPatterns(fileName.c_str(), nullptr, 0);
}
std::unordered_map<std::string, InvalidObjectInfo> ComplianceChecker::getInvalidObjects()
{
    printInvalidObjects();
    return invalidObjects;
}
void ComplianceChecker::setCompliancePath(std::string projectPath)
{
    project_path = projectPath;
}
