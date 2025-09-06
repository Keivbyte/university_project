# main.py
import sys
from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6.QtGui import QIcon
from menu_window import MenuWindow


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Python Tests - Brogli K.")
        self.setWindowIcon(QIcon("img/institute_logo.jpg"))
        self.setGeometry(100, 100, 800, 600)

        self.central_widget = MenuWindow()
        self.setCentralWidget(self.central_widget)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())