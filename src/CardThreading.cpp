/*-==============================================================-

file : CardThreading.cpp

creation date : 22/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : contains 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "CardThreading.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>


CardThreading::CardThreading() {

  initHolesToColor();
}

CardThreading::~CardThreading() {

  int i,j;
  quint8 **p1;
  quint8 *p2;
  quint32 *p3;

  for(i=0;i<4;i++) {
    p1 = holesToColor[i];
    for(j=0;j<3+i;j++) {
      p2 = p1[j];
      delete [] p2;
    }
    delete [] p1;
    p3 = nbAna[i];
    delete [] p3;
  }

  delete [] holesToColor;
  delete [] nbAna;
}

quint8 CardThreading::dualPick(quint8 pick) {

  if (pick == SF) return ZB;
  else if (pick == SB) return ZF;
  else if (pick == STF) return ZTB;
  else if (pick == STB) return ZTF;
  else if (pick == ZF) return SB;
  else if (pick == ZB) return SF;
  else if (pick == ZTF) return STB;
  else if (pick == ZTB) return STF;
  return 0;
}

void CardThreading::initHolesToColor() {

  int i;
  holesToColor = new quint8 **[4];
  nbAna = new quint32 *[4];
  for(i=0;i<4;i++) {
    holesToColor[i] = new quint8 *[3+i];
    nbAna[i] = new quint32[3+i];
  }

  /* NB : the following code is not optimized at all
     but it is written to be clear.

     For a card with H holes, it is possible to use from 1 to H
     colors to thread the card. Obviously if we use less than H
     there are some thread with the same color.
     For example, supposing we have a 5 holes card, and we use 
     three colors A, B and C, it is possible to have :
     3xA+B+C, A+3xB+C, A+B+3xC, 2xA+2xB+C, 2xA+B+2xC,
     A+2xB+2xC, i.e. 6 possibilities to choose the thread colors
     But for each possibility, we have to know how many permutations
     (or anagrams) there exists.
     For example, if we suppose that we assign a color to increasing 
     hole number (form 1 to 5), a concatenate them, we obtain :
     for 3xA+B+C : 20 anagrams (AAABC, AAACB, AABAC, AABCA, ...)
     but for 2xA+2xB+C : 30 anagrams.

     And so on.

     holesToColor contains all the possible permutations for 3,4, 5 and 6
     holes by card.
  */

  int nbHoles = 0;
  quint8 *choice = new quint8[6];
  quint8 *result = NULL;
  /*======================
     init for 3 holes :
  ======================*/
#ifdef DEBUG_PRINT
  cout << "init for 3 holes" << endl;
#endif

  nbHoles = 3;
  result = new quint8[3];
  /* 3 holes with 1 color : a single solution */
  holesToColor[HOLES_3][COLORS_1] = new quint8[nbHoles*1];
  // choice = AAA
  memset(holesToColor[HOLES_3][COLORS_1],1,nbHoles*1);
  nbAna[HOLES_3][COLORS_1] = 1;

  /* 3 holes with 2 color : 
     AAB or BBA, for each 3 anagrams
     => 6 in all */
  nbAna[HOLES_3][COLORS_2] = 6;

  holesToColor[HOLES_3][COLORS_2] = new quint8[nbHoles*6];
  memset(holesToColor[HOLES_3][COLORS_2],0,nbHoles*6);
  memset(choice,0,6);
  // choice = AAB
  choice[0] = 1; choice[1] = 1; choice[2] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_3][COLORS_2][0]));
  // choice = ABB
  choice[0] = 1; choice[1] = 2; choice[2] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_3][COLORS_2][nbHoles*3]));

  /* 3 holes with 3 color :
     ABC, 6 anagrams
  => 6 in all*/
  nbAna[HOLES_3][COLORS_3] = 6;

  holesToColor[HOLES_3][COLORS_3] = new quint8[nbHoles*6];
  memset(holesToColor[HOLES_3][COLORS_3],0,nbHoles*6);
  // choice = ABC
  choice[0] = 1; choice[1] = 2; choice[2] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_3][COLORS_3][0]));
  
  delete [] result;

  /*======================
     init for 4 holes :
  ======================*/
