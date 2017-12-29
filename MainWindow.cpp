#include "MainWindow.h"
#include "DebugConnection.h"
#include "ui_MainWindow.h"
#include <QMessageBox>


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

    ui->textBrowser->document()->setDefaultFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

MainWindow::~MainWindow()
{
    delete ui;
}


////////////////////////////////////////////////////////////////////////
// debugConnection slots
////////////////////////////////////////////////////////////////////////

void MainWindow::on_debugConnection_connected()
{
    debugConnection->scriptInfo();
}

void MainWindow::on_debugConnection_scriptGetReply(int64_t token, QVector<QString> lines)
{
    (void)token;
    ui->textBrowser->clear();

    int width = 0;

    int numLines = lines.size();
    while( numLines != 0 )
    {
        ++width;
        numLines /= 10;
    }

    int i = 1;
    for ( int i = 0; i < lines.size(); ++i )
    {
        ui->textBrowser->append(QString("%1 ").arg(i + 1, width ) + lines[i]);
    }
}

void MainWindow::on_debugConnection_scriptInfoReply(int64_t token, QVector<ScriptInfo> infos)
{
    (void)token;
    ui->scriptList->clear();
    for (auto const &info : infos)
    {
        ui->scriptList->insertItem(info.id, info.name);
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

void MainWindow::on_debugConnection_eventWatchpoint(int watchpointId, int scriptId, int lineNumbers,
                                                    QVector<QString> localNames, QVector<QVariant> localValues)
{
    if (localNames.size() != localValues.size())
    {
        QMessageBox::warning(this, "debug data error", "watchpoint name / value mismatch");
    }

    ui->watchpointValueTable->setRowCount(localNames.size());
    for (int i = 0; i < localNames.size(); ++i )
    {
        ui->watchpointValueTable->setItem(i, 0, new QTableWidgetItem(localNames[i]));
        ui->watchpointValueTable->setItem(i, 1, new QTableWidgetItem(localValues[i].toString()));
    }
}

void MainWindow::on_debugConnection_addWatchpointReply(int64_t token, int watchpointId)
{
    (void)watchpointId;

    auto it = pendingWatchpoints_.find(token);

    if (it == pendingWatchpoints_.end())
    {
        QMessageBox::warning(this, "Bad watchpoint reply", QString("received reply for non existing watchpoint (token=%1)").arg(token));
        return;
    }

    int scriptId;
    int line;
    std::tie(scriptId,line) = it.value();

    QTreeWidgetItem *treeWidgetItem = nullptr;
    {
        auto it = scriptToWatchpointItem_.find(scriptId);
        if (it == scriptToWatchpointItem_.end())
        {
            treeWidgetItem = new QTreeWidgetItem((QTreeWidget*)nullptr, QStringList(QString(ui->scriptList->item(scriptId)->text())));
            scriptToWatchpointItem_.insert(scriptId, treeWidgetItem);
        }
        else
        {
            treeWidgetItem = it.value();
        }
    }
    ui->watchpointTree->insertTopLevelItem(0, treeWidgetItem);

}

////////////////////////////////////////////////////////////////////////
// other slots
////////////////////////////////////////////////////////////////////////


void MainWindow::on_scriptList_clicked(const QModelIndex &index)
{
    debugConnection->scriptGet(index.row());
}


void MainWindow::on_textBrowser_customContextMenuRequested(const QPoint &pos)
{
    auto cursor            = ui->textBrowser->cursorForPosition(pos);
    contextMenuLineNumber_ = cursor.blockNumber() + 1;

    auto *menu = ui->textBrowser->createStandardContextMenu();
    menu->addAction(ui->actionAddWatchpoint);
    menu->exec(pos);
    delete menu;
}

void MainWindow::on_immediateInput_returnPressed()
{
    auto const &text = ui->immediateInput->text();
    historyPos       = 0;

    if (text.isEmpty())
    {
        return;
    }

    historyStack_.push(text);
    debugConnection->execute(text);
    ui->immediateInput->clear();
}

////////////////////////////////////////////////////////////////////////
// action callbacks
////////////////////////////////////////////////////////////////////////
void MainWindow::on_actionAddWatchpoint_triggered()
{
    auto selected = ui->scriptList->selectionModel()->selectedIndexes();

    if (selected.empty())
    {
        return;
    }

    auto selIndex = selected.front().row();

    QMessageBox::information( this, "add watchpoint", QString("adding watchpoint after line %1").arg(contextMenuLineNumber_));

    auto token = debugConnection->addWatchpoint(selIndex, contextMenuLineNumber_ + 1);
    pendingWatchpoints_.insert(token, {selIndex, contextMenuLineNumber_});

}


void MainWindow::on_actionHistoryBack_triggered(bool checked)
{
    if (historyPos < historyStack_.size())
    {
        auto const &text = *(historyStack_.rbegin() + historyPos);
        ui->immediateInput->setText(text);
        ++historyPos;
    }
}
