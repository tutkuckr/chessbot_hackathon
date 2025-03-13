#include "search.h"
#include "evaluate.h"
#include "generate.h"
#include "types.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

/*COMMENT THIS IF DONT WANT TO IMPLEMENT KILLER*/
#define MAX_DEPTH 64
struct move killer_moves[MAX_DEPTH][2];

int move_equals(struct move a, struct move b) {
    return a.from_square == b.from_square && a.to_square == b.to_square;
}
/*UNTIL HERE*/

struct search_result minimax(const struct position *pos, int depth, int alpha, int beta, int myturn) {
	struct search_result result;

	if (myturn == 1)
		result.score = -1000000;
	else
		result.score = 1000000;

	if (depth == 0) {
		/* we have reached our search depth, so evaluate the position.       */
		result.score = evaluate(pos);
		return result;
	}
	else
	{
		struct move moves[MAX_MOVES];
		size_t count = generate_legal_moves(pos, moves);
		size_t index;
		/*COMMENT THIS IF DONT WANT TO IMPLEMENT KILLER*/
		int i = 0;
		while (i < 2) {
			if (is_legal(pos, killer_moves[depth][i])) {

				moves[count++] = killer_moves[depth][i];
			}
			i++;
    	}
		/*UNTIL HERE*/
		for (index = 0; index < count; index++) {
			struct position copy = *pos;
			int score;

			/* do a move, the current player in `copy` is then the opponent, */
			/* and so when we call minimax we get the score of the opponent. */
			do_move(&copy, moves[index]);

			/* minimax is called recursively. this call returns the score of */
			/* negamax inputs -beta, -alpha         */
			if (myturn)
				score = minimax(&copy, depth-1, alpha, beta, false).score;
			else
				score = minimax(&copy, depth-1, alpha, beta, true).score;

			/* update the best move if we found a better one.                */
			if (myturn)
			{
				if (score > result.score)
				{
					result.move = moves[index];
					result.score = score;
				}
				if (score > alpha) {
					alpha = score;
				}
			} else {
				if (score < result.score)
				{
					result.move = moves[index];
					result.score = score;
				}
				if (score < beta) {
					beta = score;
				}
			}
			/*UNCOMMENT THIS IF WANT TO IMPLEMENT KILLER*/
			/*if (alpha >= beta)
				break; */
			/*COMMENT THIS IF DONT WANT TO IMPLEMENT KILLER*/
			if (score >= beta) {
            	if (!move_equals(moves[index], killer_moves[depth][0])) {
                	killer_moves[depth][1] = killer_moves[depth][0];
                	killer_moves[depth][0] = moves[index];
            	}
            	break;
        	}
			/*UNTIL HERE*/
		}
	}
	return result;
}

struct move search(const struct search_info *info) {
	int alpha = INT_MIN;
	int beta = INT_MAX;
	int depth = 6;
	int maximizing_player = (info->pos->side_to_move == WHITE);

	/*if (info->time[WHITE] < info->time[BLACK] && we are white)
		depth = 2;*/
	return minimax(info->pos, depth, alpha, beta, maximizing_player).move;
}
