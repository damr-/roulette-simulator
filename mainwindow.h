#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "outputitemwidget.h"
#include "qcustomplot.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString fileName = "Roulette Settings";

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);

    void setupPlot();

    int loadSettings();
    void saveSettings();

    void playRound();
    void gameOver();

    void setPaused(bool paused);
    void enableEditing(bool enabled);

    void updateUI();
    void resetAllValues();

    QTimer *timer = new QTimer(this);
    OutputItemWidget *itemWidget;
    QCPGraph *graph;

    int currentMoney = 0;
    int maxMoney = 0;
    int currentProfit = 0;
    int currentBet = 1;

    int currentWinningStreak = 0;
    int currentLosingStreak = 0;

    int highestWinningStreak = 0;
    int highestWinRound = 0;

    int highestLosingStreak = 0;
    int highestLoss = 0;
    int highestLossRound = 0;

    int roundCount = 0;

    void createSaveFeedbackMessageBox(bool success, QString itemTitle);

private slots:
    void showContextMenu(QPoint pos);    
    void saveOutput();

    void start();
    void togglePause();
    void reset();
    void play();

    void showPlotContextMenu(QPoint pos);
    void plotClicked(QMouseEvent *ev);
    void plotHover(QMouseEvent *ev);
    void saveImage();
};

#endif // MAINWINDOW_H
