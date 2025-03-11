
#include "search.h"
#include "evaluate.h"
#include "generate.h"
#include "types.h"

static const int piece_value[6] = { 100, 300, 300, 500, 900, 1000000 };
static const int rook_table[64] = {
    -5, -5,  0,  5,  5,  0, -5, -5,
    -5,  0,  5, 10, 10,  5,  0, -5,
    -5,  0,  5, 10, 10,  5,  0, -5,
    -5,  0,  5, 10, 10,  5,  0, -5,
    -5,  0,  5, 10, 10,  5,  0, -5,
    -5,  0,  5, 10, 10,  5,  0, -5,
     0,  5, 10, 15, 15, 10,  5,  0,
    -5, -5,  0,  5,  5,  0, -5, -5};

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

/*@brief ROW-> (square / 8) square = 56 → rank = 7*/
/*	COL-> (square % 8) square = 59 → file = 3*/
/* @param square */
/* @return Flip the rank (7 - rank). new square=(flipped rank∗8)+file */
int mirror(int square) {
    int rank = square / 8;
    int file = square % 8;
    return (7 - rank) * 8 + file;
}


/*(pawn, knight, bishop, rook, queen, king) */
int evaluate(const struct position *pos) {
	int score[2] = { 0, 0 };
	int square;
	struct move moves[MAX_MOVES];
	int count = 0;
	int j = 0;
	int white_mobility = 0;
	int black_mobility = 0;

	count = generate_legal_moves(pos, moves);
	while (j < count)
	{
		if (COLOR(moves[j].from_square) == WHITE) {
            white_mobility++;
        } else {
            black_mobility++;
        }
		j++;
	}
	for (square = 0; square < 64; square++) {
		int piece = pos->board[square];
		if (piece != NO_PIECE)
		{
			score[COLOR(piece)] += piece_value[TYPE(piece)];
		}
		else if (piece == PAWN)
		{
			if (COLOR(piece) == 0)
			{
				score[COLOR(piece)] += piece_value[PAWN] + pawn_table[square];
			}
			else
			{
				score[COLOR(piece)] += piece_value[PAWN] + pawn_table[mirror(square)];
			}
		}
		else if (piece == QUEEN)
		{
			if (COLOR(piece) == 0)
			{
				score[COLOR(piece)] += piece_value[QUEEN] + queen_table[square];
			}
			else
			{
				score[COLOR(piece)] += piece_value[QUEEN] + queen_table[mirror(square)];
			}
		}
		else if (piece == ROOK)
		{
			if (COLOR(piece) == 0)
			{
				score[COLOR(piece)] += piece_value[ROOK] + rook_table[square];
			}
			else
			{
				score[COLOR(piece)] += piece_value[ROOK] + rook_table[mirror(square)];
			}

		}
		else if (piece == KNIGHT)
		{
			if (COLOR(piece) == 0)
			{
				score[COLOR(piece)] += piece_value[KNIGHT] + knight_table[square];
			}
			else
			{
				score[COLOR(piece)] += piece_value[KNIGHT] + knight_table[mirror(square)];
			}
		}
		else if (piece == KING)
		{
			if (COLOR(piece) == 0)
			{
				score[COLOR(piece)] += piece_value[KING] + king_table[square];
			}
			else
			{
				score[COLOR(piece)] += piece_value[KING] + king_table[mirror(square)];
			}
		}
		else if (piece == BISHOP)
		{
			if (COLOR(piece) == 0)
			{
				score[COLOR(piece)] += piece_value[BISHOP] + bishop_table[square];
			}
			else
			{
				score[COLOR(piece)] += piece_value[BISHOP] + bishop_table[mirror(square)];
			}
		}
		score[WHITE] += white_mobility * 5;
   	    score[BLACK] += black_mobility * 5;
	}
	return score[pos->side_to_move] - score[1 - pos->side_to_move];
}
