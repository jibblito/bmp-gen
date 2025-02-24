/*
 * Text box class
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "tbox.h"

#define MIN(x,y) x < y?x:y;

int i,j;

void default_clickfunc(Tbox *self) {
	printf("Button '%s' has been pressed!\n",self->title);
}

void default_unclickfunc(Tbox *self) {
	printf("Button '%s' has been unpressed!\n",self->title);
}

/*
 * @f: font is needed to calculate size of cells
 * @w: Width of tbox in cells (variable pixel width based on font size)
 * @h: Height of tbox in cells (")
 */
Tbox tbox_init(Tbox *tbox, TboxFont *f, int x, int y, int w, int h, int padding, char *title, TboxColorPalette c) {
	// Geometry
	tbox->x = x;
	tbox->y = y;
	tbox->rowlen = w;
	tbox->collen = h;
	tbox->cell_padding_px = padding;
	tbox->cell_pix_w = f->fontheader.glyphwidth+2*padding+1;
	tbox->cell_pix_h = f->fontheader.glyphheight+2*padding+1;
	tbox->w = tbox->cell_pix_w*w+3;
	tbox->h = tbox->cell_pix_h*h+3; // 3: 2 for border, 1 for last line
	tbox->n_cells = w * h; 
	// Text state 
	tbox->hot_writing_index = 0;
	tbox->active = 0;
	// Functionality
	tbox->dynamic_color = 0;
	tbox->mode = Tbox_Linear;
	tbox->font = f;
	snprintf(tbox->title,TBOX_MAX_CHAR,title);
	tbox->clickfunc = default_clickfunc;
	tbox->unclickfunc = default_unclickfunc;
	tbox->c = c;
}

/*
 * Update a Tbox's geometry, yo!
 */
void tbox_setGeometry(Tbox *tbox, int x, int y, int w, int h, int padding){
	tbox->x= x;
	tbox->y= y;
	tbox->rowlen = w;
	tbox->collen = h;
	tbox->cell_padding_px = padding;
	tbox->cell_pix_w = tbox->font->fontheader.glyphwidth+2*padding+1;
	tbox->cell_pix_h = tbox->font->fontheader.glyphheight+2*padding+1;
	tbox->w = tbox->cell_pix_w*w+3;
	tbox->h = tbox->cell_pix_h*h+3; // 3: 2 for border, 1 for last line
	tbox->n_cells = w * h; 
}

/*
 * Overwrite text of tbox with a string
 */
void tbox_setText(Tbox *tbox, const char *string) {
	if (tbox->mode == Tbox_Freeform)
		memset(tbox->text,0,tbox->n_cells);
	snprintf(tbox->text,tbox->n_cells+1,string);
	tbox->hot_writing_index = MIN(strlen(string),tbox->n_cells);
}

/*
 * @cell_x,@cell_y: References to store intersecting cell indices
 */
int tbox_intersect(Tbox *tbox, int x, int y, int *cell_x, int *cell_y) {
	if (cell_x)
		*cell_x = (x - tbox->x) / tbox->cell_pix_w;
	if (cell_y)
		*cell_y = (y - tbox->y) / tbox->cell_pix_h;
	return ((x >=tbox->x && x <tbox->x+tbox->w &&
					y >=tbox->y && y <tbox->y+tbox->h) ? 1 : 0);
}

/*
 * Render Tbox
 */
