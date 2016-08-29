#include "outputitemwidget.h"
#include "ui_outputitem.h"

OutputItemWidget::OutputItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputItem)
{
    ui->setupUi(this);
}

OutputItemWidget::~OutputItemWidget()
{
    delete ui;
}

void OutputItemWidget::setItemData(int roundNumber, int rolledNumber, bool won)
{
    QString color = (won ? "green" : "red");
    QString winText = (won ? "won" : "lost");
    QString rightText = (rolledNumber >= 0
                        ? QString::number(rolledNumber) + ", " + winText
                        : "Game Over!");

    ui->label->setText(QString::number(roundNumber) + "     <font color='" + color + "'> " + rightText + "</font>");
}

QString OutputItemWidget::getItemData()
{    
    QString text = ui->label->text();

    QString result = text.split('<')[0].trimmed();
    result += "\t" + text.split('>')[1];
    result.remove(result.length() - 6, 6);

    QStringList l = result.split(',');
    if(l.size() > 1)
        result = l[0] + "\t" + l[1];

    return result;
}
