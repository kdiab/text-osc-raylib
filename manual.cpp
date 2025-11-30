#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
  // load
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
  
  Font fontTtf = LoadFontEx("assets/riot.ttf", 400, 0, 0);
  SetTargetFPS(30);
  
  while (!WindowShouldClose())
  {
    // update
    if (IsKeyPressed(KEY_SPACE)) {
      currentMessage = (currentMessage + 1) % messageCount;
    }
    
    const char *msg = messages[currentMessage];
    
    // draw
    BeginDrawing();
    ClearBackground((Color){ 0, 0, 0, 0 });
    Vector2 textSize = MeasureTextEx(fontTtf, msg, (float)fontTtf.baseSize, 0);
    Vector2 position = {
      (screenWidth - textSize.x) * 0.5f,
      (screenHeight - textSize.y) * 0.5f,
    };
    DrawTextEx(fontTtf, msg, position, (float)fontTtf.baseSize, 2, RED);
    EndDrawing();
  }
  
  // mem
  for (int i = 0; i < messageCount; i++) {
    free(messages[i]);
  }
  free(messages);
  
  UnloadFont(fontTtf);
  CloseWindow();
  
  return 0;
}
