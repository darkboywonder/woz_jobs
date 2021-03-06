//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);
   
    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // initial velocity
    double x_velocity = drand48() * 1.0 + 0.9;
    double y_velocity = 2.0;    
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {      
            // move ball along x-axis
            move(ball, x_velocity, y_velocity);
            
            // bounce off right edge of window
            if((getX(ball) + getWidth(ball)) >= (getWidth(window)))
            {
                x_velocity = -x_velocity;
            }
            
            // bounce off bottom edge of window
            if((getY(ball) + getWidth(ball)) >= (getHeight(window)))
            {
                lives = lives - 1;
                if (lives == 0)
                { 
                    removeGWindow(window, label);
                    GLabel g_over = newGLabel("You Lose Game Over");
                    setFont(g_over, "SansSerif-36");
                    add(window, g_over);
                    
                    double x = (getWidth(window) - getWidth(g_over)) / 2;
                    double y = (getHeight(window) - getHeight(g_over)) / 2;
                    setLocation(g_over, x, y);
                    
                    // wait for click before exiting
                    waitForClick();

                    // game over
                    closeGWindow(window);
                    return 0;   
                }
                else
                {
                setLocation(ball, 190, 290);
                pause(500);
                waitForClick();
                }
            }

            // bounce off left edge of window
            if (getX(ball) <= 0)
            {
                x_velocity = -x_velocity;
            }
            
            // bounce off top edge of window
            if (getY(ball) <= 0)
            {
                y_velocity = -y_velocity;
            }
            
            // linger before moving again
            pause(10);
        
        
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows cursor
                double x = getX(event) - getWidth(paddle) / 2;
                double y = 500;
                setLocation(paddle, x, y);
            }
        }
        
        GObject object = detectCollision(window, ball);
        if(object != NULL)
        {
            if(strcmp(getType(object), "GRoundRect") == 0)
            {
                points = points + 1;
                y_velocity = -y_velocity;
                removeGWindow(window, object);
                updateScoreboard(window, label, points);
                if(points == 50)
                {
                    removeGWindow(window, label);
                    GLabel y_win = newGLabel("You Win! Game Over");
                    setFont(y_win, "SansSerif-36");
                    add(window, y_win);
                    
                    double x = (getWidth(window) - getWidth(y_win)) / 2;
                    double y = (getHeight(window) - getHeight(y_win)) / 2;
                    setLocation(y_win, x, y);
                    
                    // wait for click before exiting
                    waitForClick();

                    // game over
                    closeGWindow(window);
                    return 0;     
                }
            }
        }
        if(object == paddle)
        {
            y_velocity = -y_velocity;
        }       
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    for(int i = 0; i < ROWS; i++)
    {
        int x = i * 15;
        for(int j = 0; j < COLS; j++)
        {
            int y = j * 40 + 2;
            GRoundRect brick = newGRoundRect(y , x , 35, 10, 0 );
            setFilled(brick, true);
            if(i == 0)
            {
                setColor(brick, "RED");
                add(window, brick);
                
            }
            else if(i == 1)
            {
                setColor(brick, "ORANGE");
                add(window, brick);
                
            }
            else if(i == 2)
            {
                setColor(brick, "YELLOW");
                add(window, brick);
               
            }
            else if(i == 3)
            {
                setColor(brick, "GREEN");
                add(window, brick);
                
            }
            else
            {
                setColor(brick, "CYAN");
                add(window, brick);
                
            }    
        }    
    }       
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(190, 290, 20, 20);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect( 160, 500, 50 , 10 );
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    int points = 0;

    GLabel label = newGLabel("");
    setFont(label, "SansSerif-36");
    add(window, label);
    
    char s[2];
    sprintf(s, "%i", points);
    setLabel(label, s);
    
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
