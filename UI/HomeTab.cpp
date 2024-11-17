#include "HomeTab.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QtWidgets>
#include <iostream>

#include "../SafetyAnalyzer/SafetyAnalyzer.h"
#include "Resources/Resources.h"

#define QAPIVARA_DESCRIPTION                                                           \
    "<h1>QApivara</h1>"                                                                \
    "<p>QApivara é um software de garantia da qualidade (QA) projetado "               \
    "especificamente "                                                                  \
    "para sistemas embarcados. Com a capacidade de gerar <b>Modelos de Dependência de " \
    "Software (DSM)</b>, "                                                              \
    "realizar análises estáticas e verificar conformidades com padrões de nomeação, "   \
    "<b>QApivara</b> "                                                                 \
    "oferece uma solução abrangente para aprimorar a qualidade e a confiabilidade de "  \
    "projetos de software.</p>"                                                         \
    "<h2>Funcionalidades</h2>"                                                          \
    "<ul>"                                                                              \
    "<li><b>Geração de DSM</b>: Criação de Modelos de Dependência de Software para "    \
    "análise detalhada.</li>"                                                           \
    "<li><b>Análise Estática</b>: Identificação de potenciais problemas no código "     \
    "antes da execução.</li>"                                                           \
    "<li><b>Verificação de Conformidades</b>: Garantia de que o software atende aos "   \
    "padrões de nomeação estabelecidos.</li>"                                           \
    "<li><b>Integração de Ferramentas</b>: Uso de ferramentas de análise e "            \
    "monitoramento para um desenvolvimento mais eficiente.</li>"                        \
    "</ul>"                                                                             \
    "<h2>Contribuição</h2>"                                                             \
    "<p>Sinta-se à vontade para contribuir com o projeto! Para mais informações, "      \
    "consulte a seção de contribuição abaixo.</p>"                                      \
    "<h2>Licença</h2>"                                                                  \
    "<p>"


// Implementação do método estático que retorna a instância única
HomeTabSingleton &HomeTabSingleton::instance()
{
    static HomeTabSingleton instance;  // Instância única
    return instance;
}

HomeTabSingleton::HomeTabSingleton()
{
    Resources resources;
    QVBoxLayout *layout = new QVBoxLayout(this);  // Layout principal em coluna

    // Definindo o tamanho máximo horizontal do layout
    layout->setSizeConstraint(QLayout::SetMaximumSize);

    // Criando o layout horizontal para os botões
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // Criando os botões quadrados com ícone e texto
    QPushButton *openButton =
        createButton("Open Project", QIcon::ThemeIcon::DocumentOpen);
    DSMButton =
        createButton("Generate DSM", QIcon::ThemeIcon::WindowNew);
    DSMButton->setEnabled(false);
    QPushButton *button3 =
        createButton("Set Compliance", QIcon::ThemeIcon::DocumentProperties);
    QPushButton *button4 =
        createButton("View Results", QIcon::ThemeIcon::ToolsCheckSpelling);
    SafetyButton =
        createButton("Safety Analyzer", QIcon::ThemeIcon::DialogInformation);
    SafetyButton->setEnabled(false);

    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(DSMButton);
    buttonLayout->addWidget(button3);
    buttonLayout->addWidget(button4);
    buttonLayout->addWidget(SafetyButton);

    // Adicionando o layout horizontal dos botões ao layout vertical principal
    layout->addLayout(buttonLayout);

    // Criando um QLabel para a descrição
    QLabel *label = new QLabel(QAPIVARA_DESCRIPTION);
    label->setWordWrap(true);            // Permite quebra de linha
    label->setFont(QFont("Arial", 12));  // Define a fonte e o tamanho

    // Criando um QScrollArea para permitir rolagem
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);  // O conteúdo do scroll area se ajusta ao tamanho
    //scrollArea->setFixedHeight(500);      // Define a altura do scroll area (ajuste conforme necessário)

    // Criando um widget que conterá o QLabel
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // Criando o QLabel para exibir a imagem
    QLabel *imageLabel = new QLabel(this);

    // Carregando a imagem com QPixmap
    QPixmap pixmap("../UI/Resources/logo.png");  // Substitua pelo caminho da imagem desejada

    // Verificando se a imagem foi carregada corretamente
    if (!pixmap.isNull()) {
        // Redimensionando a imagem antes de definir no QLabel
        pixmap = pixmap.scaled(200, 200, Qt::KeepAspectRatio);  // Mantém a proporção

        // Definindo a imagem no QLabel
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);  // Alinha a imagem no centro
    } else {
        std::cout << "Erro ao carregar a imagem." << std::endl;
    }

    // Adiciona o QLabel contendo a imagem ao layout do scroll
    scrollLayout->addWidget(imageLabel);
    scrollLayout->addWidget(label);  // Adiciona o QLabel ao layout do conteúdo do scroll

    // Definindo o conteúdo do QScrollArea
    scrollArea->setWidget(scrollContent);

    // Adicionando a QScrollArea ao layout principal
    layout->addWidget(scrollArea);

    // Definindo o layout no widget
    setLayout(layout);

    // Conectar sinais aos slots
    connect(openButton, &QPushButton::clicked, this,
            &HomeTabSingleton::onOpenButtonClicked);
    connect(DSMButton, &QPushButton::clicked, this,
            &HomeTabSingleton::onDSMButtonClicked);
    connect(button3, &QPushButton::clicked, this, &HomeTabSingleton::onComplianceButtonClicked);
    connect(button4, &QPushButton::clicked, this, &HomeTabSingleton::onButton4Clicked);
    connect(SafetyButton, &QPushButton::clicked, this, &HomeTabSingleton::onButton5Clicked);
}

