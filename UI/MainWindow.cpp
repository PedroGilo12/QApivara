#include "mainwindow.h"

#include <QtWidgets>

#include "../DSMatrizModule/DSMatrizModule.h"
#include "HomeTab.h"
#include "Resources/Resources.h"

#include "../../../../../msys64/mingw64/include/c++/12.1.0/complex"
#include "../ComplianceModule/ComplianceHandle.h"
#include "ComplianceGrid.h"
#include "../SafetyAnalyzer/SafetyAnalyzer.h"
#include <QFileInfo>

#include "MainWindow.h"
#include "Resources/Resources.h"

static QString getFileNameWithoutExtension(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
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

void showCompliancePopup(QString *fileName)
{
    // Criando o popup
    QDialog popup;
    popup.setWindowTitle("Compliance Selection");

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(&popup);

    // Caixa de seleção "Use Default Compliance"
    QCheckBox *useDefaultComplianceCheckBox =
        new QCheckBox("Use Default Compliance", &popup);
    layout->addWidget(useDefaultComplianceCheckBox);

    // Caixa de seleção "Select Custom Compliance"
    QCheckBox *selectCustomComplianceCheckBox =
        new QCheckBox("Select Custom Compliance", &popup);
    layout->addWidget(selectCustomComplianceCheckBox);

    // QLabel para mostrar o caminho do arquivo selecionado
    QLabel *filePathLabel = new QLabel(&popup);
    layout->addWidget(filePathLabel);

    // Botões Ok e Cancelar
    QPushButton *okButton     = new QPushButton("Ok", &popup);
    QPushButton *cancelButton = new QPushButton("Cancel", &popup);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    // Conectando os botões
    QObject::connect(cancelButton, &QPushButton::clicked, &popup, &QDialog::reject);
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        if (selectCustomComplianceCheckBox->isChecked()) {
            // Se "Select Custom Compliance" estiver selecionado, abre o QFileDialog
            *fileName = QFileDialog::getOpenFileName(&popup, "Select YAML File", "",
                                                     "YAML Files (*.yaml)");
            if (!(*fileName).isEmpty()) {
                filePathLabel->setText(
                    "Selected File: "
                    + *fileName);  // Exibe o caminho do arquivo selecionado
            } else {
                // Se o usuário não escolheu um arquivo, pode exibir um alerta
                QMessageBox::warning(&popup, "Warning", "No file selected.");
            }
        } else if(useDefaultComplianceCheckBox->isChecked()){
            *fileName = "../ComplianceModule/default_compliance.yaml";
        }
        popup.accept();  // Fecha o popup depois de pressionar OK
    });

    // Exibindo o popup
    popup.exec();  // Abre o popup e aguarda a interação do usuário
}

