//RETRO HACK TO REDO
//SDL SAVEBMP (used in screenSnapShot.c)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "SDL.h"

extern int SurfaceFormat;

typedef struct                       /**** BMP file header structure ****/
    {
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
    } BITMAPFILEHEADER;

#  define BF_TYPE 0x4D42             /* "MB" */

typedef struct                       /**** BMP file info structure ****/
    {
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
    } BITMAPINFOHEADER;

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct                       /**** Colormap entry structure ****/
    {
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
    } RGBQUAD;

typedef struct                       /**** Bitmap information structure ****/
    {
    BITMAPINFOHEADER bmiHeader;      /* Image header */
    RGBQUAD          bmiColors[256]; /* Image colormap */
    } BITMAPINFO;

/*
 * 'read_word()' - Read a 16-bit unsigned integer.
 */

static unsigned short     /* O - 16-bit unsigned integer */
read_word(FILE *fp)       /* I - File to read from */
    {
    unsigned char b0, b1; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);

    return ((b1 << 8) | b0);
    }


/*
 * 'read_dword()' - Read a 32-bit unsigned integer.
 */

static unsigned int               /* O - 32-bit unsigned integer */
read_dword(FILE *fp)              /* I - File to read from */
    {
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
    }


/*
 * 'read_long()' - Read a 32-bit signed integer.
 */

static int                        /* O - 32-bit signed integer */
read_long(FILE *fp)               /* I - File to read from */
    {
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(fp);
    b1 = getc(fp);
    b2 = getc(fp);
    b3 = getc(fp);

    return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
    }


/*
 * 'write_word()' - Write a 16-bit unsigned integer.
 */

static int                     /* O - 0 on success, -1 on error */
write_word(FILE           *fp, /* I - File to write to */
           unsigned short w)   /* I - Integer to write */
    {
    putc(w, fp);
    return (putc(w >> 8, fp));
    }


/*
 * 'write_dword()' - Write a 32-bit unsigned integer.
 */

static int                    /* O - 0 on success, -1 on error */
write_dword(FILE         *fp, /* I - File to write to */
            unsigned int dw)  /* I - Integer to write */
    {
    putc(dw, fp);
    putc(dw >> 8, fp);
    putc(dw >> 16, fp);
    return (putc(dw >> 24, fp));
    } 

/*
 * 'write_long()' - Write a 32-bit signed integer.
 */

static int           /* O - 0 on success, -1 on error */
write_long(FILE *fp, /* I - File to write to */
           int  l)   /* I - Integer to write */
    {
    putc(l, fp);
    putc(l >> 8, fp);
    putc(l >> 16, fp);
    return (putc(l >> 24, fp));
    }



