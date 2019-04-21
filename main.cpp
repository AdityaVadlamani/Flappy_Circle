/* ---------------------- *
 * |    Flappy Circle   | *
 * ---------------------- *
 * Authors: Aditya Vadlamani, Nick Vocaire
 * Date: 12/05/18
 * Class: Clingan 10:20
 */

//Necessary Libraries
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHBuzzer.h>
#include <stdlib.h>
#include <string.h>

//Define some constants of the program
#define GRAVITY 1
#define CONSTANTXSPEED 8
#define JUMPMAGNITUDE 8
#define BACKGROUND LIGHTSLATEGRAY
#define NUMBER_OF_PIPES 6
#define PLAYERS_ON_LEADERBOARD 5
#define DIFFICULTY 8
#define MIN_PIPE_HEIGHT 30
#define MIN_PASSING_MULTIPLIER 6
#define NUM_COLOR_OPTIONS 10

//Use standard namespace
using namespace std;

/*
 * Character class for the object that is in the game.
 * (i.e the player controller object)
 */
class character {
//Functions and constuctor for the character
public:
    //Default constructor
    character();
    character(int a, int b, int c, float aSpeed, float bSpeed);

    int getX();
    int getY();
    int getRadius();
    float getYSpeed();
    void setX(int a);
    void setY(int b);
    void setRadius(int c);
    void jump();
    void move();
    void setColor(int c);
    int getColor();

//Character attributes
private:
    int x, y, radius;
    int color;
    float xSpeed, ySpeed;

};

/*
 * Pipe class for the obstacles in the game.
 */
class pipe {
//Functions and constuctor for the pipe
public:
    //Constuctor
    pipe();
    pipe(int a, int b, int wid, int hi);

    int getWidth();
    int getHeight();
    int getX();
    int getY();
    void setWidth(int a);
    void setHeight(int b);
    void setX(int a);
    void setY(int b);

//Pipe attributes
private:
    int x, y, height, width;
};

/*
 * Player class for the user playing the game.
 */
class player {
//Functions and constuctor for the pipe
public:
    //Constuctor
    player();
    player(char nm[]);

    char* getInitials();
    void setInitials(char nm[]);
    int getScore();
    void setScore(int i);

//Player attributes
private:
    int score;
    char initials[4];
};

/*
 * Function Prototypes.
 */
void loadGame(int *s);
void resetGame();
void intro();
void runState(int *s);
void drawState(int *s);
void checkCollision(int *s, pipe p[], character c);
void gameOver(int *s);
void nextScreen();
void modifyLeaderboard(player p, player v[]);
void sort(player v[]);

/*
 * Globally Defined variables (not instantiated).
 */
//Menu buttons and back button
FEHIcon::Icon menuStartBut, menuRulesBut, menuStatsBut, menuCreditsBut, backBut, inputNameEnter, inputColorEnter;

//Array of all the FEHLCDColor and and index variable
int allColors[] = {RED,BLACK,WHITE,GOLDENROD,BLUE,YELLOW,GRAY,PURPLE,DARKGREEN,ORANGE};
int colorIndex;

//Objects
character user;
pipe pipes[NUMBER_OF_PIPES];
player currentPlayer;
player leaderboard[PLAYERS_ON_LEADERBOARD];

//Character for displayed letter
char displayLetter[1];

//Main function, is called when the program starts
int main(void)
{
    //Current state of the game.
    //(0 = menu, 1 = game, 2 = rules, 3 = credits, 4 = input name, 5 = stats, 6 = character color)
    int state;

  //X and Y coordinate of a users touch
    float xTouch, yTouch;

    //Loads global variables instantiated and runs intro
    intro();

    //Waits for the user to press the screen to contuinue
    while(!LCD.Touch(&xTouch, &yTouch));

    //Load game
    loadGame(&state);

    //Loop indefinitly until the proteus is turned off
    while(true) {
        runState(&state);

        //Sleep so the game doesn't run infinitly fast.
        Sleep(30);
    }
}

/*
 * Load game function.
 */