#ifdef DEBUG_PRINT
  cout << "init for 4 holes" << endl;
#endif
  memset(choice,0,6);
  nbHoles = 4;
  result = new quint8[nbHoles];
  /* 4 holes with 1 color : a single solution */
  nbAna[HOLES_4][COLORS_1] = 1;

  holesToColor[HOLES_4][COLORS_1] = new quint8[nbHoles*1];
  // choice = AAAA
  memset(holesToColor[HOLES_4][COLORS_1],1,nbHoles*1);

  /* 4 holes with 2 color : 
     AABB, with  6 anagrams
     AAAB or ABBB with 4 anagrams
     => 14 in all */
  nbAna[HOLES_4][COLORS_2] = 14;

  holesToColor[HOLES_4][COLORS_2] = new quint8[nbHoles*14];
  // choice AABB
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_2][0]));
  // choice AAAB
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_2][nbHoles*6]));
  // choice ABBB
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_2][nbHoles*10]));

  /* 4 holes with 3 colors : 
     AABC or ABBC or ABCC with  12 anagrams
     => 36 in all */
  nbAna[HOLES_4][COLORS_3] = 36;

  holesToColor[HOLES_4][COLORS_3] = new quint8[nbHoles*36];
  // choice AABC
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_3][0]));
  // choice ABBC
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_3][nbHoles*12]));
  // choice ABCC
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_3][nbHoles*24]));

  /* 4 holes with 4 colors : 
     ABCD, with  24 anagrams
     => 24 in all */
  nbAna[HOLES_4][COLORS_4] = 24;

  holesToColor[HOLES_4][COLORS_4] = new quint8[nbHoles*24];
  // choice ABCD
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_4][COLORS_4][0]));

  delete [] result;

  /*======================
     init for 5 holes :
  ======================*/
#ifdef DEBUG_PRINT
  cout << "init for 5 holes" << endl;  
#endif
  memset(choice,0,6);
  nbHoles = 5;
  result = new quint8[nbHoles];
  /* 5 holes with 1 color : a single solution */
  nbAna[HOLES_5][COLORS_1] = 1;
  holesToColor[HOLES_5][COLORS_1] = new quint8[nbHoles*1];
  // choice = AAAAA
  memset(holesToColor[HOLES_5][COLORS_1],1,nbHoles*1);

  /* 5 holes with 2 colors : 
     AAAAB or BBBBA, with 5 anagrams each
     AAABB or BBBAA, with 10 anagrams each     
     => 30 in all */
  nbAna[HOLES_5][COLORS_2] = 30;

  holesToColor[HOLES_5][COLORS_2] = new quint8[nbHoles*30];
  // choice AAAAB
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 1; choice[4] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_2][0]));
  // choice BBBBA
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2; choice[4] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_2][nbHoles*5]));
  // choice AAABB
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2; choice[4] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_2][nbHoles*10]));
  // choice BBBAA
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2; choice[4] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_2][nbHoles*20]));

  /* 5 holes with 3 colors : 
     AAABC or ABBBC or ABCCC, with 20 anagrams each
     AABBC or AABCC or ABBCC, with 30 anagrams each     
     => 150 in all */
  nbAna[HOLES_5][COLORS_3] = 150;

  holesToColor[HOLES_5][COLORS_3] = new quint8[nbHoles*150];
  // choice AAABC
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2; choice[4] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_3][0]));
  // choice ABBBC
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2; choice[4] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_3][nbHoles*20]));
  // choice ABCCC
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3; choice[4] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_3][nbHoles*40]));
  // choice AABBC
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2; choice[4] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_3][nbHoles*60]));
  // choice AABCC
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3; choice[4] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_3][nbHoles*90]));
  // choice ABBCC
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3; choice[4] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_3][nbHoles*120]));

  /* 5 holes with 4 colors : 
     AABCD or ABBCD or ABCCD or ABCDD, with 60 anagrams each
     => 240 in all */
  nbAna[HOLES_5][COLORS_4] = 240;

  holesToColor[HOLES_5][COLORS_4] = new quint8[nbHoles*240];
  // choice AABCD
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3; choice[4] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_4][0]));
  // choice ABBCD
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3; choice[4] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_4][nbHoles*60]));
  // choice ABCCD
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3; choice[4] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_4][nbHoles*120]));
  // choice ABCDD
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4; choice[4] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_4][nbHoles*180]));

  /* 5 holes with 5 colors : 
     ABCDE, with 120 anagrams each
     => 120 in all */
  nbAna[HOLES_5][COLORS_5] = 120;

  holesToColor[HOLES_5][COLORS_5] = new quint8[nbHoles*120];
  // choice ABCDE
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4; choice[4] = 5;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_5][COLORS_5][0]));

  delete [] result;

  /*======================
     init for 6 holes :
  ======================*/
