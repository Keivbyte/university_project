# exercise3_ui_control3.py

import curses
import time
from core import Control3


def draw_formula(stdscr, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════╗",
        "║               MATRIX PROCESSING (5×4)             ║",
        "║                                                  ║",
        "║   Step 1: Press W to enter matrix values         ║",
        "║   Step 2: Edit cells with ↑↓←→ and ENTER         ║",
        "║   Step 3: Press C to calculate                   ║",
        "╚══════════════════════════════════════════════════╝"
    ]

    # Размещаем формулу в верхней части экрана
    start_y = 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)

    return start_y + len(formula_lines) + 2  # Возвращаем Y-позицию для матрицы


def draw_matrix(stdscr, matrix, cursor_row, cursor_col, start_y, w, is_editing=False):
    """Отрисовка матрицы 5x4 ниже заголовка"""
    text_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    start_x = (w // 2) - 18  # Центрируем матрицу (4 колонки * 8 символов)

    # Отрисовка рамки вокруг матрицы
    frame_top = "┌" + "─" * 31 + "┐"
    frame_bottom = "└" + "─" * 31 + "┘"
    stdscr.addstr(start_y - 1, start_x - 1, frame_top, text_color)
    stdscr.addstr(start_y + 5, start_x - 1, frame_bottom, text_color)

    for i in range(5):
        # Рисуем вертикальные линии слева
        stdscr.addstr(start_y + i, start_x - 1, "│", text_color)

        for j in range(4):
            val = matrix[i][j]
            cell_str = f"{val:6.2f}"

            # Вычисляем точные координаты для каждой ячейки
            cell_x = start_x + j * 8

            if i == cursor_row and j == cursor_col and is_editing:
                stdscr.addstr(start_y + i, cell_x, cell_str, highlight_color)
            else:
                stdscr.addstr(start_y + i, cell_x, cell_str, text_color)

        # Рисуем вертикальные линии справа
        stdscr.addstr(start_y + i, start_x + 31, "│", text_color)


def draw_write_button(stdscr, y, w, is_highlighted=False):
    color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    line = "► WRITE ◄"
    x = (w // 2) - len(line) // 2
    stdscr.addstr(y, x, line, color)


def draw_calc_button(stdscr, y, w, is_highlighted=False):
    color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    line = "► CALC ◄"
    x = (w // 2) - len(line) // 2
    stdscr.addstr(y, x, line, color)


def display_result(stdscr, matrix, row_sums, min_row, min_sum, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    stdscr.clear()
    header = "╔════════════════════════════════════════════╗"
    stdscr.addstr(1, (w // 2) - len(header) // 2, header, title_color)
    title = "║            MATRIX & SUMS           ║"
    stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
    sep = "╠════════════════════════════════════════════╣"
    stdscr.addstr(3, (w // 2) - len(sep) // 2, sep, title_color)

    y_pos = 5
    for i, row in enumerate(matrix):
        row_str = "[" + ", ".join(f"{x:6.2f}" for x in row) + f"] → sum = {row_sums[i]:8.2f}"
        color = highlight_color if (i + 1) == min_row else text_color
        stdscr.addstr(y_pos + i, (w // 2) - len(row_str) // 2, row_str, color)

    result_line = f"MIN SUM: row {min_row} = {min_sum:.2f}"
    stdscr.addstr(y_pos + 6, (w // 2) - len(result_line) // 2, result_line, highlight_color)

    footer = "╚════════════════════════════════════════════╝"
    stdscr.addstr(y_pos + 8, (w // 2) - len(footer) // 2, footer, title_color)

    prompt = "Press any key to continue..."
    stdscr.addstr(h - 2, (w // 2) - len(prompt) // 2, prompt, text_color)
    stdscr.refresh()
    stdscr.getch()


def get_cell_input(stdscr, current_value, h, w, y, x):
    input_str = str(current_value) if current_value is not None else ""
    if input_str.endswith(".0"):
        input_str = input_str[:-2]
    cursor_pos = len(input_str)
    edit_color = curses.color_pair(3)
    text_color = curses.color_pair(2)

    while True:
        stdscr.addstr(y, x, " " * 7, text_color)
        display = input_str.ljust(7)
        stdscr.addstr(y, x, display, edit_color)
        stdscr.move(y, x + cursor_pos)
        stdscr.refresh()

        key_press = stdscr.getch()

        if key_press == 27:
            return current_value
        elif key_press in [10, 13]:
            if input_str.strip() == "":
                return 0.0
            try:
                return float(input_str)
            except ValueError:
                return current_value
        elif key_press == curses.KEY_LEFT and cursor_pos > 0:
            cursor_pos -= 1
        elif key_press == curses.KEY_RIGHT and cursor_pos < len(input_str):
            cursor_pos += 1
        elif key_press in [curses.KEY_BACKSPACE, 127, 8]:
            if cursor_pos > 0:
                input_str = input_str[:cursor_pos - 1] + input_str[cursor_pos:]
                cursor_pos -= 1
        elif key_press == curses.KEY_DC and cursor_pos < len(input_str):
            input_str = input_str[:cursor_pos] + input_str[cursor_pos + 1:]
        elif key_press == ord('-') and cursor_pos == 0 and '-' not in input_str:
            input_str = '-' + input_str
            cursor_pos += 1
        elif key_press == ord('.') and '.' not in input_str:
            input_str = input_str[:cursor_pos] + '.' + input_str[cursor_pos:]
            cursor_pos += 1
        elif 48 <= key_press <= 57:
            if len(input_str) < 7:
                input_str = input_str[:cursor_pos] + chr(key_press) + input_str[cursor_pos:]
                cursor_pos += 1


def run_exercise3_ui_control3(stdscr, control_obj):
    curses.curs_set(0)
    stdscr.nodelay(False)
    stdscr.keypad(True)

    h, w = stdscr.getmaxyx()

    if curses.has_colors():
        curses.start_color()
        curses.init_pair(1, curses.COLOR_GREEN, curses.COLOR_BLACK)
        curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)
        curses.init_pair(4, curses.COLOR_RED, curses.COLOR_BLACK)

    # Фазы: 'view' или 'edit'
    mode = 'view'
    matrix = [[0.0 for _ in range(4)] for _ in range(5)]
    cursor_row, cursor_col = 0, 0
    error_str = ""
    write_highlighted = False
    calc_highlighted = False

    while True:
        stdscr.clear()

        # Рисуем формулу и получаем позицию для матрицы
        matrix_start_y = draw_formula(stdscr, h, w)

        # Позиция для кнопки (под матрицей)
        button_y = matrix_start_y + 7

        if mode == 'view':
            # Отображаем матрицу как статичную
            draw_matrix(stdscr, matrix, -1, -1, matrix_start_y, w, is_editing=False)
            draw_write_button(stdscr, button_y, w, write_highlighted)
        else:
            # Режим редактирования
            draw_matrix(stdscr, matrix, cursor_row, cursor_col, matrix_start_y, w, is_editing=True)
            draw_calc_button(stdscr, button_y, w, calc_highlighted)

        if error_str:
            stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))

        if mode == 'view':
            instructions = "W to enter matrix, ESC to exit"
        else:
            instructions = "↑↓←→ to move, ENTER to edit, C to CALC, ESC to exit"

        stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
        stdscr.refresh()

        key = stdscr.getch()

        if key == 27:  # ESC
            break

        elif mode == 'view':
            if key in [ord('w'), ord('W')]:
                write_highlighted = True
                stdscr.refresh()
                curses.napms(100)
                mode = 'edit'
                write_highlighted = False
                error_str = ""

        elif mode == 'edit':
            if key == curses.KEY_UP and cursor_row > 0:
                cursor_row -= 1
            elif key == curses.KEY_DOWN and cursor_row < 4:
                cursor_row += 1
            elif key == curses.KEY_LEFT and cursor_col > 0:
                cursor_col -= 1
            elif key == curses.KEY_RIGHT and cursor_col < 3:
                cursor_col += 1
            elif key in [10, 13]:
                # Вычисляем точные координаты для редактируемой ячейки
                matrix_start_x = (w // 2) - 18
                y = matrix_start_y + cursor_row
                x = matrix_start_x + cursor_col * 8

                old_val = matrix[cursor_row][cursor_col]
                new_val = get_cell_input(stdscr, old_val, h, w, y, x)
                matrix[cursor_row][cursor_col] = new_val if new_val is not None else 0.0
            elif key in [curses.KEY_DC, 127, 8]:
                matrix[cursor_row][cursor_col] = 0.0
            elif key in [ord('c'), ord('C')]:
                calc_highlighted = True
                stdscr.refresh()
                curses.napms(100)

                try:
                    result, error = control_obj.exercise_3(matrix)
                    if error:
                        error_str = error
                    else:
                        min_row, min_sum = result
                        row_sums = [sum(row) for row in matrix]
                        error_str = ""
                        display_result(stdscr, matrix, row_sums, min_row, min_sum, h, w)

                except Exception as e:
                    error_str = f"Error: {str(e)}"

                calc_highlighted = False

    curses.curs_set(1)