void loadGame(int *s) {
    //Set background color of game
    LCD.SetBackgroundColor(BACKGROUND);
    LCD.Clear();

    //Load Icons
    menuStartBut.SetProperties("Start", 100, 80, 100, 35, GREEN, SCARLET);
    menuRulesBut.SetProperties("Rules", 100, 120, 100, 35, GREEN, SCARLET);
    menuStatsBut.SetProperties("Stats", 100, 160, 100, 35, GREEN, SCARLET);
    menuCreditsBut.SetProperties("Credits", 100, 200, 100, 35, GREEN, SCARLET);
    backBut.SetProperties("Back", 250, 200, 60, 35, GREEN, SCARLET);
    inputNameEnter.SetProperties("Input", 200, 200, 100, 35, GREEN, SCARLET);
    inputColorEnter.SetProperties("Choose", 200, 200, 100, 35, GREEN, SCARLET);

    //Reset the game
    resetGame();

    //Set leaderboard players to default values
    for (int i = 0; i < PLAYERS_ON_LEADERBOARD; i++) {
      leaderboard[i] = player("AAA");
    }

     //Set current index to zero
     colorIndex = 0;

    //Load menu state and draw it
    *s = 0;
    drawState(s);
}

/*
 * Reset Game function.
 */
void resetGame() {
    //Load objects
    user = character(30, 130, 10, CONSTANTXSPEED, 0);
    for (int i = 0; i < NUMBER_OF_PIPES; i+=2) {
        pipes[i] = pipe(75+(i*50), 0, 30, MIN_PIPE_HEIGHT);
        pipes[i+1] = pipe(75+(i*50), 240, 30, MIN_PIPE_HEIGHT);
    }

    //Reset inputNamebutton string
    inputNameEnter.ChangeLabelString("Input");

    //Set current display letter to A
    displayLetter[0] = 'A';

    //Set current player name to empty string
    currentPlayer = player("");

    //Reset color
    colorIndex = 0;
}

/*
 * Prints the intro of the game to the screen.
 */
void intro() {
    //Clear screen
    LCD.Clear();

    LCD.WriteLine("In a dystopian future");
    LCD.WriteLine("where the only way to live");
    LCD.WriteLine("is to ignore the laws of");
    LCD.WriteLine("gravity,");
    LCD.WriteLine("you must be....");
    LCD.WriteLine("(Tap the screen to continue)");
}

/*
 * Runs the current state.
 * (i.e executes the functionality of the current state)
 */
void runState(int *s) {
    //Boolean to check if the screen was touched
    bool touched = false;
    //X and Y coordinate of a users touch
    float xTouch, yTouch;

    //Get the position of a touch if the screen was touched
    touched = LCD.Touch(&xTouch, &yTouch);

    switch(*s) {
        //Menu
        case 0:
            if (menuStartBut.Pressed(xTouch, yTouch, 0)) {
                //Reset game
                resetGame();

                //Set the state to the game
                *s = 6;

                //Clear screen
                LCD.Clear();

                //Draw the state and wait give the user a second to start
                drawState(s);
            } else if (menuRulesBut.Pressed(xTouch, yTouch, 0)) {
                //Set the state of the game and draw it
                *s = 2;
                drawState(s);
            } else if (menuStatsBut.Pressed(xTouch, yTouch, 0)) {
                //Set the state of the game and draw it
                *s = 5;
                drawState(s);
            } else if (menuCreditsBut.Pressed(xTouch, yTouch, 0)) {
                //Set the state of the game and draw it
                *s = 3;
                drawState(s);
            }
            break;
        //Game
        case 1:
            //Clear the player
            LCD.SetFontColor(BACKGROUND);
            LCD.FillCircle(user.getX(), user.getY(), user.getRadius());

            //Check for a touch
            if(touched) {
                //Beep
                Buzzer.Buzz(20);

                user.jump();
            }

            //Move character
            user.move();

            //Check for collision
            checkCollision(s, pipes, user);

            //Draw the game
            drawState(s);
            break;
        //Rules
        case 2:
            //Check for a click on the back button
            if (backBut.Pressed(xTouch, yTouch, 0)) {
              //Set state and draw it
              *s = 0;
              drawState(s);
            }
            break;
        //Credits
        case 3:
            //Check for a click on the back button
            if (backBut.Pressed(xTouch, yTouch, 0)) {
              //Set state and draw it
              *s = 0;
              drawState(s);
            }
            break;
        //Input name
        case 4:
             //Check for a touch to the enter button else check for a touch in general
             if (inputNameEnter.Pressed(xTouch, yTouch, 0)) {
                if (strlen(currentPlayer.getInitials()) < 3) {
                    //If the name is not full, concatenate character
                    strcat(currentPlayer.getInitials(), displayLetter);

                    //Check for last letter and change the button
                    if (strlen(currentPlayer.getInitials()) == 3) {
                        inputNameEnter.ChangeLabelString("Enter");
                    }

                    //Draw state
                    drawState(s);
                } else {
                    //If it is full, add the player to the leaderboard
                    modifyLeaderboard(currentPlayer, leaderboard);

                    //Set and draw the state for the leaderboard
                    *s = 5;
                    drawState(s);
                }
             } else if(touched) {
                    /*
                     * Shift the ASCII value to some number between 0-25
                     * Increment the letter and mod 26 to ensure value between 0-25
                     * Shift the ASCII value to be some number between 65-90 to make it a letter between A-Z
                     */
                    displayLetter[0] = ((displayLetter[0] - 64) % 26) + 65;
                    drawState(s);
                }

            break;
        //Stats
        case 5:
            //Check for a click on the back button
            if (backBut.Pressed(xTouch, yTouch, 0)) {
              //Set state and draw it
              *s = 0;
              drawState(s);
            }
            break;
         //Input color
         case 6:
            if (inputColorEnter.Pressed(xTouch, yTouch, 0)) {
                //Set color to user's color
                user.setColor(allColors[colorIndex]);

                //Clear the screen
                LCD.Clear(BACKGROUND);

                //Set the state and draw it
                *s = 1;
                drawState(s);

                //Sleep
                Sleep(1.0);
            } else if (touched) {
                colorIndex = (colorIndex+1)%NUM_COLOR_OPTIONS;

                //Draw state
                drawState(s);
            }
            break;
    }
}

