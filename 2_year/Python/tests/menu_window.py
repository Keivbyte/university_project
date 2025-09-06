# menu_window.py
from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QComboBox, QGroupBox, QTabWidget, QApplication
)
from PyQt6.QtCore import Qt, QTimer  # ← Добавлен QTimer

from control_1 import Control1Widget
from control_2 import Control2Widget
from control_3 import Control3Widget


class MenuWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.layout = QVBoxLayout()

        # Выбор контрольной
        group = QGroupBox("Select a test")
        combo_layout = QVBoxLayout()
        self.combo = QComboBox()
        self.combo.addItems(["Control #1", "Control #2", "Control #3"])
        combo_layout.addWidget(self.combo)
        group.setLayout(combo_layout)
        self.layout.addWidget(group)

        # Единый QTabWidget
        self.tabs = QTabWidget()

        # === Создаём все виджеты один раз ===
        self.ctrl1_widget = Control1Widget()
        self.ctrl2_widget = Control2Widget()
        self.ctrl3_widget = Control3Widget()

        # Добавляем вкладки (один раз!)
        self.tabs.addTab(self.ctrl1_widget, "Control #1")
        self.tabs.addTab(self.ctrl2_widget, "Control #2")
        self.tabs.addTab(self.ctrl3_widget, "Control #3")

        self.layout.addWidget(self.tabs)

        # === Переключение вкладок через комбобокс ===
        self.combo.currentIndexChanged.connect(self.switch_tab)

        # Устанавливаем layout
        self.setLayout(self.layout)

        # Откладываем первоначальное переключение, чтобы интерфейс успел построиться
        QTimer.singleShot(0, self.switch_tab)

    def switch_tab(self):
        index = self.combo.currentIndex()
        self.tabs.setCurrentIndex(index)