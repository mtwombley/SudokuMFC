#include "pch.h"

#include <ctime>
#include <atlstr.h>

namespace
{
  struct fillDepth
  {
    static int  depth;
    fillDepth()
    {
      depth++;
    };
    ~fillDepth()
    {
      depth--;
    };
  };
  int fillDepth::depth = 0;

  struct solveDepth
  {
    static int  depth;
    static bool updown;
    solveDepth()
    {
      if ( !updown )
      {
        ;// __debugbreak();
      }
      depth++;
      updown = true;
    };
    ~solveDepth()
    {
      if ( updown )
      {
        ;// __debugbreak();
      }
      depth--;
      updown = false;
    };
  };
  int solveDepth::depth = 0;
  bool solveDepth::updown = true;

  /* type definitions */
  enum cell_info
  {
    YES,
    NO,
    MAYBE
  };

  struct sudoku_info
  {
    int             grid[9][9] = {0}; /* cells contain the digit if known, 0 if unknown */
    enum cell_info  info[9][9][9] = {MAYBE};

    /* cells contain 1 if cell [a][b] is c, -1 if it isn't, and 0 if unknown */

    /*int solutions; number of solutions found so far */
    int known = 0;
  };

  /* function prototypes */
  int   set_cell( sudoku_info* sudoku, int i, int j, int value );
  void  display_grid( sudoku_info* sudoku );
  int   add_cells( sudoku_info* sudoku );
  int   find_cells( sudoku_info* sudoku );
  int   find_rows( sudoku_info* sudoku );
  int   find_columns( sudoku_info* sudoku );
  int   find_grids( sudoku_info* sudoku );
  int   solve( sudoku_info* sudoku );
  void  find_assume( sudoku_info* sudoku, int* i, int* j, int* value );
  void  sud_struct_zero( sudoku_info* sudoku );
  void  fill( sudoku_info* sudoku );
  void  thin( sudoku_info* sudoku );

  /* global(s) */
  int   Solutions;

  /**
   * Initializes the Sudoku structure and sets the random seed.
   *
   * This function sets all cells in the Sudoku grid to zero and initializes the random seed
   * for generating random numbers. If DEBUG1 or DEBUG2 is defined, it allows for setting a custom seed.
   *
   * @param sudoku A pointer to a sudoku_info structure that will be initialized.
   */
  void init( sudoku_info* sudoku )
  {
    #if defined DEBUG1
    unsigned int  seed;
    #endif

    #if defined DEBUG2
    unsigned int  seed;
    #endif

      // Initialize the Sudoku structure to zero
    sud_struct_zero( sudoku );

    // Set the random seed based on the current time
    srand( (unsigned int)time( nullptr ) );

    #if defined DEBUG2
      // If DEBUG2 is defined, prompt the user to enter a seed
    printf( "Enter seed: " );
    scanf( "%u", &seed );
    srand( seed );
    #endif

    #if defined DEBUG1
      // If DEBUG1 is defined, use the current time as the seed and print it
    seed = time( nullptr );
    printf( "# Seed is %u\n", seed );
    srand( (unsigned int)seed );
    #endif
  }

  // Fills the Sudoku grid with initial values.
  void fill( sudoku_info* sudoku )
  {
    int                 i = 0;
    int                 j = 0;
    int                 value = 0;
    static sudoku_info  new_sudoku;
    static int          randCount = 0;

    fillDepth           fd;

    /* display_grid(sudoku); */
    do
    {
      Solutions = 0;

      // Copy the current state of the Sudoku grid into a temporary structure
      memcpy( &new_sudoku, sudoku, sizeof( sudoku_info ) );

      // Find a random cell that is not already filled in
      while ( sudoku->grid[i = rand() % 9][j = rand() % 9] ) { ++randCount; }

      // Find a random value that can be placed in the cell
      while ( sudoku->info[i][j][value = rand() % 9] != MAYBE ) { ++randCount; }

      // Set the cell to the value
      set_cell( &new_sudoku, i, j, value + 1 );

      // Solve the Sudoku puzzle
      solve( &new_sudoku );

      // If there is only one solution, update the original Sudoku state with the new state
      if ( Solutions == 1 )
      {
        set_cell( sudoku, i, j, value + 1 );
        return;
      }
    } while ( !Solutions );

    // Set the cell to the value
    set_cell( sudoku, i, j, value + 1 );

    // Recursively fill the Sudoku grid
    fill( sudoku );
  }

