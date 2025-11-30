#include "raylib.h"
#include "tinyosc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define OSC_PORT 9000
#define BUFFER_SIZE 2048
#define IP_ADDR "127.0.0.1"

char receivedMessage[256] = "Waiting for OSC...";

int main(void) {
  // udp socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  // listen to any ip
  // server_addr.sin_addr.s_addr = INADDR_ANY;
  // listen to specific ip
  inet_pton(AF_INET, IP_ADDR, &server_addr.sin_addr);
  server_addr.sin_port = htons(OSC_PORT);
  bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

  // raylib setup
  SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
  InitWindow(0, 0, "OSC Receiver");

  const float screenWidth = GetScreenWidth();
  const float screenHeight = GetScreenHeight();
  Font fontTtf = LoadFontEx("assets/riot.ttf", 200, 0, 0);
  SetTargetFPS(30);

  char buffer[BUFFER_SIZE];

  while (!WindowShouldClose()) {
    // update
    int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);

    if (len > 0) {
      tosc_message osc;
      if (tosc_parseMessage(&osc, buffer, len) == 0) {
        const char* address = tosc_getAddress(&osc);

        if (strcmp(address, "/butcher/kick") == 0) {

          const char* word = tosc_getNextString(&osc);
          float velocity = tosc_getNextFloat(&osc);

          if (word) {
            strncpy(receivedMessage, word, sizeof(receivedMessage) - 1);
          }
        }
      }
    }

    // draw
    BeginDrawing();
    ClearBackground((Color){ 0, 0, 0, 0 });

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
  UnloadFont(fontTtf);
  CloseWindow();

  return 0;
}
