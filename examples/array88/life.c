
// Disabled for now - works fine, I just didn't think it was very interesting.
// 8x8 is a bit small for life, it would probably do better on the glcd.

//-----------------------------------------------------------------------------
// Conway's Game of Life on the 8x8 torus display.
//
// The board wraps in both x and y. Cell color represents the age (number of
// generations survived) of a living cell, not its neighbour count.

#define LIFE_SEED_CELLS 24	// number of live cells in the initial state
#define LIFE_DELAY 15		// display scans per generation (controls speed)
#define LIFE_AGE_MAX 16		// ages are capped here (16+ is all white)

// age[y][x]: 0 == dead, otherwise the number of generations survived.
static uint8_t life_age[8][8];
static uint8_t life_next[8][8];

// Fixed base seed. Incremented every invocation so each run steps through a
// different (but deterministic) sequence of states.
static uint16_t life_seed = 0x1234;

// Map a cell age to its display color (see task.md colour/age policy).
static uint8_t life_color(uint8_t age) {
	if (age == 0)
		return BLACK;
	if (age == 1)
		return BLUE;
	if (age == 2)
		return CYAN;
	if (age <= 4)
		return GREEN;
	if (age <= 7)
		return YELLOW;
	if (age <= 11)
		return RED;
	if (age <= 15)
		return MAGENTA;
	return WHITE;
}

// Count the living neighbours of (x, y), wrapping around the torus.
static uint8_t life_neighbours(uint8_t x, uint8_t y) {
	uint8_t n = 0;
	for (int8_t dy = -1; dy <= 1; dy++) {
		for (int8_t dx = -1; dx <= 1; dx++) {
			if (dx == 0 && dy == 0)
				continue;
			uint8_t nx = (x + dx) & 7;
			uint8_t ny = (y + dy) & 7;
			if (life_age[ny][nx] != 0)
				n++;
		}
	}
	return n;
}

// Draw the current board.
static void life_render(void) {
	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 8; x++) {
			array88_plot(x, y, life_color(life_age[y][x]));
		}
	}
}

// Advance the board by one generation.
static void life_step(void) {
	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 8; x++) {
			uint8_t n = life_neighbours(x, y);
			uint8_t age = life_age[y][x];
			if (age != 0) {
				// living cell survives with 2 or 3 neighbours
				if (n == 2 || n == 3) {
					life_next[y][x] = (age < LIFE_AGE_MAX) ? age + 1 : LIFE_AGE_MAX;
				} else {
					life_next[y][x] = 0;
				}
			} else {
				// dead cell is born with exactly 3 neighbours
				life_next[y][x] = (n == 3) ? 1 : 0;
			}
		}
	}
	memcpy(life_age, life_next, sizeof(life_age));
}

// Populate the board with a random initial state.
static void life_seed_random(void) {
	memset(life_age, 0, sizeof(life_age));
	srand(life_seed++);
	for (uint8_t i = 0; i < LIFE_SEED_CELLS; i++) {
		int k = rand();
		life_age[(k >> 3) & 7][k & 7] = 1;
	}
}

// Populate the board with a single glider. On the torus a glider never dies -
// it cruises diagonally forever, wrapping around the edges. Its position is
// offset each invocation so successive runs differ.
static void life_seed_glider(void) {
	static const uint8_t cells[5][2] = {
		// (x, y) offsets forming a glider
		{1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2},
	};
	uint8_t ox = life_seed & 7;
	uint8_t oy = (life_seed >> 3) & 7;
	life_seed++;
	memset(life_age, 0, sizeof(life_age));
	for (uint8_t i = 0; i < 5; i++) {
		uint8_t x = (cells[i][0] + ox) & 7;
		uint8_t y = (cells[i][1] + oy) & 7;
		life_age[y][x] = 1;
	}
}

// Run the simulation from the already-seeded board until the user escapes.
static void life_run(void) {
	while (!key_exit()) {
		life_render();
		for (uint8_t n = 0; n < LIFE_DELAY; n++) {
			array88_scan();
		}
		life_step();
	}
	array88_init();
}

static void life_random_test(struct menu *m) {
	(void)m;
	life_seed_random();
	life_run();
}

static void life_glider_test(struct menu *m) {
	(void)m;
	life_seed_glider();
	life_run();
}

//-----------------------------------------------------------------------------
