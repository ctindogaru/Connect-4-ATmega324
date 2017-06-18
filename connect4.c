#define F_CPU 16000000

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define LEFT 101
#define UP 102
#define RIGHT 103
#define DOWN 104
#define UP_LEFT 105
#define UP_RIGHT 106
#define DOWN_LEFT 107
#define DOWN_RIGHT 108

#define GREEN 1
#define RED 2
#define NONE 0
#define M_LEFT 1
#define M_RIGHT -1

#define FINISHED 300
#define PLAY 400

int board[8][8];
int current_move;
int current_player;
int current_status;

/** Functie de initializare a tablei de joc **/
void init_board() {
    int i, j;
    for(i = 0; i < 8; i++)
        for(j = 0; j < 8; j++)
            board[i][j] = NONE;
    current_move = 0;
    current_player = GREEN;
    board[7][7] = GREEN;
    current_status = PLAY;
}

/**
    Functie ce verifica ca s-a terminat jocul de CONN4
    pe o singura directie (Sus, Jos, Stanga, Dreapta
                           sau Diagonale)
**/
int checkDirection(int row, int col, int color, int d) {
    if(board[row][col] == color)
        switch(d) {
            case UP:
                if(board[row][col] == board[row-1][col] &&
                board[row][col] == board[row-2][col] &&
                board[row][col] == board[row-3][col])
                    return 1;
                return 0;
                break;
            case DOWN:
                if(board[row][col] == board[row+1][col] &&
                board[row][col] == board[row+2][col] &&
                board[row][col] == board[row+3][col])
                    return 1;
                return 0;
                break;
            case LEFT:
                if(board[row][col] == board[row][col-1] &&
                board[row][col] == board[row][col-2] &&
                board[row][col] == board[row][col-3])
                    return 1;
                return 0;
                break;
            case RIGHT:
                if(board[row][col] == board[row][col+1] &&
                board[row][col] == board[row][col+2] &&
                board[row][col] == board[row][col+3])
                    return 1;
                return 0;
                break;
            case DOWN_LEFT:
                if(board[row][col] == board[row+1][col-1] &&
                board[row][col] == board[row+2][col-2] &&
                board[row][col] == board[row+3][col-3])
                    return 1;
                return 0;
                break;
            case DOWN_RIGHT:
                if(board[row][col] == board[row+1][col+1] &&
                board[row][col] == board[row+2][col+2] &&
                board[row][col] == board[row+3][col+3])
                    return 1;
                return 0;
                break;
            case UP_LEFT:
                if(board[row][col] == board[row-1][col-1] &&
                board[row][col] == board[row-2][col-2] &&
                board[row][col] == board[row-3][col-3])
                    return 1;
                return 0;
                break;
            case UP_RIGHT:
                if(board[row][col] == board[row-1][col+1] &&
                board[row][col] == board[row-2][col+2] &&
                board[row][col] == board[row-3][col+3])
                    return 1;
                return 0;
                break;
        }
    return 0;
}

/** Functie ce verifica ca jocul s-a terminat cu o victorie **/
int gameWon() {
    int i, j;
    int c = current_player;

    for(i = 0; i < 6; i++)
        for(j = 0; j < 6; j++) {
            if(i <= 2)
                if(checkDirection(i, j, c, DOWN))
                    return 1;
            if(i >= 3)
                if(checkDirection(i, j, c, UP))
                    return 1;
            if(j <= 3)
                if(checkDirection(i, j, c, RIGHT))
                    return 1;
            if(j >= 3)
                if(checkDirection(i, j, c, LEFT))
                    return 1;
            if(i <= 2 && j <= 4)
                if(checkDirection(i, j, c, DOWN_RIGHT))
                    return 1;
            if(i <= 2 && j >= 3)
                if(checkDirection(i, j, c, DOWN_LEFT))
                    return 1;
            if(i >= 3 && j <= 4)
                if(checkDirection(i, j, c, UP_RIGHT))
                    return 1;
            if(i >= 3 && j >= 3)
                if(checkDirection(i, j, c, UP_LEFT))
                    return 1;
        }
    return 0;
}

/** Functie ce returneaza ultimul rand liber de pe o
coloana **/
int get_last_free_row(int column) {
    int i = 5;
    while(board[i][column] == NONE) {
        i--;
    }
    return i+1;
}

/** Functie ce selecteaza coloana **/
void select_column(int column) {
    PORTA = column;
}

/**
    Functie ce muta LED-ul aprins in stanga sau in dreapta
    Prin stingerea ledului curent si aprinderea urmatorului
**/
void change_move(int direction) {
    board[7][current_move] = NONE;
    current_move += direction;
    if(current_move == -1)
        current_move = 7;
    if(current_move == 8)
        current_move = 0;
    board[7][current_move] = current_player;
}

/**
    Functie ce verifica ca tabla mai are mutari valide.
    (Nu este plina)
**/
int check_full() {
    int i, j;
    for(i = 0; i < 6; i++)
        for(j = 0; j < 8; j++)
            if(board[i][j] == 0)
                return 0;
    return 1;
}

