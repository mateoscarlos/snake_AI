#ifndef COMMON_H
#define COMMON_H

//#define __DEBUG__
//#define __DEBUG_SNAKE__
//#define __DEBUG_FRUIT__

#define BODY_SIZE  25
#define FRUIT_SIZE BODY_SIZE
#define ADVANCE    BODY_SIZE

#define SCENE_WIDTH  800
#define SCENE_HEIGHT 475

#define TIMER_SPEED 270
#define TIMER_SPEED_AUTO 1


/* COLORS */
#define SNAKE_COLOR       0x60c18a
#define FRUIT_COLOR       0xf76c6f
#define BOARD_COLOR       0xefebe1
#define BOARD_LINES_COLOR 0xc6a365


#define DBUS_INPUTS  "org.torchlight.Inputs"
#define DBUS_PATH    "/inputs"
#define THRESHOLD    0.53
#define TRAIN_EPOCHS 10000

enum TDirection {
    RIGHT,
    LEFT,
    UP,
    DOWN,
    TOTAL
};

#endif
