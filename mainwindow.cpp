#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "outputitemwidget.h"

#include "stdlib.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMenu>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->output->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->output, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(start()));
    connect(ui->togglePauseButton, SIGNAL(clicked(bool)), this, SLOT(togglePause()));
    connect(ui->resetButton, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(timer, SIGNAL(timeout()), this, SLOT(play()));

    setupPlot();

    QString msg = "Loaded settings file.";
    int res = loadSettings();
    if(res == 0)
        msg = "Settings file not found. Applied default settings.";
    else if(res == -1)
        msg = "Saved data is corrupted. Applied default settings.";

    ui->statusBar->showMessage(msg, 3000);

    currentMoney = ui->startMoney->value();
    updateUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::loadSettings()
{
    QFile file(fileName);

    if(!file.exists())
        return 0;

    if(!file.open(QFile::ReadOnly))
        return 0;

    QTextStream input(&file);
    QStringList options = input.readLine().split("|");

    if(options.size() == 2)
    {
        ui->timerIntervalSpinBox->setValue(options[0].toInt());
        ui->startMoney->setValue(options[1].toInt());
        return 1;
    }

    file.close();
    file.remove();
    return -1;
}

void MainWindow::saveSettings()
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        ui->statusBar->showMessage("Unable to open the settings file!", 3000);
        return;
    }

    QTextStream output(&file);
    output << QString::number(ui->timerIntervalSpinBox->value()) << "|"
           << QString::number(ui->startMoney->value());
    file.close();
}

void MainWindow::showContextMenu(QPoint pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Save", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(saveOutput()));
    contextMenu.addAction(&action1);

    contextMenu.exec(ui->output->mapToGlobal(pos));
}

void MainWindow::saveOutput()
{
    if(ui->output->count() == 0)
    {
        ui->statusBar->showMessage("Nothing to save.", 3000);
        return;
    }

    QString fileName = "output_" + QDate::currentDate().toString("dd-MM-yyyy") + "_" + QTime::currentTime().toString("hh-mm-ss") + ".txt";
    QString fileDir = QDir::currentPath() + "/" + fileName;
    QFile file(fileDir);

    bool success = file.open(QIODevice::WriteOnly | QFile::Truncate);
    if (success)
    {
        QTextStream out(&file);

        for(int i = 0; i < ui->output->count(); i++)
            out << ((OutputItemWidget*)ui->output->itemWidget(ui->output->item(i)))->getItemData() << "\n";
        file.close();
        ui->statusBar->showMessage("Output saved as " + fileDir, 3000);
    }

    createSaveFeedbackMessageBox(success, "output data");
    ui->statusBar->showMessage("Output could not be saved.", 3000);
}

void MainWindow::resetAllValues()
{
    ui->output->clear();
    currentBet = 1;
    currentMoney = ui->startMoney->value();
    roundCount = 0;
    currentWinningStreak = 0;
    highestWinningStreak = 0;
    highestWinRound = 0;
    currentLosingStreak = 0;
    highestLosingStreak = 0;
    highestLoss = 0;
    highestLossRound = 0;
    maxMoney = 0;
    currentProfit = 0;
    updateUI();
}

void MainWindow::setupPlot()
{
    ui->plot->xAxis->setLabel("Time");
    ui->plot->yAxis->setLabel("Money");

    graph = ui->plot->addGraph();
    graph->setPen(QPen(Qt::blue));

    ui->plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showPlotContextMenu(QPoint)));
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(plotClicked(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(plotHover(QMouseEvent*)));
}

void MainWindow::showPlotContextMenu(QPoint pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Save As Image", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(saveImage()));
    contextMenu.addAction(&action1);

    contextMenu.exec(ui->plot->mapToGlobal(pos));
}

void MainWindow::plotHover(QMouseEvent *ev)
{
    int x = ui->plot->xAxis->pixelToCoord(ev->pos().x());
    int y = ui->plot->yAxis->pixelToCoord(ev->pos().y());
    setToolTip(QString("%1 , %2").arg(x).arg(y));
}

void MainWindow::plotClicked(QMouseEvent *ev)
{
    int x = ui->plot->xAxis->pixelToCoord(ev->pos().x());

    QListWidgetItem *item = ui->output->item(x);

    if(item == 0)
        return;

    item->setSelected(true);
    ui->output->scrollToItem(item);
}

