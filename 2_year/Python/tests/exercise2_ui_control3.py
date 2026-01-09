# exercise2_ui_control3.py

import curses
from typing import List
from core import Control3

def draw_formula(stdscr, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    formula_lines = [
        "╔══════════════════════════════════════════════════╗",
        "║        SUM OF ELEMENTS GREATER THAN P            ║",
        "║   Given: x₁, x₂, ..., xₙ  (n ≤ 20)               ║",
        "║   Compute: Σ xᵢ  for all xᵢ > p                  ║",
        "║   Enter sequence and threshold p                 ║",
        "╚══════════════════════════════════════════════════╝"
    ]

    start_y = h // 2 - len(formula_lines) // 2
    for i, line in enumerate(formula_lines):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(start_y + i, x, line, title_color if i == 1 else text_color)


def draw_input_fields(stdscr, seq_str: str, p_str: str, active_field: str, h, w):
    text_color = curses.color_pair(2)
    edit_color = curses.color_pair(3)

    # Sequence input
    seq_label = "Sequence: "
    seq_line = seq_label + seq_str
    y_seq = h // 2 + 6
    x_seq = (w // 2) - len(seq_line) // 2
    seq_col = edit_color if active_field == 'seq' else text_color
    stdscr.addstr(y_seq, x_seq, seq_line, seq_col)

    # Threshold p input
    p_label = "p = "
    p_line = p_label + p_str
    y_p = h // 2 + 8
    x_p = (w // 2) - len(p_line) // 2
    p_col = edit_color if active_field == 'p' else text_color
    stdscr.addstr(y_p, x_p, p_line, p_col)


def draw_calc_button(stdscr, h, w, is_highlighted=False):
    calc_color = curses.color_pair(3) if is_highlighted else curses.color_pair(2)
    calc_line = "► CALC ◄"
    y = h // 2 + 11
    x = (w // 2) - len(calc_line) // 2
    stdscr.addstr(y, x, calc_line, calc_color)


def display_result(stdscr, sequence: List[float], p: float, total: float, h, w):
    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)
    header_color = curses.color_pair(3)

    stdscr.clear()

    header = "╔════════════════════════════════════════════╗"
    stdscr.addstr(1, (w // 2) - len(header) // 2, header, title_color)
    title = "║             COMPUTATION RESULT             ║"
    stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
    sep = "╠════════════════════════════════════════════╣"
    stdscr.addstr(3, (w // 2) - len(sep) // 2, sep, title_color)

    seq_str = "[" + ", ".join(f"{x:.3f}" for x in sequence) + "]"
    result_str = f"Sum of x > {p:.3f} = {total:.3f}"

    stdscr.addstr(5, (w // 2) - len(seq_str) // 2, seq_str, text_color)
    stdscr.addstr(6, (w // 2) - len(result_str) // 2, result_str, header_color)

    footer = "╚════════════════════════════════════════════╝"
    stdscr.addstr(8, (w // 2) - len(footer) // 2, footer, title_color)

    prompt = "Press any key to continue..."
    stdscr.addstr(h - 2, (w // 2) - len(prompt) // 2, prompt, text_color)
    stdscr.refresh()
    stdscr.getch()


def parse_float_list(s: str) -> List[float]:
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

    seq_str = "1.5, -2, 3.7, 0"
    p_str = "0.5"
    active_field = None  # 'seq' or 'p'
    error_str = ""
    calc_highlighted = False

    while True:
        stdscr.clear()
        draw_formula(stdscr, h, w)
        draw_input_fields(stdscr, seq_str, p_str, active_field, h, w)
        draw_calc_button(stdscr, h, w, calc_highlighted)

        if error_str:
            stdscr.addstr(h - 4, (w // 2) - len(error_str) // 2, error_str, curses.color_pair(4))

        instructions = "E/S to edit Seq, E/P to edit p, C to CALC, ESC to exit"
        stdscr.addstr(h - 2, (w // 2) - len(instructions) // 2, instructions, curses.color_pair(2))
        stdscr.refresh()

        key = stdscr.getch()

        if key == 27:  # ESC
            break

        elif key in [ord('s'), ord('S')]:
            active_field = 'seq'
            curses.curs_set(1)
            y = h // 2 + 6
            label_len = len("Sequence: ")
            x_start = (w // 2) - len("Sequence: " + seq_str) // 2 + label_len
            _edit_string(stdscr, y, x_start, seq_str, lambda s: setattr(run_exercise2_ui_control3, '_temp_seq', s))
            seq_str = getattr(run_exercise2_ui_control3, '_temp_seq', seq_str)
            active_field = None
            curses.curs_set(0)

        elif key in [ord('p'), ord('P')]:
            active_field = 'p'
            curses.curs_set(1)
            y = h // 2 + 8
            label_len = len("p = ")
            x_start = (w // 2) - len("p = " + p_str) // 2 + label_len
            _edit_string(stdscr, y, x_start, p_str, lambda s: setattr(run_exercise2_ui_control3, '_temp_p', s))
            p_str = getattr(run_exercise2_ui_control3, '_temp_p', p_str)
            active_field = None
            curses.curs_set(0)

        elif key in [ord('c'), ord('C')]:
            calc_highlighted = True
            stdscr.refresh()
            curses.napms(100)
            error_str = ""

            try:
                sequence = parse_float_list(seq_str)
                p_val = float(p_str.strip())

                if len(sequence) == 0:
                    error_str = "Error: Sequence cannot be empty"
                elif len(sequence) > 20:
                    error_str = "Error: Max 20 elements allowed"
                else:
                    total, err = control_obj.exercise_2(sequence, p_val)
                    if err:
                        error_str = err
                    else:
                        display_result(stdscr, sequence, p_val, total, h, w)

            except ValueError:
                error_str = "Error: Invalid number format"
            except Exception as e:
                error_str = f"Error: {str(e)}"

            calc_highlighted = False

    curses.curs_set(1)


def _edit_string(stdscr, y: int, x_start: int, initial: str, save_callback):
    """Вспомогательная функция для редактирования строки."""
    temp_str = initial
    cursor = len(temp_str)

    while True:
        stdscr.addstr(y, x_start, temp_str + " " * 30, curses.color_pair(3))
        stdscr.move(y, x_start + cursor)
        stdscr.refresh()

        k = stdscr.getch()
        if k == 27:  # ESC — отмена
            break
        elif k in [10, 13]:  # ENTER — сохранить
            save_callback(temp_str)
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
        elif 32 <= k <= 126:
            if len(temp_str) < 60:
                temp_str = temp_str[:cursor] + chr(k) + temp_str[cursor:]
                cursor += 1