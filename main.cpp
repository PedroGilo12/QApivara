#include <QApplication>

#include "ComplianceModule/ComplianceChecker.h"
#include "SafetyAnalyzer/SafetyAnalyzer.h"
#include "UI/ComplianceGrid.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return QApplication::exec();
}