int SDL_SaveBMP(SDL_Surface *surface,const char *file){

    FILE *fp;                      /* Open file pointer */
    int  i,size,                     /* Size of file */
         infosize,                 /* Size of bitmap info */
         bitsize;                  /* Size of bitmap pixels */

  unsigned char *pixels,*pixflip;
  int stype=surface->format->BytesPerPixel;


  /* Try opening the file; use "wb" mode to write this *binary* file. */
    if ((fp = fopen(file, "wb")) == NULL){
	printf("openfile faided %s\n",file);
        return (-1);
    }

    pixels = malloc(sizeof(unsigned char)*3*surface->w*surface->h/*retrow*retroh*/);
 
    bitsize = 3*surface->w*surface->h/*retrow*retroh*/;

    /* Figure out the header size */ 
    infosize = sizeof(BITMAPINFOHEADER);
   
    size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

    /* Write the file header, bitmap information, and bitmap pixel data... */

    write_word(fp, BF_TYPE);        /* bfType */
    write_dword(fp, size);          /* bfSize */
    write_word(fp, 0);              /* bfReserved1 */
    write_word(fp, 0);              /* bfReserved2 */
    write_dword(fp, 18 + infosize); /* bfOffBits */

    write_dword(fp, 40/*info->bmiHeader.biSize*/);
    write_long(fp, surface->w/*retrow*//*info->bmiHeader.biWidth*/);
    write_long(fp, surface->h/*retroh*//*info->bmiHeader.biHeight*/);
    write_word(fp,   1/* info->bmiHeader.biPlanes*/);
    write_word(fp,  24/*info->bmiHeader.biBitCount*/);
    write_dword(fp,  0/*info->bmiHeader.biCompression*/);
    write_dword(fp,  3*surface->w*surface->h/*retrow*retroh*//*info->bmiHeader.biSizeImage*/);
    write_long(fp,   0/*info->bmiHeader.biXPelsPerMeter*/);
    write_long(fp,   0/*info->bmiHeader.biYPelsPerMeter*/);
    write_dword(fp,  0/*info->bmiHeader.biClrUsed*/);
    write_dword(fp,  0/*info->bmiHeader.biClrImportant*/);

printf("Stype:%d \n",stype);

 	if(stype==1){
 		unsigned char *ptr=(unsigned char*)&surface->pixels;
 		for (i = 0;i</*retrow*retroh*/surface->w*surface->h;i++){
			//rbg?
	                pixels[(i*3)+0]=surface->format->palette->colors[*ptr].r;
	                pixels[(i*3)+1]=surface->format->palette->colors[*ptr].b;
	                pixels[(i*3)+2]=surface->format->palette->colors[*ptr].g;
                	ptr++; 

		}

	}
	else if(stype==2){
		// RGB565 to bgr
    		unsigned short int *ptr=(unsigned short int*)&surface->pixels;
    		short R8, G8 , B8 ;
		unsigned short temp;
	    	for (i = 0;i<surface->w*surface->h/*retrow*retroh*/;i++){

	                temp = (unsigned short  int) (*ptr)&0xffff;
	
			#define R5 ((temp>>11)&0x1F)
			#define G6 ((temp>>5 )&0x3F)
			#define B5 ((temp    )&0x1F)
	
	 		R8 = ( R5 * 527 + 23 ) >> 6;
			G8 = ( G6 * 259 + 33 ) >> 6;
	 		B8 = ( B5 * 527 + 23 ) >> 6;
	
	                ptr++; 
	
			//rbg?
	                pixels[(i*3)+0]=R8;
	                pixels[(i*3)+1]=B8;
	                pixels[(i*3)+2]=G8;
	    	}  
	}
	else if(stype==4){
		// RGB to bgr
    		unsigned int *ptr=(unsigned int*)&surface->pixels;
		unsigned temp;
printf("%dx%d %dx%d\n",retrow,retroh,surface->w,surface->h);
	    	for (i = 0;i</*retrow*retroh*/surface->w*surface->h;i++){

	                temp = (unsigned int) (*ptr)&0xffffffff;

			#define R8 ((temp>>16)&0xFF)
			#define G8 ((temp>>8 )&0xFF)
			#define B8 ((temp    )&0xFF)
		printf("i:%d temp:%x (%x,%x,%x)\n",i,temp,R8,G8,B8);
	                ptr++; 
	
			//rbg?
	                pixels[(i*3)+0]=R8;
	                pixels[(i*3)+1]=B8;
	                pixels[(i*3)+2]=G8;
	    	}
	}
	
printf("ici\n");

        const int bw = surface->w/*retrow*/*3;
	int pad;

       // Write the bitmap image upside down 
        pixflip = (Uint8 *) pixels + (surface->h/*retroh*/ *bw);
        pad = ((bw % 4) ? (4 - (bw % 4)) : 0);
        while (pixflip > (Uint8 *) pixels) {
            pixflip -= bw;
            if (fwrite(pixflip, 1, bw, fp) != bw) {
                printf("write erreur %d\n",bw);
		free(pixels);
		fclose(fp);
		return -1;                
            }
            if (pad) {
                const Uint8 padbyte = 0;
                for (i = 0; i < pad; ++i) {
                   fwrite( &padbyte, 1, 1, fp);
                }
            }
        }

    fclose(fp);
    free(pixels);

    return (0);    

}

