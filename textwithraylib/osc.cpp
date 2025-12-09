#include "raylib.h"
#include "tinyosc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <cstdio>
#include <cstdlib>

#define OSC_PORT 9000
#define BUFFER_SIZE 2048
#define IP_ADDR "127.0.0.1"

char receivedMessage[256] = "BuTcHeR";

int main(void) {
  srand(time(NULL));
  
  // udp socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, IP_ADDR, &server_addr.sin_addr);
  server_addr.sin_port = htons(OSC_PORT);
  bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  
  // raylib setup
  SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
  InitWindow(0, 0, "OSC Receiver");
  
  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  
  Font fonts[3];
  fonts[0] = LoadFontEx("assets/molot.otf", 350, 0, 0);
  fonts[1] = LoadFontEx("assets/kontra.ttf", 350, 0, 0);
  fonts[2] = LoadFontEx("assets/fyodor.ttf", 350, 0, 0);
  
  int currentFont = rand() % 3;
  
  SetTargetFPS(30);
  
  char buffer[BUFFER_SIZE];
  
  while (!WindowShouldClose()) {
    // update
    int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    
    if (len > 0) {
      tosc_message osc;
      if (tosc_parseMessage(&osc, buffer, len) == 0) {
        const char* address = tosc_getAddress(&osc);
        
        if (strcmp(address, "/butcher/kick") == 0){
          
          const char* word = tosc_getNextString(&osc);
          float velocity = tosc_getNextFloat(&osc);
          
          if (word) {
            currentFont = rand() % 3;
            strncpy(receivedMessage, word, sizeof(receivedMessage) - 1);
          }
        }
      }
    }
    
    // draw
    BeginDrawing();
    ClearBackground((Color){ 0, 0, 0, 0 });
    
    Font fontTtf = fonts[currentFont];
    Vector2 textSize = MeasureTextEx(fontTtf, receivedMessage, (float)fontTtf.baseSize, 2);
    Vector2 position = {
      (screenWidth - textSize.x) * 0.5f,
      (screenHeight - textSize.y) * 0.5f,
    };
    DrawTextEx(fontTtf, receivedMessage, position, (float)fontTtf.baseSize, 2, RED);
    
    EndDrawing();
  }
  
  // mem
  close(sockfd);
  for (int i = 0; i < 3; i++) {
    UnloadFont(fonts[i]);
  }
  
  CloseWindow();
  
  return 0;
}
