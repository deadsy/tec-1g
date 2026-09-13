//-----------------------------------------------------------------------------
/*

Connect 4 on the 8x8 RGB

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>

#include "array88.h"
#include "menu.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"
#include "hw.h"

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

//-----------------------------------------------------------------------------

static void delay_and_scan(void) {
	for (uint8_t i = 0; i < 10; i++) {
		array88_scan();
	}
}

//-----------------------------------------------------------------------------

struct game_state {
	bool dirty;		// display needs a refresh
	uint8_t player_col;	// current player column 0..7
	uint8_t computer_col;	// current computer column 0..7
	uint8_t cells[GAME_COLS][GAME_ROWS];
};

static void game_init(struct game_state *s) {
	memset(s, 0, sizeof(struct game_state));
	s->player_col = 0;
	s->dirty = true;
}

static void game_render(struct game_state *s) {
	if (!s->dirty) {
		return;
	}
	array88_clear();
	// player position
	array88_plot(s->player_col, 0, BLUE);
	// display the cells
	for (int8_t col = 0; col < GAME_COLS; col++) {
		for (int8_t row = 0; row < GAME_ROWS; row++) {
			array88_plot(col, row + 1, s->cells[col][row]);
		}
	}
	s->dirty = false;
}

// move the player left
static void player_left(struct game_state *s) {
	if (s->player_col > 0) {
		s->player_col--;
		s->dirty = true;
	}
}

// move the player right
static void player_right(struct game_state *s) {
	if (s->player_col < (GAME_COLS - 1)) {
		s->player_col++;
		s->dirty = true;
	}
}

// can we drop a piece on this column?
static bool can_drop(struct game_state *s, uint8_t col) {
	return s->cells[col][GAME_ROWS - 1] == EMPTY;
}

// drop a piece on a column - return the row the piece drops to.
static uint8_t drop(struct game_state *s, uint8_t col, uint8_t piece) {
	// add the piece
	uint8_t row = GAME_ROWS - 1;
	s->cells[col][row] = piece;
	s->dirty = true;
	// drop the piece
	bool dropping = true;
	do {
		game_render(s);
		delay_and_scan();
		if ((row >= 1) && (s->cells[col][row - 1] == EMPTY)) {
			s->cells[col][row--] = EMPTY;
			s->cells[col][row] = piece;
			s->dirty = true;
		} else {
			dropping = false;
		}

	} while (dropping);
	return row;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

// did we just get a win from a piece placed in this cell?
static uint8_t game_won(struct game_state *s, uint8_t col, uint8_t row) {
	uint8_t player = s->cells[col][row];
	uint8_t n;

	// note: we are double counting the starting col,row hence CONNECT + 1

	// check the row for a horizontal win
	n = count_dirn(s, col, row, 1, 0);
	n += count_dirn(s, col, row, -1, 0);
	if (n >= CONNECT + 1) {
		goto winner;
	}

	// check the column for a vertical win
	n = count_dirn(s, col, row, 0, 1);
	n += count_dirn(s, col, row, 0, -1);
	if (n >= CONNECT + 1) {
		goto winner;
	}

	// check the diagonal / for a win
	n = count_dirn(s, col, row, 1, 1);
	n += count_dirn(s, col, row, -1, -1);
	if (n >= CONNECT + 1) {
		goto winner;
	}

	// check the diagonal \ for a win
	n = count_dirn(s, col, row, -1, 1);
	n += count_dirn(s, col, row, 1, -1);
	if (n >= CONNECT + 1) {
		goto winner;
	}

	// no win yet...
	return PLAYING;

 winner:
	return (player == HUMAN) ? HUMAN_WIN : COMPUTER_WIN;
}

// evaluate the game state
static uint8_t game_evaluate(struct game_state *s, uint8_t col, uint8_t row) {
	uint8_t win = game_won(s, col, row);
	if (win != PLAYING) {
		return win;
	}
	// are we stuck?
	for (uint8_t col = 0; col < GAME_COLS; col++) {
		if (can_drop(s, col)) {
			// not stuck...
			return PLAYING;
		}
	}
	return DRAW;
}

//-----------------------------------------------------------------------------

// run the computer turn (a move is possible...), return the row dropped to.
static uint8_t computer_turn(struct game_state *s) {
	while (true) {
		// sophistication...
		uint8_t col = rand() % GAME_COLS;
		if (can_drop(s, col)) {
			s->computer_col = col;
			return drop(s, col, COMPUTER);
		}
	}
}

//-----------------------------------------------------------------------------

// run the player turn - return the row dropped to, or -1 for exit
static uint8_t player_turn(struct game_state *s) {
	while (true) {
		if (key_down()) {
			switch (key_code()) {
			case KEYPAD_Minus:
				player_left(s);
				break;
			case KEYPAD_Plus:
				player_right(s);
				break;
			case KEYPAD_Go:
				if (can_drop(s, s->player_col)) {
					return drop(s, s->player_col, HUMAN);
				}
				break;
			case KEYPAD_Address:
				return EXIT_ROW;
			}
		}
		game_render(s);
		array88_scan();
	}
}

//-----------------------------------------------------------------------------

static void connect4(struct menu *m) {

	srand(0x1234);

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
		break;
	case HUMAN_WIN:
		lcd_puts(0, 0, "Look at you...");
		lcd_puts(1, 0, "smarter than a Z80!");
		break;
	case COMPUTER_WIN:
		lcd_puts(0, 0, "You lose,");
		lcd_puts(1, 0, "monkey brain!");
		break;
	}

	// wait for exit
	while (!key_exit()) {
		array88_scan();
	}

	// turn off the display
	array88_clear();
	array88_scan();
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
	key_init();
	lcd_init();
	menu_init();
	array88_clear();

	struct menu m;
	menu_setup(&m, LCD_ROWS, LCD_COLS, root_items);
	while (1) {
		menu_run(&m);
	}
}

//-----------------------------------------------------------------------------
