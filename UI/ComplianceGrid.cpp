#include "ComplianceGrid.h"

#include <qlineedit.h>
#include <qprogressbar.h>

#include <QMessageBox>

#include "../../../../../msys64/mingw64/include/c++/12.1.0/complex"
#include "../ComplianceModule/ComplianceHandle.h"

class LoadingPopup : public QDialog {
public:
    LoadingPopup(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Carregando...");
        setModal(true); // Impede interação com a janela principal

        // Criação do layout
        QVBoxLayout *layout = new QVBoxLayout(this);

        // Criação do QProgressBar
        QProgressBar *progressBar = new QProgressBar(this);
        progressBar->setRange(0, 0);  // Progressão indeterminada
        progressBar->setTextVisible(false);  // Não exibe o texto
        progressBar->setStyleSheet("QProgressBar { border: 2px solid gray; border-radius: 5px; text-align: center; }");

        layout->addWidget(progressBar);
        setLayout(layout);
        resize(200, 100); // Tamanho do popup

        progressBar->setMinimumWidth(150);  // Defina a largura desejada para a barra
    }
};

class QLineEdit;
ComplianceGrid::ComplianceGrid(QWidget *parent) : QGridLayout(parent)
{
    parentWidget = parent;
    scrollArea = new QScrollArea(parent);
    this->addWidget(scrollArea, 0, 0, 1, 2);
    createButtons(parent);
}

void ComplianceGrid::setComplianceFile(QString complianceFile)
{
    try {
        compliancePath = complianceFile;
        complianceHandle = new ComplianceHandle(complianceFile.toStdString());
        YAML::Node config = complianceHandle->getComplianceGlobal();

        // Criação do widget que conterá os labels e botões
        labelsWidget = new QWidget(parentWidget);
        QVBoxLayout *labelsLayout = new QVBoxLayout(labelsWidget);

        if (config.IsMap()) {  // Se for um mapa (objeto)
            // Processa o node 'naming_patterns'
            if (config["naming_patterns"]) {
                QLabel *sectionLabel = new QLabel(tr("Naming Patterns:"), labelsWidget);
                labelsLayout->addWidget(sectionLabel);
                YAML::Node namingPatterns = config["naming_patterns"];
                for (YAML::const_iterator it = namingPatterns.begin(); it != namingPatterns.end(); ++it) {
                    QString key = QString::fromStdString(it->first.as<std::string>());
                    QString value = QString::fromStdString(it->second.as<std::string>());

                    // Adiciona um QLabel para a chave
                    QLabel *keyLabel = new QLabel(tr("%1:").arg(key), labelsWidget);
                    labelsLayout->addWidget(keyLabel);

                    // Cria um QLineEdit para o valor, permitindo edição
                    QLineEdit *valueLineEdit = new QLineEdit(value, labelsWidget);
                    labelsLayout->addWidget(valueLineEdit);

                    // Cria um botão "Change" ao lado do QLineEdit
                    QPushButton *changeButton = new QPushButton(tr("Change"), labelsWidget);
                    labelsLayout->addWidget(changeButton);

                    // Conecta o botão ao slot que atualizará o valor no YAML
                    connect(changeButton, &QPushButton::clicked, [this, key, valueLineEdit](){
                        QString newValue = valueLineEdit->text();
                        updateYamlValue(key, newValue);
                    });
                }
            }

            // Processa o node 'specifies_naming_patterns'
            if (config["specifies_naming_patterns"]) {
                QLabel *sectionLabel = new QLabel(tr("Specifies Naming Patterns:"), labelsWidget);
                labelsLayout->addWidget(sectionLabel);
                YAML::Node specifiesNamingPatterns = config["specifies_naming_patterns"];
                for (YAML::const_iterator it = specifiesNamingPatterns.begin(); it != specifiesNamingPatterns.end(); ++it) {
                    QString key = QString::fromStdString(it->first.as<std::string>());
                    QString value = QString::fromStdString(it->second.as<std::string>());

                    QLabel *keyLabel = new QLabel(tr("%1:").arg(key), labelsWidget);
                    labelsLayout->addWidget(keyLabel);

                    QLineEdit *valueLineEdit = new QLineEdit(value, labelsWidget);
                    labelsLayout->addWidget(valueLineEdit);

                    QPushButton *changeButton = new QPushButton(tr("Change"), labelsWidget);
                    labelsLayout->addWidget(changeButton);

                    connect(changeButton, &QPushButton::clicked, [this, key, valueLineEdit](){
                        QString newValue = valueLineEdit->text();
                        updateYamlValue(key, newValue);
                    });
                }
            }

            // Processa o node 'compiler_options'
            if (config["compiler_options"]) {
                QLabel *sectionLabel = new QLabel(tr("Compiler Options:"), labelsWidget);
                labelsLayout->addWidget(sectionLabel);
                YAML::Node compilerOptions = config["compiler_options"];
                for (YAML::const_iterator it = compilerOptions.begin(); it != compilerOptions.end(); ++it) {
                    QString key = QString::fromStdString(it->first.as<std::string>());
                    QString value = QString::fromStdString(it->second.as<std::string>());

                    QLabel *keyLabel = new QLabel(tr("%1:").arg(key), labelsWidget);
                    labelsLayout->addWidget(keyLabel);

                    QLineEdit *valueLineEdit = new QLineEdit(value, labelsWidget);
                    labelsLayout->addWidget(valueLineEdit);

                    QPushButton *changeButton = new QPushButton(tr("Change"), labelsWidget);
                    labelsLayout->addWidget(changeButton);

                    connect(changeButton, &QPushButton::clicked, [this, key, valueLineEdit](){
                        QString newValue = valueLineEdit->text();
                        updateYamlValue(key, newValue);
                    });
                }

                // Processa 'include_paths' que é uma sequência
                if (compilerOptions["include_paths"]) {
                    QLabel *includePathsLabel = new QLabel(tr("Include Paths:"), labelsWidget);
                    labelsLayout->addWidget(includePathsLabel);
                    YAML::Node includePaths = compilerOptions["include_paths"];
                    for (size_t i = 0; i < includePaths.size(); ++i) {
                        QString path = QString::fromStdString(includePaths[i].as<std::string>());
                        QLabel *pathLabel = new QLabel(path, labelsWidget);
                        labelsLayout->addWidget(pathLabel);
                    }
                }
            }
        } else {
            throw std::runtime_error("O YAML não é um mapa ou sequência válida.");
        }

        // Define o widget de labels na área de rolagem
        scrollArea->setWidget(labelsWidget);
        this->addWidget(scrollArea, 0, 0, 1, 2);  // Ocupa as 2 colunas disponíveis

    } catch (const std::exception &e) {
        // Se ocorrer qualquer exceção, exibe o popup de erro
        QMessageBox::critical(parentWidget, tr("Erro de Processamento"), tr("Ocorreu um erro ao processar o arquivo YAML:\n%1").arg(e.what()));
    } catch (...) {
        // Captura outros tipos de exceções não previstas
        QMessageBox::critical(parentWidget, tr("Erro de Processamento"), tr("Ocorreu um erro desconhecido ao processar o arquivo YAML."));
    }
}

