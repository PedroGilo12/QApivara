#include <fstream>
#include <iostream>

#include "ComplianceHandle.h"
#include "ComplianceChecker.h"
#include <filesystem>

/* Construtor que carrega o arquivo de conformidade YAML */
ComplianceHandle::ComplianceHandle(const std::string &file) : complianceFile(file)
{
    try {
        compliance = YAML::LoadFile(complianceFile);
        checker = new ComplianceChecker(compliance);
    } catch (const YAML::BadFile &e) {
        std::cerr << "Erro ao carregar o arquivo de conformidade: " << e.what()
                  << std::endl;
    }
}
YAML::Node ComplianceHandle::getComplianceGlobal()
{
    return compliance;
}

/* Método para obter um nó YAML específico */
YAML::Node ComplianceHandle::getComplianceNode(const std::string &key) const {
    return compliance[key];
}

/* Método para verificar se uma chave existe no YAML */
bool ComplianceHandle::hasKey(const std::string &key) const {
    return compliance[key] ? true : false;
}

/* Método para adicionar ou modificar uma chave no YAML */
void ComplianceHandle::setComplianceValue(const std::string &key, const YAML::Node &value) {
    compliance[key] = value;
}

/* Getter para o arquivo de conformidade */
const std::string &ComplianceHandle::getComplianceFile() const {
    return complianceFile;
}

// Função para salvar o YAML de volta no arquivo
void ComplianceHandle::saveComplianceFile() const
{
    try {
        // Abre o arquivo para escrita
        std::ofstream fout(complianceFile);
        if (!fout.is_open()) {
            throw std::runtime_error("Não foi possível abrir o arquivo para escrita.");
        }

        // Escreve o conteúdo do nó YAML no arquivo
        fout << compliance;
        fout.close();
    } catch (const std::exception &e) {
        throw std::runtime_error("Erro ao salvar o arquivo YAML: "
                                 + std::string(e.what()));
    }
}

std::unordered_map<std::string, InvalidObjectInfo> ComplianceHandle::checkCompliance(std::string targetProjectPath)
{
    // Set path for the ComplianceChecker
    checker->setCompliancePath(targetProjectPath);

    // Iterate over the directory and subdirectories
    for (const auto& entry : std::filesystem::recursive_directory_iterator(targetProjectPath)) {
        // Check if the current entry is a file
        if (std::filesystem::is_regular_file(entry)) {
            std::string filePath = entry.path().string();

            // Check if the file has a .c or .h extension
            if (filePath.substr(filePath.find_last_of(".") + 1) == "c" ||
                filePath.substr(filePath.find_last_of(".") + 1) == "h") {
                std::cout << "Verificando arquivo: " << filePath << std::endl;
                checker->checkFile(filePath);  // Call checkFile for each valid file
                }
        }
    }

    return checker->getInvalidObjects();
}

// Função que será chamada para salvar o arquivo após a alteração
void ComplianceHandle::saveComplianceGlobal(const YAML::Node &newCompliance) {
    compliance = newCompliance;  // Atualiza o nó de compliance
    checker = new ComplianceChecker(compliance);
    saveComplianceFile();        // Salva no arquivo
}