# main
import curses
from core import Control1, Control2, Control3

from exercise1_ui import run_exercise1_ui
from exercise2_ui import run_exercise2_ui
from exercise3_ui import run_exercise3_ui

from exercise1_ui_control2 import run_exercise1_ui_control2
from exercise2_ui_control2 import run_exercise2_ui_control2
from exercise3_ui_control2 import run_exercise3_ui_control2

from exercise1_ui_control3 import run_exercise1_ui_control3
from exercise2_ui_control3 import run_exercise2_ui_control3
from exercise3_ui_control3 import run_exercise3_ui_control3

# Список контрольных работ
CONTROLS = [
    Control1(),
    Control2(),
    Control3()
]

# Глобальное состояние
selected_control_index = 0
selected_exercise_index = 0
current_screen = "menu"  # "menu", "exercise_menu", "about", "exit_confirm"
is_running = True

# Логотип
LOGO = [
    "╔═════════════════════════════════════╗",
    "║              keith_bro              ║",
    "║           STUDENT TESTING           ║",
    "║               SYSTEM                ║",
    "╚═════════════════════════════════════╝"
]


def draw_menu(stdscr):
    """Отображение главного меню с логотипом и цветами"""
    stdscr.clear()
    h, w = stdscr.getmaxyx()

    # Цвета
    title_color = curses.color_pair(1)
    menu_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)
    footer_color = curses.color_pair(4)

    # Рисуем логотип по центру
    logo_start_y = 1
    for i, line in enumerate(LOGO):
        x = (w // 2) - len(line) // 2
        stdscr.addstr(logo_start_y + i, x, line, title_color)

    instructions = "Use ↑↓ to navigate, ENTER to select, Q to quit, A for About"
    stdscr.addstr(logo_start_y + len(LOGO) + 1, (w // 2) - len(instructions) // 2, instructions, menu_color)

    start_y = logo_start_y + len(LOGO) + 3
    for idx, control in enumerate(CONTROLS):
        title = f"► Control {idx + 1} ◄"
        x = w // 2 - len(title) // 2
        y = start_y + idx * 2

        if idx == selected_control_index:
            stdscr.addstr(y, x, title, highlight_color)
        else:
            stdscr.addstr(y, x, title, menu_color)

    # Кнопка выхода в меню
    exit_title = f"► Exit System ◄"
    x = w // 2 - len(exit_title) // 2
    y = start_y + len(CONTROLS) * 2
    if selected_control_index == len(CONTROLS):  # Если выбрана кнопка выхода
        stdscr.addstr(y, x, exit_title, highlight_color)
    else:
        stdscr.addstr(y, x, exit_title, menu_color)

    # Дополнительные кнопки
    extra_btn = "A - About"
    stdscr.addstr(start_y + len(CONTROLS) * 2 + 2, 2, extra_btn, menu_color)

    footer = "© 2025 keith_bro."
    stdscr.addstr(h - 2, (w // 2) - len(footer) // 2, footer, footer_color)

    stdscr.refresh()


def draw_exit_confirmation(stdscr):
    """Экран подтверждения выхода"""
    stdscr.clear()
    h, w = stdscr.getmaxyx()

    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)

    header = "╔══════════════════════════════════════╗"
    stdscr.addstr(h // 2 - 3, (w // 2) - len(header) // 2, header, title_color)
    title = "║         EXIT CONFIRMATION            ║"
    stdscr.addstr(h // 2 - 2, (w // 2) - len(title) // 2, title, title_color)
    separator = "╠══════════════════════════════════════╣"
    stdscr.addstr(h // 2 - 1, (w // 2) - len(separator) // 2, separator, title_color)

    question = "║     Are you sure you want to exit?   ║"
    stdscr.addstr(h // 2, (w // 2) - len(question) // 2, question, text_color)

    yes_btn = "► YES ◄"
    no_btn = "► NO ◄"

    yes_x = (w // 2) - len(yes_btn) // 2 - 8
    no_x = (w // 2) - len(no_btn) // 2 + 8

    stdscr.addstr(h // 2 + 1, yes_x, yes_btn, highlight_color)  # Выделенный вариант по умолчанию
    stdscr.addstr(h // 2 + 1, no_x, no_btn, text_color)

    end = "╚══════════════════════════════════════╝"
    stdscr.addstr(h // 2 + 2, (w // 2) - len(end) // 2, end, title_color)

    stdscr.refresh()

    # Обработка выбора в подтверждении
    selected_exit_option = 0  # 0 - YES, 1 - NO

    while True:
        key = stdscr.getch()

        if key == curses.KEY_LEFT or key == curses.KEY_RIGHT:
            selected_exit_option = 1 - selected_exit_option  # Переключение между YES/NO

            # Перерисовка кнопок с выделением
            stdscr.addstr(h // 2 + 1, yes_x, yes_btn, highlight_color if selected_exit_option == 0 else text_color)
            stdscr.addstr(h // 2 + 1, no_x, no_btn, highlight_color if selected_exit_option == 1 else text_color)
            stdscr.refresh()

        elif key == curses.KEY_ENTER or key in [10, 13]:
            if selected_exit_option == 0:  # YES
                return True
            else:  # NO
                return False
        elif key == 27:  # ESC - отмена
            return False


def draw_exercise_menu(stdscr, control_obj):
    """Отображение меню выбора задания внутри контрольной"""
    stdscr.clear()
    h, w = stdscr.getmaxyx()

    title_color = curses.color_pair(1)
    menu_color = curses.color_pair(2)
    highlight_color = curses.color_pair(3)
    footer_color = curses.color_pair(4)

    header = f"╔══════════════════════════════════════╗"
    stdscr.addstr(1, (w // 2) - len(header) // 2, header, title_color)
    title = f"║               {control_obj.title}              ║"
    stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
    end = "╚══════════════════════════════════════╝"
    stdscr.addstr(3, (w // 2) - len(end) // 2, end, title_color)

    instructions = "Use ↑↓ to navigate, ENTER to start, ESC to go back"
    stdscr.addstr(5, (w // 2) - len(instructions) // 2, instructions, menu_color)

    start_y = 7
    for i in range(1, 4):
        title = f"► Exercise {i} ◄"
        x = w // 2 - len(title) // 2
        y = start_y + (i - 1) * 2

        if i - 1 == selected_exercise_index:
            stdscr.addstr(y, x, title, highlight_color)
        else:
            stdscr.addstr(y, x, title, menu_color)

    footer = "© 1987 keith_bro Corp. All rights reserved."
    stdscr.addstr(h - 2, (w // 2) - len(footer) // 2, footer, footer_color)

    stdscr.refresh()


def draw_about(stdscr):
    """Отображение экрана 'О программе'"""
    stdscr.clear()
    h, w = stdscr.getmaxyx()

    title_color = curses.color_pair(1)
    text_color = curses.color_pair(2)

    header = "╔══════════════════════════════════════╗"
    stdscr.addstr(2, (w // 2) - len(header) // 2, header, title_color)
    title = "║             ABOUT SYSTEM             ║"
    stdscr.addstr(3, (w // 2) - len(title) // 2, title, title_color)
    end = "╚══════════════════════════════════════╝"
    stdscr.addstr(4, (w // 2) - len(end) // 2, end, title_color)

    lines = [
        "",
        "System: Student Testing System",
        "Version: 1.0",
        "Developer: keith_bro",
        "Language: Python + Curses",
        "Style: MS-DOS inspired",
        "",
        "Controls:",
        " - Arrow keys: navigate",
        " - Enter: select",
        " - Esc: go back",
        " - Q: quit",
        " - A: about",
        "",
        "Press any key to return..."
    ]

    for i, line in enumerate(lines):
        stdscr.addstr(6 + i, (w // 2) - len(line) // 2, line, text_color)

    stdscr.refresh()
    stdscr.getch()  # Ждём нажатия


def run_exercise(stdscr, control_obj, ex_num):
    """Запуск упражнения с графическим интерфейсом"""
    if ex_num == 1 and isinstance(control_obj, Control1):
        run_exercise1_ui(stdscr, control_obj)
    elif ex_num == 2 and isinstance(control_obj, Control1):
        run_exercise2_ui(stdscr, control_obj)
    elif ex_num == 3 and isinstance(control_obj, Control1):
        run_exercise3_ui(stdscr, control_obj)
    elif ex_num == 1 and isinstance(control_obj, Control2):
        run_exercise1_ui_control2(stdscr, control_obj)
    elif ex_num == 2 and isinstance(control_obj, Control2):
        run_exercise2_ui_control2(stdscr, control_obj)
    elif ex_num == 3 and isinstance(control_obj, Control2):
        run_exercise3_ui_control2(stdscr, control_obj)
    elif ex_num == 1 and isinstance(control_obj, Control3):
        run_exercise1_ui_control3(stdscr, control_obj)
    elif ex_num == 2 and isinstance(control_obj, Control3):
        run_exercise2_ui_control3(stdscr, control_obj)
    elif ex_num == 3 and isinstance(control_obj, Control3):
        run_exercise3_ui_control3(stdscr, control_obj)
    else:
        # Для других упражнений пока заглушка
        stdscr.clear()
        h, w = stdscr.getmaxyx()
        title_color = curses.color_pair(1)
        text_color = curses.color_pair(2)

        title = f"╔══════════════════════════════════════╗"
        stdscr.addstr(2, (w // 2) - len(title) // 2, title, title_color)
        header = f"║       {control_obj.title} - Exercise {ex_num}       ║"
        stdscr.addstr(3, (w // 2) - len(header) // 2, header, title_color)
        end = "╚══════════════════════════════════════╝"
        stdscr.addstr(4, (w // 2) - len(end) // 2, end, title_color)

        stdscr.addstr(7, 2, f"Exercise {ex_num} is not implemented yet.", text_color)
        stdscr.addstr(h - 3, 2, "Press ESC to return...", text_color)

        while True:
            key = stdscr.getch()
            if key == 27:  # ESC
                break


def main(stdscr):
    global selected_control_index, selected_exercise_index, current_screen, is_running

    curses.curs_set(0)
    stdscr.nodelay(False)
    stdscr.keypad(True)

    # Цвета
    if curses.has_colors():
        curses.start_color()
        curses.init_pair(1, curses.COLOR_GREEN, curses.COLOR_BLACK)  # Заголовки
        curses.init_pair(2, curses.COLOR_WHITE, curses.COLOR_BLACK)  # Текст
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_WHITE)  # Выделение
        curses.init_pair(4, curses.COLOR_CYAN, curses.COLOR_BLACK)  # Подвал

    while is_running:
        if current_screen == "menu":
            draw_menu(stdscr)
        elif current_screen == "exercise_menu":
            draw_exercise_menu(stdscr, CONTROLS[selected_control_index])
        elif current_screen == "about":
            draw_about(stdscr)
            current_screen = "menu"
        elif current_screen == "exit_confirm":
            if draw_exit_confirmation(stdscr):
                is_running = False
            else:
                current_screen = "menu"

        key = stdscr.getch()

        if current_screen == "menu":
            if key == ord('q') or key == ord('Q'):
                is_running = False
            elif key == ord('a') or key == ord('A'):
                current_screen = "about"
            elif key == curses.KEY_UP:
                selected_control_index = (selected_control_index - 1) % (len(CONTROLS) + 1)  # +1 для кнопки выхода
            elif key == curses.KEY_DOWN:
                selected_control_index = (selected_control_index + 1) % (len(CONTROLS) + 1)  # +1 для кнопки выхода
            elif key == curses.KEY_ENTER or key in [10, 13]:
                if selected_control_index == len(CONTROLS):  # Если выбрана кнопка выхода
                    current_screen = "exit_confirm"
                else:
                    current_screen = "exercise_menu"
                    selected_exercise_index = 0

        elif current_screen == "exercise_menu":
            if key == 27:  # ESC
                current_screen = "menu"
            elif key == curses.KEY_UP:
                selected_exercise_index = (selected_exercise_index - 1) % 3
            elif key == curses.KEY_DOWN:
                selected_exercise_index = (selected_exercise_index + 1) % 3
            elif key == curses.KEY_ENTER or key in [10, 13]:
                run_exercise(stdscr, CONTROLS[selected_control_index], selected_exercise_index + 1)


if __name__ == "__main__":
    curses.wrapper(main)