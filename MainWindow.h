#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <DebugConnection.h>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_debugConnection_connected();
    void on_debugConnection_scriptGetReply(int64_t token, QVector<QString> lines);
    void on_debugConnection_scriptInfoReply(int64_t token, QVector<ScriptInfo> infos);
    void on_listWidget_clicked(const QModelIndex &index);

    void on_actionAddWatchpoint_triggered();

private:
    Ui::MainWindow *ui;
    DebugConnection *debugConnection;
};

#endif // MAINWINDOW_H
