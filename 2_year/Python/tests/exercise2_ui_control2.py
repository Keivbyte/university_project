# exercise2_ui_control2.py

import curses
import time

def draw_formula(stdscr, h, w):
    """Отрисовка формулы для упражнения 2"""
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════╗",
        "║                     FUNCTION:                    ║",
        "║        |e⁻ˣ,        если x ≤ 0                   ║",
        "║   y =  ( cos(π·x/2), если 0 < x < 1              ║",
        "║        |0,          если x ≥ 1                   ║",
        "╚══════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_fields(stdscr, inputs, selected_idx, h, w):
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
    calc_color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    calc_line = "► CALC ◄"
    y = h // 2 + 12
    x = (w // 2) - len(calc_line) // 2
    stdscr.addstr(y, x, calc_line, calc_color)


def display_table_with_animation(stdscr, results, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)
    header_color = curses.color_pair(3)

    stdscr.clear()

    header =    "╔═══════════════════════════════╗"
    stdscr.addstr(1, (w // 2) - len(header) // 2, header, title_color)
    title =     "║        TABLE OF VALUES        ║"
    stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
    separator = "╠═══════════════════════════════╣"
    stdscr.addstr(3, (w // 2) - len(separator) // 2, separator, title_color)

    col_header = "║  #  |   x       |       y     ║"
    stdscr.addstr(4, (w // 2) - len(col_header) // 2, col_header, header_color)

    start_y = 5
    max_rows = h - 7

    for i, (x, y) in enumerate(results):
        if i >= max_rows:
            break
        y_str = f"{y:8.4f}" if y is not None else "None"
        row = f"║ {i+1:3d} |  {x:8.4f} | {y_str:10s}  ║"
        x_pos = (w // 2) - len(row) // 2
        y_pos = start_y + i

        for j in range(len(row)):
            stdscr.addstr(y_pos, x_pos + j, row[j], text_color)
            stdscr.refresh()
            time.sleep(0.01)

    footer = "╚═══════════════════════════════╝"
    footer_y = start_y + min(len(results), max_rows)
    stdscr.addstr(footer_y, (w // 2) - len(footer) // 2, footer, title_color)

    prompt = "Press any key to continue..."
    stdscr.addstr(h - 2, (w // 2) - len(prompt) // 2, prompt, text_color)
    stdscr.refresh()
    stdscr.getch()


def get_number_input(stdscr, key, old_value=""):
    h, w = stdscr.getmaxyx()
    y = h // 2 + 6 + ["a", "b", "h"].index(key)
    x_start = (w // 2) - 10

    input_str = str(old_value)
    cursor_pos = len(input_str)

    while True:
        stdscr.addstr(y, x_start, f"{key} = {' ' * 20}")
        display = f"{key} = {input_str}"
        stdscr.addstr(y, x_start, display, curses.color_pair(3))
        stdscr.move(y, x_start + len(f"{key} = ") + cursor_pos)
        stdscr.refresh()

        key_press = stdscr.getch()

        if key_press == 27:
            return old_value
        elif key_press in [10, 13]:
            cleaned = input_str.strip()
            if cleaned == "":
                return ""
            try:
                float(cleaned)
                return float(cleaned)
            except ValueError:
                return old_value
        elif key_press == curses.KEY_LEFT and cursor_pos > 0:
            cursor_pos -= 1
        elif key_press == curses.KEY_RIGHT and cursor_pos < len(input_str):
            cursor_pos += 1
        elif key_press in [curses.KEY_BACKSPACE, 127, 8]:
            if cursor_pos > 0:
                input_str = input_str[:cursor_pos-1] + input_str[cursor_pos:]
                cursor_pos -= 1
        elif key_press == curses.KEY_DC:
            if cursor_pos < len(input_str):
                input_str = input_str[:cursor_pos] + input_str[cursor_pos+1:]
        elif key_press == ord('-') and cursor_pos == 0 and '-' not in input_str:
            input_str = '-' + input_str
            cursor_pos += 1
        elif key_press == ord('.') and '.' not in input_str:
            input_str = input_str[:cursor_pos] + '.' + input_str[cursor_pos:]
            cursor_pos += 1
        elif 48 <= key_press <= 57:
            if len(input_str) < 20:
                input_str = input_str[:cursor_pos] + chr(key_press) + input_str[cursor_pos:]
                cursor_pos += 1


def run_exercise2_ui_control2(stdscr, control_obj):
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

    inputs = {"a": "", "b": "", "h": ""}
    selected_idx = 0
    fields = list(inputs.keys())
    result_list = []
    error_str = ""
    calc_highlighted = False

    while True:
        stdscr.clear()
        draw_formula(stdscr, h, w)
        draw_input_fields(stdscr, inputs, selected_idx, h, w)
        draw_calc_button(stdscr, h, w, calc_highlighted)

        if error_str:
            stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))

        instructions = "↑↓ to move, ENTER to edit, DEL/BACK to clear, C to CALC, ESC to exit"
        stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
        stdscr.refresh()

        key = stdscr.getch()

        if key == 27:
            break
        elif key == curses.KEY_UP:
            selected_idx = (selected_idx - 1) % len(fields)
        elif key == curses.KEY_DOWN:
            selected_idx = (selected_idx + 1) % len(fields)
        elif key in [10, 13]:
            current_key = fields[selected_idx]
            old_val = inputs[current_key]
            new_val = get_number_input(stdscr, current_key, old_val)
            if new_val is not None:
                inputs[current_key] = new_val
        elif key in [curses.KEY_DC, 127, 8]:
            inputs[fields[selected_idx]] = ""
        elif key in [ord('c'), ord('C')]:
            calc_highlighted = True
            stdscr.refresh()
            curses.napms(100)

            try:
                a = float(inputs["a"]) if inputs["a"] != "" else 0.0
                b = float(inputs["b"]) if inputs["b"] != "" else 1.0
                h_val = float(inputs["h"]) if inputs["h"] != "" else 0.1

                result_list, error = control_obj.exercise_2(a, b, h_val)

                if error:
                    error_str = error
                    result_list = []
                else:
                    error_str = ""
                    display_table_with_animation(stdscr, result_list, h, w)

            except ValueError as e:
                error_str = f"Error: Invalid number format"
            except Exception as e:
                error_str = f"Error: {str(e)}"

            calc_highlighted = False

    curses.curs_set(1)