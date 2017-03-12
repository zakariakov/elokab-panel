#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H
#include "statuslabel.h"
#include "utils/desktopfile.h"
#include <QWidget>
#include <QSettings>
#include <QLayout>
class StatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StatusWidget(QWidget *parent = 0);

signals:

public slots:
 void loadSettings();
private:
    QList <StatusLabel *>listStatus;
    QHBoxLayout *mHorizontalLayout ;
private slots:

};

#endif // STATUSWIDGET_H
