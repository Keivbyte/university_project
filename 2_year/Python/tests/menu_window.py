# menu_window.py
from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QTabWidget, QApplication
)
from PyQt6.QtCore import Qt

from control_1 import Control1Widget
from control_2 import Control2Widget
from control_3 import Control3Widget


class MenuWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.layout = QVBoxLayout()

        # Убрана группа с комбобоксом

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

        # Устанавливаем layout
        self.setLayout(self.layout)