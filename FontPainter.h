#include <stdio.h> 
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <ft2build.h>
#include FT_FREETYPE_H


/// Отступ между буквами (px)
#define INDENT  2
/// ppi экрана
#define PPI	 81
/// Интервал между словами (px)
#define	INDENT_WRD 10
/// кол во строк (временное решение)
#define ROWS 10
/// ориентация текста по центру
#define ALIGN_CENTER 1
/// ориентация текста по слева
#define ALIGN_LEFT 2
/// ориентация текста по справа
#define ALIGN_RIGHT 3


/// объект библиотеки Freetype
  FT_Library    library;
/// объект шрифта. Содержит тип и стиль шрифта
  FT_Face       face;
 /// объект ошибки. содержит номер ошибки 
  FT_Error      error;
/// объект в котором содержится глиф или bitmap символа
  FT_GlyphSlot  slot;

/**
@class row 
Строка. Содержит в качестве параметров свою длину (в пикселях),высоту (в пикселях) и номер своего последнего символа 
*/
struct row
{
	/// Ширина строки (в пикселях)
	int width;  
	/// Высота строки (в пикселях)
	int height;
	/// Номер последнего символа в строке
	int last_chr;
};


/// Массив строк
 struct row rows[ROWS];

/** 
@function Font_Render 
Функция для рендера и вывода во фрейм буфер изображения текста, введенного пользователем.
В качестве параметров принимает: отступ слева(в пикселях), отступ сверху(в пикселях), высоту окна вывода(в пикселях), ширину окна вывода (в пикселях), размер шрифта (в pt) и текст.  
*/
 void Font_Render (int,int,int,int,int,char*);

 /** 
 @function Font_Init
 Инициализация шрифта. В качестве параметра принимает путь к файлу необходимого шрифта.
 */
 void Font_Init (char*);
  
 /** 
 @function FB_Init
 Инициализация фрейм буфера. Считывает и выводит параметры экрана
 */
 void FB_Init (void);

 /** 
 @function Font_Free
 деструктор обьектов шрифта (library и face)
 */
 void Font_Free (void);



