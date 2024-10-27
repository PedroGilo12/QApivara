//
// Created by Pedro on 27/10/2024.
//

#include "DSMatrizModuleSample.h"

#include <clang-c/Index.h>
#include <clang/Lex/Preprocessor.h>

#include <QApplication>
#include <QFileInfo>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

#include "DSMatrizModule.h"

// Mapa para armazenar funções e seus respectivos locais
std::unordered_map<std::string, FunctionInfo> functionMap;

const char *file_path;

static QString getFileNameWithoutExtension(const QString &filePath)
{
    QFileInfo fileInfo(
        filePath);
    return fileInfo.baseName();
}


static int findColumnIndex(QTableWidget &table, const QString &searchText)
{
    for (int col = 0; col < table.columnCount(); ++col) {
        if (table.horizontalHeaderItem(col)->text() == searchText) {
            return col;
        }
        for (int row = 0; row < table.rowCount(); ++row) {
            QTableWidgetItem *item = table.item(row, col);
            if (item && item->text() == searchText) {
                return col;
            }
        }
    }
    return -1;
}

int DSMatrizModuleSample(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Matriz de Dependência Estrutural");

    file_path = R"(C:\Users\Pedro\CLionProjects\Urutau-QA\test\)";

    const char * command_line_args[] = {
        "-DMACRO_TESTE=0",
    };

    int size = std::size(command_line_args);

    auto dsmatriz = DSMatrizModule(file_path);
    dsmatriz.setMatriz(file_path, command_line_args, size);

    printf("matriz size: %d\n", dsmatriz.size);

    QTableWidget table(dsmatriz.size, dsmatriz.size);

    for (int col = 0; col < dsmatriz.size; col++) {
        auto *headerItem =
            new QTableWidgetItem(QString(dsmatriz.dsMatriz[col].file_name));
        headerItem->setTextAlignment(Qt::AlignCenter);
        table.setHorizontalHeaderItem(col, headerItem);

        table.setVerticalHeaderItem(
            col, new QTableWidgetItem(QString(dsmatriz.dsMatriz[col].file_name)));
    }

    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table.verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QSet<int> expandedColumns;

    for (int row = 0; row < dsmatriz.size; row++) {
        for (auto &it : dsmatriz.dsMatriz[row].dependencies) {
            printf("%s >>> %s\n", dsmatriz.dsMatriz[row].file_name,
                   getFileNameWithoutExtension(QString::fromStdString(it.first))
                       .toUtf8()
                       .constData());

            const char *file_dependency =
                getFileNameWithoutExtension(QString::fromStdString(it.first))
                    .toUtf8()
                    .constData();

            int col = findColumnIndex(table, file_dependency);
            if (col == -1) {
                continue;
            }
            printf("%d\n", col);

            int size_dependency =
                dsmatriz.dsMatriz[row].dependencies[it.first.c_str()].function_map.size();

            table.setItem(row, col,
                          new QTableWidgetItem(QString::number(size_dependency)));
            QTableWidgetItem *targetItem = table.item(row, col);
            if (targetItem) {
                targetItem->setBackground(
                    QBrush(QColor(30 + (size_dependency * 10), 30, 30)));
            }
        }
        printf("\n");
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(&table);
    window.setLayout(layout);

    window.resize(600, 400);
    window.show();

    return app.exec();
};

