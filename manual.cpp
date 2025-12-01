#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG

int main(void)
{
  srand(time(NULL));
  
  // load
  float offset = 0.0;
  char **messages = NULL;
  int messageCount = 0;
  
  FILE *file = fopen("words.txt", "r");
  if (file) {
    char line[256];
    int capacity = 10;
    messages = (char**)malloc(capacity * sizeof(char*));
    
    while (fgets(line, sizeof(line), file)) {
      line[strcspn(line, "\n")] = 0;
      if (strlen(line) == 0) continue;
      if (messageCount >= capacity) {
        capacity *= 2;
        messages = (char**)realloc(messages, capacity * sizeof(char*));
      }
      messages[messageCount] = (char*)malloc(strlen(line) + 1);
      strcpy(messages[messageCount], line);
      messageCount++;
    }
    fclose(file);
  } else {
    printf("Error: Could not open words.txt\n");
    return 1;
  }
  
  if (messageCount == 0) {
    printf("Error: No words loaded from file\n");
    return 1;
  }
  
  // init
  SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
  InitWindow(0, 0, "");
  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  
  int currentMessage = 0;
  
  Font fonts[3];
  fonts[0] = LoadFontEx("assets/molot.otf", 350, 0, 0);
  fonts[1] = LoadFontEx("assets/kontra.ttf", 350, 0, 0);
  fonts[2] = LoadFontEx("assets/fyodor.ttf", 350, 0, 0);
  
  int currentFont = rand() % 3;
  
  SetTargetFPS(30);
  
  while (!WindowShouldClose())
  {
    // update
    if (IsKeyPressed(KEY_SPACE)) {
      currentMessage = (currentMessage + 1) % messageCount;
      currentFont = rand() % 3;
    }
    
    const char *msg = messages[currentMessage];
    Font fontTtf = fonts[currentFont];
    
    // draw
    BeginDrawing();
    ClearBackground((Color){ 0, 0, 0, 0 });
    Vector2 textSize = MeasureTextEx(fontTtf, msg, (float)fontTtf.baseSize, 0);
    Vector2 position = {
      (screenWidth - textSize.x + offset) * 0.5f,
      (screenHeight - textSize.y) * 0.5f,
    };
    DrawTextEx(fontTtf, msg, position, (float)fontTtf.baseSize, 0, RED);
#ifdef DEBUG
  DrawRectangleLines(position.x, position.y, textSize.x, textSize.y, GREEN);
  DrawLine(screenWidth/2 - 50, screenHeight/2, screenWidth/2 + 50, screenHeight/2, BLUE);
  DrawLine(screenWidth/2, screenHeight/2 - 50, screenWidth/2, screenHeight/2 + 50, BLUE);
#endif
    EndDrawing();
  }
  
  // mem
  for (int i = 0; i < messageCount; i++) {
    free(messages[i]);
  }
  free(messages);
  for (int i = 0; i < 3; i++) {
    UnloadFont(fonts[i]);
  }
  CloseWindow();
  
  return 0;
}
