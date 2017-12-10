Short story about how to use this library in your own C-program.
First of all, include the header-file:
    #include <st2205.h>

Declare a pointer to the device handle:
    st2205_handle *h;        

Initialize the device, e.g. like this:
    h=st2205_open("/dev/sda");

The height, width and bpp of the connected device is retrievable by this:
    printf("Display: %ix%i pixels, %i dpp\n",h->height,h->width,h->bpp);

Data is sent to the display in a complete screenfull of 24x24 pixels. You
need to supply the buffer yourself.
    char* framebuffer;
    framebuffer=malloc(h->height*h->width*3);
    
Each pixel is made of 3 bytes: one red, one green and one blue. The library will
convert these to the bpp the physical device uses. 

To fill a screen completely with red:
    int x,y,r,g,b;
    r=255; g=0; b=0;
    for (x=0; x<h->width; x++) {
	for (y=0; y<h->height; y++) {
	    framebuff[(x+y*h->width)*3]=r;
	    framebuff[(x+y*h->width)*3+1]=g;
	    framebuff[(x+y*h->width)*3+2]=b;
	}
    }