MainWindow::MainWindow()
{
    Resources resources;  // Cria uma instância de Resources

    this->setWindowIcon(QIcon("../UI/Resources/logo.png"));
    // Widget central
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    // InfoLabel no topo
    infoLabel = new QLabel(tr("<i>Choose a menu option, or right-click to "
                              "invoke a context menu</i>"));
    infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    HomeTab.setOpenButtonHandler(std::bind(&MainWindow::open, this));
    HomeTab.setDSMButtonHandler(std::bind(&MainWindow::createDSM, this));
    HomeTab.setComplianceButtonHandler(std::bind(&MainWindow::setCompliance, this));
    HomeTab.setButton4Handler(std::bind(&MainWindow::viewResults, this));
    HomeTab.setButton5Handler(std::bind(&MainWindow::safetyAnalyze, this));

    // Painel central com QTabWidget
    tabWidget = new QTabWidget;  // Cria o QTabWidget para navegação entre abas
    tabWidget->addTab(&HomeTab, "Home");

    // Layout central
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(infoLabel);  // InfoLabel no topo
    centralLayout->addWidget(tabWidget);  // Adiciona o QTabWidget
    centralWidget->setLayout(centralLayout);

    // Painel Lateral Esquerdo
    QDockWidget *sideDock = new QDockWidget(tr("Painel Lateral"), this);
    sideDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    treeWidget = new QTreeWidget;  // Cria uma QTreeWidget para a estrutura em árvore
    treeWidget->setHeaderHidden(
        true);  // Oculta o cabeçalho para uma visualização mais limpa
    sideDock->setWidget(treeWidget);  // Associa o QTreeWidget ao painel lateral
    addDockWidget(Qt::LeftDockWidgetArea, sideDock);

    // Novo Painel Lateral Direito
    QDockWidget *rightDock = new QDockWidget(tr("Compliance Configuration"), this);
    rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
    rightDock->setMinimumWidth(300);
    QWidget *dockWidgetContents = new QWidget(rightDock);
    complianceGrid = new ComplianceGrid(dockWidgetContents);

    // Definindo o widget central do QDockWidget
    rightDock->setWidget(dockWidgetContents);

    // Adicionando o QDockWidget à área de dock
    addDockWidget(Qt::RightDockWidgetArea, rightDock);
    // Configuração dos menus e status bar
    createActions();
    createMenus();
    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);

    // Configurações de janela
    setWindowTitle(tr("QApivara"));
    resize(800, 600);  // Tamanho inicial
    setDockOptions(QMainWindow::AnimatedDocks | QMainWindow::AllowNestedDocks);

    showMaximized();  // Abre a janela maximizada
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(safetyAnalyzeAct);
    menu.exec(event->globalPos());
}
#endif  // QT_NO_CONTEXTMENU
//! [3]

void MainWindow::newFile()
{
    infoLabel->setText(tr("Invoked <b>File|New</b>"));
}

void MainWindow::createDSM()
{
    const char *command_line_args[] = {
        "-DMACRO_TESTE=0",
    };

    int size = std::size(command_line_args);

    auto dsmatriz = DSMatrizModule(projectPath.toStdString().c_str());
    dsmatriz.setMatriz(projectPath.toStdString(), command_line_args, size);

    table = new QTableWidget(dsmatriz.size, dsmatriz.size);

    tabWidget->addTab(table, "Dependency Structure Matriz");

    for (int col = 0; col < dsmatriz.size; col++) {
        QString headerName = QString(dsmatriz.dsMatriz[col].file_name);
        auto *headerItem   = new QTableWidgetItem(headerName);
        headerItem->setTextAlignment(Qt::AlignCenter);
        headerItem->setToolTip(
            headerName);  // Define o tooltip para o cabeçalho horizontal
        table->setHorizontalHeaderItem(col, headerItem);

        auto *verticalHeaderItem = new QTableWidgetItem(headerName);
        verticalHeaderItem->setToolTip(
            headerName);  // Define o tooltip para o cabeçalho vertical
        table->setVerticalHeaderItem(col, verticalHeaderItem);
    }

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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

            int col = findColumnIndex(*table, file_dependency);
            if (col == -1) {
                continue;
            }
            printf("%d\n", col);

            int size_dependency =
                dsmatriz.dsMatriz[row].dependencies[it.first.c_str()].function_map.size();

            table->setItem(row, col,
                           new QTableWidgetItem(QString::number(size_dependency)));
            QTableWidgetItem *targetItem = table->item(row, col);
            if (targetItem) {
                targetItem->setBackground(
                    QBrush(QColor(30 + (size_dependency * 10), 30, 30)));
            }
        }
        printf("\n");
    }
}

void MainWindow::open()
{
    // Diálogo de seleção de pasta
    QString dirPath = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"), "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty())
        return;

    projectPath = dirPath;

    complianceGrid->setProjectPath(projectPath.toStdString());

    HomeTab.DSMButton->setEnabled(true);
    HomeTab.SafetyButton->setEnabled(true);

    infoLabel->setText(tr("Selected Directory: <b>%1</b>").arg(dirPath));

    // Limpe o treeWidget antes de adicionar novos itens
    treeWidget->clear();

    // Chama a função para inserir diretórios e arquivos na árvore
    insertItemsInTree(dirPath, nullptr);
}