int tbox_renderToCanvas(Canvas *cvs, Tbox *tbox) {
	if (tbox->n_cells == 0) return -1;
	int i,j;
	fillRectWH(cvs,tbox->x,tbox->y,tbox->w,tbox->h,&tbox->c.C_bg);
	for (i= tbox->x+1; i < tbox->x+tbox->w; i+=tbox->cell_pix_w) {
		drawLine(cvs,i,tbox->y+1,i,tbox->y+tbox->h-1,&tbox->c.C_lines);
	}
	for (i= tbox->y+1; i < tbox->y+tbox->h; i+=tbox->cell_pix_h) {
		drawLine(cvs,tbox->x+1,i,tbox->x+tbox->w-1,i,&tbox->c.C_lines);
	}
	if (tbox->mode == Tbox_Linear) {
		for (i = 0; i < tbox->hot_writing_index; i++) {
			int x = tbox->x+(i%tbox->rowlen)*tbox->cell_pix_w+tbox->cell_padding_px+2;
			int y = tbox->y+(i/tbox->rowlen)*tbox->cell_pix_h+tbox->cell_padding_px+2; // 2: one for border, one for line
			if (tbox->dynamic_color)
				renderGlyphToCanvas(cvs,tbox->font,x,y,tbox->font->glyphmap[tbox->text[i]],&tbox->text_colors[i]);
			else
				renderGlyphToCanvas(cvs,tbox->font,x,y,tbox->font->glyphmap[tbox->text[i]],&tbox->c.C_text);
		}
	} else if (tbox->mode == Tbox_Freeform) {
		for (i= 0; i < tbox->n_cells; i++) {
			int x = tbox->x+(i%tbox->rowlen)*tbox->cell_pix_w+tbox->cell_padding_px+2;
			int y = tbox->y+(i/tbox->rowlen)*tbox->cell_pix_h+tbox->cell_padding_px+2; // 2: same as above
			if (tbox->dynamic_color)
				renderGlyphToCanvas(cvs,tbox->font,x,y,tbox->font->glyphmap[tbox->text[i]],&tbox->text_colors[i]);
			else
				renderGlyphToCanvas(cvs,tbox->font,x,y,tbox->font->glyphmap[tbox->text[i]],&tbox->c.C_text);
		}
	}
	if (tbox->active)
		etchRectWH(cvs,tbox->x,tbox->y,tbox->w,tbox->h,&tbox->c.C_alt);
	else 
		etchRectWH(cvs,tbox->x,tbox->y,tbox->w,tbox->h,&tbox->c.C_border);
	return 1;
}

/*
 * Insert a char into a Tbox
 * In Linear mode, appends char to >hot_writing_index
 * In Freeform mode, sets char at [@x,@y] to @c
 */
int tbox_addChar(Tbox *tbox, char c, const ColorVec *clr, int x, int y) {
	if (tbox->mode == Tbox_Linear) {
		if (tbox->hot_writing_index > TBOX_MAX_CHAR || tbox->hot_writing_index > tbox->n_cells-1 && c != '\b') {
			printf("Exceeded max chars on this tbox, yo!\n");
			return 1;
		}
		if (c == '\b') {
			if (tbox->hot_writing_index > 0)
				tbox->text[--tbox->hot_writing_index]=0;
			return 0;
		}
		tbox->text[tbox->hot_writing_index]=c;
		if (tbox->dynamic_color)
			tbox->text_colors[tbox->hot_writing_index]=*clr;
		tbox->hot_writing_index++;
		return 0;
	} else if (tbox->mode == Tbox_Freeform) {
		printf("Adding %c at %d %d\n",c,x,y);
		*(tbox->text+x+y*tbox->rowlen) = c;
		if (tbox->dynamic_color)
			*(tbox->text_colors+x+y*tbox->rowlen) = *clr;
		return 0;
	}
}

/*
 * Return char at [@x,@y]
 */
unsigned char tboxc_getChar(Tbox *tbox, int x, int y) {
	return *(tbox->text + y*tbox->rowlen+x);
}

/*
 * Set entire Tbox to a single char
 */
void tbox_fill(Tbox *tbox, char fillchar) {
	INIT_COLOR(C_black,0x000000);
	if (tbox->mode == Tbox_Linear) {
		memset(tbox->text,fillchar,tbox->n_cells);
		tbox->hot_writing_index = tbox->n_cells;
	} else if (tbox->mode == Tbox_Freeform) {
		int i;
		memset(tbox->text,fillchar,tbox->n_cells);
		if (tbox->dynamic_color)
			for (i = 0; i < tbox->n_cells; i++)
				tbox->text_colors[i] = C_black;
	}
}

/*
 * Set sub-area of Tbox to a single char
 */
void tbox_fillArea(Tbox *tbox, char fillchar, int x, int y, int w, int h) {
	INIT_COLOR(C_black,0x000000);
	int i,j;
	if (tbox->mode == Tbox_Freeform) {
		if (x + w > tbox->rowlen || y + h > tbox->collen) {
			printf("Reaching outside of tbox dimensions");
		} else {
			printf("Clearing %dx%d blok in %s\n",w,h,tbox->title);
			for (i = 0; i < h; i++) {
				memset(tbox->text+(i+y)*tbox->rowlen+x,fillchar,w);
			}
		}
	}
}

void clearTbox(Tbox *tbox) {
	tbox_fill(tbox,' ');
}

