// ConnectFour.cpp
#define DEPTH 8

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <string>

#define XMAX 7
#define YMAX 6
#define EMPTY 0
#define MIDDLE_COL 3
#define TOP 0
#define BOT_ROW (YMAX - 1)
#define BOT_COL (XMAX - 1)
#define INFINITI 1024
#define NINFINITI (-1 * INFINITI)
#define TIE 0

using namespace std;

void init_board(char (&board)[XMAX][YMAX]);
void print_board(const char (&board)[XMAX][YMAX], ostream &os = cout);
void print_numbers();
int get_move(const char (&board)[XMAX][YMAX]);
void make_move(char (&board)[XMAX][YMAX], int x_move, bool x_to_move);
bool check_row_for_win(const char (&board)[XMAX][YMAX], int row);
bool check_col_for_win(const char (&board)[XMAX][YMAX], int col);
bool check_for_win(const char (&board)[XMAX][YMAX]);
bool check_diag_for_win(const char (&board)[XMAX][YMAX], int x, int y);
bool check_diagonals(const char (&board)[XMAX][YMAX]);
int minimax(char (&board)[XMAX][YMAX], int depth, int alpha, int beta, bool maximizingPlayer);
int heuristic(const char(&board)[XMAX][YMAX]);
bool legal_move(const char(&board)[XMAX][YMAX], int move);
int max(int a, int b);
int min(int a, int b);
int choose_move(const char(&board)[XMAX][YMAX], bool to_move);

int evalBoard[XMAX][YMAX] = {
  { 3,  4,  5,  5,  4, 3},
  { 4,  6,  8,  8,  6, 4},
  { 5,  8, 11, 11,  8, 5},
  { 7, 10, 13, 13, 10, 7},
  { 5,  8, 11, 11,  8, 5},
  { 4,  6,  8,  8,  6, 4},
  { 3,  4,  5,  5,  4, 3}
};

int main(int argc, char *argv[])
{
  char board[XMAX][YMAX];
  int xmov;
  bool move = true;
  init_board(board);
  vector<int> move_log;
  ofstream logfile;
  bool win = false;

  string input;
  cout << "Would you like to move first? [Y/n]: ";
  getline(cin, input);
  if((input.at(0) == 'N') || (input.at(0) == 'n'))
    move = true;
  else
    move = false;
  

  for (int i = 0; i < (XMAX * YMAX); ++i)
    {
      if (!move)
	xmov = get_move(board);
      else
	xmov = choose_move(board, move);

      move_log.push_back(xmov);
      make_move(board, xmov, move);
      move ^= true;
      if (check_for_win(board))
	{
	  win = true;
	  break;
	}
    }

  print_board(board);
  if(win)
    {
      cout << "WIN " << ((move) ? "O's" : "X's") << endl;
    }
  if (!win)
    {
      print_board(board);
      cout << "TIE!" << endl;
    }

  logfile.open("results.log", std::ofstream::app);
  if (logfile.is_open())
    {
      logfile << "Final Position: \n" << "-------------------" << endl;
      print_board(board, logfile);
      logfile << "MOVES: ";
      for (vector<int>::const_iterator it = move_log.begin(); it != move_log.end(); ++it)
	logfile << (*it) << " ";
      logfile << "\n\n" << endl;
      logfile.close();
    }
  return 0;
}

void init_board(char(&board)[XMAX][YMAX]) {
  for (int i = 0; i < XMAX; ++i)
    {
      for (int j = 0; j < YMAX; ++j)
	board[i][j] = (char) 0;
    }
}

void print_board(const char (&board)[XMAX][YMAX], ostream &os) {
  for (int j = 0; j < YMAX; ++j)
    {
      os << "|";
      for (int i = 0; i < XMAX; ++i)
	{
	  if (board[i][j] == 'X') os << board[i][j];
	  else if (board[i][j] == 'O') os << board[i][j];
	  else if (board[i][j] == 0) os << "_";
	  else os << "*";

	  os << "|";
	}
      os << endl;
    }
}

void print_numbers() {
  cout << " ";
  for (int i = 1; i <= XMAX; ++i)
    {
      cout << i << " ";
    }
  cout << endl;
}

int get_move(const char(&board)[XMAX][YMAX]) {
  bool is_good = false;
  int move;
  do
    {
      print_board(board); print_numbers();
      cout << endl << "Move? ";
      string in;
      getline(cin, in);
      move = static_cast<int>(in.at(0)) - '0';
      if ((move <= XMAX) && (move > 0) && (board[move - 1][TOP] == 0))
	is_good = true;
    } while (!is_good);

  return (move - 1);
}

void make_move(char(&board)[XMAX][YMAX], int x_move, bool x_to_move) {
  int y_move = BOT_ROW;
  for (int i = 0; i < YMAX; ++i)
    if (board[x_move][i] == 0) y_move = i;
  board[x_move][y_move] = (x_to_move) ? 'X' : 'O';
}

inline bool legal_move(const char(&board)[XMAX][YMAX], int move) {
  return (board[move][TOP] == 0) ? true : false;
}