void MainWindow::insertItemsInTree(const QString &path, QTreeWidgetItem *parentItem)
{
    // Define o item de destino, considerando se é uma raiz ou um filho
    QTreeWidgetItem *item =
        parentItem
            ? new QTreeWidgetItem(parentItem)
            : new QTreeWidgetItem(
                  treeWidget);  // Cria um item filho da árvore se parentItem for nullptr

    item->setText(0, QFileInfo(path).fileName());

    // Se o diretório contiver subdiretórios, chame a função recursivamente para preencher
    // a árvore
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : dir.entryInfoList()) {
        insertItemsInTree(fileInfo.filePath(), item);
    }
}

void MainWindow::setCompliance()
{
    infoLabel->setText(tr("Invoked <b>Edit|Undo</b>"));

    QString complianceFile = "";

    showCompliancePopup(&complianceFile);

    complianceGrid->setComplianceFile(complianceFile);

    infoLabel->setText(tr("Selected Compliance File: ") + complianceFile);
}

void MainWindow::viewResults()
{
    infoLabel->setText(tr("Invoked <b>Edit|Redo</b>"));

    QString tabName = tr("Compliance Results");

    // Verifica se já existe uma aba com o nome adesejado e a remove
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == tabName) {
            QWidget *existingTab = tabWidget->widget(i);
            tabWidget->removeTab(i);
            delete existingTab; // Libera a memória da aba existente
            break;
        }
    }

    // Cria um novo widget para a aba e define o layout
    QWidget *complianceWidget = new QWidget(this);
    QVBoxLayout *complianceLayout = new QVBoxLayout(complianceWidget);

    // Define o espaçamento e margens para reduzir a distância entre os labels
    complianceLayout->setSpacing(0);        // Espaçamento mínimo entre os widgets
    complianceLayout->setContentsMargins(0, 0, 0, 0); // Remove as margens
    complianceLayout->addStretch();

    // Adiciona o título da aba
    QLabel *headerLabel = new QLabel("Compliance Results:");
    headerLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    complianceLayout->addWidget(headerLabel);

    // Adiciona cada resultado ao layout
    for (const auto &entry : complianceGrid->results) {
        const std::string &objectName = entry.first;
        const InvalidObjectInfo &info = entry.second;

        // Extrai apenas o nome do arquivo
        QString fileName = QFileInfo(QString::fromStdString(info.fileName)).fileName();

        // Cria uma label com todas as informações formatadas
        QString labelText = QString("Nome: %1 | Arquivo: %2 | Linha: %3 | Coluna: %4")
            .arg(QString::fromStdString(objectName))
            .arg(fileName)
            .arg(info.line)
            .arg(info.column);

        QLabel *resultLabel = new QLabel(labelText);
        resultLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); // Ajusta o tamanho ao conteúdo

        // Cria um botão ao lado da label
        QPushButton *openButton = new QPushButton("Abrir Arquivo");
        openButton->setFixedWidth(100);

        // Conecta o botão à função que abrirá o arquivo
        connect(openButton, &QPushButton::clicked, this, [filePath = QString::fromStdString(info.fileName)]() {
            // Usando QDesktopServices para abrir o arquivo no explorador
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        });

        // Cria um layout horizontal para a label e o botão
        QHBoxLayout *labelLayout = new QHBoxLayout();
        labelLayout->addWidget(resultLabel);
        labelLayout->addWidget(openButton);

        // Adiciona o layout da label + botão ao layout principal
        complianceLayout->addLayout(labelLayout);
    }

    // Cria uma área de scroll e define o complianceWidget como seu conteúdo
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(complianceWidget);

    // Adiciona a área de scroll como uma nova aba
    tabWidget->addTab(scrollArea, tabName);
}

