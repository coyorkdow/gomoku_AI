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

double negmax(STATE player, pos pre, int depth, double alpha, double beta) {
  if (depth == 0 || player_win(!player, pre)) {
    return estimate_score(player);
  }

  double val;
  pos p;
  for (int i = 0; i < 15; i++) {
    for (int j = 0; j < 15; j++) {
      if (board[i][j] != BLANK) continue;
      board[i][j] = player;
      p = (pos){i, j, player};
      append(&search_stack, p);

      val = negmax(!player, p, depth - 1, -beta, -alpha);

      board[i][j] = BLANK;
      pop(&search_stack);

      if (val > alpha) {
        if (depth == DEPTH) {
          nxt_step = p;
        }
        if (val > beta) {
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
  if (!OUT_OF_BORDER(x, y) && board[x][y] == BLANK) {
    S.buffer[S.len++] = BLANK;
    x += vec.x, y += vec.y;
  }
  DRAW_SHAPE;

#undef DRAW_SHAPE

  if (!OUT_OF_BORDER(x, y) && board[x][y] == BLANK)
    S.back_space = 1;
  else
    S.back_space = 0;

  return S;
}

static inline double calculate_score_through_shape(shape s) {
  int space = s.front_space + s.back_space;
  int front_pieces_cnt = 0;
  for (int i = 0; i < s.len && s.buffer[i] != BLANK; i++) front_pieces_cnt++;

  if (s.player_pieces_cnt >= 5) {
    puts("s.player_pieces_cnt >= 5");
    if (s.len == s.player_pieces_cnt) return WIN;
    if (space == 0) return OPPONENT_MUST_DEFEND;
    if (front_pieces_cnt == 1 && s.back_space) {
      return NXT_STEP_WIN;
    }
    if (front_pieces_cnt == 4 && s.front_space) {
      return NXT_STEP_WIN;
    }
    return OPPONENT_MUST_DEFEND;
  }
  if (s.player_pieces_cnt == 4) {
    printf("s.player_pieces_cnt == 4, s.front_space =%d s.back_space=%d\n",
           s.front_space, s.back_space);
    if (s.len == s.player_pieces_cnt) {
      if (space == 2) return NXT_STEP_WIN;
      if (space == 1) return OPPONENT_MUST_DEFEND;
      return USELESS;
    }
    return OPPONENT_MUST_DEFEND;
  }
  if (s.player_pieces_cnt == 3) {
    // puts("s.player_pieces_cnt == 3");
    if (space == 0) return USELESS;
    if (s.len == s.player_pieces_cnt) {
      if (space == 2) return OPPONENT_MUST_DEFEND;
      return NORMAL_2;
    }
    return NORMAL_1;
  }
  if (s.player_pieces_cnt == 2) {
    if (space == 0) return USELESS;
    if (s.len == s.player_pieces_cnt) return NORMAL_1;
    return NORMAL_0;
  }

  if (space == 0) return USELESS;
  return NORMAL_0;
}

double estimate_score(STATE player) {
  double player_score = 0, opponent_score = 0;
  for (int i = 0; i < search_stack.len; i++) {
    for (int j = 0; j < 8; j++) {
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