bool check_row_for_win(const char(&board)[XMAX][YMAX], int row) {
#if (XMAX < 8)
  if (board[MIDDLE_COL][row] == 0) return false;
#endif
  for (int i = TOP; i <= MIDDLE_COL; ++i)
    {
      if (board[i][row] == 0) continue;
      if ((board[i][row] == board[i + 1][row]) && (board[i+1][row] == board[i + 2][row]) && (board[i+2][row] == board[i + 3][row])) return true;
    }
  return false;
}

bool check_col_for_win(const char(&board)[XMAX][YMAX], int col) {
#if (YMAX < 8)
  for (int i = BOT_ROW; i > (BOT_ROW - 4); i--)
    if (board[col][i] == 0) return false;
#endif
  for (int i = BOT_ROW; i >= 3; --i)
    {
      if (board[col][i] == 0) return false;
      if ((board[col][i] == board[col][i - 1]) && (board[col][i - 1] == board[col][i - 2]) && (board[col][i - 2] == board[col][i - 3])) return true;
    }
  return false;
}

bool check_for_win(const char(&board)[XMAX][YMAX]) {
  for (int i = 0; i < XMAX; ++i)
    if (check_col_for_win(board, i)) return true;
  for (int j = 0; j < YMAX; ++j)
    if (check_row_for_win(board, j)) return true;
  if (check_diagonals(board)) return true;
  return false;
}

bool check_diag_for_win(const char(&board)[XMAX][YMAX], int x, int y) {
  if (board[x][y] == 0) return false;
  if (((x + 3) < XMAX) && ((y - 3) >= 0))
    {
      if ((board[x][y] == board[x + 1][y - 1]) && (board[x + 1][y - 1] == board[x + 2][y - 2]) && (board[x + 2][y - 2] == board[x + 3][y - 3])) return true;
    }
  if (((x + 3) < XMAX) && ((y + 3) < YMAX))
    {
      if ((board[x][y] == board[x + 1][y + 1]) && (board[x + 1][y + 1] == board[x + 2][y + 2]) && (board[x + 2][y + 2] == board[x + 3][y + 3])) return true;
    }
  return false;
}

bool check_diagonals(const char(&board)[XMAX][YMAX]) {
  for (int j = 0; j < YMAX; ++j)
    {
      for (int i = 0; i <= MIDDLE_COL; ++i)
	{
	  if (check_diag_for_win(board, i, j)) return true;
	}
    }
  return false;
}

inline int max(int a, int b) {
  return (a > b) ? a : b;
}

inline int min(int a, int b) {
  return (a < b) ? a : b;
}

int minimax(char (&board)[XMAX][YMAX], int depth, int alpha, int beta, bool maximizingPlayer) {
  if (check_for_win(board))
    {
      return (!maximizingPlayer) ? INFINITI : NINFINITI;
    }
  else if (depth == 0)
    {
      return heuristic(board);
    }
  else
    {
      char brd[XMAX][YMAX];

      for (int i = 0; i < XMAX; ++i)
	{
	  memcpy(brd, board, XMAX * YMAX * sizeof(char));
	  if (legal_move(brd, i))
	    {
	      make_move(brd, i, maximizingPlayer);
	      int val = minimax(brd, depth - 1, alpha, beta, !maximizingPlayer);
	      
	      if(maximizingPlayer)
		  alpha = max(val, alpha);
	      else
		  beta = min(val, beta);
	      if( beta <= alpha )
		break;
	    }
	}
      return (maximizingPlayer) ? alpha : beta;
    }
}

int choose_move(const char (&board)[XMAX][YMAX], bool to_move)
{
  int bestMov = -1;
  int bestVal = (to_move) ? NINFINITI : INFINITI;
  char brd[XMAX][YMAX];

  for (int i = 0; i < XMAX; ++i)
    {
      if (legal_move(board, i))
	{
	  if (bestMov == -1)
	    bestMov = i;

	  memcpy(brd, board, XMAX * YMAX * sizeof(char));
	  make_move(brd, i, to_move);
	  if (check_for_win(brd))
	    {
	      bestVal = INFINITI;
	      bestMov = i;
	      break;
	    }

	  int val = minimax(brd, DEPTH, NINFINITI, INFINITI, !to_move);
	  //cout << val << endl;

	  if(to_move)
	    {
	      if (val > bestVal)
		{
		  bestVal = val;
		  bestMov = i;
		}
	    }
	  else
	    {
	      if (val < bestVal)
		{
		  bestVal = val;
		  bestMov = i;
		}
	    }
	}
    }

  cout << endl << "Evaluation: " << bestVal << endl;
  return bestMov;
}

bool is_tie(const char(&board)[XMAX][YMAX]) {
  for (int j = 0; j < YMAX; ++j) {
    for (int i = 0; i < XMAX; ++i) {
      if (board[i][j] == 0)
	return false;
    }
  }
  return true;
}

int heuristic(const char (&board)[XMAX][YMAX]) {
  if (is_tie(board)) return TIE;
  else
    {
      int val = 0;
      for (int j = 0; j < YMAX; ++j)
	{
	  for (int i = 0; i < YMAX; ++i)
	    {
	      if (board[i][j] == 'X')
		{
		  val += evalBoard[i][j];
		}
	      else if (board[i][j] == 'Y')
		{
		  val -= evalBoard[i][j];
		}
	    }
	}

      return val;
    }
}