  /**
   * Solves the Sudoku puzzle using a combination of logical deduction and backtracking.
   *
   * @param sudoku A pointer to a sudoku_info structure that contains the current state of the Sudoku grid and additional information.
   * @return An integer indicating the success or failure of the solving process.
   *         Returns 1 if the puzzle is solved, 0 otherwise.
   */
  int solve( sudoku_info* sudoku )
  {
    int         i;
    int         j;
    int         value;
    sudoku_info new_sudoku1;
    sudoku_info new_sudoku2;

    solveDepth  sd;

    // Copy the current state of the Sudoku grid into a temporary structure
    memcpy( &new_sudoku1, sudoku, sizeof( sudoku_info ) );

    // Attempt to fill in cells using logical deduction
    if ( add_cells( &new_sudoku1 ) )
    {
      return 1;
    }

    // If there are already two solutions found, return 0 to indicate the puzzle is not uniquely solvable
    if ( Solutions == 2 )
    {
      return 0;
    }

    // Update the original Sudoku state with the new state
    memcpy( sudoku, &new_sudoku1, sizeof( sudoku_info ) );

    // Find a cell to assume a value for further solving
    find_assume( &new_sudoku1, &i, &j, &value );

    // Assume the value and recursively solve
    if ( !set_cell( &new_sudoku1, i, j, value ) )
    {
      memcpy( &new_sudoku2, &new_sudoku1, sizeof( sudoku_info ) );
      solve( &new_sudoku2 );
      if ( Solutions == 2 )
      {
        return 0;
      }
    }

    // Assume the value is incorrect and recursively solve
    memcpy( &new_sudoku1, sudoku, sizeof( sudoku_info ) );
    new_sudoku1.info[i][j][value - 1] = NO;
    solve( &new_sudoku1 );
    if ( Solutions == 2 )
    {
      return 0;
    }

    // Return 0 if no solution is found
    return 0;
  }

  // Thins the Sudoku grid by removing cells.
  void thin( sudoku_info* sudoku )
  {
    int         grid[9][9];
    sudoku_info new_sudoku1{};

    sudoku_info new_sudoku2{};

    // Copy the current state of the Sudoku grid into a temporary structure
    for ( int i = 0; i < 9; i++ )
    {
      for ( int j = 0; j < 9; j++ )
      {
        grid[i][j] = sudoku->grid[i][j];
      }
    }

    int left = sudoku->known;

    // Remove cells from the Sudoku grid
    while ( left )
    {
      int rndl = rand() % left;
      int testVal = -1;
      int i = 0;
      int j = 0;
      for ( i = 0; i < 9; i++ )
      {
        for ( j = 0; j < 9; j++ )
        {
          if ( !grid[i][j] )
          {
            continue;
          }

          if ( ++testVal == rndl )
          {
            break;
          }
        }

        if ( testVal == rndl )
        {
          break;
        }
      }

      grid[i][j] = 0;
      left--;
      sud_struct_zero( &new_sudoku1 );
      for ( int k = 0; k < 9; k++ )
      {
        for ( int l = 0; l < 9; l++ )
        {
          if ( sudoku->grid[k][l] && !( k == i && l == j ) )
          {
            set_cell( &new_sudoku1, k, l, sudoku->grid[k][l] );
          }
        }
      }

      Solutions = 0;
      memcpy( &new_sudoku2, &new_sudoku1, sizeof( sudoku_info ) );
      display_grid( &new_sudoku2 );
      solve( &new_sudoku2 );
      if ( Solutions == 2 )
      {
        continue;
      }

      if ( Solutions == 0 )
      {
        printf( "Error!\n" );
        exit( 1 );
      }

      memcpy( sudoku, &new_sudoku1, sizeof( sudoku_info ) );
    }
  }

  // Initializes the sudoku structure to zero
  void sud_struct_zero( sudoku_info* sudoku )
  {
    int i;
    int j;
    int k;

    sudoku->known = 0;
    for ( i = 0; i < 9; i++ )
    {
      for ( j = 0; j < 9; j++ )
      {
        sudoku->grid[i][j] = 0;
        for ( k = 0; k < 9; k++ )
        {
          sudoku->info[i][j][k] = MAYBE;
        }
      }
    }
  }

