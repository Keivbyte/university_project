import math
from typing import List, Tuple, Optional, Union

class Control1:
    def __init__(self):
        self.title = "Control 1"

    def exercise_1(self, x, y, a, b):
        """
        Вычисляет значение функции F по заданной формуле.
        F = (cbrt(x² + y²) / sqrt(x⁴ + y⁴)) * (a*x² + (a/b)*x²*y - (b/a)*x*y² + b*y²)
        """
        try:
            if a == 0 or b == 0:
                return None, "Error: Division by zero (a or b is zero)"

            numerator_cbrt = (x**2 + y**2) ** (1/3)
            denominator_sqrt = math.sqrt(x**4 + y**4)

            if denominator_sqrt == 0:
                return None, "Error: Denominator is zero (x and y are both zero)"

            bracket_term = ( a * x**2 + (a / b) * x**2 * y - (b / a) * x * y**2 +  b * y**2 )

            result = (numerator_cbrt / denominator_sqrt) * bracket_term
            return result, None

        except Exception as e:
            return None, f"Error: {str(e)}"

    def exercise_2(self, x, y, a, b):
        """
        p = (a + b + x + y) / 2
        F = sin²(x - p/a) + (y² + xy + x²)/(a² + b²) * cos²(x - p/a)
        """
        try:
            if a == 0:
                return None, "Error: Division by zero (a is zero)"

            p = (a + b + x + y) / 2
            arg = x - p / a

            sin2 = math.sin(arg) ** 2
            cos2 = math.cos(arg) ** 2

            numerator = y**2 + x*y + x**2
            denominator = a**2 + b**2

            if denominator == 0:
                return None, "Error: Denominator is zero (a and b are both zero)"

            result = sin2 + (numerator / denominator) * cos2
            return result, None

        except Exception as e:
            return None, f"Error: {str(e)}"

    def exercise_3(self, x):
        """
        Кусочная функция:
        - x ≤ -1 → y = x² - 1
        - |x| < 1 → y = cos(π/(2x))  (x ≠ 0)
        - x ≥ 1 → y = 0
        """
        try:
            if x <= -1:
                result = x * x - 1
                condition = "x ≤ -1 → y = x² - 1"
            elif abs(x) < 1:
                if x == 0:
                    return None, "Error: Division by zero in cos(π/(2x))", ""
                result = math.cos(math.pi / (2 * x))
                condition = "|x| < 1 → y = cos(π/(2x))"
            elif x >= 1:
                result = 0
                condition = "x ≥ 1 → y = 0"
            else:
                return None, "Error: Unhandled case", ""

            return result, condition, None

        except Exception as e:
            return None, "", f"Error: {str(e)}"


