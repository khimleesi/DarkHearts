
//{{BLOCK(floor)

//======================================================================
//
//	floor, 512x256@4, 
//	+ palette 256 entries, not compressed
//	+ 5 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 160 + 4096 = 4768
//
//	Time-stamp: 2017-05-11, 19:23:56
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FLOOR_H
#define GRIT_FLOOR_H

#define floorTilesLen 160
extern const unsigned short floorTiles[80];

#define floorMapLen 4096
extern const unsigned short floorMap[2048];

#define floorPalLen 512
extern const unsigned short floorPal[256];

#endif // GRIT_FLOOR_H

//}}BLOCK(floor)
