#ifndef __AI_h
#define __AI_h

#include <assert.h>
#include <stdbool.h>

#define STATE unsigned int
#define BLACK 0
#define WHITE 1
#define BLANK 2

// coordinate of chessboard specifies top-left point as origin,
// the value of x-coordinate increasing from top to bottom and
// y-coordinate increasing from left to right.
typedef struct pos {
  int x, y;
  STATE state;
} pos;

pos nxt_step;
pos dir_vec[8];
void init_AI();

#define MAXLEN 225

STATE board[15][15];

#define OUT_OF_BORDER(x, y) \
  ((x < 0 || y < 0 || x >= 15 || y >= 15) ? true : false)

struct pieces_list {
  int len;
  pos list[MAXLEN];
};

struct pieces_list search_stack;

double negmax(STATE player, pos pre, int depth, double alpha, double beta);

bool player_win(STATE player, pos p);
double estimate_score(STATE player);

static inline void append(struct pieces_list *l, pos p);
static inline void pop(struct pieces_list *l);

#define WIN 1e9
#define NXT_STEP_WIN 1e7
#define OPPONENT_MUST_DEFEND 1e4
#define NORMAL_2 500
#define NORMAL_1 100
#define NORMAL_0 50
#define USELESS 0

#define DEPTH 3
#define RATIO 0.3

#endif