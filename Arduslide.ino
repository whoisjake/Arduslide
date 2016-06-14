#include <Arduboy.h>
#include "bitmaps.h"

#define X_OFFSET 8
#define Y_OFFSET 6
#define BOX_W 20
#define BOX_H 20
#define ROWS 3
#define COLS 3

const static int rects[9][2] = {
  {33,1},{54,1},{75,1},
  {33,22},{54,22},{75,22},
  {33,43},{54,43},{75,43}
};

Arduboy arduboy;
int puzzle[9] = {0,1,2,3,4,5,6,7,8};
bool left_pressed,right_pressed,up_pressed,down_pressed = false;
bool gameOver = false;
bool intro = true;
unsigned long introStarted;

void randomizePuzzle() {
  shuffle(puzzle, sizeof(puzzle) / sizeof(int), sizeof(int));
}

// generate a value between 0 <= x < n, thus there are n possible outputs
int rand_range(int n)
{
   int r, ul;
   ul = RAND_MAX - RAND_MAX % n;
   while ((r = random(RAND_MAX+1)) >= ul);
   //r = random(ul);
   return r % n;
}

void shuffle_swap(int index_a, int index_b, int *array, int size)
{
   char *x, *y, tmp[size];

   if (index_a == index_b) return;

   x = (char*)array + index_a * size;
   y = (char*)array + index_b * size;

   memcpy(tmp, x, size);
   memcpy(x, y, size);
   memcpy(y, tmp, size);
}

// shuffle an array using fisher-yates method, O(n)
void shuffle(int *array, int nmemb, int size)
{
   int r;
   
   while (nmemb > 1) {                                                                      
       r = rand_range(nmemb--);                                                              
       shuffle_swap(nmemb, r, array, size);
   }
}

int zeroIndex() {
  for(int i = 0; i < 9; i++) {
    if(puzzle[i] == 0) {
      return i;
    }
  }
}

void moveSfx() {
  arduboy.tunes.tone(200, 40);
}

void shiftUp() {
  int zIndex = zeroIndex();
  if (zIndex != 6 && zIndex != 7 && zIndex != 8) {
    int swapIndex = zIndex + 3;
    puzzle[zIndex] = puzzle[swapIndex];
    puzzle[swapIndex] = 0;
    moveSfx();
  }
}

void shiftDown() {
  int zIndex = zeroIndex();
  if (zIndex != 0 && zIndex != 1 && zIndex != 2) {
    int swapIndex = zIndex - 3;
    puzzle[zIndex] = puzzle[swapIndex];
    puzzle[swapIndex] = 0;
    moveSfx();
  }
}

void shiftLeft() {
  int zIndex = zeroIndex();
  if (zIndex != 2 && zIndex != 5 && zIndex != 8) {
    int swapIndex = zIndex + 1;
    puzzle[zIndex] = puzzle[swapIndex];
    puzzle[swapIndex] = 0;
    moveSfx();
  }
}

void shiftRight() {
  int zIndex = zeroIndex();
  if (zIndex != 0 && zIndex != 3 && zIndex != 6) {
    int swapIndex = zIndex - 1;
    puzzle[zIndex] = puzzle[swapIndex];
    puzzle[swapIndex] = 0;
    moveSfx();
  }
}

void checkGameOver() {
  gameOver = false;

  for(int i = 0; i < 8; i++) {
    if((i+1) != puzzle[i])
      return;
  }
  
  gameOver = true;
}

void drawBoard() {
  drawBackground();

  arduboy.fillRect(32,0,64,64,BLACK);
  arduboy.drawRoundRect(32,0,64,64,1,WHITE);
    
  for(int r = 0; r < ROWS; r++) {
    for(int c = 0; c < COLS; c++) {
      int i = (r*ROWS) + c;
      if (puzzle[i] > 0) {
        arduboy.drawSlowXYBitmap(rects[i][0],rects[i][1],blockImage,20,20,1);
        arduboy.setCursor(rects[i][0]+X_OFFSET,rects[i][1]+Y_OFFSET);
        arduboy.print(puzzle[i]);
      }
    }
  }
}

void drawBackground() {
  
}

void drawIntro() {
  arduboy.drawSlowXYBitmap(0,0,introImage,128,64,1);
  if ((millis() - introStarted) > 3000) {
    intro = false;
  }
}

void drawGameOver() {
  arduboy.drawSlowXYBitmap(0,0,winImage,128,64,1);
}

void handleInput() {
  if (arduboy.pressed(UP_BUTTON)) {
    up_pressed = true;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    down_pressed = true;
  } else if (arduboy.pressed(LEFT_BUTTON)) {
    left_pressed = true;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    right_pressed = true;
  }

  if (arduboy.notPressed(UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON)) {
    if (left_pressed) {
      shiftLeft();
      left_pressed = false;
    } else if (right_pressed) {
      shiftRight();
      right_pressed = false;
    } else if (up_pressed) {
      shiftUp();
      up_pressed = false;
    } else if (down_pressed) {
      shiftDown();
      down_pressed = false;
    }
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(15);
  arduboy.initRandomSeed();
  randomizePuzzle();

  arduboy.tunes.tone(987, 160);
  delay(160);
  arduboy.tunes.tone(1318, 400);

  introStarted = millis();
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  handleInput();
  checkGameOver();
  
  arduboy.clear();

  if (intro) {
    drawIntro();
  } else if (gameOver) {
    drawGameOver();
  } else {  
    drawBoard();
  }
  
  arduboy.display();
}
