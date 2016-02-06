#include<stdio.h>
#include<stdlib.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>

int
main(int argc, char** args)
{
  int width = 800;
  int height = 600;
  
  Display* display = XOpenDisplay(0);

  if(!display) {
    printf("No display available\n");
    exit(1);
  }
  
  Window root = DefaultRootWindow(display);
  int defaultScreen = DefaultScreen(display);

  int screenBitDepth = 24;
  XVisualInfo visinfo = {};
  if(!XMatchVisualInfo(display, defaultScreen, screenBitDepth, TrueColor, &visinfo)) {
    printf("No matching visual info\n");
    exit(1);
  }

  XSetWindowAttributes windowAttr;
  windowAttr.background_pixel = 0;
  windowAttr.colormap = XCreateColormap(display, root, 
					 visinfo.visual, AllocNone);
  unsigned long attributeMask = CWBackPixel | CWColormap;

  Window window = XCreateWindow(display, root, 
				0, 0,
				width, height, 0,
				visinfo.depth, InputOutput,
				visinfo.visual, attributeMask, &windowAttr);

  if(!window) {
    printf("Window wasn't created properly\n");
    exit(1);
  }

  XStoreName(display, window, "Hello, World!");

  XMapWindow(display, window);
  XFlush(display);


  while(true)
    {
    }

  return 0;
} 
