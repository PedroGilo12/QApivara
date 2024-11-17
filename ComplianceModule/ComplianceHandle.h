//
// Created by Pedro on 03/11/2024.
//

#ifndef COMPLIACEHANDLE_H
#define COMPLIACEHANDLE_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "ComplianceChecker.h"

class ComplianceHandle {
private:
    std::string complianceFile;
    ComplianceChecker *checker;
    YAML::Node compliance;

public:
    explicit ComplianceHandle(const std::string &file);

    YAML::Node getComplianceGlobal();
    YAML::Node getComplianceNode(const std::string &key) const;

    bool hasKey(const std::string &key) const;

    void setComplianceValue(const std::string &key, const YAML::Node &value);

    void saveComplianceGlobal(const YAML::Node &newCompliance);

    const std::string &getComplianceFile() const;
    void saveComplianceFile() const;

    std::unordered_map<std::string, InvalidObjectInfo> checkCompliance(std::string targetProjectPath);
};

#endif //COMPLIACEHANDLE_H
