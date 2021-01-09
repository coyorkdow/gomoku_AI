#include "AI.h"

#include <stdio.h>

void init_AI() {
  for (int i = 0; i < 15; i++)
    for (int j = 0; j < 15; j++) board[i][j] = BLANK;

  dir_vec[0].x = 0, dir_vec[0].y = -1;
  dir_vec[1].x = 0, dir_vec[1].y = 1;

  dir_vec[2].x = -1, dir_vec[2].y = 0;
  dir_vec[3].x = 1, dir_vec[3].y = 0;

  dir_vec[4].x = -1, dir_vec[4].y = -1;
  dir_vec[5].x = 1, dir_vec[0].y = 1;

  dir_vec[6].x = -1, dir_vec[6].y = 1;
  dir_vec[7].x = 1, dir_vec[7].y = -1;
}

static inline bool has_neighbor(int x, int y) {
  int nx, ny;
  for (int i = 0; i < 8; i++) {
    nx = x + dir_vec[i].x, ny = y + dir_vec[i].y;
    if (!OUT_OF_BORDER(nx, ny) && board[nx][ny] != BLANK) return true;
  }
  return false;
}

double negmax(STATE player, pos pre, int depth, double alpha, double beta) {
  if (depth == 0 || player_win(!player, pre)) {
    return -estimate_score(player);
  }

  double val;
  pos p;
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      if (board[i][j] != BLANK) continue;
      if (!has_neighbor(i, j)) continue;
      board[i][j] = player;
      p = (pos){i, j, player};
      append(&search_stack, p);

      val = negmax(!player, p, depth - 1, -beta, -alpha);

      board[i][j] = BLANK;
      pop(&search_stack);

      if (val > alpha) {
        if (depth == DEPTH) {
          printf("val=%d\n", val);
          nxt_step = p;
        }
        if (val >= beta) {
          return -val;
        }
        alpha = val;
      }
    }
  }

  return -alpha;
}

typedef struct shape {
  int len, player_pieces_cnt;
  int front_space, back_space;
  STATE buffer[MAXLEN];
} shape;

static inline shape get_shape_through_direction(STATE player, pos p, pos vec) {
  assert(player == BLACK || player == WHITE);

  int x = p.x, y = p.y;
  shape S;

  while (!OUT_OF_BORDER(x, y) && board[x][y] == player) {
    x -= vec.x, y -= vec.y;
  }

  if (!OUT_OF_BORDER(x, y) && board[x][y] == BLANK)
    S.front_space = 1;
  else
    S.front_space = 0;

  if (OUT_OF_BORDER(x, y) || board[x][y] != player) x += vec.x, y += vec.y;

  S.len = 0, S.player_pieces_cnt = 0;

#define DRAW_SHAPE                                          \
  do {                                                      \
    while (!OUT_OF_BORDER(x, y) && board[x][y] == player) { \
      S.buffer[S.len++] = board[x][y];                      \
      if (board[x][y] == player) S.player_pieces_cnt++;     \
      x += vec.x, y += vec.y;                               \
    }                                                       \
  } while (0)

  DRAW_SHAPE;
  // if (!OUT_OF_BORDER(x, y) && board[x][y] == BLANK) {
  //   S.buffer[S.len++] = BLANK;
  //   x += vec.x, y += vec.y;
  // }
  // DRAW_SHAPE;

#undef DRAW_SHAPE

  if (!OUT_OF_BORDER(x, y) && board[x][y] == BLANK)
    S.back_space = 1;
  else
    S.back_space = 0;

  return S;
}

static inline double calculate_score_through_shape(shape s) {

  int space = s.front_space + s.back_space;

  double val;

  if (s.player_pieces_cnt >= 5) {
    val = WIN;
    // printf("s.player_pieces_cnt >= 5, val=%lf\n", val);
  } else if (s.player_pieces_cnt == 4) {
    val = NXT_STEP_WIN;
    // printf("s.player_pieces_cnt == 4, val=%lf\n", val);
  } else if (s.player_pieces_cnt == 3) {
    val = OPPONENT_MUST_DEFEND;
    // puts("s.player_pieces_cnt == 3");
    if (space == 0) {
      val = USELESS;
    }
  } else if (s.player_pieces_cnt == 2) {
    val = NORMAL_2;
  } else if (space == 0) {
    val = USELESS;
  } else {
    val = NORMAL_0;
  }

  return val;
}

double estimate_score(STATE player) {
  double player_score = 0, opponent_score = 0;
  for (int i = 0; i < search_stack.len; i++) {
    for (int j = 0; j < 8; j += 2) {
      if (search_stack.list[i].state == player) {
        player_score +=
            calculate_score_through_shape(get_shape_through_direction(
                player, search_stack.list[i], dir_vec[j]));

      } else {
        opponent_score +=
            calculate_score_through_shape(get_shape_through_direction(
                !player, search_stack.list[i], dir_vec[j]));
      }
    }
  }
  return player_score - RATIO * opponent_score;
}

bool player_win(STATE player, pos p) {
  shape s;
  for (int i = 0; i < 8; i++) {
    s = get_shape_through_direction(player, p, dir_vec[i]);
    if (s.player_pieces_cnt == 5 && s.len == 5) return true;
  }
  return false;
}

static inline void append(struct pieces_list* l, pos p) {
  assert(l->len < MAXLEN);
  l->list[l->len++] = p;
}

static inline void pop(struct pieces_list* l) {
  assert(l->len >= 0);
  l->len--;
}