/**
    Functie ce seteaza mutarea.
    (Pune jetonul pe ultimul rand disponibil de pe coloana aleasa)
**/
void make_move(int move, int player) {
    int row = get_last_free_row(move);
    if(row < 6) {
        board[row][move] = player;
        if(gameWon() || check_full()) {
            current_status = FINISHED;
        } else {
            if(player == GREEN)
                current_player = RED;
            if(player == RED)
                current_player = GREEN;
            board[7][move] = current_player;
        }
    }
}

/** Functie ce stinge toate LED-urile **/
void turn_off_all_leds() {
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);
    PORTB &= ~(1 << PB2);
    PORTB &= ~(1 << PB3);
    PORTB &= ~(1 << PB4);
    PORTB &= ~(1 << PB5);
    PORTB &= ~(1 << PB6);
    PORTB &= ~(1 << PB7);
    PORTC &= ~(1 << PC0);
    PORTC &= ~(1 << PC1);
    PORTC &= ~(1 << PC2);
    PORTC &= ~(1 << PC3);
    PORTC &= ~(1 << PC4);
    PORTC &= ~(1 << PC5);
    PORTC &= ~(1 << PC6);
    PORTC &= ~(1 << PC7);
}

/**
    Functie ce aprinde LED-ul de o culoare specifica de pe un rand.
    NOTE!!! (Este folosit impreuna cu select_column)
**/
void select_row(int row, int color) {
    turn_off_all_leds();
    if(color == GREEN) {
        if(row == 0) {
            PORTB |= (1 << PB0);
        }
        if(row == 1) {
            PORTB |= (1 << PB2);
        }
        if(row == 2) {
            PORTB |= (1 << PB4);
        }
        if(row == 3) {
            PORTB |= (1 << PB6);
        }
        if(row == 4) {
            PORTC |= (1 << PC6);
        }
        if(row == 5) {
            PORTC |= (1 << PC4);
        }
        if(row == 6) {
            PORTC |= (1 << PC2);
        }
        if(row == 7) {
            PORTC |= (1 << PC0);
        }
    }
    if(color == RED) {
        if(row == 0) {
            PORTB |= (1 << PB1);
        }
        if(row == 1) {
            PORTB |= (1 << PB3);
        }
        if(row == 2) {
            PORTB |= (1 << PB5);
        }
        if(row == 3) {
            PORTB |= (1 << PB7);
        }
        if(row == 4) {
            PORTC |= (1 << PC7);
        }
        if(row == 5) {
            PORTC |= (1 << PC5);
        }
        if(row == 6) {
            PORTC |= (1 << PC3);
        }
        if(row == 7) {
            PORTC |= (1 << PC1);
        }
    }
}

void print_board() {
    int i, j;
    for(i = 0; i < 8; i++) {
        for(j = 0; j < 8; j++) {
            select_column(j);
            select_row(i, board[i][j]);
            _delay_us(10);
            turn_off_all_leds();
        }
    }
}

//Initializeaza placuta
void init() {
    //Seteaza PA0 - 2 ca pini de iesire
    DDRA |= (1 << PA0);
    DDRA |= (1 << PA1);
    DDRA |= (1 << PA2);

    //Seteaza PA3 - 5 ca pini de intrare
    //Si activeaza pull-up
    DDRA &= ~(1 << PA3);
    DDRA &= ~(1 << PA4);
    DDRA &= ~(1 << PA5);
    PORTA |= (1 << PA3);
    PORTA |= (1 << PA4);
    PORTA |= (1 << PA5);

    //Seteaza PB0 - 7 si PC0 - 7 ca pini de iesire
    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB3);
    DDRB |= (1 << PB4);
    DDRB |= (1 << PB5);
    DDRB |= (1 << PB6);
    DDRB |= (1 << PB7);
    DDRC |= (1 << PC0);
    DDRC |= (1 << PC1);
    DDRC |= (1 << PC2);
    DDRC |= (1 << PC3);
    DDRC |= (1 << PC4);
    DDRC |= (1 << PC5);
    DDRC |= (1 << PC6);
    DDRC |= (1 << PC7);

    //Stinge ledurile
    turn_off_all_leds();

    //Initializeaza jocul
    init_board();
}

int main(void) {
	init();
	
	int i, j;
	int t3 = 1;
	int t4 = 1;
	int t5 = 1;
	
	while(1) {
        if(current_status == PLAY) {
            if((PINA & (1 << PA3)) == 0) {
                t3++;
            } else {
                t3 = 1;
            }
            if(t3 == 100) {
                change_move(M_LEFT);
            }

            if((PINA & (1 << PA4)) == 0) {
                t4++;
            } else {
                t4 = 1;
            }
            if(t4 == 100) {
                make_move(current_move, current_player);
            }

            if((PINA & (1 << PA5)) == 0) {
                t5++;
            } else {
                t5 = 1;
            }
            if(t5 == 100) {
                change_move(M_RIGHT);
            }
        } else {
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
					board[i][j] = current_player;
		}
		print_board();
	}
	
	return 0;
}
