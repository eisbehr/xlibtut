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

Status 
toggleMaximize(Display* display, Window window) 
{  
  XClientMessageEvent ev = {};
  Atom wmState = XInternAtom(display, "_NET_WM_STATE", False);
  Atom maxH  =  XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
  Atom maxV  =  XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", False);

  if(wmState == None) return 0;

  ev.type = ClientMessage;
  ev.format = 32;
  ev.window = window;
  ev.message_type = wmState;
  ev.data.l[0] = 2;
  ev.data.l[1] = maxH;
  ev.data.l[2] = maxV;
  ev.data.l[3] = 1;

  return XSendEvent(display, DefaultRootWindow(display), False,
		    SubstructureRedirectMask | SubstructureNotifyMask,
		    (XEvent *)&ev);
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
  windowAttr.bit_gravity = StaticGravity;
  windowAttr.background_pixel = 0;
  windowAttr.colormap = XCreateColormap(display, root, 
					 visinfo.visual, AllocNone);
  windowAttr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask; 
  unsigned long attributeMask = CWBitGravity | CWBackPixel | CWColormap | CWEventMask;

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
  
  XIM xInputMethod = XOpenIM(display, 0, 0, 0);
  if(!xInputMethod)
    {
      printf("Input Method could not be opened\n");
    }

  XIMStyles* styles = 0;
  if(XGetIMValues(xInputMethod, XNQueryInputStyle, &styles, NULL) || !styles)
    {
      printf("Input Styles could not be retrieved\n");
    }

  XIMStyle bestMatchStyle = 0;
  for(int i=0; i<styles->count_styles; i++)
    {
      XIMStyle thisStyle = styles->supported_styles[i];
      if (thisStyle == (XIMPreeditNothing | XIMStatusNothing))
	{
	  bestMatchStyle = thisStyle;
	  break;
	}
    }
  XFree(styles);

  if(!bestMatchStyle)
    {
      printf("No matching input style could be determined\n");
    }

  XIC xInputContext = XCreateIC(xInputMethod, XNInputStyle, bestMatchStyle,
				XNClientWindow, window,
				XNFocusWindow, window,
				NULL);
  if(!xInputContext)
    {
      printf("Input Context could not be created\n");
    }

  XMapWindow(display, window);

  //toggleMaximize(display, window);
  XFlush(display);

  int pixelBits = 32;
  int pixelBytes = pixelBits/8;
  int windowBufferSize = width*height*pixelBytes;
  char* mem  = (char*)malloc(windowBufferSize);
  
  XImage* xWindowBuffer = XCreateImage(display, visinfo.visual, visinfo.depth,
				       ZPixmap, 0, mem, width, height,
				       pixelBits, 0);
  GC defaultGC = DefaultGC(display, defaultScreen);
  
  int sizeChange = 0;  
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
	    case ConfigureNotify: {
	      XConfigureEvent* e = (XConfigureEvent*) &ev;
	      width = e->width;
	      height = e->height;
	      sizeChange = 1;
	    } break;
	    case KeyPress: {
	      XKeyPressedEvent* e = (XKeyPressedEvent*) &ev;

	      int symbol = 0;
	      Status status = 0;
	      Xutf8LookupString(xInputContext, e, (char*)&symbol,
				4, 0, &status);
      
	      if(status == XBufferOverflow)
		{
		  // Should not happen since there are no utf-8 characters larger than 24bits
		  // But something to be aware of when used to directly write to a string buffer
		  printf("Buffer overflow when trying to create keyboard symbol map\n");
		}
	      else if(status == XLookupChars)
		{
		  printf("%s\n", (char*)&symbol);
		}

	      if(e->keycode == XKeysymToKeycode(display, XK_Left)) printf("left arrow pressed\n");
	      if(e->keycode == XKeysymToKeycode(display, XK_Right)) printf("right arrow pressed\n");
	      if(e->keycode == XKeysymToKeycode(display, XK_Up)) printf("up arrow pressed\n");
	      if(e->keycode == XKeysymToKeycode(display, XK_Down)) printf("down arrow pressed\n");
	    } break;
	    case KeyRelease: {
	      XKeyPressedEvent* e = (XKeyPressedEvent*) &ev;

	      if(e->keycode == XKeysymToKeycode(display, XK_Left)) printf("left arrow released\n");
	      if(e->keycode == XKeysymToKeycode(display, XK_Right)) printf("right arrow released\n");
	      if(e->keycode == XKeysymToKeycode(display, XK_Up)) printf("up arrow released\n");
	      if(e->keycode == XKeysymToKeycode(display, XK_Down)) printf("down arrow released\n");
	    } break;
	    }
	}

      if(sizeChange)
	{
	  sizeChange = 0;
	  XDestroyImage(xWindowBuffer); // Free's the memory we malloced;
	  windowBufferSize = width*height*pixelBytes;
	  mem  = (char*)malloc(windowBufferSize);
	  
	  xWindowBuffer = XCreateImage(display, visinfo.visual, visinfo.depth,
				       ZPixmap, 0, mem, width, height,
				       pixelBits, 0);
	}

      int pitch = width*pixelBytes;
      for(int y=0; y<height; y++)
	{
	  char* row = mem+(y*pitch);
	  for(int x=0; x<width; x++)
	    {
	      unsigned int* p = (unsigned int*) (row+(x*pixelBytes));
	      if(x%16 && y%16)
		{
		  *p = 0xffffffff;
		}
	      else
		{
		  *p = 0;
		}
	    }
	}

      XPutImage(display, window,
		defaultGC, xWindowBuffer, 0, 0, 0, 0, 
		width, height);
    };

  return 0;
} 
