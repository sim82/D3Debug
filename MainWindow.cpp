#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DebugConnection.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    debugConnection = new DebugConnection(this);
    QMetaObject::connectSlotsByName(this);

    QObject::connect(debugConnection, SIGNAL(scriptGetReceived(int, QVector<QString>)), this, SLOT(on_debugConection_scriptGetReceived(int, QVector<QString>)));

    debugConnection->test();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_debugConection_scriptGetReceived(int id, QVector<QString> lines)
{
    for( auto const& line: lines )
    {
        ui->textBrowser->append(line);
    }
}
