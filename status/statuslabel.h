#ifndef STATUSLABEL_H
#define STATUSLABEL_H

#include <QLabel>
#include <QTimer>
#include <QProcess>
#include <QFutureWatcher>

class StatusLabel : public QLabel
{
    Q_OBJECT
signals:
    //-----------------------
    void readFiniched();
    void textReady(QString str);

protected slots:
    void on_textReady(QString str);
    void on_render_finished();

public slots:

    void startRender();
    void cancelRender();

    //__________________________________________
public:
    StatusLabel(QColor bgColor, QColor fgColor,QWidget *parent ,const QString text);
    StatusLabel(QColor nextColor, QColor prevColor, QWidget *parent , int style=0);

    void setCommand(QString command, int interval);
    void setLabel(QString label){mLabel=label;}
    void setName(QString name){mName=name;}
private:
    QString mCommand;
    QString mLabel;
    QString mName;
    int mInterval;
    QTimer *mTimer;

    //________________________________
    QFutureWatcher< void >* m_render;
    QString m_string;
    void render();

private slots:
    QString updateCommand();



};

#endif // STATUSLABEL_H