#ifdef DEBUG_PRINT
  cout << "init for 6 holes" << endl;
#endif
  memset(choice,0,6);
  nbHoles = 6;
  result = new quint8[nbHoles];
  /* 6 holes with 1 color : a single solution */
  nbAna[HOLES_6][COLORS_1] = 1;
  holesToColor[HOLES_6][COLORS_1] = new quint8[nbHoles*1];
  // choice = AAAAAA
  memset(holesToColor[HOLES_6][COLORS_1],1,nbHoles*1);

  /* 6 holes with 2 colors : 
     AAABBB, with 20 anagrams each
     AAAABB or BBBBAA, with 15 anagrams each
     AAAAAB or BBBBBA, with 6 anagrams each
     => 62 in all */
  nbAna[HOLES_6][COLORS_2] = 62;

  holesToColor[HOLES_6][COLORS_2] = new quint8[nbHoles*62];
  // choice AAABBB
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2; choice[4] = 2; choice[5] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_2][0]));
  // choice AAAABB
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 1; choice[4] = 2; choice[5] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_2][nbHoles*20]));
  // choice AABBBB
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2; choice[4] = 2; choice[5] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_2][nbHoles*35]));
  // choice AAAAAB
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 1; choice[4] = 1; choice[5] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_2][nbHoles*50]));
  // choice ABBBBB
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2; choice[4] = 2; choice[5] = 2;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_2][nbHoles*56]));

  /* 6 holes with 3 colors : 
     AAABBC, AAABCC, AABBBC, ABBBCC, AABCCC, ABBCCC, with 60 anagrams each
     AAAABC, ABBBBC, ABCCCC, with 30 anagrams each
     AABBCC, with 90 anagrams each
     => 540 in all */
  nbAna[HOLES_6][COLORS_3] = 540;

  holesToColor[HOLES_6][COLORS_3] = new quint8[nbHoles*540];
  // choice AAABBC
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2; choice[4] = 2; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][0]));
  // choice AAABCC
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2; choice[4] = 3; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*60]));
  // choice AABBBC
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2; choice[4] = 2; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*120]));
  // choice ABBBBCC
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2; choice[4] = 3; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*180]));
  // choice AABCCC
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3; choice[4] = 3; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*240]));
  // choice ABBCCC
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3; choice[4] = 3; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*300]));
  // choice AAAABC
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 1; choice[4] = 2; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*360]));
  // choice ABBBBC
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2; choice[4] = 2; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*390]));
  // choice ABCCCC
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3; choice[4] = 3; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*420]));
  // choice AABBCC
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2; choice[4] = 3; choice[5] = 3;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_3][nbHoles*450]));

  /* 6 holes with 4 colors : 
     AABBCD, AABCCD, AABCDD, ABBCCD, ABBCDD, ABCCCC, with 180 anagrams each
     AAABCD, ABBBCD, ABCCCD, ABCDDD, with 120 anagrams each
     => 1560 in all */
  nbAna[HOLES_6][COLORS_4] = 1560;

  holesToColor[HOLES_6][COLORS_4] = new quint8[nbHoles*1560];
  // choice AABBCD
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 2; choice[4] = 3; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][0]));
  // choice AABCCD
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3; choice[4] = 3; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*180]));
  // choice AABCDD
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3; choice[4] = 4; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*360]));
  // choice ABBCCD
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3; choice[4] = 3; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*540]));
  // choice ABBCDD
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3; choice[4] = 4; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*720]));
  // choice ABCCDD
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3; choice[4] = 4; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*900]));
  // choice AAABCD
  choice[0] = 1; choice[1] = 1; choice[2] = 1; choice[3] = 2; choice[4] = 3; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*1080]));
  // choice ABBBCD
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 2; choice[4] = 3; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*1200]));
  // choice ABCCCD
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3; choice[4] = 3; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*1320]));
  // choice ABCDDD
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4; choice[4] = 4; choice[5] = 4;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_4][nbHoles*1440]));

  /* 6 holes with 5 colors : 
     AABCDE, ABBCDE, ABCCDE, ABCDDE, ABCDEE, with 360 anagrams each
     => 1800 in all */
  nbAna[HOLES_6][COLORS_5] = 1800;

  holesToColor[HOLES_6][COLORS_5] = new quint8[nbHoles*1800];
  // choice AABCDE
  choice[0] = 1; choice[1] = 1; choice[2] = 2; choice[3] = 3; choice[4] = 4; choice[5] = 5;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_5][0]));
  // choice ABBCDE
  choice[0] = 1; choice[1] = 2; choice[2] = 2; choice[3] = 3; choice[4] = 4; choice[5] = 5;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_5][nbHoles*360]));
  // choice ABCCDE
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 3; choice[4] = 4; choice[5] = 5;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_5][nbHoles*720]));
  // choice ABCDDE
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4; choice[4] = 4; choice[5] = 5;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_5][nbHoles*1080]));
  // choice ABCDEE
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4; choice[4] = 5; choice[5] = 5;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_5][nbHoles*1440]));

  /* 6 holes with 6 colors : 
     ABCDEF, with 720 anagrams
     => 720 in all */
  nbAna[HOLES_6][COLORS_6] = 720;

  holesToColor[HOLES_6][COLORS_6] = new quint8[nbHoles*720];
  // choice ABCDEF
  choice[0] = 1; choice[1] = 2; choice[2] = 3; choice[3] = 4; choice[4] = 5; choice[5] = 6;
  anagram(choice,nbHoles,result,nbHoles,&(holesToColor[HOLES_6][COLORS_6][0]));

  delete [] result;
  delete [] choice;
}