class Control2:
    def __init__(self):
        self.title = "Control 2"

    def exercise_1(self, a: float, b: float, h: float) -> Tuple[Optional[List[Tuple[float, float]]], Optional[str]]:
        """
        Цикл с параметром (for-like via while).
        Функция:
        y = 0,               если x ≤ -1
        y = 1 + x,           если -1 < x < 0
        y = cos(π·x/2),      если x ≥ 1
        Примечание: не определено при 0 ≤ x < 1 → возвращаем None для таких x.
        """
        try:
            if h <= 0:
                return None, "Error: Step h must be positive"
            if a > b:
                return None, "Error: a must be <= b"

            results = []
            x = a
            while x <= b:
                if x <= -1:
                    y = 0.0
                elif -1 < x < 0:
                    y = 1 + x
                elif x >= 1:
                    y = math.cos(math.pi * x / 2)
                else:
                    y = None  # 0 <= x < 1 — не определено
                results.append((x, y))
                x = round(x + h, 10)  # избегаем накопления float-ошибок
            return results, None
        except Exception as e:
            return None, f"Error: {str(e)}"

    def exercise_2(self, a: float, b: float, h: float) -> Tuple[Optional[List[Tuple[float, float]]], Optional[str]]:
        """
        Цикл с предусловием (while).
        Функция:
        y = e^{-x},          если x ≤ 0
        y = cos(π·x/2),      если 0 < x < 1
        y = 0,               если x ≥ 1
        """
        try:
            if h <= 0:
                return None, "Error: Step h must be positive"
            if a > b:
                return None, "Error: a must be <= b"

            results = []
            x = a
            while x <= b:
                if x <= 0:
                    y = math.exp(-x)
                elif 0 < x < 1:
                    y = math.cos(math.pi * x / 2)
                elif x >= 1:
                    y = 0.0
                else:
                    y = None
                results.append((x, y))
                x = round(x + h, 10)
            return results, None
        except Exception as e:
            return None, f"Error: {str(e)}"

    def exercise_3(self, a: float, b: float, h: float) -> Tuple[Optional[List[Tuple[float, float]]], Optional[str]]:
        """
        Комбинированный: параметр + предусловие (логически то же, но соответствует ТЗ).
        Функция:
        y = 0,               если x < -π/2
        y = cos(x),          если -π/2 ≤ x ≤ 0
        y = 1,               если x > 0
        """
        try:
            if h <= 0:
                return None, "Error: Step h must be positive"
            if a > b:
                return None, "Error: a must be <= b"

            results = []
            x = a
            while x <= b:
                if x < -math.pi / 2:
                    y = 0.0
                elif -math.pi / 2 <= x <= 0:
                    y = math.cos(x)
                elif x > 0:
                    y = 1.0
                else:
                    y = None
                results.append((x, y))
                x = round(x + h, 10)
            return results, None
        except Exception as e:
            return None, f"Error: {str(e)}"


class Control3:
    def __init__(self):
        self.title = "Control 3"

    def exercise_1(self, x: float = math.pi / 8, n: int = 10) -> Tuple[Optional[float], Optional[str]]:
        """
        Вычисляет сумму ряда:
        S = Σ (cos^k(x) / k!) for k = 2 to n
        """
        try:
            if n < 2:
                return None, "Error: n must be >= 2"

            total = 0.0
            cos_x = math.cos(x)
            factorial = 1

            # Предвычислим факториалы итеративно
            for k in range(1, n + 1):
                factorial *= k
                if k >= 2:
                    term = (cos_x ** k) / factorial
                    total += term

            return total, None
        except Exception as e:
            return None, f"Error: {str(e)}"

    def exercise_2(self, sequence: List[float], p: float) -> Tuple[Optional[float], Optional[str]]:
        """
        Вычисляет сумму элементов последовательности, для которых x > p.
        sequence: список чисел [x1, x2, ..., xn], n <= 20
        p: пороговое значение
        Возвращает: (сумма, None) или (None, сообщение об ошибке)
        """
        try:
            if not isinstance(sequence, list):
                return None, "Error: Sequence must be a list"
            if len(sequence) == 0:
                return 0.0, None
            if len(sequence) > 20:
                return None, "Error: Sequence length must be ≤ 20"

            total = sum(x for x in sequence if x > p)
            return total, None

        except Exception as e:
            return None, f"Error: {str(e)}"

    def exercise_3(self, matrix: List[List[float]]) -> Tuple[Optional[Tuple[int, float]], Optional[str]]:
        """
        Для матрицы B размером 5×4:
        - вычисляет сумму элементов каждой строки,
        - находит минимальную сумму и номер строки (с 1!).
        """
        try:
            if not matrix or len(matrix) != 5:
                return None, "Error: Matrix must have exactly 5 rows"
            for row in matrix:
                if len(row) != 4:
                    return None, "Error: Each row must have exactly 4 elements"

            row_sums = []
            for row in matrix:
                row_sums.append(sum(row))

            min_sum = min(row_sums)
            min_index = row_sums.index(min_sum) + 1  # нумерация с 1

            return (min_index, min_sum), None
        except Exception as e:
            return None, f"Error: {str(e)}"