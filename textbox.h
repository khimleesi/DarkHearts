
//{{BLOCK(textbox)

//======================================================================
//
//	textbox, 512x256@4, 
//	+ palette 256 entries, not compressed
//	+ 5 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 160 + 4096 = 4768
//
//	Time-stamp: 2017-05-23, 16:50:47
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TEXTBOX_H
#define GRIT_TEXTBOX_H

#define textboxTilesLen 160
extern const unsigned short textboxTiles[80];

#define textboxMapLen 4096
extern const unsigned short textboxMap[2048];

#define textboxPalLen 512
extern const unsigned short textboxPal[256];

#endif // GRIT_TEXTBOX_H

//}}BLOCK(textbox)
