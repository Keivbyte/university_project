# control_2.py
from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QFormLayout,
    QLabel, QLineEdit, QPushButton, QTabWidget, QGroupBox,
    QTableWidget, QTableWidgetItem, QHeaderView
)
from PyQt6.QtCore import Qt, QTimer
from PyQt6.QtGui import QPixmap
import math


class Control2Widget(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()

        # === Вкладки ===
        self.tabs = QTabWidget()
        self.tabs.addTab(self.setup_tab1(), "Task 1")
        self.tabs.addTab(self.setup_tab2(), "Task 2")
        self.tabs.addTab(self.create_placeholder("Task 3"), "Task 3")

        layout.addWidget(self.tabs)
        self.setLayout(layout)

    def create_placeholder(self, text):
        widget = QWidget()
        layout = QVBoxLayout()
        label = QLabel(text + " — coming soon")
        label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        label.setStyleSheet("color: gray; font-size: 14px;")
        layout.addWidget(label)
        widget.setLayout(layout)
        return widget

    def setup_tab1(self):
        widget = QWidget()
        layout = QVBoxLayout()

        title = QLabel("Вычисление функции по шагам")
        title.setStyleSheet("font-size: 16px; font-weight: bold;")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(title)

        image_label = QLabel()
        pixmap = QPixmap("img/ex2_1.PNG")
        if pixmap.isNull():
            fallback = QLabel("Изображение ex2_1.PNG не найдено")
            fallback.setStyleSheet("color: red; font-style: italic;")
            fallback.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(fallback)
        else:
            scaled = pixmap.scaled(500, 120, Qt.AspectRatioMode.KeepAspectRatio)
            image_label.setPixmap(scaled)
            image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(image_label)

        input_group = QGroupBox("Параметры интервала")
        form_layout = QFormLayout()
        self.a_input = QLineEdit("-2.0")
        self.b_input = QLineEdit("2.0")
        self.h_input = QLineEdit("0.5")
        self.delay_input = QLineEdit("300")

        form_layout.addRow("a:", self.a_input)
        form_layout.addRow("b:", self.b_input)
        form_layout.addRow("h:", self.h_input)
        form_layout.addRow("Задержка (мс):", self.delay_input)
        input_group.setLayout(form_layout)
        layout.addWidget(input_group)

        btn_layout = QHBoxLayout()
        self.start_btn = QPushButton("Запустить")
        self.reset_btn = QPushButton("Сбросить")

        #self.start_btn.setStyleSheet("padding: 8px; background-color: #4CAF50; color: white; font-weight: bold;")
        #self.reset_btn.setStyleSheet("padding: 8px; background-color: #f44336; color: white;")

        btn_layout.addWidget(self.start_btn)
        btn_layout.addWidget(self.reset_btn)
        layout.addLayout(btn_layout)

        self.table = QTableWidget()
        self.table.setColumnCount(4)
        self.table.setHorizontalHeaderLabels(["x", "Условие", "Формула", "y"])
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        self.table.setEditTriggers(QTableWidget.EditTrigger.NoEditTriggers)
        layout.addWidget(self.table)

        self.status_label = QLabel("Готов к вычислениям")
        self.status_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.status_label.setStyleSheet("color: gray; font-style: italic;")
        layout.addWidget(self.status_label)

        # Таймер
        self.timer = QTimer()
        self.timer.timeout.connect(self.perform_next_step)

        self.start_btn.clicked.connect(self.start_calculation)
        self.reset_btn.clicked.connect(self.reset_all)

        widget.setLayout(layout)
        return widget

    def setup_tab2(self):
        widget = QWidget()
        layout = QVBoxLayout()

        title = QLabel("Вычисление функции по шагам")
        title.setStyleSheet("font-size: 16px; font-weight: bold;")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(title)

        # Изображение (если есть)
        image_label = QLabel()
        pixmap = QPixmap("img/ex2_2.PNG")  # Убедись, что файл существует
        if pixmap.isNull():
            fallback = QLabel("Изображение ex2_2.PNG не найдено")
            fallback.setStyleSheet("color: red; font-style: italic;")
            fallback.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(fallback)
        else:
            scaled = pixmap.scaled(500, 120, Qt.AspectRatioMode.KeepAspectRatio)
            image_label.setPixmap(scaled)
            image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(image_label)

        # Ввод параметров
        input_group = QGroupBox("Параметры интервала")
        form_layout = QFormLayout()
        self.a_input2 = QLineEdit("-2.0")
        self.b_input2 = QLineEdit("3.0")
        self.h_input2 = QLineEdit("0.5")
        self.delay_input2 = QLineEdit("300")

        form_layout.addRow("a:", self.a_input2)
        form_layout.addRow("b:", self.b_input2)
        form_layout.addRow("h:", self.h_input2)
        form_layout.addRow("Задержка (мс):", self.delay_input2)
        input_group.setLayout(form_layout)
        layout.addWidget(input_group)

        # Кнопки
        btn_layout = QHBoxLayout()
        self.start_btn2 = QPushButton("Запустить")
        self.reset_btn2 = QPushButton("Сбросить")

        btn_layout.addWidget(self.start_btn2)
        btn_layout.addWidget(self.reset_btn2)
        layout.addLayout(btn_layout)

        # Таблица
        self.table2 = QTableWidget()
        self.table2.setColumnCount(4)
        self.table2.setHorizontalHeaderLabels(["x", "Условие", "Формула", "y"])
        self.table2.horizontalHeader().setSectionResizeMode(QHeaderView.ResizeMode.Stretch)
        self.table2.setEditTriggers(QTableWidget.EditTrigger.NoEditTriggers)
        layout.addWidget(self.table2)

        # Статус
        self.status_label2 = QLabel("Готов к вычислениям")
        self.status_label2.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.status_label2.setStyleSheet("color: gray; font-style: italic;")
        layout.addWidget(self.status_label2)

        # Таймер
        self.timer2 = QTimer()
        self.timer2.timeout.connect(self.perform_next_step2)

        self.start_btn2.clicked.connect(self.start_calculation2)
        self.reset_btn2.clicked.connect(self.reset_all2)

        widget.setLayout(layout)
        return widget


    def start_calculation(self):
        try:
            a = float(self.a_input.text().strip())
            b = float(self.b_input.text().strip())
            h = float(self.h_input.text().strip())
            delay = max(50, int(self.delay_input.text().strip()))  # минимум 50 мс

            if h <= 0:
                self.status_label.setText("Ошибка: h > 0")
                return
            if a > b:
                self.status_label.setText("Ошибка: a ≤ b")
                return

            self.x_values = []
            x = a
            while x <= b + 1e-6:
                self.x_values.append(round(x, 6))
                x += h

            self.current_index = 0
            self.table.setRowCount(0)
            self.status_label.setText(f"Вычисляем {len(self.x_values)} точек...")

            self.timer.start(delay)
            self.start_btn.setEnabled(False)

        except Exception as e:
            self.status_label.setText(f"Ошибка: {e}")

    def start_calculation2(self):
        try:
            a = float(self.a_input2.text().strip())
            b = float(self.b_input2.text().strip())
            h = float(self.h_input2.text().strip())
            delay = max(50, int(self.delay_input2.text().strip()))

            if h <= 0:
                self.status_label2.setText("Ошибка: h > 0")
                return
            if a > b:
                self.status_label2.setText("Ошибка: a ≤ b")
                return

            self.x_values2 = []
            x = a
            while x <= b + 1e-6:  # Цикл с предусловием
                self.x_values2.append(round(x, 6))
                x += h

            self.current_index2 = 0
            self.table2.setRowCount(0)
            self.status_label2.setText(f"Вычисляем {len(self.x_values2)} точек...")

            self.timer2.start(delay)
            self.start_btn2.setEnabled(False)

        except Exception as e:
            self.status_label2.setText(f"Ошибка: {e}")

    def perform_next_step(self):
        # Проверка: вдруг виджет уже уничтожен
        try:
            if self.current_index >= len(self.x_values):
                self.timer.stop()
                self.status_label.setText("Готово")
                self.start_btn.setEnabled(True)
                return

            x = self.x_values[self.current_index]

            if x < -0.5:
                y = 1 + math.sin(x + 0.5) ** 3
                condition = "x < -0.5"
                formula = "1 + sin³(x+0.5)"
                color = "color: #0066cc;"
            elif -0.5 <= x <= 0.5:
                y = 1
                condition = "-0.5 ≤ x ≤ 0.5"
                formula = "1"
                color = "color: green;"
            else:
                y = 1 + math.sin(x - 0.5) ** 3
                condition = "x > 0.5"
                formula = "1 + sin³(x-0.5)"
                color = "color: #cc6600;"

            # Добавляем строку в таблицу
            row_pos = self.table.rowCount()
            self.table.insertRow(row_pos)

            self.table.setItem(row_pos, 0, QTableWidgetItem(f"{x:.3f}"))
            self.table.setItem(row_pos, 1, QTableWidgetItem(condition))
            self.table.setItem(row_pos, 2, QTableWidgetItem(formula))

            # Установка значения y с цветом
            y_item = QTableWidgetItem(f"{y:.6f}")
            y_item.setTextAlignment(Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter)

            if x < -0.5:
                y_item.setForeground(Qt.GlobalColor.blue)
            elif -0.5 <= x <= 0.5:
                y_item.setForeground(Qt.GlobalColor.green)
            else:
                y_item.setForeground(Qt.GlobalColor.red)

            self.table.setItem(row_pos, 3, y_item)

            self.table.scrollToBottom()
            self.status_label.setText(f"x = {x:.3f} → y = {y:.6f}")
            self.current_index += 1

        except RuntimeError:
            # Qt object уже уничтожен — тихо выходим
            self.timer.stop()
        except Exception as e:
            self.status_label.setText(f"Ошибка: {e}")
            self.timer.stop()

    def perform_next_step2(self):
        try:
            if self.current_index2 >= len(self.x_values2):
                self.timer2.stop()
                self.status_label2.setText("Готово")
                self.start_btn2.setEnabled(True)
                return

            x = self.x_values2[self.current_index2]

            if x <= 0:
                y = x + math.exp(-x)
                condition = "x ≤ 0"
                formula = "x + e⁻ˣ"
                color = "color: #0066cc;"
            elif x < 1:
                y = 1
                condition = "0 < x < 1"
                formula = "1"
                color = "color: green;"
            else:
                y = 1 + (x - 1) ** 2
                condition = "x ≥ 1"
                formula = "1 + (x−1)²"
                color = "color: #cc6600;"

            # Добавляем строку
            row_pos = self.table2.rowCount()
            self.table2.insertRow(row_pos)

            self.table2.setItem(row_pos, 0, QTableWidgetItem(f"{x:.3f}"))
            self.table2.setItem(row_pos, 1, QTableWidgetItem(condition))
            self.table2.setItem(row_pos, 2, QTableWidgetItem(formula))

            # Значение y с цветом
            y_item = QTableWidgetItem(f"{y:.6f}")
            y_item.setTextAlignment(Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter)

            if x <= 0:
                y_item.setForeground(Qt.GlobalColor.blue)
            elif x < 1:
                y_item.setForeground(Qt.GlobalColor.green)
            else:
                y_item.setForeground(Qt.GlobalColor.red)

            self.table2.setItem(row_pos, 3, y_item)

            self.table2.scrollToBottom()
            self.status_label2.setText(f"x = {x:.3f} → y = {y:.6f}")
            self.current_index2 += 1

        except RuntimeError:
            self.timer2.stop()
        except Exception as e:
            self.status_label2.setText(f"Ошибка: {e}")
            self.timer2.stop()

    def reset_all(self):
        self.timer.stop()
        self.table.setRowCount(0)
        self.status_label.setText("Готов")
        self.start_btn.setEnabled(True)

    def reset_all2(self):
        self.timer2.stop()
        self.table2.setRowCount(0)
        self.status_label2.setText("Готов")
        self.start_btn2.setEnabled(True)

    def hideEvent(self, event):
        self.timer.stop()
        self.status_label.setText("Готов")
        self.start_btn.setEnabled(True)
        super().hideEvent(event)