//-----------------------------------------------------------------------------
/*

Connect 4 on the 8x8 RGB

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "array88.h"
#include "menu.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"
#include "hw.h"
#include "mml.h"
#include "util.h"

#include "term.h"
#include "glcd.h"

//-----------------------------------------------------------------------------

#define GAME_COLS 8
#define GAME_ROWS 7

// cell states
#define EMPTY BLACK
#define HUMAN YELLOW
#define COMPUTER RED

// game evaluations
#define PLAYING 0		// still playing
#define DRAW 1			// no moves left
#define HUMAN_WIN 2		// the human has won
#define COMPUTER_WIN 3		// the computer has won

// the eponymous number
#define CONNECT 4

// row value used to indicate game exit
#define EXIT_ROW 0xff

// invalid column number
#define NO_COLUMN 0xff

//-----------------------------------------------------------------------------

struct game_state {
	uint8_t player_col;	// current player column 0..7
	uint8_t computer_col;	// current computer column 0..7
	uint8_t cells[GAME_COLS][GAME_ROWS];
};

static void game_init(struct game_state *s) {
	memset(s, EMPTY, sizeof(struct game_state));
	s->player_col = 0;
}

static void game_render(struct game_state *s) {
	array88_fill(EMPTY);
	// player position
	array88_plot(s->player_col, 0, BLUE);
	// display the cells
	for (int8_t col = 0; col < GAME_COLS; col++) {
		for (int8_t row = 0; row < GAME_ROWS; row++) {
			array88_plot(col, row + 1, s->cells[col][row]);
		}
	}
}

//-----------------------------------------------------------------------------
// dropping the player pieces

// can we drop a piece on this column?
static bool can_drop(struct game_state *s, uint8_t col) {
	return s->cells[col][GAME_ROWS - 1] == EMPTY;
}

// drop a piece on a column (with animation) - return the row the player piece drops to.
static uint8_t drop_animate(struct game_state *s, uint8_t col, uint8_t player) {
	// add the player piece
	uint8_t row = GAME_ROWS - 1;
	s->cells[col][row] = player;
	// drop the piece
	bool dropping = true;
	do {
		game_render(s);
		array88_scan(10);
		if ((row >= 1) && (s->cells[col][row - 1] == EMPTY)) {
			s->cells[col][row--] = EMPTY;
			s->cells[col][row] = player;
		} else {
			dropping = false;
		}
	} while (dropping);
	return row;
}

// drop a piece on a column - return the row the player piece drops to.
static uint8_t drop(struct game_state *s, uint8_t col, uint8_t player) {
	for (uint8_t row = 0; row < GAME_ROWS; row++) {
		if (s->cells[col][row] == EMPTY) {
			s->cells[col][row] = player;
			return row;
		}
	}
	// shouldn't get here...'
	return 0;
}

// undo a drop, return the cell to empty
static void undo_drop(struct game_state *s, uint8_t col, uint8_t row) {
	s->cells[col][row] = EMPTY;
}

//-----------------------------------------------------------------------------
// game evaluation

// starting at a position, count the cells of the same type in a direction.
static uint8_t count_dirn(struct game_state *s, int8_t x, int8_t y, int8_t dx, int8_t dy) {
	uint8_t player = s->cells[x][y];
	uint8_t n = 0;
	while (s->cells[x][y] == player) {
		x += dx;
		y += dy;
		n++;
		if ((x < 0) || (x >= GAME_COLS)) {
			break;
		}
		if ((y < 0) || (y >= GAME_ROWS)) {
			break;
		}
	}
	return n;
}

// did we just get a win from a piece placed in this cell?
static uint8_t game_won(struct game_state *s, uint8_t col, uint8_t row) {
	uint8_t player = s->cells[col][row];
	uint8_t n;

	// note: we are double counting the starting col,row hence CONNECT + 1

	// check the row for a horizontal win
	n = count_dirn(s, col, row, 1, 0);
	n += count_dirn(s, col, row, -1, 0);
	if (n >= CONNECT + 1) {
		return player;
	}

	// check the column for a vertical win
	n = count_dirn(s, col, row, 0, 1);
	n += count_dirn(s, col, row, 0, -1);
	if (n >= CONNECT + 1) {
		return player;
	}

	// check the diagonal / for a win
	n = count_dirn(s, col, row, 1, 1);
	n += count_dirn(s, col, row, -1, -1);
	if (n >= CONNECT + 1) {
		return player;
	}

	// check the diagonal \ for a win
	n = count_dirn(s, col, row, -1, 1);
	n += count_dirn(s, col, row, 1, -1);
	if (n >= CONNECT + 1) {
		return player;
	}

	// no win yet...
	return EMPTY;
}

// is the game drawn? (no moves possible)
static bool game_drawn(struct game_state *s) {
	for (uint8_t col = 0; col < GAME_COLS; col++) {
		if (can_drop(s, col)) {
			// not stuck...
			return false;
		}
	}
	return true;
}

// return the number of winning moves the player has from the current game state.
static uint8_t winning_moves(struct game_state *s, uint8_t player) {
	uint8_t n = 0;
	for (uint8_t col = 0; col < GAME_COLS; col++) {
		if (!can_drop(s, col)) {
			continue;
		}
		uint8_t row = drop(s, col, player);
		if (game_won(s, col, row) == player) {
			n += 1;
		}
		undo_drop(s, col, row);
	}
	return n;
}

// evaluate the game state
static uint8_t game_evaluate(struct game_state *s, uint8_t col, uint8_t row) {
	uint8_t win = game_won(s, col, row);
	if (win != EMPTY) {
		return (win == COMPUTER) ? COMPUTER_WIN : HUMAN_WIN;
	}
	// are we drawn?
	if (game_drawn(s)) {
		return DRAW;
	}
	return PLAYING;
}

//-----------------------------------------------------------------------------
// computer move

#define SCORE_WIN 100
#define SCORE_DRAW 0

static int8_t max_int8(int8_t a, int8_t b) {
	return (a > b) ? a : b;
}

static int8_t min_int8(int8_t a, int8_t b) {
	return (a < b) ? a : b;
}

static int8_t game_score(struct game_state *s, bool player) {
	(void)s;
	(void)player;
	// TODO
	return 0;
}

static int8_t minimax(struct game_state *s, uint8_t depth, bool player) {
	if (depth == 0) {
		return game_score(s, player);
	}
	int8_t score, move_score;
	if (player) {
		// computer, return the max score
		score = INT8_MIN;
		for (uint8_t col = 0; col < GAME_COLS; col++) {
			if (!can_drop(s, col)) {
				continue;
			}
			uint8_t row = drop(s, col, COMPUTER);
			if (game_won(s, col, row) != EMPTY) {
				move_score = SCORE_WIN;
			} else if (game_drawn(s)) {
				move_score = SCORE_DRAW;
			} else {
				// recurse
				move_score = minimax(s, depth - 1, false);
			}
			undo_drop(s, col, row);
			score = max_int8(score, move_score);
		}
	} else {
		// human, return the min score
		score = INT8_MAX;
		for (uint8_t col = 0; col < GAME_COLS; col++) {
			if (!can_drop(s, col)) {
				continue;
			}
			uint8_t row = drop(s, col, HUMAN);
			if (game_won(s, col, row) != EMPTY) {
				move_score = -SCORE_WIN;
			} else if (game_drawn(s)) {
				move_score = -SCORE_DRAW;
			} else {
				// recurse
				move_score = minimax(s, depth - 1, true);
			}
			undo_drop(s, col, row);
			score = min_int8(score, move_score);
		}
	}
	return score;
}

//-----------------------------------------------------------------------------
// computer move

#define SCORE_ILLEGAL -10000
#define SCORE_COMPUTER_WIN 10000
#define SCORE_HUMAN_WIN -1000
#define SCORE_TWO_WIN 500
#define SCORE_ONE_WIN 100

static const uint8_t column_bias[GAME_COLS] = { 0, 2, 4, 8, 8, 4, 2, 0 };

// return the score for a move made on this column
static int16_t evaluate_move(struct game_state *s, uint8_t col) {
	if (!can_drop(s, col)) {
		// can't do it
		return SCORE_ILLEGAL;
	}

	// can we win with this move?
	uint8_t row = drop(s, col, COMPUTER);
	if (game_won(s, col, row) == COMPUTER) {
		undo_drop(s, col, row);
		return SCORE_COMPUTER_WIN;
	}

	int16_t score = 0;

	// does the human have a winning move?
	if (winning_moves(s, HUMAN) > 0) {
		score += SCORE_HUMAN_WIN;
	}

	// how many winning moves do I have?
	uint8_t n = winning_moves(s, COMPUTER);
	if (n >= 2) {
		score += SCORE_TWO_WIN;
	}
	if (n == 1) {
		score += SCORE_ONE_WIN;
	}

	// bias to central columns
	score += column_bias[col];

	undo_drop(s, col, row);

	return score;
}

// run the computer turn (a move is possible...), return the row dropped to.
static uint8_t computer_turn(struct game_state *s) {
	int16_t best_score = INT16_MIN;
	uint8_t best_col = NO_COLUMN;

	char tmp[32];

	// create a column search order array (random shuffle)
	// In the event two scores are the same the first column
	// in the search order will be chosen. This helps stop
	// the computer being predictable.
	uint8_t order[GAME_COLS] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	shuffle(order, GAME_COLS);

	// whilst I think...
	array88_blank();

	// evaluate each column
	for (uint8_t i = 0; i < GAME_COLS; i++) {

		uint8_t col = order[i];
		int16_t score = evaluate_move(s, col);

		sprintf(tmp, "\n%d: %d", col, score);
		term_puts(tmp);

		if (score > best_score) {
			best_score = score;
			best_col = col;
		}
	}

	term_flush();

	s->computer_col = best_col;
	return drop_animate(s, best_col, COMPUTER);
}

//-----------------------------------------------------------------------------
// player move

// move the player left
static bool player_left(struct game_state *s) {
	if (s->player_col > 0) {
		s->player_col--;
		return true;
	}
	return false;
}

// move the player right
static bool player_right(struct game_state *s) {
	if (s->player_col < (GAME_COLS - 1)) {
		s->player_col++;
		return true;
	}
	return false;
}

// run the player turn - return the row dropped to, or -1 for exit
static uint8_t player_turn(struct game_state *s) {
	bool dirty = true;
	while (true) {
		if (key_down()) {
			switch (key_code()) {
			case KEYPAD_Minus:
				dirty = player_left(s);
				break;
			case KEYPAD_Plus:
				dirty = player_right(s);
				break;
			case KEYPAD_Go:
				if (can_drop(s, s->player_col)) {
					return drop_animate(s, s->player_col, HUMAN);
				}
				break;
			case KEYPAD_Address:
				return EXIT_ROW;
			}
		}
		if (dirty) {
			game_render(s);
			dirty = false;
		}
		array88_scan(1);
	}
}

//-----------------------------------------------------------------------------

//static const char song_lose[] = "T140 O4 L8 G#4 G4 F#4 F4";
//static const char song_win[] = "T180 L8 O5 C E G O6 C4.";
//static const char song_draw[] = "T100 O4 L4 G A B L2 O5 D";

static void connect4(struct menu *m) {

	lcd_clear();
	lcd_puts(0, 0, "Playing...");

	struct game_state s;
	game_init(&s);

	// game loop
	uint8_t state = PLAYING;
	while (true) {
		uint8_t row = player_turn(&s);
		if (row == EXIT_ROW) {
			break;
		}
		state = game_evaluate(&s, s.player_col, row);
		if (state != PLAYING) {
			break;
		}
		row = computer_turn(&s);
		state = game_evaluate(&s, s.computer_col, row);
		if (state != PLAYING) {
			break;
		}
	}

	// last message
	lcd_clear();
	lcd_putc(m->rows - 1, 0, LEFT_ARROW);
	switch (state) {
	case PLAYING:
		lcd_puts(0, 0, "Bye Felicia...");
		break;
	case DRAW:
		lcd_puts(0, 0, "Alright, we'll");
		lcd_puts(1, 0, "call it a draw...");
		//mml_play(song_draw);
		break;
	case HUMAN_WIN:
		lcd_puts(0, 0, "Look at you...");
		lcd_puts(1, 0, "smarter than a Z80!");
		//mml_play(song_win);
		break;
	case COMPUTER_WIN:
		lcd_puts(0, 0, "You lose,");
		lcd_puts(1, 0, "monkey brain!");
		//mml_play(song_lose);
		break;
	}

	// wait for exit
	while (!key_exit()) {
		array88_scan(1);
	}

	// drop the pieces
	for (uint8_t i = 0; i < 8; i++) {
		array88_scan(10);
		array88_shift_down();
	}

	// turn off the display
	array88_blank();
}

//-----------------------------------------------------------------------------

static void about(struct menu *m) {
	menu_about(m, "connect4", "https://github.com/deadsy/tec-1g" URL_PAD);
}

static const struct menu_item root_items[] = {
	{"new game", connect4},
	{"about", about},
	MENU_EOL,
};

//-----------------------------------------------------------------------------

int main(void) {

	srand(0xcafe);

	key_init();
	lcd_init();
	menu_init();
	array88_init();

	glcd_init();
	term_init(false, true);

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
