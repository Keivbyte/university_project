# exercise1_ui.py

import curses

def draw_formula(stdscr, h, w):
    """Отрисовка формулы в псевдографике"""
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════════════════════╗",
        "║                        FORMULA:                                  ║",
        "║      ┌───────────────┐                                           ║",
        "║      │ ³√(x² + y²)   │                                           ║",
        "║ F =  │ ───────────── │ · (a·x² + (a/b)·x²·y - (b/a)·x·y² + b·y²) ║",
        "║      │  √(x⁴ + y⁴)   │                                           ║",
        "║      └───────────────┘                                           ║",
        "╚══════════════════════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_fields(stdscr, inputs, selected_idx, h, w):
    """Отрисовка полей ввода x, y, a, b"""
    menu_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    fields = ["x", "y", "a", "b"]
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


def run_exercise1_ui(stdscr, control_obj):
    """Графический интерфейс для выполнения упражнения 1"""
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
    inputs = {"x": "", "y": "", "a": "", "b": ""}
    selected_idx = 0  # Индекс выбранного поля
    fields = list(inputs.keys())

    result_str = ""
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

        # Отображаем результат или ошибку внизу
        if error_str:
            stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))
        elif result_str:
            stdscr.addstr(h - 4, (w // 2) - len(result_str) // 2, result_str, curses.color_pair(1))

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
            elif result_str:
                stdscr.addstr(h - 4, (w // 2) - len(result_str) // 2, result_str, curses.color_pair(1))
            instructions = "↑↓ to move, ENTER to edit, DEL/BACK to clear, C to CALC, ESC to exit"
            stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
            stdscr.refresh()
            curses.napms(100)  # Небольшая задержка для эффекта нажатия

            try:
                x = float(inputs["x"]) if inputs["x"] else 0
                y = float(inputs["y"]) if inputs["y"] else 0
                a = float(inputs["a"]) if inputs["a"] else 0
                b = float(inputs["b"]) if inputs["b"] else 0

                result, error = control_obj.exercise_1(x, y, a, b)

                if error:
                    result_str = ""
                    error_str = error
                else:
                    result_str = f"F = {result:.6f}"
                    error_str = ""

            except ValueError:
                error_str = "Error: Invalid number format"
                result_str = ""

            calc_highlighted = False

    curses.curs_set(0)


def get_number_input(stdscr, key, old_value=""):
    """
    Ввод числа с поддержкой стрелок, редактирования, удаления.
    """
    h, w = stdscr.getmaxyx()
    y = h // 2 + 6 + ["x", "y", "a", "b"].index(key)
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
                if input_str.strip() == "":
                    return ""
                return float(input_str)
            except ValueError:
                return old_value
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