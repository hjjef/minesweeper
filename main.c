//
//  main.c
//  Minesweeper
//
//  Created by Henry Jeffers on 8/8/17.
//  Copyright © 2017 Henry Jeffers. All rights reserved.
//


/*
 This is a simple minesweeper program
 The program asks the user for the size of the board, and the ammount of mines.
 Every turn the user is asked for coordinate points of their move and if they want to toggle flag or reveal the position.
 Every turn a new board is printed to the screen with the results of the turn.
 If a mine is revealed it is game over.
 If all mines are flagged the game is won.
 The numbers on each position is the number of mines that position is directly adjacent to (in the 8 positions immediately surrounding it).
 
 For this program,
 - # represents a covered position
 - F represents a flagged position
 - M represents a position with a mine
 - _ represents "0".
 - 1,2,3,4,5,6,7,8 represent the number of mines next to the position.
 
 For rules and strategy refer to www.minesweeper.info/wiki/strategy
 
 GOOD LUCK!!
 */

#include <stdio.h> //standard input output library
#include <stdlib.h>
#include <string.h> //string library

int mine_check = 1; //global variable checks if the game has been won. If at the end this is still a 1, the game is won. If it is a 0 the game has been lost.
#define Mine 0
#define Flag 1
#define Revealed 2 //macros for relative bit positions in the location.status_byte variable.

//#define DEBUG
//#define DEBUGTAB    //used in second tab
//#define DEBUGTAB2  //used in third tab
//#define DEBUGREVEAL //sets all positions to revealed
//#define DEBUGSTATUSBYTE //to debug the status_byte

struct Location
{
    int value;
    char status_byte;
}; //defining struct location.
struct Game
{
    int width;
    int height;
    int mines;
    int mines_flagged;
    int flags;
}; //defining struct game.

void initialize_mine_board(struct Game game, struct Location** mine_board);
void initialize_mine_values(struct Game game, struct Location** mine_board);
void make_move(struct Game game, struct Location** mine_board, int x, int y);
void get_move(int* x_ptr, int* y_ptr, int* flag_ptr, int max_x, int max_y);
void print_mine_board(struct Game game, struct Location** mine_board);
void uncover_zeros(struct Location** mine_board, int x, int y);    //function prototypes

int main()
{
    printf("**19063931 Henry Jeffers Assignment 3**\n\n");
    
    struct Game game;
    
    game.mines_flagged = 0;
    game.flags = 0;
    
    printf("Enter the width of the board: ");
    scanf("%d", &(game.width));
    printf("Enter the height of the board: ");
    scanf("%d", &(game.height));
    printf("Enter the number of mines: ");
    scanf("%d", &(game.mines));             // collecting information about the board.
    while(game.mines > game.width*game.height)
    {
        printf("Too many mines, please enter a valid value for the number of mines: ");
        scanf("%d", &(game.mines));
    }                                       // loops back until a valid number of mines is entered. The number of mines cant be larger than the size of the board.
    
#ifdef DEBUG
    printf("debug width, height, #mines: %d, %d, %d\n", game.width, game.height, game.mines);
#endif                                                        // (test works)
    
    struct Location** mine_board = malloc(((game.height)+2)*(sizeof(struct Location*)));
    int i;                                  // dynamically allocating the first column which has size height of the mine board +2.
    for(i=0;i<(game.height)+2;i++)
    {
        mine_board[i] = (struct Location*)malloc(((game.width)+2)*sizeof(struct Location));
    }                                       // dynamically allocating an array to each element in the first array creating a 2D array. This 2D array or "board" has a boarder.
#ifdef DEBUG
    mine_board[2][8].value = 64;
    printf("debug test array(64): %d\n", mine_board[2][8].value);     // (test works)
#endif
    
    initialize_mine_board(game, mine_board);
    initialize_mine_values(game, mine_board);
    print_mine_board(game, mine_board); // calling functions to initialize the mine board, initialize mine values and print board.
    
    int flag, x_location, y_location;   // declaring variables
    
    while((mine_check==1)&&!((game.mines_flagged==game.flags)&&(game.flags==game.mines)))
    {                                   // while global variable is not 0 (game not lost), and all mines are not flagged (game won).
        do
        {                               // the user is asked for their move.
            get_move(&x_location, &y_location, &flag, game.height, game.width);
        }while((mine_board[x_location][y_location].status_byte&(1<<Revealed))||((mine_board[x_location][y_location].status_byte&(1<<Flag))&&(flag==0)));
        // if chosen positon is already uncovered or position is flagged and user chooses to reveal, user prompted for another move.
        if(flag==0)
        {                               // if user chooses to reveal and not flag, the move is made and the board printed.
            make_move(game, mine_board, x_location, y_location);
            print_mine_board(game, mine_board);
        }
        else if(mine_board[x_location][y_location].status_byte&(1<<Flag))
        {                               // else if the user chooses to flag and the position is flagged, the flag is toggled and the number of flags is decremented.
            mine_board[x_location][y_location].status_byte^=(1<<Flag);
            game.flags--;
            if(mine_board[x_location][y_location].status_byte&(1<<Mine))
            {                           // if the user toggled a flag that was on a mine, the number of flagged mines is decremented and board is printed.
                game.mines_flagged--;
                print_mine_board(game, mine_board);
            }
            else
            {                           // else the board is printed.
                print_mine_board(game, mine_board);
            }
        }
        else
        {                              // else (if user chooses to flag and position is not flagged), the flag is set and the number of flags is incremented.
            mine_board[x_location][y_location].status_byte|=(1<<Flag);
            game.flags++;
            if(mine_board[x_location][y_location].status_byte&(1<<Mine))
            {                          // if the flag was placed on a mine, the number of mines flagged is incremented, and the board is printed.
                game.mines_flagged++;
                print_mine_board(game, mine_board);
            }
            else
            {                          // else the board is printed
                print_mine_board(game, mine_board);
            }
        }
    }
    
    if(mine_check==1)
    {                                  // if the global variable at the end of the game is 1, this means all mines were flagged. The game is won.
        printf("You beat the game, good work!\n");
    }
    else
    {                                  // if the global variable at the end of the game is 0, a mine was revealed and the game is over.
        printf("Game over! you have lost.\n");
    }
    
    for(i=0;i<game.height+2;i++)
    {
        free(mine_board[i]);
    }                                  // freeing dynamically allocated array
    free(mine_board);
    return(0);
}

