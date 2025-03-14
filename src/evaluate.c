#include "evaluate.h"
#include "types.h"
#include "generate.h"
#include <stdbool.h>

static const int piece_value[6] = { 100, 300, 300, 500, 900, 1000000 };

static const int rook_table [64] = {0,   0,   5,  10,  10,   5,   0,   0,
									0,   0,   5,  10,  10,   5,   0,   0,
									-5,   0,   5,  10,  10,   5,   0,  -5,
									-5,   0,   5,  10,  10,   5,   0,  -5,
									-5,   0,   5,  10,  10,   5,   0,  -5,
									-5,   0,   5,  10,  10,   5,   0,  -5,
									5,  10,  10,  10,  10,  10,  10,   5,
									0,   0,   5,  15,  15,   5,   0,   0 };
static const int knight_table[64] = {-50, -40, -30, -30, -30, -30, -40, -50,
									-40, -20,   0,   5,   5,   0, -20, -40,
									-30,   5,  10,  15,  15,  10,   5, -30,
									-30,   0,  15,  20,  20,  15,   0, -30,
									-30,   5,  15,  20,  20,  15,   5, -30,
									-30,   0,  10,  15,  15,  10,   0, -30,
									-40, -20,   0,   0,   0,   0, -20, -40,
									-50, -40, -30, -30, -30, -30, -40, -50};
static const int pawn_table[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
									50,  50,  50,  50,  50,  50,  50,  50,
									10,  10,  20,  30,  30,  20,  10,  10,
									5,   5,  10,  25,  25,  10,   5,   5,
									0,   0,   0,  20,  20,   0,   0,   0,
									5,  -5, -10,   0,   0, -10,  -5,   5,
									5,  10,  10, -20, -20,  10,  10,   5,
									0,   0,   0,   0,   0,   0,   0,   0};
static const int queen_table[64] = {-20, -10, -10,  -5,  -5, -10, -10, -20,
									-10,   0,   5,   0,   0,   0,   0, -10,
									-10,   5,   5,   5,   5,   5,   0, -10,
									-5,   0,   5,   5,   5,   5,   0,  -5,
									0,   0,   5,   5,   5,   5,   0,  -5,
									-10,   0,   5,   5,   5,   5,   0, -10,
									-10,   5,   0,   0,   0,   0,   0, -10,
									-20, -10, -10,  -5,  -5, -10, -10, -20};
static const int king_table[64] = {-50, -40, -30, -20, -20, -30, -40, -50,
									-30, -20, -10,   0,   0, -10, -20, -30,
									-30, -10,  20,  30,  30,  20, -10, -30,
									-30, -10,  30,  40,  40,  30, -10, -30,
									-30, -10,  30,  40,  40,  30, -10, -30,
									-30, -10,  20,  30,  30,  20, -10, -30,
									-30, -30,   0,   0,   0,   0, -30, -30,
									-50, -30, -30, -30, -30, -30, -30, -50};
static const int bishop_table[64] = {-20, -10, -10, -10, -10, -10, -10, -20,
									-10,   5,   0,   0,   0,   0,   5, -10,
									-10,  10,  10,  10,  10,  10,  10, -10,
									-10,   0,  10,  10,  10,  10,   0, -10,
									-10,   5,   5,  10,  10,   5,   5, -10,
									-10,   0,   5,  10,  10,   5,   0, -10,
									-10,   0,   0,   0,   0,   0,   0, -10,
									-20, -10, -10, -10, -10, -10, -10, -20};

int mirror(int square) {
    int rank = square / 8;
    int file = square % 8;
    return (7 - rank) * 8 + file;
}

int	psq_calc(int piece, int square)
{
	switch (piece)
	{
		case PAWN:  
			return pawn_table[square];
		case KNIGHT: 
			return knight_table[square];
		case BISHOP: 
			return bishop_table[square];
		case ROOK: 
			return rook_table[square];
		case QUEEN: 
			return queen_table[square];
		case KING: 
			return king_table[square];
		default:
			return 0;
	}
}

int is_pawn_connected(const struct position *pos, int square, int color) {
   
    int file = square % 8;
	int mult = 1;

    if (file > 0) {
        int left_diagonal; /* -9 for white, +7 for black*/
		if (color == WHITE)
		{
			left_diagonal = square - 9;
			if (pos->board[left_diagonal] == WHITE)
				return 10;
			mult = -1;
		}	
		else if (color == BLACK)
		{
			left_diagonal = square + 7;
			if (pos->board[left_diagonal] == BLACK)
				return -10;
		}
    }
    if (file < 7) {
        int right_diagonal;  /* -7 for white, +9 for black */
		if (color == WHITE)
		{
			right_diagonal = square - 7;
			if (pos->board[right_diagonal] == WHITE)
				return 10;
			mult = -1;
		}
		else if (color == BLACK )
		{
			right_diagonal = square + 9;
			if (pos->board[right_diagonal] == BLACK)
				return -10;
		}
    }
    return 15 * mult;
}

int	pawn_doubled_or_isolated(const struct position *pos, int color)
{
	int pawn_count[8] = {0};
	int square;
	int score = 0;
	int file;
	int check_left;
	int check_right;

	for (square = 0; square < 64; square++) {
		if (pos->board[square] == PAWN && COLOR(pos->board[square]) == color) {
			file = FILE(square);
			pawn_count[file]++;
		}
	}
	for (file = 0; file < 8; file++) {
		if (pawn_count[file] > 1)
			score -= 10 * (pawn_count[file] - 1);

		if (pawn_count[file] > 0) {
			check_left  = (file > 0) ? (pawn_count[file - 1] > 0) : 0;
			check_right = (file < 7) ? (pawn_count[file + 1] > 0) : 0;

			if (!check_left && !check_right)
				score -= 15;
		}
	}
	return score;
}

int evaluate(const struct position *pos) {
	int score[2] = { 0, 0 };
    int square;
	int piece; 
	int piece_val;
	int psq_val;
    
	for (square = 0; square < 64; square++) {
		piece = pos->board[square];
		if (piece == NO_PIECE)
			continue;
		piece_val = piece_value[TYPE(piece)];
		if (COLOR(piece) == 0) {
			psq_val = psq_calc(TYPE(piece), square);
			score[WHITE] += piece_val + psq_val;
		} else {
			psq_val = psq_calc(TYPE(piece), mirror(square));
			score[BLACK] += piece_val + psq_val;
		}
	}
	if (pos->side_to_move == WHITE)
		score[WHITE] += pawn_doubled_or_isolated(pos, WHITE) + is_pawn_connected(pos, square, WHITE);
	if (pos->side_to_move == BLACK)
		score[BLACK] = score[BLACK] - pawn_doubled_or_isolated(pos, BLACK) + is_pawn_connected(pos, square, BLACK);
	return (score[pos->side_to_move] - score[1 - pos->side_to_move]);
}
