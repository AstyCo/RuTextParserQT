#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPlainTextEdit>
#include <QtCharts>

#include <QDebug>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    installStatusBar();
    installMdiArea();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::installStatusBar()
{
    QStatusBar *bar = new QStatusBar();

    setStatusBar(bar);

    statusBar()->showMessage("status bar");
}

#include <QTabWidget>

void MainWindow::installMdiArea()
{
    ui->mdiArea->setTabShape(QTabWidget::Triangular);

    QChart *chart = new QChart();
    QBarSet *set0 = new QBarSet("Jane");
    QBarSet *set1 = new QBarSet("John");
    QBarSet *set2 = new QBarSet("Axel");
    QBarSet *set3 = new QBarSet("Mary");
    QBarSet *set4 = new QBarSet("Samantha");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;
    *set2 << 3 << 5 << 8 << 13 << 8 << 5;
    *set3 << 5 << 6 << 7 << 3 << 4 << 5;
    *set4 << 9 << 7 << 5 << 3 << 1 << 2;

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);
    series->append(set3);
    series->append(set4);

    chart->addSeries(series);
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QPlainTextEdit *output = new QPlainTextEdit(this);

    QIcon barChartIcon(":/rsc/icons/bars-chart.png");
    QIcon terminalIcon(":/rsc/icons/terminal.png");

    output->setWindowIcon(terminalIcon);
    chartView->setWindowIcon(barChartIcon);

    output->setLineWidth(1);
    output->setFrameStyle(QFrame::Panel | QFrame::Raised);

//    chartView->setFrameStyle(QFrame::NoFrame);
    chartView->setMidLineWidth(0);

    ui->mdiArea->addSubWindow(output);
    ui->mdiArea->addSubWindow(chartView);

    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    tabWidget->addTab(new QWidget(tabWidget), "1");
    ui->mdiArea->addSubWindow(tabWidget);

    ui->mdiArea->tileSubWindows();

}