// Método para criar um botão com texto e ícone
QPushButton *HomeTabSingleton::createButton(const QString &text, const QIcon::ThemeIcon &themeIconName)
{
    QPushButton *button = new QPushButton(nullptr);
    button->setFixedSize(100, 100);
    QVBoxLayout *layout = new QVBoxLayout(button);
    layout->setContentsMargins(0, 0, 0, 0);  // Remove as margens do layout

    // Ícone do botão
    QLabel *iconLabel = new QLabel();
    QIcon icon = QIcon::fromTheme(themeIconName);  // Carrega o ícone a partir do nome do tema
    iconLabel->setPixmap(icon.pixmap(50, 50));  // Escala o ícone
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);

    // Texto do botão
    QLabel *textLabel = new QLabel(text);
    textLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(textLabel, 0, Qt::AlignCenter);

    return button;
}


void HomeTabSingleton::setOpenButtonHandler(std::function<void()> handler)
{
    button1Handler = handler;
}

void HomeTabSingleton::setDSMButtonHandler(std::function<void()> handler)
{
    dsmButtonHandler = handler;
}

void HomeTabSingleton::setComplianceButtonHandler(std::function<void()> handler)
{
    complianceButtonHandler = handler;
}

void HomeTabSingleton::setButton4Handler(std::function<void()> handler)
{
    button4Handler = handler;
}

void HomeTabSingleton::setButton5Handler(std::function<void()> handler)
{
    button5Handler = handler;
}

void HomeTabSingleton::onOpenButtonClicked()
{
    if (button1Handler) {
        button1Handler();
    } else {
        qDebug() << "Button 1 clicked (default)!";
    }
}

void HomeTabSingleton::onDSMButtonClicked()
{
    if (dsmButtonHandler) {
        dsmButtonHandler();
    } else {
        qDebug() << "DSM Button clicked (default)!";
    }
}

void HomeTabSingleton::onComplianceButtonClicked()
{
    if (complianceButtonHandler) {
        complianceButtonHandler();
    } else {
        qDebug() << "Button 3 clicked (default)!";
    }
}

void HomeTabSingleton::onButton4Clicked()
{
    if (button4Handler) {
        button4Handler();
    } else {
        qDebug() << "Button 4 clicked (default)!";
    }
}

void HomeTabSingleton::onButton5Clicked()
{
    if (button5Handler) {
        button5Handler();
    } else {
        qDebug() << "Button 5 clicked (default)!";
    }
}
