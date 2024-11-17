//
// Created by Pedro on 02/11/2024.
//

#ifndef HOMETAB_H
#define HOMETAB_H

#include <QGridLayout>
#include <QWidget>
#include <QPushButton>
#include <functional> // Para std::function

class HomeTabSingleton : public QWidget {
public:
    static HomeTabSingleton& instance();

    // Métodos para redefinir comportamentos dos botões
    void setOpenButtonHandler(std::function<void()> handler);
    void setDSMButtonHandler(std::function<void()> handler);
    void setComplianceButtonHandler(std::function<void()> handler);
    void setButton4Handler(std::function<void()> handler);
    void setButton5Handler(std::function<void()> handler);

    QPushButton *DSMButton;
    QPushButton *SafetyButton;

private:
    HomeTabSingleton();
    QPushButton *createButton(const QString &text, const QIcon::ThemeIcon &themeIconName);

    // Slots padrão
    void onOpenButtonClicked();
    void onDSMButtonClicked();
    void onComplianceButtonClicked();
    void onButton4Clicked();
    void onButton5Clicked();

    std::function<void()> button1Handler;  // Armazenará a nova função para o botão 1
    std::function<void()> dsmButtonHandler; // Armazenará a nova função para o botão DSM
    std::function<void()> complianceButtonHandler;   // Armazenará a nova função para o botão 3
    std::function<void()> button4Handler;   // Armazenará a nova função para o botão 4
    std::function<void()> button5Handler;   // Armazenará a nova função para o botão 5

    HomeTabSingleton(const HomeTabSingleton&) = delete;
    HomeTabSingleton& operator=(const HomeTabSingleton&) = delete;
    HomeTabSingleton(HomeTabSingleton&&) = delete;
    HomeTabSingleton& operator=(HomeTabSingleton&&) = delete;
};

#endif // HOMETAB_H
