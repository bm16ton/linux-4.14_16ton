#include<stdio.h>
#include<gd.h>

#include<gdfontt.h>
#include<gdfonts.h>
#include<gdfontl.h>
#include<gdfontg.h>

int main()
{
    int cw=gdFontGiant->w,ch=gdFontGiant->h;
    gdImagePtr im=gdImageCreate(cw,ch);
    int black=gdImageColorAllocate(im,0,0,0);       // black
    int white=gdImageColorAllocate(im,255,255,255); // white

    gdFontPtr fonts[4]={gdFontTiny,gdFontSmall,gdFontLarge,gdFontGiant};

    printf("%d %d %d %d %d %d %d %d\n",
           fonts[0]->w,fonts[0]->h, fonts[1]->w,fonts[1]->h,
           fonts[2]->w,fonts[2]->h, fonts[3]->w,fonts[3]->h);

    int f,chr,x,y;
    for (f=0;f<4;f++)
    {
        for (chr=32;chr<127;chr++)
        {
            gdImageFilledRectangle(im,0,0,cw-1,ch-1,black);
            gdImageChar(im,fonts[f],0,0,chr,white);

            for (y=0;y<fonts[f]->h;y++)
            {
                for (x=0;x<fonts[f]->w;x++)
                    putchar(gdImageRed(im,gdImageGetPixel(im,x,y))?'#':' ');
                printf("\n");
            }
        }
    }

return 0;
}