/*
 Function initializes the mine board.
 Input: a 2D array of type struct Location, and a struct Game which includes the board width and height
 Output: a 2D array with "mines" planted randomly (the status_byte of randomly selected positions is changed to a 1 for the Mine bit)
 Output: all positions without mines have status_byte set to 0, including the border area. (this means they are not mines, flagged or revealed).
 */
void initialize_mine_board(struct Game game, struct Location** mine_board)
{
    int i, j;
    for(i=0;i<=(game.height+1);i++)
    {
        for(j=0;j<=(game.width+1);j++)
        {
            mine_board[i][j].status_byte = 0b00000000;
#ifdef DEBUGTAB
            char a = mine_board[i][j].status_byte;
            printf("debug reset status byte: %d\n", a);
#endif
        }
    }
    int temp_mines = game.mines;       // initializing temp_mines to the number of mines in the game
#ifdef DEBUG
    printf("debug #ofmines: %d\n", temp_mines);
#endif                             // (works as intended)
    int x,y;
    while(temp_mines>0)
    {                                  // while temp_mines is above zero
        y=rand()%game.height+1;        // y is initilized to a random value between 0 and game.height+1
        x=rand()%game.width+1;         // x is initilized to a random value between 0 and game.height+1
#ifdef DEBUGTAB
        printf("debug mine position: %dx, %dy\n", x, y);
#endif                        // (works as intended)
        if(!(mine_board[y][x].status_byte & (1<<Mine)))
        {                              // if the position is not already a mine, its status_byte is changed to a 1 for the "Mine" bit.
            mine_board[y][x].status_byte |= (1<<Mine);
            temp_mines--;              // and temp_mines is decremented.
#ifdef DEBUGSTATUSBYTE
            char a = mine_board[y][x].status_byte;
            int k;
            for(k=0;k<8;k++)
            {                          // this debug feature prints the status_byte of each mine in binary form.
                printf("status byte of mine position(f): %d\n", !!((a << k) & 0x80));
            }
#endif                      // (works as intended)
        }
    }
}

/*
 Function initializes the mine boards values depending on where the mines are.
 Input: The 2D mine board with mines placed on it, and struct Game which includes game width and height.
 Output: A mine board with numbers associated with each position.
 Output: The numbers represent how many mines are in their 3x3 square. (eg '2' means it is next to two mines).
 */
