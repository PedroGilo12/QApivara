//
// Created by Pedro on 10/11/2024.
//

#ifndef SAFETYANALYZER_H
#define SAFETYANALYZER_H
#include <string>
#include <unordered_map>

class SafetyAnalyzer
{
   public:
    explicit SafetyAnalyzer(std::string projectPath);
    void checkFile(std::string fileName);
    std::unordered_map<std::string, std::string> getUnsafetyObjects();
};

#endif  // SAFETYANALYZER_H
