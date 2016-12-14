#pragma once
#include "N3s3d.hpp"

const static int characterCount = 63;

using namespace std;

class Overlay {
public:
	static void init();
	static void unload();
	static void drawString(int x, int y, int scale, string s);
	static void drawVoxelPreview(int x, int y, int z);
	static bool shadow;
};

struct BitmapCharacter
{
	int pixels[64];
};

VoxelMesh createMeshFromBitmapCharacter(BitmapCharacter bitmap);

void buildVoxelPreviewMesh();
void buildVoxelGridMesh();
void buildNametableGridMesh();

//						  012345678901234567890123456789012345678901234567890123456789012
// starting with a space: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVQXYZ[\]^`
// a..z then {|}~
static BitmapCharacter bitmapCharacters[characterCount] =
{
	{	// 0 !
		0,0,0,1,1,0,0,0,
		0,0,1,1,1,1,0,0,
		0,0,1,1,1,1,0,0,
		0,0,1,1,1,1,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,1,1,0,0,0
	},
	{	// 1 "
		1,1,0,1,1,0,0,0,
		1,1,0,1,1,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 2 #
		0,1,1,0,1,1,0,0,
		0,1,1,0,1,1,0,0,
		1,1,1,1,1,1,1,0,
		0,1,1,0,1,1,0,0,
		0,1,1,0,1,1,0,0,
		1,1,1,1,1,1,1,0,
		0,1,1,0,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 3 $
		0,1,1,1,1,0,0,0,
		1,1,0,1,1,1,0,0,
		1,1,0,1,0,0,0,0,
		0,1,1,1,1,1,0,0,
		0,0,0,1,0,1,1,0,
		1,1,0,1,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 4 %
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,1,0,0,
		0,0,0,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,1,1,0,0,
		0,0,0,0,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 5 &
		0,0,1,1,0,0,0,0,
		0,1,0,0,1,0,0,0,
		0,1,0,0,1,0,0,0,
		0,0,1,1,0,0,0,0,
		0,1,0,0,1,1,1,0,
		1,0,0,0,0,1,0,0,
		0,1,1,1,1,0,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 6 '
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 7 (
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 8 )
		0,0,1,0,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 9 *
		0,0,1,0,0,0,0,0,
		0,1,1,1,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 10 +
		0,0,0,0,0,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,1,1,1,1,1,1,0,
		0,1,1,1,1,1,1,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 11 ,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 12 -
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,1,1,1,1,1,0,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 13 .
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 14 /
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,1,1,0,
		0,0,0,0,1,1,0,0,
		0,0,0,1,1,0,0,0,
		0,0,1,1,0,0,0,0,
		0,1,1,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 15 0
		0,0,1,1,1,0,0,0,
		0,1,0,0,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,0,0,0,1,0,0,
		0,0,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 16 1
		0,0,0,1,1,0,0,0,
		0,0,1,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 17 2
		0,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,1,1,1,0,
		0,0,1,1,1,1,0,0,
		0,1,1,1,1,0,0,0,
		1,1,1,0,0,0,0,0,
		1,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 18 3
		0,1,1,1,1,1,1,0,
		0,0,0,0,1,1,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,0,1,1,0,0,
		0,0,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 19 4
		0,0,0,0,1,1,0,0,
		0,0,1,1,1,1,0,0,
		0,1,1,0,1,1,0,0,
		1,1,0,0,1,1,0,0,
		1,1,1,1,1,1,1,0,
		0,0,0,0,1,1,0,0,
		0,0,0,0,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 20 5
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,0,0,0,
		1,1,1,1,1,1,0,0,
		0,0,0,0,0,1,1,0,
		0,0,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 21 6
		0,0,1,1,1,1,0,0,
		0,1,1,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 22 7
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,1,1,0,0,
		0,0,0,1,1,0,0,0,
		0,0,1,1,0,0,0,0,
		0,0,1,1,0,0,0,0,
		0,0,1,1,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 23 8
		0,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 24 9
		0,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,1,0,
		0,0,0,0,0,1,1,0,
		0,0,0,0,1,1,0,0,
		0,1,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 25 :
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 26 ;
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 27 <
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 28 =
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 29 >
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 30 ?
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 31 @
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 32 A
		0,0,1,1,1,0,0,0,
		0,1,1,0,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 33 B
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,0,1,0,
		1,1,0,0,0,0,1,0,
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,0,1,0,
		1,1,0,0,0,0,1,0,
		1,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 34 C
		0,0,1,1,1,1,0,0,
		0,1,1,0,0,1,1,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		0,1,1,0,0,1,1,0,
		0,0,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 35 D
		1,1,1,1,1,0,0,0,
		1,1,0,0,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,1,1,0,0,
		1,1,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 36 E
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 37 F
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 38 G
		0,0,1,1,1,1,1,0,
		0,1,1,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,1,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,0,0,1,1,0,
		0,0,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 39 H
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,1,1,1,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 40 I
		0,1,1,1,1,1,1,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 41 J
		0,0,0,0,1,1,1,0,
		0,0,0,0,0,1,1,0,
		0,0,0,0,0,1,1,0,
		0,0,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 42 K
		1,1,0,0,0,1,1,0,
		1,1,0,0,1,1,0,0,
		1,1,0,1,1,0,0,0,
		1,1,1,1,0,0,0,0,
		1,1,1,1,1,0,0,0,
		1,1,0,1,1,1,0,0,
		1,1,0,0,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 43 L
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 44 M
		1,0,0,0,0,0,1,0,
		1,1,0,0,0,1,1,0,
		1,1,1,0,1,1,1,0,
		1,1,0,1,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 45 N
		1,1,0,0,0,1,1,0,
		1,1,1,0,0,1,1,0,
		1,1,1,1,0,1,1,0,
		1,1,1,1,1,1,1,0,
		1,1,0,1,1,1,1,0,
		1,1,0,0,1,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 46 O
		0,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 47 P
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,0,0,0,
		1,1,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 48 Q
		0,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,1,1,1,1,0,
		1,1,0,0,1,1,0,0,
		0,1,1,1,1,0,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 49 R
		1,1,1,1,1,1,0,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,1,1,1,0,
		1,1,1,1,1,0,0,0,
		1,1,0,1,1,0,0,0,
		1,1,0,0,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 50 S
		0,1,1,1,1,0,0,0,
		1,1,0,0,1,1,0,0,
		1,1,0,0,0,0,0,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 51 T
		0,1,1,1,1,1,1,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 52 U
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 53 V
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,1,0,1,1,1,0,
		0,1,1,0,1,1,0,0,
		0,0,1,1,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 54 W
		1,1,0,0,0,1,1,0,
		1,1,0,0,0,1,1,0,
		1,1,0,1,0,1,1,0,
		1,1,1,1,1,1,1,0,
		1,1,1,1,1,1,1,0,
		1,1,1,0,1,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 55 X
		1,1,0,0,0,1,1,0,
		1,1,1,0,1,1,1,0,
		0,1,1,1,1,1,0,0,
		0,0,1,1,1,0,0,0,
		0,1,1,1,1,1,0,0,
		1,1,1,0,1,1,1,0,
		1,1,0,0,0,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 56 Y
		0,1,1,0,0,1,1,0,
		0,1,1,0,0,1,1,0,
		0,1,1,0,0,1,1,0,
		0,0,1,1,1,1,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 57 Z
		1,1,1,1,1,1,1,0,
		0,0,0,0,1,1,1,0,
		0,0,0,1,1,1,0,0,
		0,0,1,1,1,0,0,0,
		0,1,1,1,0,0,0,0,
		1,1,1,0,0,0,0,0,
		1,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	},
	{	// 58 [
		0,1,1,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,0,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 59 backslach
		1,1,0,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,0,1,1,0,0,0,0,
		0,0,0,1,1,0,0,0,
		0,0,0,0,1,1,0,0,
		0,0,0,0,0,1,1,0,
		0,0,0,0,0,0,1,1,
		0,0,0,0,0,0,0,0
	},
	{	// 60 ]
		0,1,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,1,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 61 ^
		0,0,1,0,0,0,0,0,
		0,1,0,1,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	},
	{	// 62 `
		0,1,0,0,0,0,0,0,
		0,0,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
	}
};