/*
 * Draw the current state.
 * (i.e draws the icons or objects of the state)
 */
void drawState(int *s) {
    switch(*s) {
        //Menu
        case 0:
            //Clear screen
            LCD.Clear(BACKGROUND);

            //Draw title
            LCD.SetFontColor(SCARLET);
            LCD.WriteAt("Flappy Circle", 70, 30);

            //Draw buttons
            menuStartBut.Draw();
            menuRulesBut.Draw();
            menuStatsBut.Draw();
            menuCreditsBut.Draw();
            break;
        //Game
        case 1:
            //Draw player
            LCD.SetFontColor(user.getColor());
            LCD.FillCircle(user.getX(), user.getY(), user.getRadius());

            LCD.SetFontColor(BLACK);
            LCD.WriteAt(currentPlayer.getScore(), 110, 10);

            //Draw Pipes
            LCD.SetFontColor(GREEN);
            for (int i = 0; i < NUMBER_OF_PIPES; i+=2) {
                //Top pipe
                LCD.FillRectangle(pipes[i].getX(), pipes[i].getY(),
                                     pipes[i].getWidth(), pipes[i].getHeight());
                //Bottom pipe
                LCD.FillRectangle(pipes[i+1].getX(), pipes[i+1].getY() - pipes[i+1].getHeight(),
                                     pipes[i+1].getWidth(), pipes[i+1].getHeight());
            }
            break;
        //Rules
        case 2:
            //Clear screen
            LCD.Clear(BACKGROUND);

            //Write text
            LCD.SetFontColor(BLACK);
            LCD.WriteLine("How to play");
            LCD.WriteLine("-------------------------");
            LCD.WriteLine("The goal is to keep the");
            LCD.WriteLine("circle from touching the");
            LCD.WriteLine("pipes on the screen.");
            LCD.WriteLine("");
            LCD.WriteLine("Tap the screen to make thecircle jump.");

            //Draw back button
            backBut.Draw();
            break;
        //Credits
        case 3:
            //Clear screen
            LCD.Clear(BACKGROUND);

            //Write text
            LCD.SetFontColor(BLACK);
            LCD.WriteLine("This amazing game was");
            LCD.WriteLine("created by none other than");
            LCD.WriteLine("the FEH dynamic duo team");
            LCD.WriteLine("N/A consisting of");
            LCD.WriteLine("Nick Vocaire and");
            LCD.WriteLine("Aditya Vadlamani.");

            //Draw back button
            backBut.Draw();
            break;
        //Input name
        case 4:
            //Clear screen
            LCD.Clear(BACKGROUND);

            //Draw text to prompt user
            LCD.SetFontColor(BLACK);
            LCD.WriteLine("Tap the screen to change");
            LCD.WriteLine("the letter and click enter to choose.\n");
            LCD.WriteAt(currentPlayer.getInitials(), 130, 120);
            LCD.WriteAt(displayLetter[0],140,150);

            //Draw button for user to enter name
            inputNameEnter.Draw();
            break;
        //Stats
        case 5:
            //Clear screen
            LCD.Clear(BACKGROUND);

            //Draw text to show user leaderboard;
            LCD.SetFontColor(BLACK);
            LCD.WriteAt("Top", 120,10);
            LCD.WriteAt(PLAYERS_ON_LEADERBOARD, 160,10);
            LCD.WriteAt("Name      Score",70,30);
            for (int i = 0; i < PLAYERS_ON_LEADERBOARD; i++){
              LCD.WriteAt(leaderboard[i].getInitials(),70,30*(i + 2));
              LCD.WriteAt(leaderboard[i].getScore(),206,30*(i + 2));
            }

            //Draw back button
            backBut.Draw();
            break;
        //Input color
      case 6:
        //Clear screen
            LCD.Clear(BACKGROUND);

            //Draw text to prompt user
            LCD.SetFontColor(BLACK);
            LCD.WriteLine("Tap the screen to change");
            LCD.WriteLine("the color and click chooseto choose.\n");

            LCD.SetFontColor(allColors[colorIndex]);
            LCD.FillCircle(150, 120, 15);

            //Draw button for user to enter name
            inputColorEnter.Draw();
        break;
    }
}

