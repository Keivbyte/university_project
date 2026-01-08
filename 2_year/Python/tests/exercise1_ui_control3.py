# exercise1_ui_control3.py

import curses
import time
import math

def draw_formula(stdscr, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════╗",
        "║                SERIES SUMMATION                  ║",
        "║        n                                         ║",
        "║       ┌───┐                                      ║",
        "║        \     cosᵏ(x)                             ║",
        "║   S =  /     ───────                             ║",
        "║       └───┘     k!                               ║",
        "║       k = 2                                      ║",
        "║                     Default: x = π/8, n = 10     ║",
        "╚══════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_fields(stdscr, inputs, selected_idx, h, w):
    menu_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    fields = ["x", "n"]
    start_y = h // 2 + 8

    for i, key in enumerate(fields):
        label = f"{key} = "
        value = str(inputs[key])
        line = label + value
        color = highlight_color if i == selected_idx else menu_color
        stdscr.addstr(start_y + i, (w // 2) - len(line) // 2, line, color)


def draw_calc_button(stdscr, h, w, is_highlighted=False):
    calc_color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    calc_line = "► CALC ◄"
    y = h // 2 + 13
    x = (w // 2) - len(calc_line) // 2
    stdscr.addstr(y, x, calc_line, calc_color)


def display_result(stdscr, result, x_val, n_val, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    stdscr.clear()

    header = "╔════════════════════════════════════╗"
    stdscr.addstr(2, (w // 2) - len(header) // 2, header, title_color)
    title = "║           RESULT DISPLAY           ║"
    stdscr.addstr(3, (w // 2) - len(title) // 2, title, title_color)
    sep = "╠════════════════════════════════════╣"
    stdscr.addstr(4, (w // 2) - len(sep) // 2, sep, title_color)

    lines = [
        f"x = {x_val:.6f}",
        f"n = {int(n_val)}",
        f"S = {result:.8f}"
    ]

    for i, line in enumerate(lines):
        stdscr.addstr(6 + i, (w // 2) - 10, line, text_color)

    footer = "╚════════════════════════════════════╝"
    stdscr.addstr(9, (w // 2) - len(footer) // 2, footer, title_color)

    prompt = "Press any key to continue..."
    stdscr.addstr(h - 2, (w // 2) - len(prompt) // 2, prompt, text_color)
    stdscr.refresh()
    stdscr.getch()


def get_number_input(stdscr, key, old_value=""):
    h, w = stdscr.getmaxyx()
    y = h // 2 + 8 + ["x", "n"].index(key)
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
                val = float(cleaned)
                if key == "n":
                    if val < 2 or val != int(val):
                        return old_value
                    return int(val)
                return val
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
        elif key_press == curses.KEY_DC and cursor_pos < len(input_str):
            input_str = input_str[:cursor_pos] + input_str[cursor_pos+1:]
        elif key_press == ord('-') and cursor_pos == 0 and '-' not in input_str:
            input_str = '-' + input_str
            cursor_pos += 1
        elif key_press == ord('.') and '.' not in input_str and key == "x":
            input_str = input_str[:cursor_pos] + '.' + input_str[cursor_pos:]
            cursor_pos += 1
        elif 48 <= key_press <= 57:
            if len(input_str) < 20:
                input_str = input_str[:cursor_pos] + chr(key_press) + input_str[cursor_pos:]
                cursor_pos += 1


def run_exercise1_ui_control3(stdscr, control_obj):
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

    # Умолчания
    inputs = {"x": str(math.pi / 8), "n": "10"}
    selected_idx = 0
    fields = list(inputs.keys())
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
            if new_val != "":
                inputs[current_key] = new_val
        elif key in [curses.KEY_DC, 127, 8]:
            inputs[fields[selected_idx]] = ""
        elif key in [ord('c'), ord('C')]:
            calc_highlighted = True
            stdscr.refresh()
            curses.napms(100)

            try:
                x_val = float(inputs["x"]) if inputs["x"] != "" else math.pi / 8
                n_val = int(inputs["n"]) if inputs["n"] != "" else 10

                if n_val < 2:
                    error_str = "Error: n must be integer ≥ 2"
                else:
                    result, error = control_obj.exercise_1(x=x_val, n=n_val)
                    if error:
                        error_str = error
                    else:
                        error_str = ""
                        display_result(stdscr, result, x_val, n_val, h, w)

            except Exception as e:
                error_str = f"Error: {str(e)}"

            calc_highlighted = False

    curses.curs_set(1)