SDL_Surface * SDL_LoadBMP(const char *file)
{
	int width,height,size;
	SDL_Surface *surf;
	RGBQUAD  LUT[256];

	//printf("IN SDL LOAD BMP\n");

	//LOAD bmp 24bit no compression in 16B SDL surface.

	FILE *fp;                      /* Open file pointer */

	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	// value to be used in ReadFile funcs
	int bytesread;
	// open file to read from
	
	if ((fp = fopen(file, "rb")) == NULL){
		printf("openfile failed %s\n",file);
	        return (NULL);
	}

	int result=0;

	fseek(fp,0,SEEK_SET);
	unsigned char fileh[40];
   	result=fread(fileh, 1,14, fp);
	if( result!=14)
	{
		printf("IN SDL LOAD BMP err read fileheader %d \n",result);
		fclose ( fp);
		return NULL;
	}

bmpheader.bfType=fileh[0]|fileh[1]<<8;
bmpheader.bfSize=fileh[2]|fileh[3]<<8|fileh[4]<<16|fileh[5]<<24;
bmpheader.bfReserved1=fileh[6]|fileh[7]<<8;
bmpheader.bfReserved2=fileh[8]|fileh[9]<<8;
bmpheader.bfOffBits=fileh[10]|fileh[11]<<8|fileh[12]<<16|fileh[13]<<24;

printf(" %x %d %x %x %x \n",
bmpheader.bfType,
bmpheader.bfSize,
bmpheader.bfReserved1,
bmpheader.bfReserved2 ,
bmpheader.bfOffBits  );


   	result=fread(fileh, 1, 40, fp);

	if( result!=40)
	{
		printf("IN SDL LOAD BMP err read infoheader %d \n",result);
		fclose ( fp );
		return NULL;
	}

bmpinfo.biSize=fileh[0]|fileh[1]<<8|fileh[2]<<16|fileh[3]<<24;
bmpinfo.biWidth=fileh[4]|fileh[5]<<8|fileh[6]<<16|fileh[7]<<24;
bmpinfo.biHeight=fileh[8]|fileh[9]<<8|fileh[10]<<16|fileh[11]<<24;
bmpinfo.biPlanes=fileh[12]|fileh[13]<<8;
bmpinfo.biBitCount=fileh[14]|fileh[15]<<8;
bmpinfo.biCompression=fileh[16]|fileh[17]<<8|fileh[18]<<16|fileh[19]<<24;
bmpinfo.biSizeImage=fileh[20]|fileh[21]<<8|fileh[22]<<16|fileh[23]<<24;
bmpinfo.biXPelsPerMeter=fileh[24]|fileh[25]<<8|fileh[26]<<16|fileh[27]<<24;
bmpinfo.biYPelsPerMeter=fileh[28]|fileh[29]<<8|fileh[30]<<16|fileh[31]<<24;
bmpinfo.biClrUsed=fileh[32]|fileh[33]<<8|fileh[34]<<16|fileh[35]<<24;
bmpinfo.biClrImportant=fileh[36]|fileh[37]<<8|fileh[38]<<16|fileh[39]<<24;

printf(" %x %x %x %x %x  %x %x %x %x %x %x\n",
bmpinfo.biSize,
bmpinfo.biWidth,
bmpinfo.biHeight,
bmpinfo.biPlanes,
bmpinfo.biBitCount,
bmpinfo.biCompression,
bmpinfo.biSizeImage,
bmpinfo.biXPelsPerMeter,
bmpinfo.biYPelsPerMeter,
bmpinfo.biClrUsed,
bmpinfo.biClrImportant  );

	// check if file is actually a bmp
	if ( bmpheader.bfType != 'MB' )
	{
		printf("IN SDL LOAD BMP err not MB %x \n",bmpheader.bfType);
		fclose ( fp );
		return NULL;
	}

//printf("IN SDL LOAD BMP after headers read %x %x %x %x %x\n",bmpheader.bfType,bmpinfo.biBitCount,bmpinfo.biSize,bmpinfo.biWidth,bmpinfo.biHeight);

	// get image measurements
	width   = bmpinfo.biWidth;
	height  = abs ( bmpinfo.biHeight );

	// check if bmp is uncompressed
	if ( bmpinfo.biCompression != BI_RGB )
	{
		printf("IN SDL LOAD BMP err bicomp=%x\n",bmpinfo.biCompression);
		fclose ( fp );
		return NULL;
	}

	// check if we have 24 bit bmp
	if ( bmpinfo.biBitCount != 24 )
	{
		printf("IN SDL LOAD BMP err biBitCount=%d deb:%d\n",bmpinfo.biBitCount,bmpheader.bfOffBits);

		if(bmpinfo.biBitCount != 8){
			fclose ( fp );
			return NULL;
		}
	}


	if(bmpinfo.biBitCount == 8){
		printf("IN SDL LOAD BMP err deb:%d\n",bmpheader.bfOffBits);
		int lutsize;

		if(bmpinfo.biClrUsed!=0)lutsize=bmpinfo.biClrUsed*4;
		else lutsize=256*4;


		//copy lut
		fseek(fp,54/*bmpheader.bfOffBits-1024*/,  SEEK_SET);
		result= fread(LUT, 1,lutsize, fp);

		size = bmpheader.bfSize-(54+lutsize); //bmpinfo.biSizeImage;

		printf("size:%d /%d\n",size,bmpheader.bfOffBits);
		
		fseek(fp,bmpheader.bfOffBits,  SEEK_SET);
	
		unsigned char* Buffer = malloc(sizeof(char)*size);
	
		result= fread(Buffer, 1, size, fp);
		if(result!=size)
		{
			printf("IN SDL LOAD BMP err readbuffer =%d\n",result);
			fclose ( fp);
			free(Buffer);
			return NULL;
		}
 	if(SurfaceFormat==0){printf("create 32b surf\n");

		surf=Retro_CreateRGBSurface(width ,height, 32, 0,0,0,0);
	}
	else if(SurfaceFormat==1){printf("create 16b surf\n");
		surf=Retro_CreateRGBSurface(width ,height, 16, 0,0,0,0);
	}
	else{printf("create 8b surf\n");
		surf=Retro_CreateRGBSurface(width ,height, 8, 0,0,0,0);
	}
	if(surf==NULL)printf("failed createRGBsurf\n");
//printf("after createsurf\n");
	
		unsigned coul;
		unsigned *pix32=surf->pixels;
		unsigned short *pix16=surf->pixels;
		unsigned char *pix8=surf->pixels;
		long bufpos = 0; 



		for ( int y = 0; y < height; y++ )
			for ( int x = 0; x < width; x++)
			{			 
				bufpos = ( height - y - 1 ) * width + x;

				coul=SDL_MapRGB(surf->format, LUT[Buffer[bufpos]].rgbRed,\
					LUT[Buffer[bufpos]].rgbGreen,LUT[Buffer[bufpos]].rgbBlue);
	//printf("%d)%x-%x%x%x%x \n",bufpos,coul,LUT[Buffer[bufpos]].rgbRed,LUT[Buffer[bufpos]].rgbGreen,LUT[Buffer[bufpos]].rgbBlue,LUT[Buffer[bufpos]].rgbReserved);	
	if(SurfaceFormat==0){
				*pix32=coul;pix32++;
	}
	else if(SurfaceFormat==1){
				*pix16=coul;pix16++;
	}
	else{
				*pix8=Buffer[bufpos]/*coul*/;pix8++;
	}
			}

	if(1/*SurfaceFormat!=0 && SurfaceFormat!=1*/){
 SDL_Color *pptr=surf->format->palette->colors;
for ( int y = 0; y < 256; y++ ){
if(y<lutsize){
	(*pptr).r=LUT[y].rgbRed;
	(*pptr).g=LUT[y].rgbGreen;
	(*pptr).b=LUT[y].rgbBlue;
	(*pptr).a=LUT[y].rgbReserved;
}
else{
	(*pptr).r=0;
	(*pptr).g=0;
	(*pptr).b=0;
	(*pptr).a=0;
}

pptr++;
}

	}
//printf("Return %d\n",SurfaceFormat);
		free(Buffer);

		fclose ( fp );
		//printf("IN SDL LOAD BMP retrun\n");
		return surf;

	}

	size = bmpinfo.biSizeImage;

	//printf("size:%d /%d\n",size,bmpheader.bfOffBits);
	
	fseek(fp,bmpheader.bfOffBits,  SEEK_SET);

	unsigned char* Buffer = malloc(sizeof(char)*size);

	result= fread(Buffer, 1, size, fp);
	if(result!=size)
	{
		printf("IN SDL LOAD BMP err readbuffer =%d\n",result);
		fclose ( fp);
		free(Buffer);
		return NULL;
	}

//printf("IN SDL LOAD BMP CreateRGBSurface %d %d \n",width ,height);
	surf=Retro_CreateRGBSurface(width ,height, 16, 0,0,0,0);
if(surf==NULL)printf("failed createRGBsurf\n");
//printf("IN SDL LOAD BMP after CreateRGBSurface\n");


	int padding = 0;
	int scanlinebytes = width * 3;
	while ( ( scanlinebytes + padding ) % 4 != 0 )     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;   
	long newpos = 0;
	unsigned short coul;
	unsigned short *pix=surf->pixels;

	for ( int y = 0; y < height; y++ )
		for ( int x = 0; x < 3 * width; x+=3 )
		{			 
			bufpos = ( height - y - 1 ) * psw + x;
	
			coul=SDL_MapRGB(0, Buffer[bufpos + 2],Buffer[bufpos+1],Buffer[bufpos]);
			*pix=(unsigned short)coul;
			pix++;                                                                                 
			
		}

	free(Buffer);

	fclose ( fp );
//printf("IN SDL LOAD BMP retrun\n");
	return surf;

}
 