/*
 * Checks for collision of the player and a pipe.
 */
void checkCollision(int *s, pipe p[], character c) {
    //Variables to check collision with p
    int RIGHT_SIDE = c.getX() + c.getRadius();
    int LEFT_SIDE = c.getX() - c.getRadius();
    int TOP_SIDE = c.getY() - c.getRadius();
    int BOTTOM_SIDE = c.getY() + c.getRadius();

    //Check for collision with p
    for (int i = 0; i < NUMBER_OF_PIPES; i+=2){
        //Top pipe
        if(RIGHT_SIDE >= p[i].getX() && LEFT_SIDE <= p[i].getX() + p[i].getWidth()
                        && TOP_SIDE <= p[i].getY() + p[i].getHeight() && BOTTOM_SIDE >= p[i].getY()) {
            gameOver(s);
            return;
        }

        //Bottom Pipe
        if(RIGHT_SIDE >= p[i+1].getX() && LEFT_SIDE <= p[i+1].getX() + p[i+1].getWidth()
                        && TOP_SIDE <= p[i+1].getY() && BOTTOM_SIDE >= p[i+1].getY() - p[i+1].getHeight()) {
            gameOver(s);
            return;
        }
    }

    //Check collision with the top of the screen
    if(TOP_SIDE <= 0){
           c.setY(c.getRadius());
    }

    //Check collision with the bottom of the screen
    if(BOTTOM_SIDE >= 240){
       gameOver(s);
       return;
    }


    //Check for collision with the opposite side of the screen
    if(RIGHT_SIDE >= 320) {
        nextScreen();
  }

}

/*
 * The gameover screen
 */
void gameOver(int *s) {
    //Clear screen
    LCD.Clear(BLACK);

    //Give a game over message
    LCD.SetFontColor(WHITE);
    LCD.WriteAt("Game Over", 110, 110);
    Buzzer.Tone(FEHBuzzer::C5,200);
    Buzzer.Tone(FEHBuzzer::C5,200);
    Buzzer.Tone(FEHBuzzer::C3,1000);

    //Clear sceen
    LCD.Clear(BACKGROUND);

    //Check to see if user made leaderboard
    if (currentPlayer.getScore() > leaderboard[PLAYERS_ON_LEADERBOARD-1].getScore()) {
        //Go to the input name screen
        *s = 4;

        //Draw the input name screen
        drawState(s);
    } else {
        //Go to the leaderboard screen
        *s = 5;

        //Draw the input name screen
        drawState(s);
    }

}

/*
 * Generates a new screen after the player gets to the end of the screen
 */
