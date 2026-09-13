/*-==============================================================-

file : PatternGuesser.hpp

creation date : 22/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : contains 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __PATTERNGUESSER_HPP__
#define __PATTERNGUESSER_HPP__

#include "BandCst.hpp"

#define MOVE(sol,pick) possibleMoves[(sol)*(patternLength)+(pick)]

#include <QtCore>

#include "ProjectParams.hpp"

using namespace std;
using namespace Qt;

class PatternGuesser : public QObject {

  Q_OBJECT

public:

  /* NB : the goal of PatternGuesser is to find the best card threading and movements 
     (Forward, Backward, Idle) to create the main pattern of a band. This pattern
     will be repeated as many times as the weaver wants. Generally, pattern are 4 or 8 picks
     long but the guesser can match any length, up to 16 (for memory allocation reasons).
     The guesser is also driven by the number of hole for each card. Theoreticcaly, it could
     determine the minimum number of holes but in practice, the weaver choose this number.
     
  */
  PatternGuesser(ProjectParams *_params);
  ~PatternGuesser();  

  /* possibleMoves : contains all possible series of card movments considering
     the shape of the picks on a warp.
     Taking account forward/backward and idle lead to a maximum of 2^patternLength 
     possibilities : in some case of 2 following pick, there is a single choice and not 2.
   */
  quint8 *possibleMoves; // array of bandWidth*nbPossMoves quint8
  /*
    moveQuality : f/b counts for 1 and idle for 10.
   */
  quint16 *movesQuality; // array of nbPossMoves quint16
  /*
    possibleThreading : each cell refers to an index in the appropriate CardThreading array
   */
  QList<quint16> *possibleThreadings; // array of nbPossMoves quint16

  bool isVoid;  

private:

  ProjectParams *params;

  /* NB : the 3 following parameters are simply copied from params value.
     It is necessary since guesser allocates memory taking account these values.
     thus if params value change, freeing memory should be done with old values.
     */
  //int bandWidth;
  quint32 patternLength;
  quint32 bandLength;
  quint32 nbHoles;
  quint32 nbPossMoves;

  /*
These two array are used to copy the bests solution because of
forcedGuessIdel() calls : a temporary possibleMovesarray is created
that must be cleared with a call to update. Nevertheless, the update call
may clear the memory BEFORE the moves have been copied by MOvesWidget and
ThreadingWidget => a copy is done before

*/
  quint8* bestMovesForS ; // copy of the best move for S mount
  quint8* bestMovesForZ; // copy of the best move for Z mount

  void clear();

  int setWarpPossibleMoves(quint8 *warp);
  int possibleMovesWithoutIdles(quint8 *warp);
  int possibleMovesWithIdles(quint8 *warp);


  void setPossibleMovesQuality();
  void setPossibleMovesCardSide();

  void clearPossibleThreadings();

  int testThreadings(int warpId, quint8 *warp, int nbColor);

  bool possibleMoveFromLast(quint8 currShape, quint8 lastPick, quint8 *pos1, quint8 *pos2);

  void printPossibleThreading(quint16 threading); // nbcolor is in the solution

  int findBestSSolution();

  int findBestZSolution();

  void printPossibleMoves();

  quint32 nbIdlePossibilities(int nbIdles, int length);

public slots:
  void update();

  /* NB : autoGuessForWarp() takes an array of quint8 as an entry.
     But BandWidget manipulates QList<quint8>, thus there must be
     an translation between the two methods.
     Furthermore, the quint8 of BandWidget contains id of color in the
     params palette. But the guesser takes id from 1 to nb colors on the warp.
     Thus, there is also a translation of colr id.

     If withIdle = true, the idle "moves" will be taken account for card threading
     discover. If false, a simple solution with just F or B is computed which is straight
     forward from picks shapes.

    */
  int autoGuessForWarp(int warpId);

  /*
    forcedGuessForWarp() is called via a dialogwidget called from the menu
    It forces the search of idles for the warpId warp. Since the number of idles
    may be as long as the number of F movments, it may be very long and merely impossible
    for very long patterns. Thus, the method uses a limiter on nbIdles. Furthermore, it
    may decides to limit itself this number if it leads to too much possibilities.
    */
  int forcedGuessForWarp(int warpId, int maxIdles);

signals:
  void solutionFound();
  /* signal to the MovesWidget */
  void movesChanged(int warpId,quint8* movesForS, quint8* movesForZ);
  /* signal to the ThreadingWidget */
  void threadingChanged(int warpId,QList<quint16> & lstThreadingForS, QList<quint16> & lstThreadingForZ);
};


#endif // __PATTERNGUESSER_HPP__
