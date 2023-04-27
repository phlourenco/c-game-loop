#include <stdio.h>
#include <SDL2/SDL.h>
#include "./constants.h"

int ball_direction_right = TRUE;
int ball_direction_down = TRUE;
int game_is_running = FALSE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int last_frame_time = 0;

struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return FALSE;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }

    return TRUE;
}

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        game_is_running = FALSE;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            game_is_running = FALSE;
        } else if (event.key.keysym.sym == SDLK_LEFT) {
            // Set paddle velocity based on left/right arrow keys
            paddle.vel_x = -400;
        } else if (event.key.keysym.sym == SDLK_RIGHT) {
            // Set paddle velocity based on left/right arrow keys
            paddle.vel_x = 400;
        }
        break;
    case SDL_KEYUP:
        // TODO: Reset paddle velocity based on left/right arrow keys
        if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT) {
            paddle.vel_x = 0;
        }
        break;
    default:
        break;
    }
}

void setup() {
    //Initialize ball object
    ball.width = 15;
    ball.height = 15;
    ball.x = (WINDOW_WIDTH / 2) - (ball.width / 2);
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    //Initialize paddle object
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}


void change_ball_x_direction() {
    if (ball_direction_right) {
        ball_direction_right = FALSE;
    } else {
        ball_direction_right = TRUE;
    }
}

void change_ball_y_direction() {
    if (ball_direction_down) {
        ball_direction_down = FALSE;
    } else {
        ball_direction_down = TRUE;
    }
}

void reset_game() {
    ball_direction_right = TRUE;
    ball_direction_down = TRUE;
    setup();
}

void update() {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // Get delta time factor converted to seconds to be used to update my objects
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    last_frame_time = SDL_GetTicks();

    if (ball_direction_right) {
        ball.x += ball.vel_x * delta_time;
    } else {
        ball.x -= ball.vel_x * delta_time;
    }

    if (ball_direction_down) {
        ball.y += ball.vel_y * delta_time;
    } else {
        ball.y -= ball.vel_y * delta_time;
    }

    // Update paddle position based on its velocity
    paddle.x += paddle.vel_x * delta_time;

    // Check for ball collision with the walls
    int ballCollidedWithLeftWall = (ball.x <= 0);
    int ballCollidedWithRightWall = (ball.x >= (WINDOW_WIDTH - ball.width));

    int ballCollidedWithTopWall = (ball.y <= 0);
    int ballCollidedWithBottomWall = (ball.y >= (WINDOW_HEIGHT - ball.height));

    if (ballCollidedWithLeftWall || ballCollidedWithRightWall) {
        change_ball_x_direction();
    }

    if (ballCollidedWithTopWall) {
        change_ball_y_direction();
    }

    if (ballCollidedWithBottomWall) {
        reset_game();
        return;
    }

    // TODO: Check for ball collision with the paddle

    int ballInPaddleX = (ball.x >= paddle.x && ball.x <= (paddle.x + paddle.width));
    int ballInPaddleY = ((ball.y + ball.height) >= paddle.y);
    int ballCollideWithPaddle = (ballInPaddleX && ballInPaddleY);

    // printf("ballInPaddleX: %d", ballInPaddleX);
    
    if (ballCollideWithPaddle) {
        change_ball_x_direction();
        change_ball_y_direction();
    }

    // Prevent paddle from moving outside the boundaries of the window
    if (paddle.x <= 0) {
        paddle.x = 0;
    }

    if (paddle.x + paddle.width >= WINDOW_WIDTH) {
        paddle.x = (WINDOW_WIDTH - paddle.width);
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //Draw rectangle for the ball
    SDL_Rect ball_rect = { 
        (int)ball.x,
        (int)ball.y,
        (int)ball.width, 
        (int)ball.height 
    };
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    //Draw rectangle for the paddle
    SDL_Rect paddle_rect = { 
        (int)paddle.x,
        (int)paddle.y,
        (int)paddle.width, 
        (int)paddle.height 
    };
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main() {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}