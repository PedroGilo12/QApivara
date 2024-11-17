// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QTreeWidget>  // Inclui QTreeWidget para a árvore de diretórios

#include "HomeTab.h"


class ComplianceGrid;
QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

   private:
    QTreeWidget *treeWidget;
    QString projectPath;
    QTabWidget *tabWidget;
    QTableWidget *table;
    HomeTabSingleton &HomeTab = HomeTabSingleton::instance();
    ComplianceGrid *complianceGrid;

   public:
    MainWindow();

   protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif  // QT_NO_CONTEXTMENU

   private slots:
    void newFile();
    void createDSM();
    void open();
    void setCompliance();
    void viewResults();
    void safetyAnalyze();
    void about();

   private:
    void createActions();
    void createMenus();
    void insertItemsInTree(const QString &path,
                           QTreeWidgetItem *parentItem =
                               nullptr);  // Novo método para inserir itens recursivamente

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *helpMenu;
    QActionGroup *alignmentGroup;

    QAction *newAct;
    QAction *viewResultAct;
    QAction *openAct;
    QAction *exitAct;
    QAction *setComplianceAct;
    QAction *safetyAnalyzeAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QLabel *infoLabel;
};

#endif
