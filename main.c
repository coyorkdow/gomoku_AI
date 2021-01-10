#include <stdio.h>
#include <stdlib.h>

#include "AI.h"
#define EDGE 2
char display_board[1 + 15 * 2][EDGE + 15 * 3 + 16 + 1];

char color[2] = {'B', 'W'};

static inline void put_piece(int x, int y, STATE player) {
  display_board[x * 2 - 1][y * 4] = color[player];
  board[x - 1][y - 1] = player;
}

void init_board() {
  //  ___________
  // 1 | w | B |
  //  |___|___|
  // 2 | B |
  //  |___|

  for (int i = 0; i <= 15 * 2; i++)
    for (int j = 0; j < EDGE + 15 * 3 + 16; j++) display_board[i][j] = ' ';

  int cnt = 0;
  for (int i = 1; i <= 15 * 2; i++) {
    display_board[i][EDGE] = '|';
    display_board[i][EDGE + 15 * 3 + 15] = '|';
    if (i & 1) {
      cnt++;
      if (cnt < 10) {
        display_board[i][0] = cnt + '0';
      } else {
        display_board[i][0] = '1';
        display_board[i][1] = (cnt - 10) + '0';
      }
    }
    for (int j = EDGE + 1; j < EDGE + 15 * 3 + 15; j++) {
      if ((j - EDGE) % 4 == 0)
        display_board[i][j] = '|';
      else if (i % 2 == 0)
        display_board[i][j] = '_';
    }
  }

  cnt = 0;
  for (int i = EDGE; i < EDGE + 15 * 3 + 16; i++) {
    display_board[0][i] = '_';
    if ((i - EDGE - 2) % 4 == 0) {
      cnt++;
      if (cnt < 10) {
        display_board[0][i - 1] = cnt + '0';
      } else {
        display_board[0][i - 1] = '1';
        display_board[0][i] = (cnt - 10) + '0';
      }
    }
  }
}

static inline void draw_board() {
  for (int i = 0; i < 15 * 2 + 1; i++) puts(display_board[i]);
}

int main() {
  init_AI();
  init_board();
  draw_board();

  int cnt = 0;
  int x, y;
  while (++cnt <= MAXLEN) {
    if (cnt & 1) {
      puts("yout play black");
      puts("input coordinate (x, y), i.e: 4 5");
      scanf("%d%d", &x, &y);
      assert(board[x - 1][y - 1] == BLANK);
      put_piece(x, y, BLACK);
      system("cls");
      draw_board();

      if (player_win(BLACK, (pos){x - 1, y - 1})) {
        puts("you are win");
        return 0;
      }
    } else {
      negmax(WHITE, (pos){}, DEPTH, -1e9, 1e9);
      put_piece(nxt_step.x + 1, nxt_step.y + 1, WHITE);
      system("cls");
      draw_board();

      if (player_win(WHITE, nxt_step)) {
        puts("you are lose");
        return 0;
      }
    }
  }
  puts("there is a draw, game over");
}
