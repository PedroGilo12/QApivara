#include <clang-c/Index.h>
#include <clang/Lex/Preprocessor.h>
#include <iostream>
#include <string>
#include <cstring>

// Função de callback para visitar os nós da AST
CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData client_data) {
    CXFile file;
    unsigned int line, column, offset;

    // Obtém o local da função
    CXSourceLocation location = clang_getCursorLocation(cursor);
    clang_getSpellingLocation(location, &file, &line, &column, &offset);

    // Obtém o nome do arquivo associado ao cursor
    CXString fileName = clang_getFileName(file);
    const char* fileNameStr = clang_getCString(fileName);

    if (clang_getCursorKind(cursor) == CXCursor_FunctionDecl && fileNameStr != nullptr && std::strlen(fileNameStr) > 0) {
        const char* originalFilename = static_cast<const char*>(client_data);

        // Compara o arquivo atual com o arquivo de origem
        if (std::strcmp(fileNameStr, originalFilename) == 0) {
            // Obtém o nome da função
            CXString functionName = clang_getCursorSpelling(cursor);
            std::cout << "Função encontrada: " << clang_getCString(functionName) << " na linha " << line << std::endl;
            clang_disposeString(functionName);
        }
    }

    clang_disposeString(fileName);
    return CXChildVisit_Recurse;
}

void listFunctionsInFile(const char* filename) {
    // Cria um índice para a análise
    CXIndex index = clang_createIndex(0, 0);

    // Cria uma unidade de tradução para o arquivo
    CXTranslationUnit translationUnit = clang_parseTranslationUnit(
        index, filename, nullptr, 0, nullptr, 0, CXTranslationUnit_None);

    if (translationUnit == nullptr) {
        std::cerr << "Falha ao carregar o arquivo: " << filename << std::endl;
        clang_disposeIndex(index);
        return;
    }

    // Obtém o cursor raiz da unidade de tradução
    CXCursor rootCursor = clang_getTranslationUnitCursor(translationUnit);

    // Percorre a árvore da AST
    clang_visitChildren(rootCursor, visitor, const_cast<char*>(filename));

    // Limpa os recursos
    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.c>" << std::endl;
        return 1;
    }

    // Chama a função para listar as funções no arquivo fornecido
    listFunctionsInFile(argv[1]);

    return 0;
}