void initialize_mine_values(struct Game game, struct Location** mine_board)
{
    int i, j;
    for(i=0;i<=(game.height+1);i++)
    {
        for(j=0;j<=(game.width+1);j++)
        {                               // increments from 0 to game height and o to game width
            if((i==0)||(i==game.height+1)||(j==0)||(j==game.width+1))
            {                           // (if) for the first and last increment of each column and row of the board, the value is set to 0 and the status byte is set to revealed. (these are not part of the game).
                mine_board[i][j].value = 0;
                mine_board[i][j].status_byte |= (1<<Revealed);
#ifdef DEBUGTAB2
                printf("debug value if no mines: %d\ndebug statusbyte(decimal) if no mines: %d\n", mine_board[i][j].value, mine_board[i][j].status_byte);
#endif
            }
            else
            {                           // else the function checks every position around the position being tested for mines, if one is found, "count" is incremented.
                int count = 0;
                if((mine_board[i-1][j-1].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i][j-1].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i+1][j-1].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i-1][j].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i+1][j].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i-1][j+1].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i][j+1].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                if((mine_board[i+1][j+1].status_byte&(1<<Mine))==1)
                {
                    count++;
                }
                mine_board[i][j].value = count;  // the value of the position being checked is then set to count (representing how many mines surround it).
#ifdef DEBUGTAB2                 // this is repeated for every position on the board and stops before the border.
                printf("debug count: %d\n", count);
#endif
            }
        }
    }
}

/*
 Function to print the mine board, prints a formatted minesweeper board.
 Input: The mine board or 2D array, and the dimensions of the board in the type of struct Game.
 Output: A formatted minesweeper board printed to the console.
 */
void print_mine_board(struct Game game, struct Location** mine_board)
{
    int i,j;
    printf("    ");
    for(i=1;i<=(game.width);i++)
    {
        if(i<10)                   // prints the reference numbers for the columns
        {
            printf(" %d ", i);
        }
        else if((i>=10)&&(i<100))
        {
            printf(" %d", i);      // if the value has two digits it requires less space
        }
        else
        {
            printf("%d", i);       // if the mine board happens to be larger than 99 units wide, even less space is needed.
        }
    }
    printf("\n");
    for(i=1;i<=(game.height);i++)
    {
        if(10>i)                   // prints the reference numbers for the rows
        {
            printf("\n%d   ", i);
        }
        else if((i>=10)&&(i<100))
        {
            printf("\n%d  ", i);   // if the value has two digits it requires less space.
        }
        else
        {
            printf("\n%d ", i);    // if the value has tree digits it requires even less space
        }
        
        for(j=1;j<=(game.width);j++)
        {                          // while incrementing throught the whole board
#ifdef DEBUGREVEAL
            mine_board[i][j].status_byte |=(1<<Revealed);
#endif
#ifdef DEBUGTAB2
            printf("debug value (%d,%d): %d\ndebug status(%d,%d): %d\n", i, j, mine_board[i][j].value, i, j, mine_board[i][j].status_byte);
#endif
            if(mine_board[i][j].status_byte&(1<<Flag))
            {                      // if the position is a flag, an 'F' is printed.
                printf(" F ");
            }
            else if(mine_board[i][j].status_byte&(1<<Revealed))
            {                      // else if the position is Revealed, its relative contents are printed
                if(mine_board[i][j].status_byte&(1<<Mine))
                {
                    printf(" M ");  // 'M' is printed for a mine.
                }
                else if((mine_board[i][j].value)==0)
                {
                    printf(" _ ");  // '_' is printed for a zero
                }
                else
                {
                    printf(" %d ", mine_board[i][j].value);  // or the decimal integer is printed representing the value of the position
                }
            }
            else
            {
                printf(" # ");  // else if covered (not revealed), '#' is printed.
            }
        }
    }
    printf("\n");
}

/*
 Function to make a move
 Input: the game information of type struct Game, the mine board or 2D array, and the x and y coordinates of the chosen move.
 Output: A new mineboard with a move implemented, depending on user input. The only changes can be revealed or flagged.
 Note: The function does not ask the user for the move, it only excecutes a chosen move within the program
 */