void clearTboxArea(Tbox *tbox,int x, int y, int w, int h) {
	tbox_fillArea(tbox,' ',x,y,w,h);
}

// FONT FUNCTIONS

/*
 * Add a glyph to a font at index @index
 */
void addGlyph(TboxFont *font, char index, char *glyph) {
	int glyph_width_in_chars = (font->fontheader.glyphwidth-1) / 8 + 1;
	int glyph_size_in_chars = glyph_width_in_chars*font->fontheader.glyphheight;
	memcpy(font->glyphs+index*glyph_size_in_chars,glyph,glyph_size_in_chars);
	printf("Added glyph of dimension %dx%d at index %d\n",font->fontheader.glyphwidth,font->fontheader.glyphheight,index);
}

/*
 * Render a glyph from TboxFont @font to Canvas
 *
 * this method uses Font Glyph Index to render a char
 */
void renderGlyphToCanvas(Canvas *cvs, TboxFont *font, int x, int y, char glyphIndex, const ColorVec *clr) {
	int glyph_width_in_chars = (font->fontheader.glyphwidth-1) / 8 + 1;
	int glyph_size_in_chars = glyph_width_in_chars*font->fontheader.glyphheight;
	int i,j;
	unsigned char *glyph = font->glyphs+glyphIndex*glyph_size_in_chars;
	for(i = 0; i < font->fontheader.glyphheight; i++) {
		unsigned char charOffset = 0;
		for(j = 0; j < font->fontheader.glyphwidth; j++) {
			if (j % 8==0 && j != 0) charOffset++;
			if (*(glyph+i*glyph_width_in_chars+charOffset) & (0x01 << (j%8))) plot(cvs,x+j,y+i,clr);
		}
	}
}

/*
 * Render a glyph from TboxFont @font to Canvas
 * 
 * this method simply has a char for input - this is the human-readable function to call
 */
int renderChar(Canvas *cvs, TboxFont *font, int x, int y, char letter, const ColorVec *clr) {
	int i;
	unsigned char index = font->glyphmap[letter];
	if (index == -1) {
		fprintf(stderr,"Glyph '%c' not found in font %s",letter,"<font-filename>");
		return 1;
	}
	renderGlyphToCanvas(cvs,font,x,y,72,clr);
	return 0;
}

/*
 * Associate index @index in the font file with a char in the map
 *
 * >glyphmap is a glyphmap where you get an index based on an input char
 * >r_glyphmap is a reverse glyphmap where you get a char based on index in the font file
 */
void mapGlyphIndexToChar(TboxFont *font,char index, unsigned char c) {
	font->glyphmap[c] = index;
	font->r_glyphmap[index] = c;
}

/*
 * Renders a TboxFont to a Canvas object, showing "all them glyphs".
 */
void render_entire_font(Canvas *cvs, TboxFont *font, int x, int y, const ColorVec *glyphclr, int hl_index) {
	INIT_COLOR(C_white,0xffffff);
	INIT_COLOR(C_black,0x000000);
	INIT_COLOR(C_red,0xff0000);
	int i=0,j=-1;

	int box_width = font->fontheader.glyphwidth+1;
	int box_height = font->fontheader.glyphheight+1;

	int n_rows = font->fontheader.n_glyphs / TBOX_ALPHABET_WID + 1;

	fillRectWH(cvs,x,y,TBOX_ALPHABET_WID*box_width+1,n_rows*box_height+1,&C_white);
	while (i < n_rows * TBOX_ALPHABET_WID) {
		if(!(i % TBOX_ALPHABET_WID)) j++;
		if (i < font->fontheader.n_glyphs)
			renderGlyphToCanvas(cvs,font,x+(i%TBOX_ALPHABET_WID)*box_width+1,y+j*box_height+1,i,glyphclr);
		else
			fillRectWH(cvs,x+(i%TBOX_ALPHABET_WID)*box_width+1,y+j*box_height+1,box_width-1,box_height-1,&C_red);
		++i;
	}
	++j;
	for (i = 0; i <= TBOX_ALPHABET_WID; i++)
		drawLine(cvs,x+i*box_width,y,x+i*box_width,y+j*box_height,&C_black);

	for (j = 0; j <= n_rows; j++)
		drawLine(cvs,x,y+j*box_width,x+TBOX_ALPHABET_WID*box_width,y+j*box_height,&C_black);
	if (hl_index != -1)
		etchRectWH(cvs,x+(hl_index%TBOX_ALPHABET_WID)*box_width,y+(hl_index/TBOX_ALPHABET_WID)*box_height,box_width+1,box_height+1,&C_red);
}

