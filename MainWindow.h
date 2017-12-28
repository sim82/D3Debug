#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DebugConnectionTypes.h>
#include <QMainWindow>
#include <QStack>
#include <QVariant>

namespace Ui
{
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
    void on_debugConnection_executeReply(int64_t token, QString consoleOutput, bool bError);
    void on_debugConnection_eventWatchpoint(int watchpointId, int scriptId, int lineNumbers,
                                            QVector<QString> localNames, QVector<QVariant> localValues);

    void on_listWidget_clicked(const QModelIndex &index);

    void on_actionAddWatchpoint_triggered();

    void on_textBrowser_customContextMenuRequested(const QPoint &pos);

    void on_immediateInput_returnPressed();

    void on_actionHistoryBack_triggered(bool checked);

private:
    Ui::MainWindow *ui;
    DebugConnection *debugConnection;

    int contextMenuLineNumber_{0};

    QStack<QString> historyStack_;
    int historyPos{0};
};

#endif // MAINWINDOW_H
