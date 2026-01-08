# exercise1_ui_control2.py

import curses
import time

def draw_formula(stdscr, h, w):
    """Отрисовка формулы в псевдографике"""
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════╗",
        "║                     FUNCTION:                    ║",
        "║        |0,         если x ≤ -1                   ║",
        "║   y =  ( 1 + x,     если -1 < x < 0              ║",
        "║        |cos(π·x/2), если x ≥ 1                   ║",
        "╚══════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_fields(stdscr, inputs, selected_idx, h, w):
    """Отрисовка полей ввода a, b, h"""
    menu_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    fields = ["a", "b", "h"]
    start_y = h // 2 + 6

    for i, key in enumerate(fields):
        label = f"{key} = "
        value = str(inputs[key])
        line = label + value

        color = highlight_color if i == selected_idx else menu_color
        stdscr.addstr(start_y + i, (w // 2) - len(line) // 2, line, color)


def draw_calc_button(stdscr, h, w, is_highlighted=False):
    """Отрисовка кнопки CALC"""
    calc_color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    calc_line = "► CALC ◄"
    y = h // 2 + 12
    x = (w // 2) - len(calc_line) // 2
    stdscr.addstr(y, x, calc_line, calc_color)


def run_exercise1_ui_control2(stdscr, control_obj):
    """Графический интерфейс для выполнения упражнения 1 контрольной 2"""
    global is_running

    curses.curs_set(0)  # Скрываем курсор
    stdscr.nodelay(False)
    stdscr.keypad(True)

    h, w = stdscr.getmaxyx()

    # Цвета
    if curses.has_colors():
        curses.start_color()
        curses.init_pair(1, curses.COLOR_GREEN, curses.COLOR_BLACK)  # Заголовки
        curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_BLACK)  # Текст
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)  # Выделение
        curses.init_pair(4, curses.COLOR_RED, curses.COLOR_BLACK)    # Ошибки

    # Инициализация значений
    inputs = {"a": "", "b": "", "h": ""}
    selected_idx = 0  # Индекс выбранного поля
    fields = list(inputs.keys())

    result_list = []
    error_str = ""
    calc_highlighted = False

    while True:
        stdscr.clear()

        # Отображаем формулу
        draw_formula(stdscr, h, w)

        # Отображаем поля ввода
        draw_input_fields(stdscr, inputs, selected_idx, h, w)

        # Отображаем кнопку CALC
        draw_calc_button(stdscr, h, w, calc_highlighted)

        # Отображаем ошибку внизу
        if error_str:
            stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))

        # Инструкции
        instructions = "↑↓ to move, ENTER to edit, DEL/BACK to clear, C to CALC, ESC to exit"
        stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))

        stdscr.refresh()

        key = stdscr.getch()

        if key == 27:  # ESC
            break
        elif key == curses.KEY_UP:
            selected_idx = (selected_idx - 1) % len(fields)
        elif key == curses.KEY_DOWN:
            selected_idx = (selected_idx + 1) % len(fields)
        elif key == curses.KEY_ENTER or key in [10, 13]:
            # Редактирование выбранного поля
            current_key = fields[selected_idx]
            old_value = inputs[current_key]
            new_value = get_number_input(stdscr, current_key, old_value)
            if new_value is not None:
                inputs[current_key] = new_value
        elif key == curses.KEY_DC or key == 127 or key == 8:  # DEL / BACKSPACE
            current_key = fields[selected_idx]
            inputs[current_key] = ""
        elif key == ord('c') or key == ord('C'):  # Вычислить
            calc_highlighted = True
            stdscr.clear()
            draw_formula(stdscr, h, w)
            draw_input_fields(stdscr, inputs, selected_idx, h, w)
            draw_calc_button(stdscr, h, w, calc_highlighted)
            if error_str:
                stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))
            instructions = "↑↓ to move, ENTER to edit, DEL/BACK to clear, C to CALC, ESC to exit"
            stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
            stdscr.refresh()
            curses.napms(100)

            try:
                # Отладочный вывод
                a_str = inputs["a"]
                b_str = inputs["b"]
                h_str = inputs["h"]

                print(f"DEBUG: a_str = '{a_str}', type = {type(a_str)}")
                print(f"DEBUG: b_str = '{b_str}', type = {type(b_str)}")
                print(f"DEBUG: h_str = '{h_str}', type = {type(h_str)}")

                # Явно проверим и преобразуем
                if a_str == "":
                    a = 0
                else:
                    a = float(a_str)

                if b_str == "":
                    b = 0
                else:
                    b = float(b_str)

                if h_str == "":
                    h_val = 0.1
                else:
                    h_val = float(h_str)

                print(f"DEBUG: a = {a}, b = {b}, h = {h_val}")

                result_list, error = control_obj.exercise_1(a, b, h_val)

                if error:
                    error_str = error
                    result_list = []
                else:
                    error_str = ""
                    # Анимированная отрисовка таблицы
                    display_table_with_animation(stdscr, result_list, h, w)

            except ValueError as e:
                error_str = f"Error: Invalid number format ({str(e)})"
                result_list = []
            except Exception as e:
                error_str = f"Error: {str(e)}"
                result_list = []

            calc_highlighted = False

    curses.curs_set(0)