/* Just for testing */
#ifdef DEBUG_PRINT
void CardThreading::printAnagrams(int nbHoles) {

  int i,j,k;

  for(i=0;i<nbHoles;i++) {

    cout << "There are " << nbAna[nbHoles-3][i] << " anagrams for " << i+1 << " color(s) on " << nbHoles << " holes card" << endl;

    for(j=0;j<nbAna[nbHoles-3][i];j++) {
      
      for(k=0;k<nbHoles;k++) {
        cout << (char)('A'-1+holesToColor[nbHoles-3][i][j*nbHoles+k]);
      }
      cout << endl;
    }
  }
}
#endif

int CardThreading::anagram(quint8 *word, int length, quint8 *result, int size, quint8 *solus) {

  static int nbSol = 0;

  int i,j,k,l;
  quint8 *used = NULL;
  quint8 *copy = NULL;

  if (size == length) nbSol = 0;

  copy = (quint8 *)malloc(size*sizeof(quint8));
  if (copy == NULL) {
    nbSol = -1;
    return nbSol;
  }

  for (i = 0; i < size; i++) {
    copy[i] = word[i];
  }

  if (size > 1) {
    
    used = (quint8 *)malloc(size*sizeof(quint8));    
    if (used == NULL) {
      nbSol = -1;
      return nbSol;
    }
    memset(used,0,size);

    for (j = 0; j < size; j++) {

      if (used[j] == 0) {

        for (k = 0; k < size; k++) {
          if (copy[k] == copy[j])
            used[k] = 1;
        }
        result[length - size] = copy[j];

        for (l = 0; l < size; l++) {
          word[l] = copy[l];
        }

        word[j] = copy[0];
        word[0] = result[length - size];

        anagram(word+1,length,result, size-1, solus);
      }
    }
    free(used);
  }
  else {
    
    result[length - 1] = copy[0];
    if (solus != NULL) {
      for(i=0;i<length;i++) {
        solus[nbSol*length+i] = result[i];
      }
    }
    nbSol += 1;
  }
  free(copy);
  return nbSol;
}

