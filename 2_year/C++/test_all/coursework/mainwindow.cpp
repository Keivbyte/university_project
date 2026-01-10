#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include "KineticAnalyzer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::pair<std::vector<double>, std::vector<double>> readTableData(QTableWidget* table) {
    std::vector<double> times, concs;
    int rows = table->rowCount();
    for (int i = 0; i < rows; ++i) {
        auto timeItem = table->item(i, 0);
        auto concItem = table->item(i, 1);
        if (!timeItem || !concItem) continue;

        QString timeStr = timeItem->text().trimmed();
        QString concStr = concItem->text().trimmed();
        if (timeStr.isEmpty() || concStr.isEmpty()) continue;

        bool ok1, ok2;
        double t = timeStr.toDouble(&ok1);
        double c = concStr.toDouble(&ok2);
        if (ok1 && ok2 && t >= 0 && c >= 0) {
            times.push_back(t);
            concs.push_back(c);
        }
    }
    return {times, concs};
}

void MainWindow::on_btn_readData_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл с данными", "", "Текстовые файлы (*.txt *.csv);;Все файлы (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines << in.readLine();
    }
    file.close();

    // Пропустим пустые строки и комментарии
    std::vector<std::pair<double, double>> data;
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#')) continue;

        QStringList parts = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            bool ok1, ok2;
            double t = parts[0].toDouble(&ok1);
            double ca = parts[1].toDouble(&ok2);
            if (ok1 && ok2) {
                data.emplace_back(t, ca);
            }
        }
    }

    if (data.empty()) {
        QMessageBox::warning(this, "Ошибка", "Файл не содержит корректных данных.");
        return;
    }

    // Заполним таблицу
    ui->tableWidget_data->setRowCount(static_cast<int>(data.size()));
    ui->tableWidget_data->setColumnCount(2);
    ui->tableWidget_data->setHorizontalHeaderLabels({"Время (с)", "Концентрация A (моль/л)"});

    for (size_t i = 0; i < data.size(); ++i) {
        ui->tableWidget_data->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(data[i].first)));
        ui->tableWidget_data->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::number(data[i].second)));
    }
}

void MainWindow::on_btn_enterData_clicked()
{
    // Установим заголовки
    ui->tableWidget_data->setHorizontalHeaderLabels({"Время (с)", "Концентрация A (моль/л)"});

    // Запросим количество точек
    bool ok;
    int rows = QInputDialog::getInt(this, "Количество точек",
                                    "Введите число экспериментальных точек:", 5, 2, 100, 1, &ok);
    if (!ok) return;

    // Настроим таблицу
    ui->tableWidget_data->setRowCount(rows);
    ui->tableWidget_data->setColumnCount(2);

    // Очистим старые данные
    for (int i = 0; i < rows; ++i) {
        ui->tableWidget_data->setItem(i, 0, new QTableWidgetItem(""));
        ui->tableWidget_data->setItem(i, 1, new QTableWidgetItem(""));
    }

    // Сделаем ячейки редактируемыми (по умолчанию они и так редактируемые, но явно укажем)
    ui->tableWidget_data->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
}

void MainWindow::on_btn_graphic_clicked()
{
    try {
        auto [times, concsA] = readTableData(ui->tableWidget_data);
        if (times.empty()) {
            QMessageBox::warning(this, "Ошибка", "Нет данных в таблице!");
            return;
        }

        // === Читаем начальные концентрации из полей ввода ===
        bool ok;
        auto toDoubleSafe = [&](QLineEdit* edit) -> double {
            QString s = edit->text().trimmed();
            if (s.isEmpty()) return 0.0;
            double val = s.toDouble(&ok);
            return ok ? val : 0.0;
        };

        double cb0 = toDoubleSafe(ui->le_Cb);
        double cc0 = toDoubleSafe(ui->le_Cc);
        double cd0 = toDoubleSafe(ui->le_Cd);
        double ce0 = toDoubleSafe(ui->le_Ce);

        KineticAnalyzer analyzer(cb0, cc0, cd0, ce0);
        analyzer.setExperimentalData(times, concsA);
        analyzer.analyze();

        // === Вывод результатов ===
        QString msg = QString("Порядок реакции (n): %1\n"
                              "Константа скорости (k): %2\n"
                              "Дисперсия: %3\n"
                              "Коэффициент корреляции (r): %4")
                          .arg(analyzer.getOrder(), 0, 'g', 4)
                          .arg(analyzer.getRateConstant(), 0, 'g', 4)
                          .arg(analyzer.getDispersion(), 0, 'g', 6)
                          .arg(analyzer.getCorrelation(), 0, 'g', 4);
        QMessageBox::information(this, "Результаты анализа", msg);

        // === Построение графика ===
        chrt = new QChart();
        chrt->setTitle("Experimental points and approximation curve");
        chrt->legend()->setVisible(true);
        chrt->legend()->setAlignment(Qt::AlignBottom);

        // Экспериментальные точки A
        QScatterSeries *seriesExp = new QScatterSeries();
        seriesExp->setName("A (эксперимент)");
        seriesExp->setMarkerSize(8);
        for (size_t i = 0; i < times.size(); ++i) {
            seriesExp->append(times[i], concsA[i]);
        }
        chrt->addSeries(seriesExp);

        // Модель A
        QLineSeries *seriesModel = new QLineSeries();
        seriesModel->setName("A (модель)");
        double ca = concsA[0];
        seriesModel->append(times[0], ca);
        for (size_t i = 1; i < times.size(); ++i) {
            double dt = times[i] - times[i - 1];
            double dca = analyzer.getRateConstant() * std::pow(ca, analyzer.getOrder()) * dt;
            ca = std::max(0.0, ca - dca);
            seriesModel->append(times[i], ca);
        }
        chrt->addSeries(seriesModel);

        // Моделирование всех компонентов
        auto sim = analyzer.simulateConcentrations();

        QLineSeries *seriesB = new QLineSeries(); seriesB->setName("B");
        QLineSeries *seriesC = new QLineSeries(); seriesC->setName("C");
        QLineSeries *seriesD = new QLineSeries(); seriesD->setName("D");
        QLineSeries *seriesE = new QLineSeries(); seriesE->setName("E");

        for (size_t i = 0; i < times.size(); ++i) {
            double t = times[i];
            size_t base = i * 5;
            seriesB->append(t, sim[base + 1]);
            seriesC->append(t, sim[base + 2]);
            seriesD->append(t, sim[base + 3]);
            seriesE->append(t, sim[base + 4]);
        }

        chrt->addSeries(seriesB);
        chrt->addSeries(seriesC);
        chrt->addSeries(seriesD);
        chrt->addSeries(seriesE);

        chrt->createDefaultAxes();
        chrt->axisX()->setTitleText("Время (с)");
        chrt->axisY()->setTitleText("Концентрация (моль/л)");

        // Отображение
        QChartView *chartView = new QChartView(chrt);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setParent(ui->w_charts);
        chartView->setGeometry(ui->w_charts->rect());
        chartView->show();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", QString("Ошибка анализа:\n%1").arg(e.what()));
    }
}