void make_move(struct Game game, struct Location** mine_board, int x, int y)
{
    if(mine_board[x][y].status_byte&(1<<Mine))
    {                            // if the position is a mine, the mine is revealed and global variable mine_check is set to 0 to represent game over.
        mine_board[x][y].status_byte|=(1<<Revealed);
        mine_check = 0;
    }
    else
    {                           // else if not a mine, the position is revealed.
        mine_board[x][y].status_byte|=(1<<Revealed);
        if(mine_board[x][y].status_byte&(1<<Flag))
        {                       // if the position is a flag, flag is toggled which always results in a zero if it is initially a 1.
            mine_board[x][y].status_byte^=(1<<Flag);
        }
        if(mine_board[x][y].value==0)
        {                       // if the value of the chosen position is a zero, the uncover_zeros function is called to uncover all surrounding zeros.
            uncover_zeros(mine_board, x, y);
        }
    }
}

/*
 Function to reveal zeros directly connected by zeros to the original chosen zero. Function also reveals the value of the first position that is not a zero.
 Input: a mine board with mines and values set up. A chosen move in the form of x and y coordinates.
 Output: a modified mine board with all connected zeros and the first adjacent values with their revealed bit set to 1.
 */
/*
 For this function, every position around a chosen bit is checked if it is revealed and if it is a zero.
 If both of these conditions are true, the position is revealed and a new uncover_zeros function is started
 on the new position simultaneously. This continues for all positions that are not revealed and that are zeros.
 When the function hits a position that is not a zero, it reveals it and checks the next position, without starting a new function.
 When the function hits the border, since it is set to revealed and zero, it does not change it or start a new function.
 */
void uncover_zeros(struct Location** mine_board, int x, int y)
{
    if((!(mine_board[x-1][y-1].status_byte&(1<<Revealed)))&&(mine_board[x-1][y-1].value==0))
    {
        mine_board[x-1][y-1].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x-1, y-1);
    }
    mine_board[x-1][y-1].status_byte|=(1<<Revealed);
    if((!(mine_board[x][y-1].status_byte&(1<<Revealed)))&&(mine_board[x][y-1].value==0))
    {
        mine_board[x][y-1].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x, y-1);
    }
    mine_board[x][y-1].status_byte|=(1<<Revealed);
    if(!((mine_board[x+1][y-1].status_byte&(1<<Revealed)))&&(mine_board[x+1][y-1].value==0))
    {
        mine_board[x+1][y-1].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x+1, y-1);
    }
    mine_board[x+1][y-1].status_byte|=(1<<Revealed);
    if(!((mine_board[x-1][y].status_byte&(1<<Revealed)))&&(mine_board[x-1][y].value==0))
    {
        mine_board[x-1][y].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x-1, y);
    }
    mine_board[x-1][y].status_byte|=(1<<Revealed);
    if(!((mine_board[x-1][y+1].status_byte&(1<<Revealed)))&&(mine_board[x-1][y+1].value==0))
    {
        mine_board[x-1][y+1].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x-1, y+1);
    }
    mine_board[x-1][y+1].status_byte|=(1<<Revealed);
    if(!((mine_board[x+1][y].status_byte&(1<<Revealed)))&&(mine_board[x+1][y].value==0))
    {
        mine_board[x+1][y].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x+1, y);
    }
    mine_board[x+1][y].status_byte|=(1<<Revealed);
    if(!((mine_board[x][y+1].status_byte&(1<<Revealed)))&&(mine_board[x][y+1].value==0))
    {
        mine_board[x][y+1].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x, y+1);
    }
    mine_board[x][y+1].status_byte|=(1<<Revealed);
    if(!((mine_board[x+1][y+1].status_byte&(1<<Revealed)))&&(mine_board[x+1][y+1].value==0))
    {
        mine_board[x+1][y+1].status_byte|=(1<<Revealed);
        uncover_zeros(mine_board, x+1, y+1);
    }
    mine_board[x+1][y+1].status_byte|=(1<<Revealed);
}

/*
 Function to get a move from the player
 Input: pointer to the x and y coordinates of the move variable, and the flag variable. The width and height of the board.
 Output: The pointers to the chosen move and the flag decision.
 Note: This function does not make a move, it just stores the move.
 */
void get_move(int* x_ptr, int* y_ptr, int* flag_ptr, int max_x, int max_y)
{
    do
    {                   // keeps asking until all valid responses are entered.
        printf("Enter the x and y values and flag(1/0): \n");
        scanf("%d", y_ptr);  //inverted because y coordinate comes first
        scanf("%d", x_ptr);
        scanf("%d", flag_ptr);
    }while((*x_ptr<1)||(*x_ptr>max_x)||(*y_ptr<1)||(*y_ptr>max_y)||(*flag_ptr>1)||(*flag_ptr<0));
}
