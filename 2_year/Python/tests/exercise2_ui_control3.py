# exercise2_ui_control3.py

import curses
import time
from core import Control3

def draw_formula(stdscr, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════╗",
        "║            POLYNOMIAL DERIVATIVE                 ║",
        "║   P(x) = a₀ + a₁x + a₂x² + ... + aₙxⁿ             ║",
        "║   P'(x) = a₁ + 2a₂x + 3a₃x² + ... + n·aₙxⁿ⁻¹      ║",
        "║   Enter coefficients [a₀, a₁, ..., aₙ]           ║",
        "╚══════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_field(stdscr, coeffs_str, is_editing, h, w):
    text_color = curses.color_pair(2)
    edit_color = curses.color_pair(3)

    label = "Coeffs: "
    full_line = label + coeffs_str
    y = h // 2 + 6
    x = (w // 2) - len(full_line) // 2
    color = edit_color if is_editing else text_color
    stdscr.addstr(y, x, full_line, color)


def draw_calc_button(stdscr, h, w, is_highlighted=False):
    calc_color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    calc_line = "► CALC ◄"
    y = h // 2 + 9
    x = (w // 2) - len(calc_line) // 2
    stdscr.addstr(y, x, calc_line, calc_color)


def display_derivative(stdscr, orig, deriv, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)
    header_color = curses.color_pair(3)

    stdscr.clear()

    header = "╔════════════════════════════════════════════╗"
    stdscr.addstr(1, (w // 2) - len(header) // 2, header, title_color)
    title = "║           DERIVATIVE COEFFICIENTS          ║"
    stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
    sep = "╠════════════════════════════════════════════╣"
    stdscr.addstr(3, (w // 2) - len(sep) // 2, sep, title_color)

    orig_str = "P(x):  [" + ", ".join(f"{c:.3f}" for c in orig) + "]"
    deriv_str = "P'(x): [" + ", ".join(f"{c:.3f}" for c in deriv) + "]"

    stdscr.addstr(5, (w // 2) - len(orig_str) // 2, orig_str, text_color)
    stdscr.addstr(6, (w // 2) - len(deriv_str) // 2, deriv_str, header_color)

    footer = "╚════════════════════════════════════════════╝"
    stdscr.addstr(8, (w // 2) - len(footer) // 2, footer, title_color)

    prompt = "Press any key to continue..."
    stdscr.addstr(h - 2, (w // 2) - len(prompt) // 2, prompt, text_color)
    stdscr.refresh()
    stdscr.getch()


def parse_coeff_string(s):
    s = s.strip()
    if not s:
        return []
    parts = s.split(',')
    return [float(x.strip()) for x in parts]


def run_exercise2_ui_control3(stdscr, control_obj):
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

    coeffs_str = "1, 0, -2, 3"  # Пример: 1 - 2x² + 3x³
    is_editing = False
    error_str = ""
    calc_highlighted = False

    while True:
        stdscr.clear()
        draw_formula(stdscr, h, w)
        draw_input_field(stdscr, coeffs_str, is_editing, h, w)
        draw_calc_button(stdscr, h, w, calc_highlighted)

        if error_str:
            stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))

        instructions = "E to edit, ENTER to confirm, C to CALC, ESC to exit"
        stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
        stdscr.refresh()

        key = stdscr.getch()

        if key == 27:
            break
        elif key == ord('e') or key == ord('E'):
            is_editing = True
            # Показать курсор
            curses.curs_set(1)
            # Открываем ввод строки
            y = h // 2 + 6
            x_start = (w // 2) - len("Coeffs: " + coeffs_str) // 2 + len("Coeffs: ")
            stdscr.move(y, x_start + len(coeffs_str))
            stdscr.refresh()

            new_str = ""
            cursor = len(coeffs_str)
            temp_str = coeffs_str

            while True:
                stdscr.addstr(y, x_start, temp_str + " " * 20, curses.color_pair(3))
                stdscr.move(y, x_start + cursor)
                stdscr.refresh()

                k = stdscr.getch()
                if k == 27:
                    break
                elif k in [10, 13]:
                    coeffs_str = temp_str
                    break
                elif k == curses.KEY_LEFT and cursor > 0:
                    cursor -= 1
                elif k == curses.KEY_RIGHT and cursor < len(temp_str):
                    cursor += 1
                elif k in [curses.KEY_BACKSPACE, 127, 8]:
                    if cursor > 0:
                        temp_str = temp_str[:cursor-1] + temp_str[cursor:]
                        cursor -= 1
                elif k == curses.KEY_DC and cursor < len(temp_str):
                    temp_str = temp_str[:cursor] + temp_str[cursor+1:]
                elif 32 <= k <= 126:  # printable chars
                    if len(temp_str) < 60:
                        temp_str = temp_str[:cursor] + chr(k) + temp_str[cursor:]
                        cursor += 1

            is_editing = False
            curses.curs_set(0)
        elif key in [ord('c'), ord('C')]:
            calc_highlighted = True
            stdscr.refresh()
            curses.napms(100)

            try:
                coeffs = parse_coeff_string(coeffs_str)
                if not coeffs:
                    error_str = "Error: Enter at least one coefficient"
                else:
                    deriv, error = control_obj.exercise_2(coeffs)
                    if error:
                        error_str = error
                    else:
                        error_str = ""
                        display_derivative(stdscr, coeffs, deriv, h, w)

            except ValueError:
                error_str = "Error: Invalid number format (use: 1, -2.5, 3)"
            except Exception as e:
                error_str = f"Error: {str(e)}"

            calc_highlighted = False

    curses.curs_set(1)