void nextScreen() {
    //Find minimum passing distance
    int minPass = user.getRadius()*MIN_PASSING_MULTIPLIER;

    //Clear screen
    LCD.Clear(BACKGROUND);

    //Increment score
    currentPlayer.setScore(currentPlayer.getScore() + 1);

    //Seed rand
    srand(TimeNow());

    //Loop though each pipe and change the height randomly
    for (int i = 0; i < NUMBER_OF_PIPES; i+=2) {
        //Randomly set the pipe height based on score
        pipes[i].setHeight(MIN_PIPE_HEIGHT + rand()%(DIFFICULTY*currentPlayer.getScore()));
        pipes[i+1].setHeight(MIN_PIPE_HEIGHT + rand()%(DIFFICULTY*currentPlayer.getScore()));
        LCD.WriteLine(pipes[i].getHeight());

        //Check to see if the sum of the heights makes it impossible to pass
        if (pipes[i].getHeight() + pipes[i+1].getHeight() > 240 - minPass) {
            //Check how much bigger the pipes are from the minimum possible passing height
            int diff = pipes[i].getHeight() + pipes[i+1].getHeight() - (240 - minPass);

            //Subtract half the difference from the top pipe and bottom pipe
            pipes[i].setHeight(pipes[i].getHeight()-diff/2);
            pipes[i+1].setHeight(pipes[i+1].getHeight()-diff/2);

            //If changing the pipe height for the top pipe makes it less than the min height,
            //set it to the min height and set the bottom pipe such that the pipes are possible to pass
            if (pipes[i].getHeight() < MIN_PIPE_HEIGHT) {
                pipes[i].setHeight(MIN_PIPE_HEIGHT);
                pipes[i+1].setHeight(240 - MIN_PIPE_HEIGHT - minPass);
            }

            //If changing the pipe height for the bottom pipe makes it less than the min height,
            //set it to the min height and set the top pipe such that the pipes are possible to pass
            if (pipes[i+1].getHeight() < MIN_PIPE_HEIGHT) {
                pipes[i+1].setHeight(MIN_PIPE_HEIGHT);
                pipes[i].setHeight(240 - MIN_PIPE_HEIGHT - minPass);
            }
        }
    }

    //Set the user
    user.setX(user.getRadius());
}

/*
 * Modifys leaderboard of players (adding players)
 */
void modifyLeaderboard(player p, player v[]) {
   v[PLAYERS_ON_LEADERBOARD-1] = p;
   sort(v);
}

/*
 * Sorts leaderboard of players
 */
void sort(player q[]){
  for (int i = 0; i < PLAYERS_ON_LEADERBOARD - 1; i++){
    for (int j = i + 1; j <PLAYERS_ON_LEADERBOARD; j++){
      if(q[i].getScore() < q[j].getScore()){
         //Swap the scores
        int temp = q[i].getScore();
        q[i].setScore(q[j].getScore());
        q[j].setScore(temp);
    //Swap the names
        char tempI[4];
        strcpy(tempI,q[i].getInitials());
        q[i].setInitials(q[j].getInitials());
        q[j].setInitials(tempI);

      }
    }
  }
}
/*
 * Character constructor and functions.
 */
character::character() {
}
character::character(int a, int b, int c, float aSpeed, float bSpeed) {
    x = a;
    y = b;
    radius = c;
    xSpeed = aSpeed;
    ySpeed = bSpeed;
}
void character::jump(){
    //Set y speed to the jump magnitude
    ySpeed = -JUMPMAGNITUDE;
}
void character::move(){
    //Change ySpeed based on gravity
    ySpeed += GRAVITY;

    //Move character
    x += xSpeed;
    y += ySpeed;
}
int character::getX() {
    return x;
}
int character::getY() {
    return y;
}
int character::getRadius() {
    return radius;
}
float character::getYSpeed() {
    return ySpeed;
}
void character::setX(int a) {
    x = a;
}
void character::setY(int b) {
    y = b;
}
void character::setRadius(int c) {
    radius = c;
}
void character::setColor(int c){
  color = c;
}
int character::getColor(){
  return color;
}

/*
 * Pipe constructor and functions.
 */
pipe::pipe() {
}
pipe::pipe(int a, int b, int wid, int hi) {
    x = a;
    y = b;
    width = wid;
    height = hi;
}
int pipe::getX() {
    return x;
}
int pipe::getY() {
    return y;
}
int pipe::getWidth() {
    return width;
}
int pipe::getHeight() {
    return height;
}
void pipe::setX(int a) {
    x = a;
}
void pipe::setY(int b) {
    y = b;
}
void pipe::setWidth(int a) {
    width = a;
}
void pipe::setHeight(int b) {
    height = b;
}

/*
 * Player constructor and functions.
 */
player::player() {
}
player::player(char nm[]) {
    strcpy(initials, nm);
    score = 0;
}
char* player::getInitials() {
    return initials;
}
int player::getScore() {
    return score;
}
void player::setScore(int i) {
    score = i;
}
void player::setInitials(char nm[]) {
    strcpy(initials, nm);
}
