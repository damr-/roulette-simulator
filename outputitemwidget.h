#ifndef OUTPUTITEM_H
#define OUTPUTITEM_H

#include <QWidget>

namespace Ui {
class OutputItem;
}

class OutputItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputItemWidget(QWidget *parent = 0);
    ~OutputItemWidget();
    void setItemData(int roundNumber, int rolledNumber, bool won);
    QString getItemData();

private:
    Ui::OutputItem *ui;
};

#endif // OUTPUTITEM_H