def display_table_with_animation(stdscr, results, h, w):
    """Анимированная отрисовка таблицы значений x | y"""
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)
    header_color = curses.color_pair(3)

    # Полностью очищаем экран перед отрисовкой таблицы
    stdscr.clear()

    # Заголовок таблицы
    header =    "╔═══════════════════════════════╗"
    stdscr.addstr(1, (w // 2) - len(header) // 2, header, title_color)
    title =     "║        TABLE OF VALUES        ║"
    stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
    separator = "╠═══════════════════════════════╣"
    stdscr.addstr(3, (w // 2) - len(separator) // 2, separator, title_color)

    # Шапка таблицы
    col_header = "║  #  |   x       |       y     ║"
    stdscr.addstr(4, (w // 2) - len(col_header) // 2, col_header, header_color)

    # Тело таблицы — анимированная отрисовка
    start_y = 5
    max_rows = h - 7

    for i, (x, y) in enumerate(results):
        # Форматируем y: если None — строка 'None', иначе float
        y_str = f"{0.0:8.4f}" if y == 0.0 else f"{y:8.4f}" if y is not None else "None"
        row = f"║ {i+1:3d} |  {x:8.4f} | {y_str:10s}  ║"
        x_pos = (w // 2) - len(row) // 2
        y_pos = start_y + i

        # Если строка выходит за экран — прерываем
        if i >= max_rows:
            break

        # Анимация: постепенно рисуем строку
        for j in range(len(row)):
            stdscr.addstr(y_pos, x_pos + j, row[j], text_color)
            stdscr.refresh()
            time.sleep(0.01)

    # Футер таблицы
    footer =    "╚═══════════════════════════════╝"
    footer_y = start_y + min(len(results), max_rows)
    stdscr.addstr(footer_y, (w // 2) - len(footer) // 2, footer, title_color)

    # Подсказка внизу
    prompt = "Press any key to continue..."
    stdscr.addstr(h - 2, (w // 2) - len(prompt) // 2, prompt, text_color)
    stdscr.refresh()
    stdscr.getch()


def get_number_input(stdscr, key, old_value=""):
    """
    Ввод числа с поддержкой стрелок, редактирования, удаления.
    """
    h, w = stdscr.getmaxyx()
    y = h // 2 + 6 + ["a", "b", "h"].index(key)
    x_start = (w // 2) - 10  # Примерное начало поля

    input_str = str(old_value)
    cursor_pos = len(input_str)

    while True:
        # Очищаем строку
        stdscr.addstr(y, x_start, f"{key} = {' ' * 20}")

        # Выводим текущее значение
        display = f"{key} = {input_str}"
        stdscr.addstr(y, x_start, display, curses.color_pair(3))

        # Устанавливаем курсор
        stdscr.move(y, x_start + len(f"{key} = ") + cursor_pos)
        stdscr.refresh()

        key_press = stdscr.getch()

        if key_press == 27:  # ESC
            return old_value
        elif key_press == ord('\n') or key_press == 10 or key_press == 13:  # ENTER
            try:
                # Убираем лишние пробелы и проверяем строку
                cleaned_str = input_str.strip()
                if cleaned_str == "":
                    return ""
                # Проверим, что строка может быть числом
                float(cleaned_str)  # Это вызовет ValueError, если строка невалидна
                return float(cleaned_str)
            except ValueError:
                return old_value  # Если ошибка — возвращаем старое значение
        elif key_press == curses.KEY_LEFT and cursor_pos > 0:
            cursor_pos -= 1
        elif key_press == curses.KEY_RIGHT and cursor_pos < len(input_str):
            cursor_pos += 1
        elif key_press == curses.KEY_BACKSPACE or key_press == 127:
            if len(input_str) > 0 and cursor_pos > 0:
                input_str = input_str[:cursor_pos-1] + input_str[cursor_pos:]
                cursor_pos -= 1
        elif key_press == curses.KEY_DC:  # DEL
            if len(input_str) > 0 and cursor_pos < len(input_str):
                input_str = input_str[:cursor_pos] + input_str[cursor_pos+1:]
        elif key_press == ord('-') and cursor_pos == 0 and '-' not in input_str:
            input_str = '-' + input_str
            cursor_pos += 1
        elif key_press == ord('.') and '.' not in input_str:
            input_str = input_str[:cursor_pos] + '.' + input_str[cursor_pos:]
            cursor_pos += 1
        elif 48 <= key_press <= 57:  # Цифры 0-9
            if len(input_str) < 20:
                input_str = input_str[:cursor_pos] + chr(key_press) + input_str[cursor_pos:]
                cursor_pos += 1