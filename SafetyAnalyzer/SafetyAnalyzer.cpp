#include "SafetyAnalyzer.h"

#include <clang-c/Index.h>

#include <iostream>
#include <unordered_map>
#include <string>

static std::string projectPathToVerify = "";
static std::unordered_map<std::string, std::string> UnsafetyObjects;

// Função de callback chamada para cada declaração encontrada
CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData clientData)
{
    // Obtenha o tipo de cursor para identificar o tipo de declaração
    CXCursorKind cursorKind = clang_getCursorKind(cursor);

    // Obtém a localização do cursor para pegar o arquivo
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned int line, column;
    clang_getSpellingLocation(location, &file, &line, &column, nullptr);
    CXString fileName        = clang_getFileName(file);
    const char* fileNameCStr = clang_getCString(fileName);

    if (fileNameCStr == nullptr) {
        return CXChildVisit_Recurse;
    }
    std::string fileNameStr(fileNameCStr);
    size_t found = fileNameStr.find(projectPathToVerify);

    if (found != std::string::npos) {
        // Detecta variáveis declaradas sem inicialização
        if (cursorKind == CXCursor_VarDecl) {
            // Verifica se a variável está inicializada
            CXString varName = clang_getCursorSpelling(cursor);
            if (!clang_Cursor_getVarDeclInitializer(cursor).data) {
                std::string message = "Possível bug: variável '" + std::string(clang_getCString(varName)) + "' declarada sem inicialização.";
                UnsafetyObjects[fileNameStr + ":" + std::to_string(line) + ":" + std::to_string(column)] = message;
            }
            clang_disposeString(varName);
        }

        // Detecta operações com ponteiros que podem estar incorretas
        if (cursorKind == CXCursor_UnaryOperator) {
            CXString operatorStr = clang_getCursorSpelling(cursor);
            std::string opStr    = clang_getCString(operatorStr);
            if (opStr == "*") {
                CXType cursorType = clang_getCursorType(cursor);
                if (cursorType.kind == CXType_Pointer) {
                    std::string message = "Possível bug: operação com ponteiro não inicializado detectada.";
                    UnsafetyObjects[fileNameStr + ":" + std::to_string(line) + ":" + std::to_string(column)] = message;
                }
            }
            clang_disposeString(operatorStr);
        }

        // Regra MISRA-C: Evitar variáveis globais
        if (cursorKind == CXCursor_VarDecl
            && clang_getCursorLinkage(cursor) == CXLinkage_External) {
            CXString varName = clang_getCursorSpelling(cursor);
            std::string message = "Possível violação MISRA-C: Variável global '" + std::string(clang_getCString(varName)) + "' encontrada.";
            UnsafetyObjects[fileNameStr + ":" + std::to_string(line) + ":" + std::to_string(column)] = message;
            clang_disposeString(varName);
        }
    }
    return CXChildVisit_Recurse;  // Continua visitando os filhos do cursor atual
}

void printDiagnostics(CXTranslationUnit translationUnit) {
    unsigned numDiagnostics = clang_getNumDiagnostics(translationUnit);

    for (unsigned i = 0; i < numDiagnostics; ++i) {
        CXDiagnostic diagnostic = clang_getDiagnostic(translationUnit, i);
        CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);

        // Obtém a mensagem do diagnóstico
        CXString diagnosticMessage = clang_getDiagnosticSpelling(diagnostic);
        const char* message = clang_getCString(diagnosticMessage);

        // Verifica se a mensagem não é vazia
        if (message && *message) {
            // Obtém a localização do diagnóstico
            CXSourceLocation location = clang_getDiagnosticLocation(diagnostic);
            CXFile file;
            unsigned int line, column;
            clang_getSpellingLocation(location, &file, &line, &column, nullptr);

            // Obtém o nome do arquivo e cria a string de localização
            CXString fileName = clang_getFileName(file);
            std::string locationStr = "Arquivo: " + std::string(clang_getCString(fileName)) +
                                      ", Linha: " + std::to_string(line) +
                                      ", Coluna: " + std::to_string(column);

            // Determina o tipo de diagnóstico
            std::string diagnosticType = (severity == CXDiagnostic_Warning) ? "Warning" : "Error";

            // Armazenando diagnóstico no mapa UnsafetyObjects
            UnsafetyObjects[locationStr + " | " + diagnosticType] = message;

            // Libera a string do nome do arquivo
            clang_disposeString(fileName);
        }

        clang_disposeString(diagnosticMessage);
        clang_disposeDiagnostic(diagnostic);  // Libera o diagnóstico
    }
}

SafetyAnalyzer::SafetyAnalyzer(std::string projectPath)
{
    projectPathToVerify = projectPath;
}

void SafetyAnalyzer::checkFile(std::string fileName)
{
    CXIndex index                     = clang_createIndex(0, 0);

    // Opções de compilação: essas opções podem ser ajustadas conforme necessário
    const char* args[] = {
        "-Wall",
        "-std=c11", // Define o padrão C para compilação (pode ser ajustado para c99, c11, etc.)
        "-I."       // Incluir o diretório atual (pode ser ajustado para outros diretórios de inclusão)
    };


    CXTranslationUnit translationUnit = clang_parseTranslationUnit(
        index, fileName.c_str(), args, 3, nullptr, 0, CXTranslationUnit_None);

    if (translationUnit == nullptr) {
        std::cerr << "Erro ao processar o arquivo: " << fileName << std::endl;
        return;
    }

    CXCursor rootCursor = clang_getTranslationUnitCursor(translationUnit);
    clang_visitChildren(rootCursor, visitor, nullptr);

    printDiagnostics(translationUnit);

    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
}

std::unordered_map<std::string, std::string> SafetyAnalyzer::getUnsafetyObjects()
{
    return UnsafetyObjects;
}