void MainWindow::createSaveFeedbackMessageBox(bool success, QString itemTitle)
{
    if(!success)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("The " + itemTitle + " could not be saved.");
        msgBox.setWindowIcon(windowIcon());
        msgBox.exec();
    }
    else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Information");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("The " + itemTitle + " has been saved.");
        msgBox.setWindowIcon(windowIcon());

        QPushButton *showButton = msgBox.addButton(tr("Show in Folder"), QMessageBox::ActionRole);
        QPushButton *closeButton = msgBox.addButton(tr("Close"), QMessageBox::ActionRole);

        msgBox.setDefaultButton(closeButton);
        msgBox.exec();

        if (msgBox.clickedButton() == showButton)
        {
            QString path = QDir::toNativeSeparators(QDir::currentPath());
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }

        msgBox.accept();
    }
}

void MainWindow::saveImage()
{
    QString fileName = "graph_" + QDate::currentDate().toString("dd-MM-yyyy") + "_" + QTime::currentTime().toString("hh-mm-ss") + ".jpg";
    bool result = ui->plot->saveJpg(fileName, ui->plot->width(), ui->plot->height());
    createSaveFeedbackMessageBox(result, "graph");
}

void MainWindow::start()
{
    setPaused(false);
    enableEditing(false);

    graph->clearData();
    resetAllValues();

    ui->resetButton->setEnabled(true);

    timer->start(ui->timerIntervalSpinBox->value());
}

void MainWindow::togglePause()
{
    setPaused(timer->isActive());
}

void MainWindow::setPaused(bool paused)
{
    if(paused)
        timer->stop();
    else
        timer->start(ui->timerIntervalSpinBox->value());

    ui->togglePauseButton->setText(paused ? "Continue" : "Pause");
}

void MainWindow::enableEditing(bool enabled)
{
    ui->startMoney->setReadOnly(!enabled);
    ui->togglePauseButton->setEnabled(!enabled);
    ui->startButton->setEnabled(enabled);
    ui->timerIntervalSpinBox->setEnabled(enabled);
}

void MainWindow::reset()
{
    setPaused(true);
    enableEditing(true);

    graph->clearData();
    ui->plot->replot();

    ui->resetButton->setEnabled(false);

    resetAllValues();
}

void MainWindow::play()
{
    itemWidget = new OutputItemWidget();
    roundCount++;

    if (currentMoney > currentBet)
        playRound();
    else
        gameOver();

    QListWidgetItem *item = new QListWidgetItem();
    ui->output->addItem(item);
    ui->output->setItemWidget(item, itemWidget);
    item->setSizeHint(QSize(0, itemWidget->height()));
    ui->output->scrollToBottom();

    updateUI();
}

void MainWindow::playRound()
{
    currentMoney -= currentBet;
    int number = rand() % 37;

    if(number % 2 == 0 and number != 0)
    {
        itemWidget->setItemData(roundCount, number, true);
        currentWinningStreak++;
        currentLosingStreak = 0;
        if(currentWinningStreak > highestWinningStreak)
        {
            highestWinningStreak = currentWinningStreak;
            highestWinRound = roundCount;
        }
        currentMoney += currentBet * 2;
        currentBet = 1;

        if(currentMoney > maxMoney)
        {
            maxMoney = currentMoney;
            ui->plot->yAxis->setRange(0, maxMoney);
        }
    }
    else
    {
        itemWidget->setItemData(roundCount, number, false);
        currentLosingStreak++;
        currentWinningStreak = 0;
        if(currentLosingStreak > highestLosingStreak)
        {
            highestLosingStreak = currentLosingStreak;
            highestLoss = (currentBet * 2) - 1;
            highestLossRound = roundCount;
        }
        currentBet *= 2;
    }

    if(currentMoney > ui->startMoney->value())
        currentProfit = (currentMoney - ui->startMoney->value());
    else
        currentProfit = 0;

    graph->addData(roundCount, currentMoney);
    ui->plot->xAxis->setRange(0, roundCount + 1);
    ui->plot->replot();
}

void MainWindow::gameOver()
{
    itemWidget->setItemData(roundCount, -1, false);

    setPaused(true);
    enableEditing(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::updateUI()
{
    ui->moneyDisplay->setValue(currentMoney);
    ui->profitDisplay->setText("<font color=green>" + QString::number(currentProfit) + "</font> € profit");
    ui->betDisplay->setValue(currentBet);
    ui->loseStreakDisplay->setText(QString::number(highestLosingStreak) + "( " + QString::number(highestLoss) + " €) in round " + QString::number(highestLossRound));
    ui->winStreakDisplay->setText(QString::number(highestWinningStreak) + "( " + QString::number(highestWinningStreak) + " €) in round " + QString::number(highestWinRound));
    ui->roundCountDisplay->setValue(roundCount);
    ui->maxMoneyDisplay->setValue(maxMoney);
}
