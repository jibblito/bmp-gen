/*
 * Tbox.h: Contains font structures and Tbox structure for use
 * with displaying text on the screen
 */

#include "shapes.h"
#include "canvas.h"
#include "colorvec.h"

#define TBOX_MAX_CHAR 2048
#define TBOX_MAX_TBOX 16

#define TBOX_ALPHABET_WID 40
#define TBOX_ALPHABET_HEI 4
#define TBOX_ALPHABET_TOTAL 160 

typedef struct _TboxColorPalette {
	ColorVec C_lines,C_bg,C_text,C_alt,C_border;
} TboxColorPalette;

static const TboxColorPalette defaultTboxPalette = { C_gray50,C_white,C_blue,C_magenta,C_black };

struct FontHeader {
	unsigned int glyphwidth;  // width in bits
	unsigned int glyphheight; // height in bits
	int n_glyphs;
};

typedef struct _TboxFont {
	struct FontHeader fontheader;
	unsigned char *glyphs;
	unsigned char glyphmap[256]; // Char indexes to glyph #
	unsigned char r_glyphmap[256]; // Glyph # indexes to char
} TboxFont;

enum TboxMode { Tbox_Linear, Tbox_Freeform };

typedef struct _Tbox {
	/* Geometry */
	int x,y,w,h; 
	int cell_pix_w,cell_pix_h;
	int rowlen,collen;
	int n_cells;
	int cell_padding_px;
	/* Data State */
	int hot_writing_index;
	int active_cell;
	int active; // Is the tbox highlighted
	unsigned char text[TBOX_MAX_CHAR];
	ColorVec text_colors[TBOX_MAX_CHAR];
	/* Member data */
	enum TboxMode mode;
	int dynamic_color;
	TboxFont *font;
	unsigned char title[TBOX_MAX_CHAR];
	TboxColorPalette c;
	void (*clickfunc) (struct _Tbox *);
	void (*unclickfunc) (struct _Tbox *);
} Tbox;

// Tbox Functions

void default_clickfunc(Tbox *self);
void default_unclickfunc(Tbox *self);
Tbox tbox_init(Tbox *tbox, TboxFont *f, int x, int y, int w, int h, int padding, char *title, TboxColorPalette c);
void tbox_setGeometry(Tbox *tbox, int x, int y, int w, int h, int cell_padding_px);
void tbox_setText(Tbox *tbox, const char *string);
int tbox_intersect(Tbox *tbox, int x, int y, int *cell_x, int *cell_y);

int tbox_renderToCanvas(Canvas *cvs, Tbox *tbox);
int tbox_addChar(Tbox *tbox, char c, const ColorVec *clr, int x, int y);
unsigned char tboxc_getChar(Tbox *tbox, int x, int y);
void tbox_fill(Tbox *tbox, char fillchar);
void tbox_fillArea(Tbox *tbox, char fillchar, int x, int y, int w, int h);
void clearTbox(Tbox *tbox);
void clearTboxArea(Tbox *tbox,int x, int y, int w, int h);


// Font functions

void renderGlyphToCanvas(Canvas *cvs, TboxFont *font, int x, int y, char glyphIndex, const ColorVec *);
void addGlyph(TboxFont *font, char index, char *glyph);
void renderGlyphToCanvas(Canvas *cvs, TboxFont *font, int x, int y, char glyphIndex, const ColorVec *);
int renderChar(Canvas *cvs, TboxFont *font, int x, int y, char letter, const ColorVec *clr);
void mapGlyphIndexToChar(TboxFont *font,char index, unsigned char c);
void render_entire_font(Canvas *cvs, TboxFont *font, int x, int y, const ColorVec *glyphclr, int hl_index);
int save_font(TboxFont *font, const char *filename);
TboxFont load_font(const char *filename);
void init_blank_font (TboxFont *font, unsigned char glyphwidth, unsigned char glyphheight, unsigned char n_charslots);
void freeFont(TboxFont *font);

