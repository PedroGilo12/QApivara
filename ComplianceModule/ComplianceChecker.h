//
// Created by Pedro on 09/11/2024.
//

#ifndef COMPLIANCECHECKER_H
#define COMPLIANCECHECKER_H
#include <yaml-cpp/yaml.h>

#include <unordered_map>

struct InvalidObjectInfo {
  std::string fileName;
  unsigned int line;
  unsigned int column;
};

class ComplianceChecker {

public:
  explicit ComplianceChecker(YAML::Node config);
  void checkFile(std::string fileName);
  std::unordered_map<std::string, InvalidObjectInfo> getInvalidObjects();
  void setCompliancePath(std::string projectPath);

};



#endif //COMPLIANCECHECKER_H
