# exercise3_ui.py

import curses
import math

def draw_formula(stdscr, h, w):
    """Отрисовка формулы в псевдографике"""
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔═════════════════════════════════════════════════════╗",
        "║                        FUNCTION:                    ║",
        "║                                                     ║",
        "║       | x·x - 1,       если x ≤ -1                  ║",
        "║   y = ( cos(π/(2x)),   если |x| < 1                 ║",
        "║       | 0,             если x ≥ 1                   ║",
        "╚═════════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_field(stdscr, x_value, selected, h, w):
    """Отрисовка поля ввода x"""
    menu_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    label = "x = "
    line = label + str(x_value)
    y = h // 2 + 6
    x_pos = (w // 2) - len(line) // 2

    color = highlight_color if selected else menu_color
    stdscr.addstr(y, x_pos, line, color)


def draw_calc_button(stdscr, h, w, is_highlighted=False):
    """Отрисовка кнопки CALC"""
    calc_color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    calc_line = "► CALC ◄"
    y = h // 2 + 8
    x = (w // 2) - len(calc_line) // 2
    stdscr.addstr(y, x, calc_line, calc_color)


def run_exercise3_ui(stdscr, control_obj):
    """Графический интерфейс для выполнения упражнения 3"""
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
    x_value = ""
    selected = True  # Поле всегда выбрано
    result_num = 0.0
    condition_str = ""
    error_str = ""
    calc_highlighted = False

    while True:
        stdscr.clear()

        # Отображаем формулу
        draw_formula(stdscr, h, w)

        # Отображаем поле ввода x
        draw_input_field(stdscr, x_value, selected, h, w)

        # Отображаем кнопку CALC
        draw_calc_button(stdscr, h, w, calc_highlighted)

        # Отображаем результат, условие и ошибку внизу
        if error_str:
            stdscr.addstr(h - 5, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))
        elif result_num is not None and condition_str:
            result_line = f"y = {result_num:.6f}  [ {condition_str} ]"
            stdscr.addstr(h - 5, (w // 2) - len(result_line) // 2, result_line, curses.color_pair(1))

        # Инструкции
        instructions = "ENTER to edit, DEL/BACK to clear, C to CALC, ESC to exit"
        stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))

        stdscr.refresh()

        key = stdscr.getch()

        if key == 27:  # ESC
            break
        elif key == curses.KEY_ENTER or key in [10, 13]:
            # Редактирование x
            old_value = x_value
            new_value = get_number_input(stdscr, "x", old_value)
            if new_value is not None:
                x_value = new_value
        elif key == curses.KEY_DC or key == 127 or key == 8:  # DEL / BACKSPACE
            x_value = ""
        elif key == ord('c') or key == ord('C'):  # Вычислить
            calc_highlighted = True
            stdscr.clear()
            draw_formula(stdscr, h, w)
            draw_input_field(stdscr, x_value, selected, h, w)
            draw_calc_button(stdscr, h, w, calc_highlighted)
            if error_str:
                stdscr.addstr(h - 5, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))
            elif result_num is not None and condition_str:
                result_line = f"y = {result_num:.6f}  [ {condition_str} ]"
                stdscr.addstr(h - 5, (w // 2) - len(result_line) // 2, result_line, curses.color_pair(1))
            instructions = "ENTER to edit, DEL/BACK to clear, C to CALC, ESC to exit"
            stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
            stdscr.refresh()
            curses.napms(100)  # Эффект нажатия

            try:
                x = float(x_value) if x_value else 0

                result, condition, error = control_obj.exercise_3(x)

                if error:
                    result_num = None
                    condition_str = ""
                    error_str = error
                else:
                    result_num = result  # Сохраняем как число
                    condition_str = condition
                    error_str = ""

            except ValueError:
                error_str = "Error: Invalid number format"
                result_num = None
                condition_str = ""

            calc_highlighted = False

    curses.curs_set(0)


def get_number_input(stdscr, key, old_value=""):
    """
    Ввод числа с поддержкой стрелок, редактирования, удаления.
    """
    h, w = stdscr.getmaxyx()
    y = h // 2 + 6
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