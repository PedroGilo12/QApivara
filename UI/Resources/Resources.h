//
// Created by Pedro on 02/11/2024.
//

#ifndef RESOURCES_H
#define RESOURCES_H

#include <QIcon>
#include <QColor>

class Resources {
public:
    // Construtor
    Resources() :
        buttonIcon("../UI/Resources/DSMicon.png"),
        openIcon("../UI/Resources/OpenIcon.png"),
        configIcon("../UI/Resources/ConfigIcon.png"),
        primaryColor("#0078D7"),      // Azul
        secondaryColor("#FFFFFF"),      // Branco
        accentColor("#FF5733")          // Vermelho
    {}

    // Método para acessar o ícone, se necessário
    QIcon getButtonIcon() const {
        return buttonIcon;
    }

    QIcon getOpenIcon() const
    {
        return openIcon;
    }

    QIcon getConfigIcon() const
    {
        return configIcon;
    }

    // Métodos para acessar as cores
    QColor getPrimaryColor() const {
        return primaryColor;
    }

    QColor getSecondaryColor() const {
        return secondaryColor;
    }

    QColor getAccentColor() const {
        return accentColor;
    }

private:
    QIcon buttonIcon;  // Membro da classe
    QIcon openIcon;
    QIcon configIcon;
    QColor primaryColor;      // Cor primária
    QColor secondaryColor;    // Cor secundária
    QColor accentColor;       // Cor de destaque
};

#endif // RESOURCES_H
