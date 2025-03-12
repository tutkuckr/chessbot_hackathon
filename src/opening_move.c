#include "uci.h"
#include "search.h"
#include "move.h"
#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char	*opening_move_white( const struct position *pos, int counter){
	const char *opening_moves[] = {
        "e2e4",
        "g1f3",
        "f1c4" 
    };

	if (counter >= 0 && counter < 3) {
		if (counter == 1 && pos->board[35] == NO_PIECE)
			return (NULL);
		if (counter == 2 && pos->board[42] == NO_PIECE)
			return (NULL);
		return ((char *)opening_moves[counter]);
	}
	return (NULL);
}

char *opening_move_black(const struct position *pos, int *counter){
	int line = -1;
	int i;
	static char *move[][4] =
	{{"f7f5", "b8c6", "g7g6", "f8g7"},
	{"g8f6", "e7e6", "c8g4", "d7d5"}};

/*First line Hedgehog*/
	for (i = 16; i <= 23; i++)
	{
		if (pos->board[i] != NO_PIECE)
		{
			line = 0;
			break ;
		}
	}
/*Second line Nimzo*/
	if (line == -1)
	{
		for (i = 24; i <= 31; i++)
		{
			if (pos->board[i] != NO_PIECE)
			{
				line = 1;
				break ;
			}
		}
	}
	if (line != -1 && *counter < 4)
		return (move[line][(*counter)++]);
	return (NULL);
}