  // Sets a cell in the sudoku grid to a specific value
  // Returns 1 if the cell is already set to the value, 0 otherwise
  int set_cell( sudoku_info* sudoku, int i, int j, int value )
  {

    // If the cell is already set to the value, return 1
    if ( sudoku->info[i][j][value - 1] == NO )
    {
      return 1;
    }

    sudoku->grid[i][j] = value;

    // If the cell is not set to the value, update the cell and the information in the sudoku structure
    if ( ++( sudoku->known ) == 81 )
    {
      Solutions++;
      sudoku->grid[i][j] = 0;
      --( sudoku->known );
      return 1;
    }

    // Update the information in the sudoku structure
    sudoku->info[i][j][value - 1] = YES;

    // Update the information in the sudoku structure for the row, column, and 3x3 grid
    for ( int k = 0; k < 9; k++ )
    {
      if ( k != value - 1 )
      {
        sudoku->info[i][j][k] = NO;
      }

      if ( k != j )
      {
        sudoku->info[i][k][value - 1] = NO;
      }

      if ( k != i )
      {
        sudoku->info[k][j][value - 1] = NO;
      }
    }

    for ( int k = ( i - ( i % 3 ) ); k < i - ( i % 3 ) + 3; k++ )
    {
      for ( int l = ( j - ( j % 3 ) ); l < j - ( j % 3 ) + 3; l++ )
      {
        if ( k != i || l != j )
        {
          sudoku->info[k][l][value - 1] = NO;
        }
      }
    }

    return 0;
  }

  // Adds cells to the sudoku grid based on certain rules
  int add_cells( sudoku_info* sudoku )
  {
    int flag;
    int val;

    do
    {
      flag = 0;
      if ( ( val = find_cells( sudoku ) ) == 1 )
      {
        return 1;
      }

      flag |= val;
      if ( ( val = find_rows( sudoku ) ) == 1 )
      {
        return 1;
      }

      flag |= val;
      if ( ( val = find_columns( sudoku ) ) == 1 )
      {
        return 1;
      }

      flag |= val;
      if ( ( val = find_grids( sudoku ) ) == 1 )
      {
        return 1;
      }

      flag |= val;
    } while ( flag );
    return 0;
  }

  // Finds cells in the sudoku grid that can be set based on certain rules
  int find_cells( sudoku_info* sudoku )
  {
    int k;
    int counter;
    int flag = 0;

    // Check each cell in the sudoku grid
    for ( int i = 0; i < 9; i++ )
    {
      for ( int j = 0; j < 9; j++ )
      {

  // Skip cells that are already filled in
        if ( sudoku->grid[i][j] )
        {
          continue;
        }

        // Count the number of possible values for the cell
        for ( k = 0, counter = 0; k < 9; k++ )
        {
          if ( sudoku->info[i][j][k] == NO )
          {
            counter++;
          }
        }

        if ( counter < 8 )
        {
          continue;
        }

        if ( counter > 8 )
        {
          return 1;
        }

        // Find the first possible value for the cell
        for ( k = 0; k < 9; k++ )
        {
          if ( sudoku->info[i][j][k] == MAYBE )
          {
            break;
          }
        }

        // If no possible value is found, return an error
        if ( set_cell( sudoku, i, j, k + 1 ) )
        {
          return 1;
        }

        flag = -1;

        /* {printf("Counter >= 8 !\n"); exit(1);} */
      }
    }

    return flag;
  }

  // Finds rows in the Sudoku grid that can be set based on current information.
  int find_rows( sudoku_info* sudoku )
  {
    int i;
    int j;
    int k;
    int counter;
    int flag = 0;

    for ( i = 0; i < 9; i++ )
    {
      for ( j = 0; j < 9; j++ )
      {
        counter = 0;
        for ( k = 0; k < 9; k++ )
        {
          if ( sudoku->info[k][j][i] == YES )
          {
            counter = -1;
            break;
          }

          if ( sudoku->info[k][j][i] == NO )
          {
            counter++;
          }
        }

        if ( counter < 8 )
        {
          continue;
        }

        if ( counter > 8 )
        {
          return 1;
        }

        for ( k = 0; k < 9; k++ )
        {
          if ( sudoku->info[k][j][i] == MAYBE )
          {
            break;
          }
        }

        if ( k == 9 )
        {
          printf( "Internal error (find_rows)\n" );
          exit( 1 );
        } /* for testing */

        if ( set_cell( sudoku, k, j, i + 1 ) )
        {
          /*printf("Internal error\n"); test();*/
          return 1;
        }

        flag = -1;
      }
    }

    return flag;
  }

  // Finds columns in the Sudoku grid that can be set based on current information.
  int find_columns( sudoku_info* sudoku )
  {
    int i;
    int j;
    int k;
    int counter;
    int flag = 0;

    for ( i = 0; i < 9; i++ )
    {
      for ( j = 0; j < 9; j++ )
      {
        counter = 0;
        for ( k = 0; k < 9; k++ )
        {
          if ( sudoku->info[j][k][i] == YES )
          {
            counter = -1;
            break;
          }

          if ( sudoku->info[j][k][i] == NO )
          {
            counter++;
          }
        }

        if ( counter < 8 )
        {
          continue;
        }

        if ( counter > 8 )
        {
          return 1;
        }

        for ( k = 0; k < 9; k++ )
        {
          if ( sudoku->info[j][k][i] == MAYBE )
          {
            break;
          }
        }

        if ( k > 8 )
        {
          printf( "Internal error (find_columns)\n" );
          exit( 1 );
        } /* for testing */

        if ( set_cell( sudoku, j, k, i + 1 ) )
        {
          return 1;
        }

        flag = -1;
      }
    }

    return flag;
  }

