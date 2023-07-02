#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>

//
#include <random>
#include <algorithm>

class State
{
public:
 // variables
 int n;
 std::vector<std::vector<int>> board;
 std::vector<std::vector<int>> prev_board;
 int heuristic_value;

 // Constructor
 State(int ni) : n(ni), board(ni, std::vector<int>(ni, 0)), heuristic_value(0) {}

 // methods
 int get_n() const
 {
  return this->n;
 }

 void board_copy(std::vector<std::vector<int>> &board_copy) const
 {
  for (int i = 0; i < n; i++)
  {
   for (int j = 0; j < n; j++)
   {
    board_copy[i][j] = board_copy[i][j];
   }
  }
 }

 // Method to generate a random initial state
 void randomize()
 {
  std::vector<int> values = {1, 2, 3, 4, 5, 6, 0, 0, 0};
  // Use a random device as the seed for the random number generator
  std::random_device rd;
  std::mt19937 gen(rd());
  // Shuffle the vector using std::shuffle
  std::shuffle(values.begin(), values.end(), gen);

  // filling
  for (int i = 0; i < n; i++)
  {
   for (int j = 0; j < n; j++)
   {
    int num = values.back();
    values.pop_back();
    board[i][j] = num;
   }
  }

  // checking for 0 below
  for (int k = 0; k < n - 1; k++)
  {
   for (int i = n - 1; i >= 0; i--)
   {
    for (int j = 0; j < n; j++)
    {
     int num = board[i][j];
     if (i < n && i > 0)
     {
      if (num != 0 && board[i - 1][j] == 0)
      {
       board[i][j] = board[i - 1][j];
       board[i - 1][j] = num;
      }
     }
    }
   }
  }
 }

 void display() const
 {
  for (int i = n - 1; i >= 0; --i)
  {
   for (int j = 0; j < n; ++j)
   {
    // std::cout << " " << board[i][j] << " " << (char)124 << " ";
    if (j == 0)
    {
     if (board[i][j] == 0)
     {
      std::cout << (char)124 << " "
                << "  "
                << " " << (char)124 << " ";
     }
     else
     {
      std::cout << (char)124 << " "
                << " " << board[i][j] << " " << (char)124 << " ";
     }
    }
    else
    {
     if (board[i][j] == 0)
     {
      std::cout << " "
                << " "
                << " " << (char)124 << " ";
     }
     else
     {
      std::cout << " " << board[i][j] << " " << (char)124 << " ";
     }
    }
   }
   std::cout << '\n';
  }
  std::cout << "---  ---  ---\n";
 }

 bool is_goal(int block, int row, int col) const
 {
  return board[row][col] == block;
 }

 std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> legal_actions() const
 {
  // [source (i,j) nd dest(i,j)] list
  std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> allActions;
  std::vector<std::pair<int, int>> sourcePoints;
  std::vector<std::pair<int, int>> destPoints;

  // the top row element can be give directly for check
  // the other rows element have to fill the pre condition that the element top if them should be zero
  for (int i = 0; i < n; i++)
  {
   for (int j = 0; j < n; j++)
   {
    if (board[i][j] != 0 && this->ifAtTop(i, j))
    {
     // this j could be source
     // this i will show top
     // std::cout << "adding " << i << "th elenent of " << j << "th col as source" << std::endl;
     sourcePoints.push_back(std::make_pair(i, j));
    }
    else if (board[i][j] == 0 && this->ifAtTop(i, j))
    {
     if (i != 0)
     {
      if (board[i - 1][j] != 0)
      {
       // this j could be dest
       // this i will show top
       // std::cout << "adding " << i << "th elenent of " << j << "th col as dest" << std::endl;
       destPoints.push_back(std::make_pair(i, j));
      }
     }
     else
     {
      // std::cout << "adding " << i << "th elenent of " << j << "th col as dest" << std::endl;
      destPoints.push_back(std::make_pair(i, j));
     }
    }
   }
  }
  for (const auto &dest : destPoints)
  {
   for (const auto &src : sourcePoints)
   {
    bool pairExists = false;
    // eg if src(0,0) and dest(2,2) exists in allActions as AllActions have vector of Actions in a way that each Action is (src,dest)
    for (auto &action : allActions)
    {
     if (action.first.first == src.first && action.first.second == src.second && action.second.first == dest.first && action.second.second == dest.second)
     {
      pairExists = true;
      break;
     }
    }
    if (!pairExists && src.second != dest.second)
    {
     allActions.push_back(std::make_pair(src, dest));
    }
   }
  }
  return allActions;
 }

 State apply_action(std::pair<int, int> source, std::pair<int, int> dest) const
 {
  State next_state(*this);
  next_state.board[dest.first][dest.second] = next_state.board[source.first][source.second];
  next_state.board[source.first][source.second] = 0;
  std::cout << "Move " << board[source.first][source.second] << " from (" /*<< source.first << ","*/ << source.second << ") to (" /*<< dest.first << "," */ << dest.second << ")" << '\n';
  next_state.display();
  return next_state;
 }

 bool operator<(const State &other) const
 {
  // Compare two states based on their heuristic values
  return this->heuristic_value < other.heuristic_value;
 }

 bool operator==(const State &other) const
 {
  // Compare two states based on their board values
  for (int i = 0; i < n; i++)
  {
   for (int j = 0; j < n; j++)
   {
    if (this->board[i][j] != other.board[i][j])
    {
     return false;
    }
   }
  }
  return true;
 }