// Método que atualiza o valor no YAML
void ComplianceGrid::updateYamlValue(const QString &key, const QString &newValue)
{
    printf("Salvando: %s %s\n", key.toStdString().c_str(), newValue.toStdString().c_str());
    try {
        YAML::Node config = complianceHandle->getComplianceGlobal();  // Recupere o nó original
        if (config["specifies_naming_patterns"].IsMap() && config["specifies_naming_patterns"][key.toStdString()]) {
            config["specifies_naming_patterns"][key.toStdString()] = newValue.toStdString();
        }
        if (config["compiler_options"].IsMap() && config["compiler_options"][key.toStdString()]) {
            config["compiler_options"][key.toStdString()] = newValue.toStdString();
        }
        if (config["naming_patterns"].IsMap() && config["naming_patterns"][key.toStdString()]) {
            config["naming_patterns"][key.toStdString()] = newValue.toStdString();
        }

        complianceHandle->saveComplianceGlobal(config);
    } catch (const std::exception &e) {
        QMessageBox::critical(parentWidget, tr("Erro de Atualização"), tr("Ocorreu um erro ao atualizar o valor no YAML:\n%1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(parentWidget, tr("Erro de Atualização"), tr("Ocorreu um erro desconhecido ao atualizar o valor no YAML."));
    }
}

void ComplianceGrid::createButtons(QWidget *parent)
{
    // Cria os botões
    verifyButton = new QPushButton(tr("Verify Compliance"), parent);
    changeButton = new QPushButton(tr("Change Compliance"), parent);

    verifyButton->setEnabled(false);

    connect(verifyButton, &QPushButton::clicked, this, &ComplianceGrid::verifyButtonClick);

    this->addWidget(verifyButton, 1, 0);
    this->addWidget(changeButton, 1, 1);

    this->setColumnStretch(0, 1);
    this->setColumnStretch(1, 1);
}

void ComplianceGrid::verifyButtonClick()
{
    results = complianceHandle->checkCompliance(project);
}

void ComplianceGrid::setProjectPath(std::string projectPath)
{
    project = projectPath;
    verifyButton->setEnabled(true);
}

void ComplianceGrid::unsetProjectPath(std::string projectPath)
{
    project = "";
    verifyButton->setEnabled(false);
}



