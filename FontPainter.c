
#include "FontPainter.h"




int swtch=1;
int fbfd = 0;
long int screensize = 0;
long int location = 0;
char *fbp = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;



/** 
@function Font_Init
Инициализация шрифта. Запускает библиотеку Freetype и устанавливает нужный шрифт
Initialize freetype library and load font
@param filename - путь к файлу шрифта
*/
void Font_Init (char* filename)
{
	  error = FT_Init_FreeType( &library );              /* initialize library */
	  

	  error = FT_New_Face( library, filename, 0, &face );/* create face object */
	  /* error handling omitted */

	  error = FT_Set_Char_Size( face, 20 * 64, 0,
	                            PPI, 0 );                /* set character size */
	  slot = face->glyph;
}

 /** 
 @function FB_Init
 Инициализация фрейм буфера. Считывает и выводит параметры экрана
 Initialize linux framebuffer.
 */
void FB_Init (void)
{
	 fbfd = open("/dev/fb0", O_RDWR);
	  if (fbfd == -1) {
	  perror("Error: cannot open framebuffer device");
	  exit(1);
	  }
	  printf("The framebuffer device was opened successfully.\n");

	  
	  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1)
	  {
	  perror("Error reading fixed information");
	  exit(2);
	  }

	  
	  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
	  perror("Error reading variable information");
	  exit(3);
	  }

	  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	  
	  screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	  
	  fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
	  fbfd, 0);
	  if ((int)fbp == -1) {
	  perror("Error: failed to map framebuffer device to memory");
	  exit(4);
	  }
}

/** 
@function Font_Render 
Функция для рендера и вывода во фрейм буфер изображения текста, введенного пользователем
Render text and output it into framebuffer
@param x_origin - Отступ слева (в пикселях)
@param y_origin - Отступ сверху (в пикселях)
@param height - Высота окна, в котором будет выводиться текст (в пикселях)
@param width - Ширина окна, в котором будет выводиться текст (в пикселях)
@param pt - Размер шрифта (в pt)
@param text - текст для вывода
*/
void Font_Render( int x_origin, int y_origin, int height, int width, int pt_def, char* text)

{
  int  num_chars;                        
  num_chars     = strlen( text );


  	int n,a,x_drw,y_drw,pt=pt_def, bmp_w=0, row_pos=1, new_word=0, bmp_lst;
      FT_Bitmap*  bitmap;

    rows[0].last_chr= -2;
    unsigned char * image = (unsigned char *)malloc(height*width);                                                         

    error = FT_Set_Char_Size( face, pt_def * 64, 0,
    	                            PPI, 0 );

  	for ( n=0 ; n < num_chars+1; n++ )
  	{
  	    if (n==num_chars)
  	    {
  	    	bmp_lst = bmp_w;
  	    	bmp_w += width;
  	    	new_word = n+1;
  	    }
  		 if (bmp_w > width)
  				 {
  			         if (new_word==rows[row_pos-1].last_chr+2)
  			         {
  			        	 pt -= 1;
  			        	 error = FT_Set_Char_Size( face, pt * 64, 0, PPI, 0 );
  			        	 n = new_word;
  			           	 bmp_w=0;
  			         }
  			          else
  			         {
                     double j = 40;
  			         rows[row_pos].height= j;
  					 rows[row_pos].last_chr= new_word -2;
  					 for ( a=rows[row_pos-1].last_chr+2 ; a < new_word-1; a++)
  					 {

  						 error = FT_Load_Char( face, text[a], FT_LOAD_RENDER );
  							    if ( error )
  							      continue;
  							    bitmap = &slot->bitmap;

  							    switch (swtch)
  							    {
  							      case 0:
  							      x_drw = slot->bitmap_left+ rows[row_pos].width + (width- bmp_lst);
  							      break;
  							      case 1:
  							      x_drw = slot->bitmap_left+ rows[row_pos].width + ((width- bmp_lst)/2);
  							      break;
  							      case 2:
  							      x_drw = slot->bitmap_left+ rows[row_pos].width;
  							      break;

  							    }

  							    y_drw = rows[row_pos].height*row_pos - slot->bitmap_top;

  							  
  							    FT_Int  i, j, p, q;
  							    FT_Int  x_max = x_drw + bitmap->width;
  							    FT_Int  y_max = y_drw + bitmap->rows;

  							    for ( i = x_drw, p = 0; i < x_max; i++, p++ )
  							    {
  							      for ( j = y_drw, q = 0; j < y_max; j++, q++ )
  							      {
  							        if (i >= width || j >= height )
  							          continue;

  							        image[j*width+i] |= bitmap->buffer[q * bitmap->width + p];
  							      }
  							    }
  							  //



  							  rows[row_pos].width += bitmap->width + INDENT;
  							if (text[a]==' ')
  							  	 	 {
  								rows[row_pos].width += INDENT_WRD;
  							  	 	 }

  					 }
  					 bmp_lst = 0;
  					 row_pos +=1;
  					 pt=pt_def;
  					 error = FT_Set_Char_Size( face, pt * 64, 0, PPI, 0 );
  					 bmp_w=0;
  					 n = new_word;
  			         }
  				 }
  				 ;

  	 error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
  	 bitmap = &slot->bitmap;
  	 bmp_w += bitmap->width + INDENT;

 	  if (text[n]==' ')
 	    	 	 {
 	    		     bmp_lst = bmp_w ;
 	    		     bmp_w += INDENT_WRD;
 	    	 		 new_word = n+1;
 	    	 	 }
  	}




  	  int  c, v;
  	  unsigned char grade;
  	  for ( c = 0; c < (rows[row_pos-1].height*(row_pos)); c++ )
  	    for ( v = 0; v < width; v++ )

  	    	{
  	    	location = (v+x_origin) * (vinfo.bits_per_pixel/8) +
  	    	(c+y_origin) * finfo.line_length;
  	        grade = image [c*width+v];
  	        if (image [c*width+v] > 0)
  	                {
  	    	if (vinfo.bits_per_pixel == 32) // если bpp  32 бита
  	    	{
  	    	*(fbp + location) = grade; // синий
  	    	*(fbp + location + 1) = grade; // зеленый
  	    	*(fbp + location + 2) = grade; // красный
  	    	*(fbp + location + 3) = 128; // прозрачность (походу не работает)
  	        } else
  	        {
  	        	int b = grade;
  	        	int g = grade; 
  	        	int r = grade; 
  	        	int t = r << 11 || g << 5 || b;
  	        	*((unsigned short int*)(fbp + location)) = t;
  	        }
  	                }
  	    	}
//
  	  int cnt;
  	  for (cnt=0 ; cnt<ROWS; cnt++)
  	  {
  		  rows [cnt].last_chr=0;
  		  rows [cnt].width =0;
  	  }
  	  free(image);
}

 /** 
 @function Font_Free
 деструктор обьектов шрифта (library и face)
 */
void Font_Free (void)
{
	  FT_Done_Face    ( face );
	  FT_Done_FreeType( library );
}
