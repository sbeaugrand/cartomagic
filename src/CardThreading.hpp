/*-==============================================================-

file : CardThreading.hpp

creation date : 22/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : contains 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __CARDTHREADING_HPP__
#define __CARDTHREADING_HPP__

/* for holesToColor array */
#define HOLES_3 0
#define HOLES_4 1
#define HOLES_5 2
#define HOLES_6 3

#define COLORS_1 0 // means that there is 1 color for all holes
#define COLORS_2 1 // means that there is 2 colors for all holes
#define COLORS_3 2 // means that there is 3 colors for all holes
#define COLORS_4 3 // means that there is 4 colors for all holes
#define COLORS_5 4 // means that there is 5 colors for all holes
#define COLORS_6 5 // means that there is 6 colors for all holes

#include <QtCore>
#include "BandCst.hpp"

using namespace std;
using namespace Qt;

class CardThreading {

public:

  CardThreading();
  ~CardThreading();

  /* tables that give all the possibilities to 
     thread the cards with 1, 2,... colors 
     NB : each quint8* table is a pseudo 2D array,
     with x by nbHole cells. x depends on the number
     of colors and holes. For example, for 3 colors, the table
     is 36*4 long.

     CAUTION : colors are numbered from 1 to 6
  */
  quint8 *** holesToColor;

  /* nbAna contains the size of each quint8* array */
  quint32 ** nbAna;

  /* dualPick() takes a pick movement and find the dual.
     For example it transforms SB to ZF
    */
  quint8 dualPick(quint8 pick);

  /*Just for testing */
#ifdef DEBUG
  void printAnagrams(int nbHoles);
#endif
  /* anagram() : recusive method that computes all anagrams from
     a word given in word. The initial call should pass the whole word in
     word parameter along its size in length. The result parameter should point
     to a pre-allocated array of length char, and size should be equal to length.

     If solus != NULL, the successive solutions are stored in solus

     this method returns the number of anagrams or -1 if something bad happens.

     BEWARE : word is modified during the call.
   */
  int anagram(quint8 *word, int length, quint8 *result, int size, quint8 *solus = NULL);
private:

  /* 
     initHolesToColor() : initialize the associated array for cards
     with 3, 4, 5, and 6 holes.
   */
  void initHolesToColor();

};


#endif // __CARDTHREADING_HPP__
