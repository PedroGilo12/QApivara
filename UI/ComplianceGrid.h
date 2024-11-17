#ifndef COMPLIANCEGRID_H
#define COMPLIANCEGRID_H

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

#include "../ComplianceModule/ComplianceHandle.h"

class ComplianceGrid : public QGridLayout  {
public:
    explicit ComplianceGrid(QWidget *parent = nullptr);
    void setComplianceFile(QString complianceFile);
    void updateYamlValue(const QString &key, const QString &newValue);
    void setProjectPath(std::string projectPath);
    void unsetProjectPath(std::string projectPath);

    QWidget *parentWidget;
    std::unordered_map<std::string, InvalidObjectInfo> results;

private:
    void createLabels(QWidget *parent);
    void createButtons(QWidget *parent);
    void verifyButtonClick();
    std::string project;
    QPushButton *verifyButton;
    QPushButton *changeButton;

    ComplianceHandle * complianceHandle;
    QString compliancePath;
    QWidget *labelsWidget;
    QScrollArea *scrollArea;
};

#endif // COMPLIANCEGRID_H
