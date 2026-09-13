/*-==============================================================-

file : BandCst.hpp

creation date : 27/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : contains 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDCST_HPP__
#define __BANDCST_HPP__

/* to describe a pick of a single warp */
#define COLOR_MASK (quint16)63
#define NOCOLOR_MASK (quint16)65472
#define COLOR_STEP 0

#define SHAPE_MASK (quint16)192
#define NOSHAPE_MASK (quint16)65343
#define SHAPE_STEP 6

#define HOLE_MASK (quint16)65280
#define NOHOLE_MASK (quint16)255
#define HOLE_STEP 8

#define PARA_RIGHT (quint16)0
#define PARA_LEFT (quint16)1
#define TRI_RIGHT (quint16)2
#define TRI_LEFT (quint16)3

#define PICK_COLOR(a) (a & COLOR_MASK)
#define PICK_SHAPE(a) ((a & SHAPE_MASK)>>SHAPE_STEP)
#define PICK_HOLE(a) ((a & HOLE_MASK)>>HOLE_STEP)


#define PICK(color,shape,hole) ((color) | ((shape)<<SHAPE_STEP) | ((hole)<<HOLE_STEP))

#define VOID (quint16)0 // a pick that has no color/shape/hole associated

/* to describe pick moves */
#define SZ_MASK (quint8)1
#define NOSZ_MASK (quint8)254
#define SZ_STEP 0

#define TW_MASK (quint8)2
#define NOTW_MASK (quint8)253
#define TW_STEP 1

#define MOVE_MASK (quint8)12
#define NOMOVE_MASK (quint8)243
#define MOVE_STEP 2

#define SIDE_MASK (quint8)112
#define NOSIDE_MASK (quint8)143
#define SIDE_STEP 4

#define S_MOUNT (quint8)0
#define Z_MOUNT (quint8)1
#define NO_TWIST (quint8)0
#define TWIST (quint8)1
#define FORWARD (quint8)0
#define BACKWARD (quint8)1
// IDLE = 3, cf. forceGuessIdles() in PatternGuesser
#define IDLE (quint8)3
#define PICK_MOUNT(a) (a & SZ_MASK)
#define PICK_TWIST(a) ((a & TW_MASK)>>TW_STEP)
#define PICK_MOVE(a) ((a & MOVE_MASK)>>MOVE_STEP)
#define PICK_SIDE(a) ((a & SIDE_MASK)>>SIDE_STEP)

#define SF (S_MOUNT | (FORWARD << 2))
#define SB (S_MOUNT | (BACKWARD << 2))
#define SI (S_MOUNT | (IDLE << 2))
#define STF (S_MOUNT | (TWIST << 1) | (FORWARD << 2))
#define STB (S_MOUNT | (TWIST << 1) | (BACKWARD << 2))
#define STI (S_MOUNT | (TWIST << 1) | (IDLE << 2))
#define ZF (Z_MOUNT | (FORWARD << 2))
#define ZB (Z_MOUNT | (BACKWARD << 2))
#define ZI (Z_MOUNT | (IDLE << 2))
#define ZTF (Z_MOUNT | (TWIST << 1) | (FORWARD << 2))
#define ZTB (Z_MOUNT | (TWIST << 1) | (BACKWARD << 2))
#define ZTI (Z_MOUNT | (TWIST << 1) | (IDLE << 2))

#define NO_MOVE (quint8)128

#endif // __BANDCST_HPP__
