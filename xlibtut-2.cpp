#include<stdio.h>
#include<stdlib.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/Xatom.h>

void 
setSizeHint(Display* display, Window window, 
	    int minWidth, int minHeight, 
	    int maxWidth, int maxHeight) 
{
  XSizeHints hints = {};
  if(minWidth > 0 && minHeight > 0) hints.flags |= PMinSize;
  if(maxWidth > 0 && maxHeight > 0) hints.flags |= PMaxSize;

  hints.min_width = minWidth;
  hints.min_height = minHeight;
  hints.max_width = maxWidth;
  hints.max_height = maxHeight;

  XSetWMNormalHints(display, window, &hints);
}

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
  windowAttr.event_mask = StructureNotifyMask; 
  unsigned long attributeMask = CWBackPixel | CWColormap | CWEventMask;

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
  setSizeHint(display, window, 400, 300, 0, 0); 

  XMapWindow(display, window);
  XFlush(display);

  int windowOpen = 1;
  while(windowOpen)
    {
      XEvent ev = {};
      while(XPending(display) > 0)
	{
	  XNextEvent(display, &ev);
	  switch(ev.type)
	    {
	    case DestroyNotify: {
	      XDestroyWindowEvent* e = (XDestroyWindowEvent*) &ev;
	      if(e->window == window)
		{
		  windowOpen = 0;
		}
	    } break;
	    }
	}
    };

  return 0;
} 