/*
 * Save a TboxFont to file @filename
 */
int save_font(TboxFont *font, const char *filename) {
	int fd;
	if (strlen(filename)==0) {
		fprintf(stderr,"Please supplie a filename's to tha font, yo!!!\n");
		return -1;
	}
	if (font == NULL) {
		fprintf(stderr,"Please supplie a TboxFont to save_font()\n");
		return -1;
	}

	fd = open(filename,O_CREAT|O_RDWR|O_TRUNC,S_IRUSR|S_IWUSR);
	if (fd != -1) {
		int n_bytes;
		n_bytes = write(fd,&font->fontheader,sizeof(struct FontHeader));
		int glyph_width_in_chars = (font->fontheader.glyphwidth-1) / 8 + 1;
		int glyph_size_in_chars = glyph_width_in_chars*font->fontheader.glyphheight;
		int glyph_block_size = glyph_size_in_chars * font->fontheader.n_glyphs;
		n_bytes += write(fd,font->glyphs,glyph_block_size);
		n_bytes += write(fd,font->glyphmap,sizeof(font->glyphmap));
		n_bytes += write(fd,font->r_glyphmap,sizeof(font->r_glyphmap));
		printf("Wrote %d bytes to %s\n",n_bytes,filename);
	} else {
		printf("Unable to open font '%s': %s\n",filename,strerror(errno));
		return -1;
	}
	if (close(fd) == -1) {
		fprintf(stderr,"Unable to close font file %d\n",filename);
		return 1;
	}
	return 0;
}

/*
 * Load a TboxFont from file @filename
 */
TboxFont load_font(const char *filename) {
	int fd;
	TboxFont font;
	fd = open(filename,0,O_RDONLY);
	if (fd != -1) {
		printf("Font file opened\n");
		int n_bytes;
		n_bytes = read(fd,&font.fontheader,sizeof(struct FontHeader));
		/* NEED TO CHECK FOR CORRUPT FONT FILEHEADER */
		int glyph_width_in_chars = (font.fontheader.glyphwidth-1) / 8 + 1;
		int glyph_size_in_chars = glyph_width_in_chars*font.fontheader.glyphheight;
		int glyph_block_size = glyph_size_in_chars * font.fontheader.n_glyphs;
		font.glyphs = malloc(glyph_block_size);
		n_bytes += read(fd,font.glyphs,glyph_block_size);
		n_bytes += read(fd,font.glyphmap,sizeof(font.glyphmap));
		n_bytes += read(fd,font.r_glyphmap,sizeof(font.r_glyphmap));
		printf("Read %d glyphs from %s (%d bytes)\n",font.fontheader.n_glyphs,filename,n_bytes);
	} else {
		printf("Unable to load font %s: %s\n",filename,strerror(errno));
		font.fontheader.n_glyphs=0;
		return font;
	}
	if (close(fd) == -1) {
		fprintf(stderr,"Unable to close font file %d\n",filename);
		exit(1);
	}
	return font;
}

/*
 * Initialize an empty font with certain glyphwidth and glyphheight and 64 characters to start
 */
void init_blank_font (TboxFont *font, unsigned char glyphwidth, unsigned char glyphheight, unsigned char n_charslots) {
	if (n_charslots < 12) {
		printf("Font contain at least 12 chars, man!\n");
		exit(ENOTSUP);
	}
	if (glyphwidth < 2 || glyphheight < 2) {
		printf("Font must be at least 2x2, man!\n");
		exit(ENOTSUP);
	}
	font->fontheader.glyphwidth = glyphwidth;
	font->fontheader.glyphheight = glyphheight;
	font->fontheader.n_glyphs = n_charslots;
	font->glyphs = calloc(n_charslots,glyphheight * ((glyphwidth-1) / 8) + 1); // glyph stored as chars with $glyphheight rows and $glyphwidth bits per row and therefore some multiple of 1 char to store each row
	// set mappings from indices to chars
	memset(font->glyphmap,0,256);
	memset(font->r_glyphmap,0,256);
	printf("Font created with %dx%d grid of empty %d chars!\n",glyphwidth,glyphheight,n_charslots);
}

/*
 * Free allocated memory for TboxFont
 */
void freeFont(TboxFont *font) {
	if (font->glyphs)
		free(font->glyphs);
}