  // Finds 3x3 grids in the Sudoku grid that can be set based on current information.
  int find_grids( sudoku_info* sudoku )
  {
    int flag = 0;

    for ( int i = 0; i < 9; i++ )
    {
      for ( int j = 0; j < 3; j++ )
      {
        for ( int k = 0; k < 3; k++ )
        {
          int counter = 0;
          int l = 0;
          int m = 0;
          for ( l = j * 3; l < ( j + 1 ) * 3; l++ )
          {
            for ( m = k * 3; m < ( k + 1 ) * 3; m++ )
            {
              if ( sudoku->info[l][m][i] == YES )
              {
                counter = -1;
                break;
              }

              if ( sudoku->info[l][m][i] == NO )
              {
                counter++;
              }
            }

            if ( counter == -1 )
            {
              break;
            }
          }

          if ( counter < 8 )
          {
            continue;
          }

          if ( counter > 8 )
          {
            return 1;
          }

          for ( l = j * 3; l < ( j + 1 ) * 3; l++ )
          {
            for ( m = k * 3; m < ( k + 1 ) * 3; m++ )
            {
              if ( sudoku->info[l][m][i] == MAYBE )
              {
                break;
              }
            }

            if ( ( m < ( k + 1 ) * 3 ) && sudoku->info[l][m][i] == MAYBE )
            {
              break;
            }
          }

          if ( m == ( k + 1 ) * 3 )
          {
            printf( "Internal error (find_grids)\n" );
            exit( 1 );
          } /* for testing */

          if ( set_cell( sudoku, l, m, i + 1 ) )
          {
            return 1;
          }

          flag = -1;
        }
      }
    }

    return flag;
  }

  // Displays the sudoku grid
  void display_grid( sudoku_info* sudoku )
  {
    int   i;
    int   j;
    char  buffer[10];

    /* printf("---------"); */
    printf( "\n" );
    for ( i = 0; i < 9; i++ )
    {
      for ( j = 0; j < 9; j++ )
      {
        printf( "%d", sudoku->grid[i][j] );
        _itoa_s( sudoku->grid[i][j], buffer, 10 );
        OutputDebugStringA( buffer );
        if ( j % 3 == 2 )
        {
          printf( "  " );
        }
      }

      printf( "\n" );
      if ( i % 3 == 2 )
      {
        printf( "\n" );
      }
    }

    OutputDebugStringA( "\n" );

    /* printf("---------\n"); */
  }

  // Finds a cell to assume a value for further solving
  void find_assume( sudoku_info* sudoku, int* i, int* j, int* value )
  {
    int counter = 0;
    int most = -1;

    // Find the cell with the most possible values
    for ( int k = 0; k < 9; k++ )
    {
      for ( int l = 0; l < 9; l++ )
      {

  // Skip cells that are already filled in
        if ( sudoku->grid[k][l] )
        {
          continue;
        }

        counter = 0;

        // Count the number of possible values for the cell
        for ( int m = 0; m < 9; m++ )
        {
          if ( sudoku->info[k][l][m] == NO )
          {
            counter++;
          }
        }

        // If the number of possible values is greater than the current maximum, update the maximum
        if ( counter > most )
        {
          *i = k;
          *j = l;
          most = counter;
        }
      }
    }

    // Find the first possible value for the cell
    for ( int k = 0; k < 9; k++ )
    {
      if ( sudoku->info[*i][*j][k] == MAYBE )
      {
        *value = k + 1;
        break;
      }
    }
  }

}

CString GeneratePuzzle()
{
  CString     puzzle;
  sudoku_info sudoku;
  init( &sudoku );
  fill( &sudoku );
  //display_grid( &sudoku );
  thin( &sudoku );

  wchar_t  buffer[2]{};
  for ( int i = 0; i < 9; i++ )
  {
    for ( int j = 0; j < 9; j++ )
    {
      _itow_s( sudoku.grid[i][j], buffer, 10 );
      puzzle += buffer;
    }
  }

  return puzzle;
}

// int main()
// {
//   GeneratePuzzle();
//   return 0;
// }
