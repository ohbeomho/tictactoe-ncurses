#include <locale.h>
#include <ncursesw/ncurses.h>

#define equal3(a, b, c) ((a) == (b) && (b) == (c))

char players[3] = {' ', 'O', 'X'};
char menu[2][10] = {"시작", "종료"};

int board[3][3] = {{0}};

int menuSelect = 0;
int selectY = 0, selectX = 0;
int currentPlayer = 1, currentScreen = 0;

void printPlayer(int player, int y, int x) {
  int selected = selectY == y && selectX == x;
  mvaddch(2 + 2 * y, 2 + 2 * x,
          players[player] | COLOR_PAIR(selected ? 3 : player));
}

int checkBoard() {
  int stalemate = 1;

  for (int i = 0; i < 3; i++) {
    int hor = 1, ver = 1;

    for (int j = 0; j < 3; j++) {
      if (!board[i][j])
        stalemate = 0;

      if (!board[i][j] || board[i][j] != board[i][0])
        hor = 0;
      if (!board[j][i] || board[j][i] != board[0][i])
        ver = 0;
    }

    if (hor)
      return board[i][0];
    else if (ver)
      return board[0][i];
  }

  if (stalemate)
    return 0;

  if (board[1][1] && (equal3(board[0][0], board[1][1], board[2][2]) ||
                      equal3(board[0][2], board[1][1], board[2][0])))
    return board[1][1];

  return -1;
}

int mainScreen() {
  mvprintw(0, 1, "TIC TAC TOE");

  for (int i = 0; i < 2; i++) {
    if (i == menuSelect)
      attron(COLOR_PAIR(3));
    mvprintw(2 + i, 0, (i == menuSelect) ? "> %s " : "  %s ", menu[i]);
    if (i == menuSelect)
      attroff(COLOR_PAIR(3));
  }

  refresh();

  int ch = getch();

  if (ch == KEY_UP)
    menuSelect--;
  if (ch == KEY_DOWN)
    menuSelect++;
  if (ch == '\n') {
    if (menuSelect == 0) {
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          board[i][j] = 0;
        }
      }

      selectY = 0;
      selectX = 0;

      return 2;
    } else
      return -1;
  }

  if (menuSelect > 1)
    menuSelect = 0;
  else if (menuSelect < 0)
    menuSelect = 1;

  return 0;
}

int gameScreen() {
  for (int i = 0; i <= 3; i++) {
    mvhline(1 + 2 * i, 1, '-', 7);
  }

  for (int i = 0; i <= 3; i++) {
    mvvline(1, 1 + 2 * i, '|', 7);
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      printPlayer(board[i][j], i, j);
    }
  }

  int winner = checkBoard();

  if (winner != -1) {
    move(9, 0);
    clrtoeol();

    if (winner == 0) {
      mvprintw(9, 0, "무승부!");
    } else {
      mvaddch(9, 0, players[winner] | COLOR_PAIR(winner));
      mvprintw(9, 2, "승리!");
    }

    mvprintw(10, 0, "아무 키나 눌러 메뉴로 이동");
    getch();
    return 1;
  }

  refresh();

  int ch = getch();

  switch (ch) {
  case KEY_UP:
    selectY--;
    break;
  case KEY_DOWN:
    selectY++;
    break;
  case KEY_LEFT:
    selectX--;
    break;
  case KEY_RIGHT:
    selectX++;
    break;
  case '\n':
    if (!board[selectY][selectX]) {
      board[selectY][selectX] = currentPlayer;
      currentPlayer = (currentPlayer == 1) ? 2 : 1;
      move(9, 0);
      clrtoeol();
    } else
      mvprintw(9, 0, "다른 곳을 선택해 주세요.");
  }

  if (selectY > 2)
    selectY = 2;
  else if (selectY < 0)
    selectY = 0;

  if (selectX > 2)
    selectX = 2;
  else if (selectX < 0)
    selectX = 0;

  return 0;
}

int main() {
  setlocale(LC_ALL, "");

  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_BLACK, COLOR_WHITE);

  int (*screens[2])() = {mainScreen, gameScreen};

  while (1) {
    int s = screens[currentScreen]();

    if (s == -1)
      break;
    else if (s) {
      clear();
      currentScreen = s - 1;
    }
  }

  endwin();

  return 0;
}