void MainWindow::safetyAnalyze() {
    infoLabel->setText(tr("Invoked <b>Edit|Cut</b>"));

    QString tabName = tr("Safety Analysis Results");

    SafetyAnalyzer safety(projectPath.toStdString());

    // Percorre todos os arquivos no diretório
    for (const auto& entry : std::filesystem::directory_iterator(projectPath.toStdString())) {
        if (entry.is_regular_file()) {
            // Chama checkFile para cada arquivo encontrado
            std::string filePath = entry.path().string();
            std::cout << "Analisando o arquivo: " << filePath << std::endl;
            safety.checkFile(filePath);
        }
    }

    // Obtém os objetos inseguros detectados
    std::unordered_map<std::string, std::string> unsafetyObjects = safety.getUnsafetyObjects();

    // Processa e imprime os objetos inseguros
    std::cout << "Objetos inseguros encontrados:" << std::endl;
    for (const auto& entry : unsafetyObjects) {
        std::cout << "Localização: " << entry.first << " | Tipo: " << entry.second << std::endl;
    }

    // Verifica se já existe uma aba com o nome desejado e a remove
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == tabName) {
            QWidget *existingTab = tabWidget->widget(i);
            tabWidget->removeTab(i);
            delete existingTab; // Libera a memória da aba existente
            break;
        }
    }

    // Cria um novo widget para a aba e define o layout
    QWidget *safetyWidget = new QWidget(this);
    QVBoxLayout *safetyLayout = new QVBoxLayout(safetyWidget);

    // Define o espaçamento e margens para reduzir a distância entre os labels
    safetyLayout->setSpacing(0);        // Espaçamento mínimo entre os widgets
    safetyLayout->setAlignment(Qt::AlignTop);
    safetyLayout->setContentsMargins(0, 0, 0, 0); // Margens de 0 entre os widgets


    // Cria uma QTableWidget para mostrar os resultados
    QTableWidget *safetyTable = new QTableWidget(unsafetyObjects.size(), 2, safetyWidget); // 2 colunas: Localização e Tipo
    safetyTable->setHorizontalHeaderLabels({"Localização", "Tipo"});

    // Preenche a tabela com os dados de segurança
    int row = 0;
    for (const auto& entry : unsafetyObjects) {
        safetyTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(entry.first))); // Localização
        safetyTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(entry.second))); // Tipo
        row++;
    }

    // Ajusta o redimensionamento automático das colunas e linhas
    safetyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    safetyTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Adiciona a tabela ao layout da nova aba
    safetyLayout->addWidget(safetyTable);
    safetyLayout->addStretch(1);

    // Adiciona a nova aba ao tabWidget
    tabWidget->addTab(safetyWidget, tabName);

    // Atualiza a barra de status
    infoLabel->setText(tr("Safety analysis results displayed."));
}

void MainWindow::about()
{
    infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    QMessageBox::about(this, tr("About Menu"),
                       tr("GitHub do projeto: https://github.com/PedroGilo12/QApivara"));
}

//! [4]
void MainWindow::createActions()
{
    //! [5]
    newAct =
        new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    //! [4]

    openAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen),
                          tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    exitAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ApplicationExit),
                          tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    setComplianceAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties),
                                   tr("&Set Compliance"), this);
    setComplianceAct->setStatusTip(tr("Define the file for compliance assessment"));
    connect(setComplianceAct, &QAction::triggered, this, &MainWindow::setCompliance);

    viewResultAct =
        new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditRedo), tr("&Redo"), this);
    viewResultAct->setShortcuts(QKeySequence::Redo);
    viewResultAct->setStatusTip(tr("Redo the last operation"));
    connect(viewResultAct, &QAction::triggered, this, &MainWindow::viewResults);

    safetyAnalyzeAct = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DialogInformation), tr("SafetyAnalyzer"), this);
    safetyAnalyzeAct->setStatusTip(tr("Verifique possíveis falhas no código"));
    connect(safetyAnalyzeAct, &QAction::triggered, this, &MainWindow::safetyAnalyze);

    aboutAct =
        new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(setComplianceAct);
    editMenu->addSeparator();
    editMenu->addAction(safetyAnalyzeAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    formatMenu = editMenu->addMenu(tr("&Format"));
    formatMenu->addSeparator()->setText(tr("Alignment"));
    formatMenu->addSeparator();
}
