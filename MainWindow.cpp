#include "MainWindow.h"
#include "DebugConnection.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    debugConnection = new DebugConnection(this);
    debugConnection->setObjectName("debugConnection");


    ui->setupUi(this);

    auto *contextMenu = ui->textBrowser->createStandardContextMenu();
    contextMenu->addAction(ui->actionAddWatchpoint);
    ui->mainToolBar->addAction(ui->actionAddWatchpoint);
    ui->textBrowser->addAction(ui->actionAddWatchpoint);
    ui->immediateInput->addAction(ui->actionHistoryBack);
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
    (void)token;
    ui->textBrowser->clear();
    for (auto const &line : lines)
    {
        ui->textBrowser->append(line);
    }
}

void MainWindow::on_debugConnection_scriptInfoReply(int64_t token, QVector<ScriptInfo> infos)
{
    (void)token;
    ui->listWidget->clear();
    for (auto const &info : infos)
    {
        ui->listWidget->insertItem(info.id, info.name);
    }
}

void MainWindow::on_debugConnection_executeReply(int64_t token, QString consoleOutput, bool bError)
{
    (void)token;
    if (bError)
    {
        ui->immediateOutput->appendPlainText("remote error:\n");
    }
    ui->immediateOutput->appendPlainText(consoleOutput);
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    debugConnection->scriptGet(index.row());
}

void MainWindow::on_actionAddWatchpoint_triggered()
{

}

void MainWindow::on_textBrowser_customContextMenuRequested(const QPoint &pos)
{
    auto cursor = ui->textBrowser->cursorForPosition(pos);
    contextMenuLineNumber_ = cursor.blockNumber();

    auto *menu = ui->textBrowser->createStandardContextMenu();
    menu->addAction(ui->actionAddWatchpoint);
    menu->exec(pos);
    delete menu;
}

void MainWindow::on_immediateInput_returnPressed()
{
    auto const &text = ui->immediateInput->text();
    historyPos = 0;

    if (text.isEmpty())
    {
        return;
    }

    historyStack_.push(text);
    debugConnection->execute(text);
    ui->immediateInput->clear();
}

void MainWindow::on_actionHistoryBack_triggered(bool checked)
{
    if (historyPos < historyStack_.size())
    {
        auto const& text = *(historyStack_.rbegin() + historyPos);
        ui->immediateInput->setText(text);
        ++historyPos;
    }
}
