#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrandr.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "tinyosc.h"

#define OSC_PORT 9000
#define BUFFER_SIZE 2048
#define IP_ADDR "127.0.0.1"

typedef struct {
    Display *display;
    Window window;
    XftDraw *xft_draw;
    XftFont *fonts[3];
    XftColor color;
    int screen;
    int width;
    int height;
} X11Context;

char receivedMessage[256] = "BuTcHeR";
int currentFont = 0;

void init_x11(X11Context *ctx) {
    ctx->display = XOpenDisplay(NULL);
    if (!ctx->display) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
    
    ctx->screen = DefaultScreen(ctx->display);
    
    ctx->width = DisplayWidth(ctx->display, ctx->screen);
    ctx->height = DisplayHeight(ctx->display, ctx->screen);
    
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    attrs.background_pixel = BlackPixel(ctx->display, ctx->screen);
    
    ctx->window = XCreateWindow(
        ctx->display,
        RootWindow(ctx->display, ctx->screen),
        0, 0, ctx->width, ctx->height, 0,
        CopyFromParent, InputOutput, CopyFromParent,
        CWOverrideRedirect | CWBackPixel,
        &attrs
    );
    
    XMapRaised(ctx->display, ctx->window);
    
    Region region = XCreateRegion();
    XShapeCombineRegion(ctx->display, ctx->window, ShapeInput, 0, 0, region, ShapeSet);
    XDestroyRegion(region);
    
    Visual *visual = DefaultVisual(ctx->display, ctx->screen);
    Colormap colormap = DefaultColormap(ctx->display, ctx->screen);
    ctx->xft_draw = XftDrawCreate(ctx->display, ctx->window, visual, colormap);
    
    XRenderColor xrcolor;
    xrcolor.red = 0xFFFF;
    xrcolor.green = 0x0000;
    xrcolor.blue = 0x0000;
    xrcolor.alpha = 0xFFFF;
    XftColorAllocValue(ctx->display, visual, colormap, &xrcolor, &ctx->color);
    
    ctx->fonts[0] = XftFontOpen(ctx->display, ctx->screen,
        XFT_FILE, XftTypeString, "assets/molot.otf",
        XFT_SIZE, XftTypeDouble, 120.0,
        NULL);
    ctx->fonts[1] = XftFontOpen(ctx->display, ctx->screen,
        XFT_FILE, XftTypeString, "assets/kontra.ttf",
        XFT_SIZE, XftTypeDouble, 120.0,
        NULL);
    ctx->fonts[2] = XftFontOpen(ctx->display, ctx->screen,
        XFT_FILE, XftTypeString, "assets/fyodor.ttf",
        XFT_SIZE, XftTypeDouble, 120.0,
        NULL);
    
    for (int i = 0; i < 3; i++) {
        if (!ctx->fonts[i]) {
            ctx->fonts[i] = XftFontOpen(ctx->display, ctx->screen,
                XFT_FAMILY, XftTypeString, "monospace",
                XFT_SIZE, XftTypeDouble, 120.0,
                NULL);
        }
    }
    
    XSelectInput(ctx->display, ctx->window, ExposureMask);
}

void draw_text(X11Context *ctx) {
    XClearWindow(ctx->display, ctx->window);
    
    XGlyphInfo extents;
    XftTextExtentsUtf8(ctx->display, ctx->fonts[currentFont],
        (XftChar8 *)receivedMessage, strlen(receivedMessage), &extents);
    
    int x = (ctx->width - extents.width) / 2;
    int y = (ctx->height + extents.height) / 2;
    
    XftDrawStringUtf8(ctx->xft_draw, &ctx->color, ctx->fonts[currentFont],
        x, y, (XftChar8 *)receivedMessage, strlen(receivedMessage));
    
    XFlush(ctx->display);
}

void cleanup_x11(X11Context *ctx) {
    for (int i = 0; i < 3; i++) {
        if (ctx->fonts[i]) XftFontClose(ctx->display, ctx->fonts[i]);
    }
    XftColorFree(ctx->display, DefaultVisual(ctx->display, ctx->screen),
        DefaultColormap(ctx->display, ctx->screen), &ctx->color);
    XftDrawDestroy(ctx->xft_draw);
    XDestroyWindow(ctx->display, ctx->window);
    XCloseDisplay(ctx->display);
}

int main(void) {
    srand(time(NULL));
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDR, &server_addr.sin_addr);
    server_addr.sin_port = htons(OSC_PORT);
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    X11Context ctx;
    init_x11(&ctx);
    
    currentFont = rand() % 3;
    
    draw_text(&ctx);
    
    char buffer[BUFFER_SIZE];
    
    while (1) {
        while (XPending(ctx.display)) {
            XEvent event;
            XNextEvent(ctx.display, &event);
            if (event.type == Expose) {
                draw_text(&ctx);
            }
        }
        
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        
        if (len > 0) {
            tosc_message osc;
            if (tosc_parseMessage(&osc, buffer, len) == 0) {
                const char* address = tosc_getAddress(&osc);
                
                if (strcmp(address, "/butcher/kick") == 0) {
                    const char* word = tosc_getNextString(&osc);
                    float velocity = tosc_getNextFloat(&osc);
                    
                    if (word) {
                        currentFont = rand() % 3;
                        strncpy(receivedMessage, word, sizeof(receivedMessage) - 1);
                        receivedMessage[sizeof(receivedMessage) - 1] = '\0';
                        draw_text(&ctx);
                    }
                }
            }
        }
        
        usleep(33333); // ~30 FPS
    }
    
    // mem
    close(sockfd);
    cleanup_x11(&ctx);
    
    return 0;
}
