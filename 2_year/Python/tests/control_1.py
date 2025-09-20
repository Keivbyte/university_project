# control_1.py
from PyQt6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QLabel, QLineEdit,
    QPushButton, QFormLayout, QTabWidget, QGroupBox
)
from PyQt6.QtGui import QPixmap
from PyQt6.QtCore import Qt
import math


class Control1Widget(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()
        self.tabs = QTabWidget()

        tab1 = self.setup_tab1()
        tab2 = self.setup_tab2()
        tab3 = self.setup_tab3()

        self.tabs.addTab(tab1, "Task 1")
        self.tabs.addTab(tab2, "Task 2")
        self.tabs.addTab(tab3, "Task 3")

        layout.addWidget(self.tabs)
        self.setLayout(layout)

    def setup_tab1(self):
        widget = QWidget()
        layout = QVBoxLayout()

        # === Heading with formula ===
        title = QLabel("Evaluating expression:")
        title.setStyleSheet("font-size: 16px; font-weight: bold;")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(title)

        # Formula as an image
        image_label = QLabel()
        pixmap = QPixmap("img/ex1_1.PNG")
        if pixmap.isNull():
            formula_text = QLabel("F = |x² - y²|^(1/5) / [x·√(a·y) + y·√(b·x)]")
            formula_text.setAlignment(Qt.AlignmentFlag.AlignCenter)
            formula_text.setStyleSheet("font: italic; color: gray;")
            layout.addWidget(formula_text)
        else:
            scaled = pixmap.scaled(600, 100, Qt.AspectRatioMode.KeepAspectRatio)
            image_label.setPixmap(scaled)
            image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(image_label)

        # === Input fields ===
        input_group = QGroupBox("Enter values")
        form_layout = QFormLayout()
        self.x1 = QLineEdit()
        self.y1 = QLineEdit()
        self.a1 = QLineEdit()
        self.b1 = QLineEdit()

        # Hints
        self.x1.setPlaceholderText("For example: 2.5")
        self.y1.setPlaceholderText("For example: 1.0")
        self.a1.setPlaceholderText("For example: 4.0")
        self.b1.setPlaceholderText("For example: 9.0")

        form_layout.addRow("x:", self.x1)
        form_layout.addRow("y:", self.y1)
        form_layout.addRow("a:", self.a1)
        form_layout.addRow("b:", self.b1)
        input_group.setLayout(form_layout)
        layout.addWidget(input_group)

        # === Buttons ===
        btn_layout = QHBoxLayout()
        calc_btn = QPushButton("Calculate")
        reset_btn = QPushButton("Reset")
        # Button style
        #calc_btn.setStyleSheet("padding: 8px; font-weight: bold; background-color: #4CAF50; color: white;")
        #reset_btn.setStyleSheet("padding: 8px; background-color: #f44336; color: white;")
        btn_layout.addWidget(calc_btn)
        btn_layout.addWidget(reset_btn)
        layout.addLayout(btn_layout)

        # === Intermediate and final results ===
        result_group = QGroupBox("Calculation results")
        result_layout = QVBoxLayout()

        self.step1_label = QLabel("1. |x² - y²| = ?")
        self.step2_label = QLabel("2. Numerator: |x² - y²|^(1/5) = ?")
        self.step3_label = QLabel("3. a·y = ?, b·x = ?")
        self.step4_label = QLabel("4. √(a·y) = ?, √(b·x) = ?")
        self.step5_label = QLabel("5. x·√(a·y) + y·√(b·x) = ?")
        self.res1 = QLabel("<b>Summary: F = ?</b>")

        for label in [self.step1_label, self.step2_label, self.step3_label,
                      self.step4_label, self.step5_label, self.res1]:
            label.setStyleSheet("padding: 4px;")
            result_layout.addWidget(label)

        result_group.setLayout(result_layout)
        layout.addWidget(result_group)

        # === Connecting buttons ===
        calc_btn.clicked.connect(self.calc1)
        reset_btn.clicked.connect(self.reset1)

        widget.setLayout(layout)
        return widget

    def calc1(self):
        try:
            x = float(self.x1.text().strip())
            y = float(self.y1.text().strip())
            a = float(self.a1.text().strip())
            b = float(self.b1.text().strip())

            # Step 1: |x² - y²|
            diff_sq = x ** 2 - y ** 2
            abs_diff_sq = abs(diff_sq)
            self.step1_label.setText(f"1. |x² - y²| = |{x:.3f}² - {y:.3f}²| = |{diff_sq:.6f}| = {abs_diff_sq:.6f}")

            # Step 2:
            numerator = abs_diff_sq ** (1 / 5)
            self.step2_label.setText(f"2. Numerator: |x² - y²|^(1/5) = {abs_diff_sq:.6f}^(0.2) = {numerator:.6f}")

            # Step 3: a*y и b*x
            ay = a * y
            bx = b * x
            self.step3_label.setText(f"3. a·y = {a:.3f}·{y:.3f} = {ay:.6f};  b·x = {b:.3f}·{x:.3f} = {bx:.6f}")

            # Checking radical expressions
            if ay < 0:
                self.res1.setText("<b style='color: red;'>Error: a y < 0 → √(a y) is not defined</b>")
                self.step4_label.setText("4. √(a y) is <span style='color: red;'>impossible</span>")
                self.step5_label.setText("5. Denominator is <span style='color: red;'>not evaluated</span>")
                return
            if bx < 0:
                self.res1.setText("<b style='color: red;'>Error: b x < 0 → √(b x) is not defined</b>")
                self.step4_label.setText("4. √(b x) is <span style='color: red;'>impossible</span>")
                self.step5_label.setText("5. Denominator is <span style='color: red;'>not calculated</span>")
                return

            # Шаг 4:
            sqrt_ay = math.sqrt(ay)
            sqrt_bx = math.sqrt(bx)
            self.step4_label.setText(f"4. √(a·y) = √{ay:.6f} = {sqrt_ay:.6f};  √(b·x) = √{bx:.6f} = {sqrt_bx:.6f}")

            # Шаг 5:
            denominator = x * sqrt_ay + y * sqrt_bx
            self.step5_label.setText(
                f"5. x·√(a·y) + y·√(b·x) = {x:.3f}·{sqrt_ay:.6f} + {y:.3f}·{sqrt_bx:.6f} = {denominator:.6f}")

            # Check for division by zero
            if abs(denominator) < 1e-12:
                self.res1.setText("<b style='color: red;'>Error: denominator ≈ 0 — division impossible</b>")
                return

            # Summary
            result = numerator / denominator
            self.res1.setText(
                f"<b style='color: green;'>Summary: F = {numerator:.6f} / {denominator:.6f} = {result:.6f}</b>")

        except ValueError as ve:
            self.res1.setText("<b style='color: red;'>Error: Please enter valid numeric values</b>")
            self.clear_steps_error()
        except Exception as e:
            self.res1.setText(f"<b style='color: red;'>Unknown error: {e}</b>")
            self.clear_steps_error()

    def clear_steps_error(self):
        self.step1_label.setText("1. |x² - y²| = ?")
        self.step2_label.setText("2. Numerator: |x² - y²|^(1/5) = ?")
        self.step3_label.setText("3. a·y = ?, b·x = ?")
        self.step4_label.setText("4. √(a·y) = ?, √(b·x) = ?")
        self.step5_label.setText("5. x·√(a·y) + y·√(b·x) = ?")

    def reset1(self):
        self.x1.clear()
        self.y1.clear()
        self.a1.clear()
        self.b1.clear()

        # Сброс надписей
        self.step1_label.setText("1. |x² - y²| = ?")
        self.step2_label.setText("2. Numerator: |x² - y²|^(1/5) = ?")
        self.step3_label.setText("3. a·y = ?, b·x = ?")
        self.step4_label.setText("4. √(a·y) = ?, √(b·x) = ?")
        self.step5_label.setText("5. x·√(a·y) + y·√(b·x) = ?")
        self.res1.setText("<b>Summary: F = ?</b>")

    def setup_tab2(self):
        widget = QWidget()
        layout = QVBoxLayout()

        # === Заголовок и формула ===
        title = QLabel("Вычисление выражения:")
        title.setStyleSheet("font-size: 16px; font-weight: bold;")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(title)

        image_label = QLabel()
        pixmap = QPixmap("img/ex1_2.PNG")
        if pixmap.isNull():
            formula_text = QLabel("F = [(x³ + a·x² + b·x + c) / (√|x−a| + √|y−b|)] · [a·cos(x/p) + b·sin(y/p)]")
            formula_text.setAlignment(Qt.AlignmentFlag.AlignCenter)
            formula_text.setStyleSheet("font: italic; color: gray;")
            layout.addWidget(formula_text)
        else:
            scaled = pixmap.scaled(600, 100, Qt.AspectRatioMode.KeepAspectRatio)
            image_label.setPixmap(scaled)
            image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(image_label)

        # === Поля ввода ===
        input_group = QGroupBox("Введите значения")
        form_layout = QFormLayout()




        # Создаём поля
        self.x2 = QLineEdit()
        self.a2 = QLineEdit()
        self.b2 = QLineEdit()
        self.c2 = QLineEdit()
        self.y2 = QLineEdit()
        self.p2 = QLineEdit()

        # Подсказки
        for field in [self.x2, self.a2, self.b2, self.c2, self.y2, self.p2]:
            field.setPlaceholderText("введите число")

        # Добавляем в форму
        fields = [(self.x2, "x"), (self.a2, "a"), (self.b2, "b"),
                  (self.c2, "c"), (self.y2, "y"), (self.p2, "p")]
        for field, name in fields:
            form_layout.addRow(f"{name}:", field)

        input_group.setLayout(form_layout)
        layout.addWidget(input_group)

        # === Кнопки ===
        btn_layout = QHBoxLayout()
        calc_btn = QPushButton("Рассчитать")
        reset_btn = QPushButton("Сбросить")
        #Button style
        #calc_btn.setStyleSheet("padding: 8px; font-weight: bold; background-color: #2196F3; color: white;")
        #reset_btn.setStyleSheet("padding: 8px; background-color: #9E9E9E; color: white;")
        btn_layout.addWidget(calc_btn)
        btn_layout.addWidget(reset_btn)
        layout.addLayout(btn_layout)

        # === Детализированные результаты ===
        result_group = QGroupBox("Пошаговые вычисления")
        result_layout = QVBoxLayout()

        self.step1_label = QLabel("1. Полином: x³ + a·x² + b·x + c = ?")
        self.step2_label = QLabel("2. |x − a| = ?, |y − b| = ?")
        self.step3_label = QLabel("3. √|x − a| = ?, √|y − b| = ?")
        self.step4_label = QLabel("4. Знаменатель: √|x−a| + √|y−b| = ?")
        self.step5_label = QLabel("5. Тригонометрическая часть: a·cos(x/p) + b·sin(y/p) = ?")
        self.res2 = QLabel("<b>Итог: F = ?</b>")
        self.res2.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.res2.setStyleSheet("font-size: 14px;")

        for label in [self.step1_label, self.step2_label, self.step3_label,
                      self.step4_label, self.step5_label, self.res2]:
            label.setStyleSheet("padding: 4px;")
            result_layout.addWidget(label)

        result_group.setLayout(result_layout)
        layout.addWidget(result_group)

        # === Подключение сигналов ===
        calc_btn.clicked.connect(self.calc2)
        reset_btn.clicked.connect(self.reset2)

        widget.setLayout(layout)
        return widget

    def calc2(self):
        try:
            x = float(self.x2.text().strip())
            a = float(self.a2.text().strip())
            b = float(self.b2.text().strip())
            c = float(self.c2.text().strip())
            y = float(self.y2.text().strip())
            p = float(self.p2.text().strip())

            # Проверка p ≠ 0
            if p == 0:
                self.res2.setText("<b style='color: red;'>Ошибка: p = 0 — деление в cos/sin невозможно</b>")
                self.clear_steps2_error()
                return

            # Шаг 1: полином
            poly = x ** 3 + a * x ** 2 + b * x + c
            self.step1_label.setText(
                f"1. Полином: {x:.3f}³ + {a:.3f}·{x:.3f}² + {b:.3f}·{x:.3f} + {c:.3f} = {poly:.6f}")

            # Шаг 2: модули
            mod1 = abs(x - a)
            mod2 = abs(y - b)
            self.step2_label.setText(
                f"2. |x − a| = |{x:.3f} − {a:.3f}| = {mod1:.6f};  |y − b| = |{y:.3f} − {b:.3f}| = {mod2:.6f}")

            # Шаг 3: корни
            try:
                sqrt1 = mod1 ** 0.5
                sqrt2 = mod2 ** 0.5
                self.step3_label.setText(f"3. √|x−a| = √{mod1:.6f} = {sqrt1:.6f};  √|y−b| = √{mod2:.6f} = {sqrt2:.6f}")
            except ValueError:
                self.res2.setText(
                    "<b style='color: red;'>Ошибка: отрицательное значение под корнем (не должно быть)</b>")
                return

            # Шаг 4: знаменатель
            denominator = sqrt1 + sqrt2
            self.step4_label.setText(f"4. Знаменатель: √|x−a| + √|y−b| = {sqrt1:.6f} + {sqrt2:.6f} = {denominator:.6f}")

            if abs(denominator) < 1e-12:
                self.res2.setText("<b style='color: red;'>Ошибка: знаменатель ≈ 0 — деление невозможно</b>")
                return

            # Шаг 5: тригонометрическая часть
            cos_part = math.cos(x / p)
            sin_part = math.sin(y / p)
            trig = a * cos_part + b * sin_part
            self.step5_label.setText(
                f"5. a·cos(x/p) + b·sin(y/p) = {a:.3f}·cos({x:.3f}/{p:.3f}) + {b:.3f}·sin({y:.3f}/{p:.3f}) = "
                f"{a:.3f}·{cos_part:.6f} + {b:.3f}·{sin_part:.6f} = {trig:.6f}"
            )

            # Итог
            result = (poly / denominator) * trig
            self.res2.setText(
                f"<b style='color: green;'>"
                f"F = ({poly:.6f} / {denominator:.6f}) × {trig:.6f} = {result:.6f}"
                f"</b>"
            )

        except ValueError:
            self.res2.setText("<b style='color: red;'>Ошибка: введите корректные числовые значения</b>")
            self.clear_steps2_error()
        except Exception as e:
            self.res2.setText(f"<b style='color: red;'>Ошибка: {e}</b>")
            self.clear_steps2_error()

    def clear_steps2_error(self):
        """Сброс промежуточных шагов при ошибке"""
        self.step1_label.setText("1. Полином: x³ + a·x² + b·x + c = ?")
        self.step2_label.setText("2. |x − a| = ?, |y − b| = ?")
        self.step3_label.setText("3. √|x−a| = ?, √|y−b| = ?")
        self.step4_label.setText("4. Знаменатель: √|x−a| + √|y−b| = ?")
        self.step5_label.setText("5. Тригонометрическая часть: a·cos(x/p) + b·sin(y/p) = ?")

    def reset2(self):
        self.x2.clear();
        self.a2.clear();
        self.b2.clear()
        self.c2.clear();
        self.y2.clear();
        self.p2.clear()

        # Сброс надписей
        self.step1_label.setText("1. Полином: x³ + a·x² + b·x + c = ?")
        self.step2_label.setText("2. |x − a| = ?, |y − b| = ?")
        self.step3_label.setText("3. √|x−a| = ?, √|y−b| = ?")
        self.step4_label.setText("4. Знаменатель: √|x−a| + √|y−b| = ?")
        self.step5_label.setText("5. Тригонометрическая часть: a·cos(x/p) + b·sin(y/p) = ?")
        self.res2.setText("<b>Итог: F = ?</b>")

    def setup_tab3(self):
        widget = QWidget()
        layout = QVBoxLayout()

        # === Заголовок ===
        title = QLabel("Кусочная функция")
        title.setStyleSheet("font-size: 16px; font-weight: bold;")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(title)

        # === Формула как изображение ===
        image_label = QLabel()
        pixmap = QPixmap("img/ex1_3.PNG")

        if pixmap.isNull():
            # Резервный текст, если изображение не найдено
            fallback_label = QLabel("Изображение формулы не найдено (ex1_3.PNG)")
            fallback_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            fallback_label.setStyleSheet("color: red; font-style: italic;")
            layout.addWidget(fallback_label)
        else:
            # Масштабируем с сохранением пропорций
            scaled_pixmap = pixmap.scaled(
                500, 150,  # Примерные размеры — подбери под свой файл
                Qt.AspectRatioMode.KeepAspectRatio,
                Qt.TransformationMode.SmoothTransformation
            )
            image_label.setPixmap(scaled_pixmap)
            image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
            layout.addWidget(image_label)

        # === Поле ввода ===
        input_group = QGroupBox("Введите значение x")
        input_layout = QFormLayout()
        self.x3 = QLineEdit()
        self.x3.setPlaceholderText("например: -2.5, 0, 1.5")
        input_layout.addRow("x:", self.x3)
        input_group.setLayout(input_layout)
        layout.addWidget(input_group)

        # === Кнопки ===
        btn_layout = QHBoxLayout()
        calc_btn = QPushButton("Вычислить")
        reset_btn = QPushButton("Сбросить")
        example_btn = QPushButton("Пример")

        #Button style
        #calc_btn.setStyleSheet("padding: 8px; font-weight: bold; background-color: #4CAF50; color: white;")
        #reset_btn.setStyleSheet("padding: 8px; background-color: #f44336; color: white;")
        #example_btn.setStyleSheet("padding: 8px; background-color: #2196F3; color: white;")

        btn_layout.addWidget(calc_btn)
        btn_layout.addWidget(reset_btn)
        btn_layout.addWidget(example_btn)
        layout.addLayout(btn_layout)

        # === Результат и анализ ===
        result_group = QGroupBox("Анализ и результат")
        result_layout = QVBoxLayout()

        self.condition_label = QLabel("Условие не проверено")
        self.condition_label.setStyleSheet("padding: 6px;")

        self.cos_value_label = QLabel("cos(x) не вычислен")
        self.cos_value_label.setStyleSheet("padding: 6px; color: #0066cc;")

        self.res3 = QLabel("<b>y = ?</b>")
        self.res3.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.res3.setStyleSheet("font-size: 18px; font-weight: bold; padding: 12px;")

        result_layout.addWidget(self.condition_label)
        result_layout.addWidget(self.cos_value_label)
        result_layout.addWidget(self.res3)
        result_group.setLayout(result_layout)
        layout.addWidget(result_group)

        # === Подключение кнопок ===
        calc_btn.clicked.connect(self.calc3)
        reset_btn.clicked.connect(self.reset3)
        example_btn.clicked.connect(self.load_example3)

        widget.setLayout(layout)
        return widget

    def calc3(self):
        try:
            x = float(self.x3.text().strip())
            pi_half = math.pi / 2  # ≈ 1.5708

            # Анализ, в какой диапазон попадает x
            if x <= -pi_half:
                condition = f"x = {x:.4f} ≤ -π/2 ({-pi_half:.4f}) → <b>выбрано y = 0</b>"
                y = 0.0
                cos_val = math.cos(x)
                self.condition_label.setText(f"<span style='color: green;'>{condition}</span>")
                self.cos_value_label.setText(f"cos(x) = cos({x:.4f}) = {cos_val:.6f} (не используется)")
                self.res3.setText(f"<b style='color: green;'>y = 0</b>")

            elif -pi_half < x <= 0:
                condition = f"-π/2 ({-pi_half:.4f}) < x = {x:.4f} ≤ 0 → <b>выбрано y = cos(x)</b>"
                cos_val = math.cos(x)
                y = cos_val
                self.condition_label.setText(f"<span style='color: #FF9800;'>{condition}</span>")
                self.cos_value_label.setText(f"cos(x) = cos({x:.4f}) = <b>{cos_val:.6f}</b>")
                self.res3.setText(f"<b style='color: #FF9800;'>y = {y:.6f}</b>")

            else:  # x > 0
                condition = f"x = {x:.4f} > 0 → <b>выбрано y = 1</b>"
                cos_val = math.cos(x)
                y = 1.0
                self.condition_label.setText(f"<span style='color: blue;'>{condition}</span>")
                self.cos_value_label.setText(f"cos(x) = cos({x:.4f}) = {cos_val:.6f} (не используется)")
                self.res3.setText(f"<b style='color: blue;'>y = 1</b>")

        except ValueError:
            self.res3.setText("<b style='color: red;'>Ошибка: введите число</b>")
            self.condition_label.setText("Условие: ошибка ввода")
            self.cos_value_label.setText("cos(x): не вычислен")
        except Exception as e:
            self.res3.setText(f"<b style='color: red;'>Ошибка: {e}</b>")
            self.condition_label.setText("Условие: не определено")
            self.cos_value_label.setText("cos(x): не вычислен")

    def reset3(self):
        self.x3.clear()
        self.condition_label.setText("Условие не проверено")
        self.cos_value_label.setText("cos(x) не вычислен")
        self.res3.setText("<b>y = ?</b>")

    def load_example3(self):
        """Подставляет пример и сразу считает"""
        self.x3.setText("-2.0")  # Попадает в первый диапазон: x ≤ -π/2
        self.calc3()