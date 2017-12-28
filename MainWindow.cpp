#include "MainWindow.h"
#include "DebugConnection.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    debugConnection = new DebugConnection(this);
    debugConnection->setObjectName("debugConnection");
    QMetaObject::connectSlotsByName(this);

    //    QObject::connect(debugConnection, SIGNAL(scriptInfoReply(int64_t, QVector<DebugConnection::ScriptInfo>)),
    //    this, SLOT(on_debugConnection_scriptInfoReply(int64_t, QVector<DebugConnection::ScriptInfo>)));
    //    QObject::connect(debugConnection, SIGNAL(scriptGetReceived(int, QVector<QString>)), this,
    //    SLOT(on_debugConection_scriptGetReceived(int, QVector<QString>)));

    //    debugConnection->test();
    auto *contextMenu = ui->textBrowser->createStandardContextMenu();
    contextMenu->addAction(ui->actionAddWatchpoint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_debugConnection_connected()
{
    debugConnection->scriptInfo();
}

void MainWindow::on_debugConnection_scriptGetReply(int64_t token, QVector<QString> lines)
{
    ui->textBrowser->clear();
    for (auto const &line : lines)
    {
        ui->textBrowser->append(line);
    }
}

void MainWindow::on_debugConnection_scriptInfoReply(int64_t token, QVector<ScriptInfo> infos)
{
    ui->listWidget->clear();
    for (auto const &info : infos)
    {
        ui->listWidget->insertItem(info.id, info.name);
    }
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    debugConnection->scriptGet(index.row());
}

void MainWindow::on_actionAddWatchpoint_triggered()
{

}