 void move_gen(State prev, State next) const
 {
  if (prev == next)
  {
   std::cout << "No move generated" << std::endl;
   return;
  }
  else
  {
   for (int i = 0; i < n; i++)
   {
    for (int j = 0; j < n; j++)
    {
     if (prev.board[i][j] != next.board[i][j])
     {
      // it means that the block has been moved to prev.board[i][j]
      if (prev.board[i][j] == 0)
      {
       // finding from previous where it has been taken
       int prev_row, prev_col = 0;
       for (int k = 0; k < n; k++)
       {
        for (int l = 0; l < n; l++)
        {
         if (prev.board[k][l] == next.board[i][j])
         {
          prev_row = k;
          prev_col = l;
         }
        }
       }
       std::cout << "up_Move " << next.board[i][j] << " from (" << prev_row << "," << prev_col << ") to (" << i << "," << j << ")" << std::endl;
       return;
      }
      // it means that the block has been moved from prev.board[i][j]
      else
      {
       // finding from previous where it has been taken
       int prev_row, prev_col = 0;
       for (int k = 0; k < n; k++)
       {
        for (int l = 0; l < n; l++)
        {
         if (next.board[k][l] == prev.board[i][j])
         {
          prev_row = k;
          prev_col = l;
         }
        }
       }
       std::cout << "dn_Move " << prev.board[i][j] << " from (" << i << "," << j << ") to (" << prev_row << "," << prev_col << ")" << std::endl;
       return;
      }
     }
    }
   }
  }
 }

private:
 bool ifAtTop(int rowi, int colj) const
 {
  // it tells any eleent which is present at rowith and coljth position is at top or not
  if (rowi == n - 1)
  {
   return true;
  }
  for (int i = rowi + 1; i < n; i++)
  {
   if (board[i][colj] != 0)
   {
    return false;
   }
  }
  return true;
 }
};

// Solver Class
class Solver
{
public:
 Solver(State initial_state, int block, int goal_row, int goal_col) : initial_state(initial_state), block(block), goal_row(goal_row), goal_col(goal_col) {}

 int heuristic_manhattan(State state) const
 {
  for (int i = 0; i < n; i++)
  {
   for (int j = 0; j < n; j++)
   {
    if (state.board[i][j] == block)
    {
     return abs(i - goal_row) + abs(j - goal_col);
    }
   }
  }
 }

 bool simple_search()
 {
  std::queue<State> fringe;
  fringe.push(initial_state);

  int steps = 1;
  while (!fringe.empty() && steps < 101)
  {
   State current_state = fringe.front();
   fringe.pop();

   if (current_state.is_goal(block, goal_row, goal_col))
   {
    std::cout << "Goal state found:\n";
    current_state.display();
    return true;
   }

   std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> actions = current_state.legal_actions();
   for (const auto &action : actions)
   {
    std::cout << "Step:" << steps << ":";
    State next_state = current_state.apply_action(action.first, action.second);
    fringe.push(next_state);
   }
   ++steps;
  }
  std::cout << "Goal state not found within 100 steps.\n";
  return false;
 }

 bool heuristic_search()
 {
  std::priority_queue<std::pair<int, State>, std::vector<std::pair<int, State>>, std::greater<std::pair<int, State>>> fringe;
  initial_state.heuristic_value = heuristic_manhattan(initial_state);
  fringe.push(std::make_pair(initial_state.heuristic_value, initial_state));
  int steps = 1;
  while (!fringe.empty() && steps < 101)
  {
   State current_state = fringe.top().second;
   fringe.pop();

   // checking if its Goal
   if (current_state.is_goal(block, goal_row, goal_col))
   {
    std::cout << "Goal state found:\n";
    current_state.display();
    return true;
   }
   std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> actions = current_state.legal_actions();
   for (const auto &action : actions)
   {
    std::cout << "Step:" << steps << ": ";
    State next_state = current_state.apply_action(action.first, action.second);
    next_state.heuristic_value = heuristic_manhattan(next_state);
    fringe.push(std::make_pair(next_state.heuristic_value, next_state));
   }
   ++steps;
  }
  std::cout << "Goal state not found within 100 steps.\n";
  return false;
 }

private:
 State initial_state;
 int block, goal_row, goal_col;
 int n = initial_state.get_n();
 std::vector<std::vector<int>> boardHere = initial_state.board;
};

int main()
{
 int n = 3;
 State state(n);
 int block = 1, goal_row = 1, goal_col = 1;
 int level = 0;
 std::cout << "Choose a level to run:\n";
 std::cout << "1. Single goal solver without heuristics\n";
 std::cout << "2. Single goal solver with heuristics\n";
 std::cout << "3. Disjunctive goal solver\n";
 std::cout << "4. Conjunctive goal solver\n";
 std::cout << "5. Conjunctive goal solver on n*n board\n";
 std::cout << "6. A Star solver on n*n board\n";
 std::cout << "7. Quit\n";
 std::cin >> level;
 switch (level)
 {
 case 1:
 {
  state.randomize();
  std::cout << "Initial state:\n";
  state.display();
  std::cout << "Input a goal in the format (block, row, col):\n";
  std::cout << "Block (1-6): ";
  std::cin >> block;
  std::cout << "Row (0-2): ";
  std::cin >> goal_row;
  std::cout << "Col (0-2): ";
  std::cin >> goal_col;
  Solver solver(state, block, goal_row, goal_col);
  solver.simple_search();
  break;
 }
 case 2:
 {
  state.randomize();
  std::cout << "Initial state:\n";
  state.display();
  std::cout << "Input a goal in the format (block, row, col):\n";
  std::cout << "Block (1-6): ";
  std::cin >> block;
  std::cout << "Row (0-2): ";
  std::cin >> goal_row;
  std::cout << "Col (0-2): ";
  std::cin >> goal_col;
  Solver solver(state, block, goal_row, goal_col);
  solver.heuristic_search();
  break;
 }
 default:
  break;
 }
 return 0;
}
