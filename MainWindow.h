#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class DebugConnection;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_debugConection_scriptGetReceived(int id, QVector<QString> lines);

private:
    Ui::MainWindow *ui;
    DebugConnection *debugConnection;
};

#endif // MAINWINDOW_H
