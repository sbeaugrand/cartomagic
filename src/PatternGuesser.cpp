/*-==============================================================-

file : PatternGuesser.cpp

creation date : 25/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : contains 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/

#include "PatternGuesser.hpp"
#include <iostream>

#ifdef DEBUG_PRINT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

double uclock() {
  double time;
  struct timeval t;

  gettimeofday(&t,NULL);
  time = 1.0e6 * (double)t.tv_sec + (double)t.tv_usec;
  return(time);
}
#endif

PatternGuesser::PatternGuesser(ProjectParams *_params) : QObject() {

  params = _params;
  isVoid = true;

  nbPossMoves = 0;
  bestMovesForS = new quint8[100];
  bestMovesForZ = new quint8[100];

  possibleMoves = NULL;
  movesQuality = NULL;
  possibleThreadings = NULL;    

}

PatternGuesser::~PatternGuesser() {
  clear();
  delete [] bestMovesForS;
  delete [] bestMovesForZ;
}

void PatternGuesser::clear() {

  delete [] possibleMoves;
  delete [] movesQuality;
  delete [] possibleThreadings;
  isVoid = true;
}

void PatternGuesser::update() {

#ifdef DEBUG_PRINT
  cout << "updating PatternGuesser" << endl;
#endif
  if (!isVoid) {
    clear();
  }

  patternLength = params->patternLength;
  bandLength = params->bandLength-1; // get the real bandLength not taking account the first pick that never changes
  nbHoles = params->nbHoles;
  
  if (params->guessWithIdles) {
    nbPossMoves = 3*(1<<(patternLength-1));
  }
  else {
    nbPossMoves = 2;
  }

  possibleMoves = new quint8[patternLength*nbPossMoves];
  movesQuality = new quint16[nbPossMoves];
  possibleThreadings = new QList<quint16>[nbPossMoves];

  isVoid = false;
}

int PatternGuesser::forcedGuessForWarp(int warpId, int maxIdles) {

  quint8 *warp = (params->completedWarps).at(warpId);
  int nbColor = (params->nbColors).at(warpId);

  quint32 i,j,k;
  quint32 nbIdles = maxIdles;
  /* retrieve moves without idles : MOVE(0,...) are mounted in S
     and MOVE(1,... ) are in Z. Note that an idle always correspond
     to a forward movment. Thus a backward movment cannot be replaced
     by an idle
     */

  possibleMovesWithoutIdles(warp);  


  quint8 *saveMoves = new quint8[2*patternLength];
  for(i=0;i<2*patternLength;i++) {
    saveMoves[i] = possibleMoves[i];
  }

  if (nbIdles > (patternLength/2)) nbIdles = patternLength/2;

  quint32 nbPoss;
  quint32 nbSPoss,nbZPoss;
  quint32 allocAna;
  quint8* anagrams = NULL;
  quint8* wordS = NULL;
  quint8* resultS = NULL;
  quint8* wordZ = NULL;
  quint8* resultZ = NULL;
  int idMinS = -1,idMinZ = -1;

  bool stop = false;
#ifdef DEBUG_PRINT
  double t;
#endif

  for(i=1;(i<=nbIdles) && (!stop);i++) {

    nbSPoss = nbIdlePossibilities(i,patternLength);
    nbZPoss = nbSPoss;
    nbPoss = nbSPoss+nbZPoss;

    // test if the allocation for sol is not too big.
    if ((nbPoss > 0) && (nbPoss*patternLength < (quint64)(50*1024*1024))) {
#ifdef DEBUG_PRINT
      cout << "searching for " << i << " idles" << endl;

      t = uclock();
#endif
      allocAna = nbPoss*patternLength;
      anagrams = new quint8[allocAna];

      // clear array to contain possibilities
      clear();
      nbPossMoves = nbPoss;
      possibleMoves = new quint8[patternLength*nbPossMoves];
      movesQuality = new quint16[nbPossMoves];
      possibleThreadings = new QList<quint16>[nbPossMoves];
      isVoid = false;


      wordS = new quint8[patternLength];
      resultS = new quint8[patternLength];
      memset(wordS,0,patternLength);
      memset(resultS,0,patternLength);
      for(j=0;j<patternLength;j++) {
        if (j<i) wordS[j] = 128 | (IDLE << 2);
        else wordS[j] = 128;
      }
      params->threadPerm->anagram(wordS,patternLength,resultS,patternLength,anagrams);
      delete [] wordS;
      delete [] resultS;

      for(j=0;j<nbSPoss;j++) {
        memcpy(&(MOVE(j,0)),saveMoves,patternLength);
      }

      // finally transpose anagrams
      for(j=0;j<nbSPoss;j++) {
        for(k=0;k<patternLength;k++) {
          // change nothing if selected anagrams[] = 128, else set MOVE bits to IDLE
          MOVE(j,k) = MOVE(j,k) | (anagrams[j*patternLength+k] & 127);
        }
      }
#ifdef DEBUG_PRINT
      printPossibleMoves();
#endif


      wordZ = new quint8[patternLength];
      resultZ = new quint8[patternLength];
      // secondly generates anagrams for Z
      memset(wordZ,0,patternLength);
      memset(resultZ,0,patternLength);
      for(j=0;j<patternLength;j++) {
        /*
          if (j<i) wordZ[j] = 128  | ZI;
          else wordZ[j] = 128 | ZF;
          */
        if (j<i) wordZ[j] = 128  | (IDLE << 2);
        else wordZ[j] = 128;
      }
      params->threadPerm->anagram(wordZ,patternLength,resultZ,patternLength,&(anagrams[nbSPoss*patternLength]));
      delete [] wordZ;
      delete [] resultZ;

      for(j=0;j<nbZPoss;j++) {
        memcpy(&(MOVE(nbSPoss+j,0)),&(saveMoves[patternLength]),patternLength);
      }

      // finally transpose anagrams
      for(j=0;j<nbZPoss;j++) {
        for(k=0;k<patternLength;k++) {
          // change nothing if selected anagrams[] = 128, else set MOVE bits to IDLE
          MOVE(nbSPoss+j,k) = MOVE(nbSPoss+j,k) | (anagrams[nbSPoss*patternLength+ j*patternLength + k] & 127);
        }
      }
#ifdef DEBUG_PRINT
      printPossibleMoves();
#endif

      delete [] anagrams;
      setPossibleMovesQuality();
      setPossibleMovesCardSide();

      clearPossibleThreadings();
      testThreadings(warpId, warp,nbColor);

      idMinS = findBestSSolution();
      idMinZ = findBestZSolution();

#ifdef DEBUG_PRINT
      t = uclock() - t;

      cout << "for " << i << " idles : solutions explored in " << t*1.0e-6 << "s" << endl;
#endif

#ifdef DEBUG_PRINT
      int i,j;
      quint8 *allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];
      quint8 m;
      if (idMinS != -1) {

        cout << "The best solution for S mount is : ";
        i = (possibleThreadings[idMinS]).at(0);
        for(j=0;j<nbHoles;j++) {
          cout << (char)('A'-1+allPoss[i*nbHoles+j]);
        }
        cout << ", for moves : ";
        for(j=0;j<patternLength;j++) {
          m = MOVE(idMinS,j);
          if (PICK_MOUNT(m) == S_MOUNT) cout << "S";
          else cout << "Z";
          if (PICK_TWIST(m) == TWIST) cout << "T";
          if (PICK_MOVE(m) == FORWARD) cout << "F,";
          else if (PICK_MOVE(m) == BACKWARD) cout << "B,";
          else cout << "I,";
        }
        cout << endl;
      }

      if (idMinZ != -1) {

        cout << "The best solution for Z mount is : ";
        i = (possibleThreadings[idMinZ]).at(0);
        for(j=0;j<nbHoles;j++) {
          cout << (char)('A'-1+allPoss[i*nbHoles+j]);
        }
        cout << ", for moves : ";
        for(j=0;j<patternLength;j++) {
          m = MOVE(idMinZ,j);
          if (PICK_MOUNT(m) == S_MOUNT) cout << "S";
          else cout << "Z";
          if (PICK_TWIST(m) == TWIST) cout << "T";
          if (PICK_MOVE(m) == FORWARD) cout << "F,";
          else if (PICK_MOVE(m) == BACKWARD) cout << "B,";
          else cout << "I,";
        }
        cout << endl;
      }
#endif

      if ((idMinS != -1) || (idMinZ != -1)) stop = true;
    }
  }

  delete [] saveMoves;


  quint8* movesForS = NULL;
  quint8* movesForZ = NULL;
  QList<quint16> lstForS;
  QList<quint16> lstForZ;

  if (idMinS != -1) {
    memcpy(bestMovesForS,&(MOVE(idMinS,0)),patternLength);
    movesForS = bestMovesForS;
    lstForS = possibleThreadings[idMinS];

    if (idMinZ == -1) {
      params->setWarpMount(warpId,S_MOUNT);
    }
  }
  if (idMinZ != -1) {
    memcpy(bestMovesForZ,&(MOVE(idMinZ,0)),patternLength);
    movesForZ = bestMovesForZ;
    lstForZ = possibleThreadings[idMinZ];

    if (idMinS == -1) {
      params->setWarpMount(warpId,Z_MOUNT);
    }
  }

  // emit signal to the MovesView
  emit movesChanged(warpId,movesForS,movesForZ);
  // emit signal to ThreadingView
  emit threadingChanged(warpId,lstForS,lstForZ);

  /*
    PB: can call update() now since best solutions have been copied
  */
  update();

  return 0;

}

quint32 PatternGuesser::nbIdlePossibilities(int nbIdles, int length) {

  if (nbIdles >= 20 ) {
    return 0;
  }
  int i;
  quint64 den = 1;
  for(i=1;i<=nbIdles;i++) {
    den = den*i;
  }
  quint64 num = 1;
  for(i=length;i>(length-nbIdles);i--) {
    num = num*i;
  }
  return (quint32)(num/den);
}

int PatternGuesser::autoGuessForWarp(int warpId) {

#ifdef DEBUG_PRINT
  double t = uclock();
#endif

  quint8 *warp = (params->completedWarps).at(warpId);
  int nbColor = (params->nbColors).at(warpId);
  int nbMaxColor = (params->nbMaxColors).at(warpId);

  setWarpPossibleMoves(warp);
  setPossibleMovesQuality();
  setPossibleMovesCardSide();

#ifdef DEBUG_PRINT
  printPossibleMoves();
#endif

  clearPossibleThreadings();
  int col;
  for(col = nbColor;col<=nbMaxColor;col++) {
    testThreadings(warpId, warp,col);
  }
  //testThreadings(warpId, warp,params->nbHoles);

#ifdef DEBUG_PRINT
  t = uclock() - t;

  cout << "found solutions in " << t*1.0e-6 << "s" << endl;
#endif

#ifdef DEBUG_PRINT
  printPossibleThreadings(nbColor);
#endif

  int idMinS,idMinZ;

  idMinS = findBestSSolution();
  idMinZ = findBestZSolution();

#ifdef DEBUG_PRINT
  int i,j;
  quint8 *allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];
  quint8 m;
  if (idMinS != -1) {

    cout << "The best solution for S mount is : ";
    i = (possibleThreadings[idMinS]).at(0);
    for(j=0;j<nbHoles;j++) {
      cout << (char)('A'-1+allPoss[i*nbHoles+j]);
    }
    cout << ", for moves : ";
    for(j=0;j<patternLength;j++) {
      m = MOVE(idMinS,j);
      if (PICK_MOUNT(m) == S_MOUNT) cout << "S";
      else cout << "Z";
      if (PICK_TWIST(m) == TWIST) cout << "T";
      if (PICK_MOVE(m) == FORWARD) cout << "F,";
      else if (PICK_MOVE(m) == BACKWARD) cout << "B,";
      else cout << "I,";
    }
    cout << endl;
  }

  if (idMinZ != -1) {

    cout << "The best solution for Z mount is : ";
    i = (possibleThreadings[idMinZ]).at(0);
    for(j=0;j<nbHoles;j++) {
      cout << (char)('A'-1+allPoss[i*nbHoles+j]);
    }
    cout << ", for moves : ";
    for(j=0;j<patternLength;j++) {
      m = MOVE(idMinZ,j);
      if (PICK_MOUNT(m) == S_MOUNT) cout << "S";
      else cout << "Z";
      if (PICK_TWIST(m) == TWIST) cout << "T";
      if (PICK_MOVE(m) == FORWARD) cout << "F,";
      else if (PICK_MOVE(m) == BACKWARD) cout << "B,";
      else cout << "I,";
    }
    cout << endl;
  }
#endif

  quint8* movesForS = NULL;
  quint8* movesForZ = NULL;
  QList<quint16> lstForS;
  QList<quint16> lstForZ;

  if (idMinS != -1) {
    movesForS = &(MOVE(idMinS,0));
    lstForS = possibleThreadings[idMinS];

#ifdef DEBUG_PRINT
    cout << "there are " << (int)(lstForS.size()) << " possible threadings in S" << endl;

    for(int aa=0;aa<lstForS.size();aa++) {
      printPossibleThreading(lstForS.at(aa));
    }
#endif

    if (idMinZ == -1) {
      params->setWarpMount(warpId,S_MOUNT);
    }
  }
  if (idMinZ != -1) {
    movesForZ = &(MOVE(idMinZ,0));
    lstForZ = possibleThreadings[idMinZ];

    if (idMinS == -1) {
      params->setWarpMount(warpId,Z_MOUNT);
    }
  }

  // emit signal to the MovesView
  emit movesChanged(warpId,movesForS,movesForZ);
  // emit signal to ThreadingView
  emit threadingChanged(warpId,lstForS,lstForZ);

  return 0;
}

int PatternGuesser::setWarpPossibleMoves(quint8 *warp) {

  if (params->guessWithIdles) {
    possibleMovesWithIdles(warp);
  }
  else {
    possibleMovesWithoutIdles(warp);
  }
  return 0;
}

int PatternGuesser::possibleMovesWithoutIdles(quint8 *warp) {
  quint32 i;

  memset(possibleMoves,0,patternLength*nbPossMoves);
  for(i=0;i<patternLength;i++) {
    if ( (PICK_SHAPE(warp[i]) == PARA_RIGHT) ||
         (PICK_SHAPE(warp[i]) == TRI_RIGHT) ) {
      MOVE(0,i) = SF;
      MOVE(1,i) = ZB;
    }
    else if ((PICK_SHAPE(warp[i]) == PARA_LEFT) ||
             (PICK_SHAPE(warp[i]) == TRI_LEFT)) {
      MOVE(0,i) = SB;
      MOVE(1,i) = ZF;
    }
  }
  return 0;
}

int PatternGuesser::possibleMovesWithIdles(quint8 *warp) {

  quint32 i,j,k;
  quint8 pos1,pos2,pos3;

  memset(possibleMoves,0,patternLength*nbPossMoves);

  /* starts from the first pick.
     the last pick can only lead to 2 possible moves, depending on the shape of the pick.
   */

  if ( (PICK_SHAPE(warp[0]) == PARA_RIGHT) ||
       (PICK_SHAPE(warp[0]) == TRI_RIGHT) ) {
    pos1 = SF;
    pos2 = ZB;
    pos3 = SI;
  }
  else if ((PICK_SHAPE(warp[0]) == PARA_LEFT) ||
           (PICK_SHAPE(warp[0]) == TRI_LEFT)) {
    pos1 = SB;
    pos2 = ZF;
    pos3 = ZI;
  }
  else {
    cerr << "Cannot determine the orientation of the following pick" << endl;
    return -1;
  }

  quint32 gap = 1<<(patternLength-1); // the number of times the current solution is copied in solutions

  for(i=0;i<gap;i++) {
    MOVE(i,0) = pos1;
  }
  for(i=0;i<gap;i++) {
    MOVE(i+gap,0) = pos2;
  }
  for(i=0;i<gap;i++) {
    MOVE(i+2*gap,0) = pos3;
  }

  gap = gap/2; 
  quint32 nbSolForLastPick = 3; // the number of solutions for last pick, i.e. the i-1

  quint8 lastPick;
  bool ok;

  for(i=1;i<patternLength;i++) {

    for(j=0;j<nbSolForLastPick;j++) {

      lastPick = MOVE(j*gap*2,i-1);
      ok = possibleMoveFromLast(warp[i],lastPick,&pos1,&pos2);

      for(k=0;k<gap;k++) {
        MOVE(j*gap*2 + k,i) = pos1;
      }
      for(k=0;k<gap;k++) {
        MOVE(j*gap*2 + gap + k,i) = pos2;
      }
    }
    gap = gap/2;
    nbSolForLastPick = nbSolForLastPick*2;
  }

  return 0;
}


void PatternGuesser::setPossibleMovesQuality() {

  quint32 i,j;
  quint8 mCur;
  quint16 idleQual = 50;

  /* NB : there is no need to memset the movesQuality arrays
     since it is completly updated at each call
     */
  for(i=0;i<nbPossMoves;i++) {

    idleQual = 50;
    movesQuality[i] = 0;

    for(j=0;j<patternLength;j++) {

      mCur = MOVE(i,j);
      if (PICK_MOVE(mCur) == IDLE) {
        movesQuality[i] += idleQual;
        idleQual += 50;
      }
      else {
        movesQuality[i] += 1;
        idleQual = 50;
      }
    }
  }
}

void PatternGuesser::setPossibleMovesCardSide() {

  quint32 j,k;

  quint8 frontHole = 0; // the number of the upper-front hole
  quint8 backHole = nbHoles-1; // the number of the  upper-back hole
  quint8 tmp;
  quint8 side = 0;

  bool isTwisted = false;

  quint8 move;

  for(j=0;j<nbPossMoves;j++) {

    frontHole = 0;
    backHole = nbHoles-1;    
    isTwisted = false;

    for (k=0;k<patternLength;k++) {

      move = MOVE(j,k);

      // no twist before curretn move
      if (PICK_TWIST(move) == NO_TWIST) {

        // no twist before
        if (!isTwisted) {
          if (PICK_MOVE(move) == FORWARD) {
            frontHole = (frontHole-1+nbHoles)%nbHoles;
            backHole = (backHole-1+nbHoles)%nbHoles;
          }
          else if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (frontHole+1)%nbHoles;
            backHole = (backHole+1)%nbHoles;
          }
          // else if IDLE : no changes
        }        
        // twist before
        else {        
          if (PICK_MOVE(move) == FORWARD) {
            frontHole = (frontHole+1)%nbHoles;
            backHole = (backHole+1)%nbHoles;
          }
          else if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (frontHole-1+nbHoles)%nbHoles;
            backHole = (backHole-1+nbHoles)%nbHoles;
          }
          // else if IDLE : no changes
        }
      }
      // twist before current move
      else {

        // no twist before
        if (!isTwisted) {
          if (PICK_MOVE(move) == FORWARD) {
            backHole = (frontHole+1)%nbHoles;
          }
          else if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (backHole-1+nbHoles)%nbHoles;
          }
          else {
            tmp = frontHole;
            frontHole = backHole;
            backHole = tmp;
          }
        }
        // twist before
        else {
          if (PICK_MOVE(move) == FORWARD) {
            backHole = (frontHole-1+nbHoles)%nbHoles;
          }
          else if (PICK_MOVE(move) == BACKWARD) {
            frontHole = (backHole+1)%nbHoles;
          }
          else {
            tmp = frontHole;
            frontHole = backHole;
            backHole = tmp;
          }
        }
        isTwisted = !isTwisted;
      }

      if (backHole == frontHole+1) {
        side = backHole;
      }
      else if (frontHole == backHole+1) {
        side = frontHole;
      }
      else {
        side = nbHoles;
      }

      MOVE(j,k) = (move & NOSIDE_MASK) | (side << SIDE_STEP);
    }
  }
}

void PatternGuesser::clearPossibleThreadings() {

  quint32 i;
  for(i=0;i<nbPossMoves;i++) {
    (possibleThreadings[i]).clear();
  }
}

int PatternGuesser::testThreadings(int warpId, quint8 *warp, int nbColor) {

  quint8 *allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];
  quint32 nbPoss = ((params->threadPerm)->nbAna)[nbHoles-3][nbColor-1];

#ifdef DEBUG_PRINT
  cout << "there are " << nbPoss << " possible card threadings for this config" << endl;
#endif

  quint32 j,i,k,endLength;
  bool stop;

  quint8 frontHole = 0; // the number of the upper-front hole
  quint8 backHole = nbHoles-1; // the number of the  upper-back hole
  quint8 wovenHole = 0;
  quint8 lastWovenHole = 0; // to take account for idles.
  quint8 wovenColor = 1;

  quint8 move;  
  quint16 solus;

  // upper loop : on all possibilities of movments
  for(i=0;i<nbPossMoves;i++) {

    // inner loop : on all possibilities of threading
    for(j=0;j<nbPoss;j++) {

      frontHole = 0;
      backHole = nbHoles-1;
      wovenHole = frontHole;
      lastWovenHole = frontHole;

#ifdef DEBUG_PRINT
      cout << "n°" << i << " : ";
#endif

      if ((params->cyclicCardPosition).at(warpId) == CYCLIC) {
        endLength = patternLength;
      }
      else {
        endLength = bandLength;        
      }
      stop = false;
      k = 0;
      while ((!stop) && (k < endLength)) {
        move = MOVE(i,(k % patternLength));

#ifdef DEBUG_PRINT
        cout << "[" << (int)frontHole << "," << (int)backHole << "]->";

        if (PICK_MOUNT(move) == S_MOUNT) cout << "S";
        else cout << "Z";
        if (PICK_TWIST(move) == TWIST) cout << "T";
        if (PICK_MOVE(move) == FORWARD) cout << "F->";
        else if (PICK_MOVE(move) == BACKWARD) cout << "B->";
        else cout << "I->";
#endif

        if (PICK_MOVE(move) == FORWARD) {
          frontHole = (frontHole-1+nbHoles)%nbHoles;
          backHole = (backHole-1+nbHoles)%nbHoles;
        }
        else if (PICK_MOVE(move) == BACKWARD) {
          frontHole = (frontHole+1)%nbHoles;
          backHole = (backHole+1)%nbHoles;
        }

        if (PICK_MOVE(move) == BACKWARD) {
          wovenHole = backHole;
        }
        else if (PICK_MOVE(move) == FORWARD) {
          wovenHole = frontHole;
        }
        else if (PICK_MOVE(move) == IDLE) {
          wovenHole = lastWovenHole;
        }        
        wovenColor = allPoss[j*nbHoles+wovenHole];
        lastWovenHole = wovenHole;

#ifdef DEBUG_PRINT
        cout << "[" << (int)frontHole << "," << (int)backHole << "] :" << "WH=" << (int)wovenHole << ", WC=" << (int)wovenColor << " | ";
#endif

        if (wovenColor != PICK_COLOR(warp[k])) {
          stop = true;
        }
        else {
          k += 1;
        }
      } // while !stop & k<patternLength
#ifdef DEBUG_PRINT
      cout << endl;
#endif
      if (!stop) {
#ifdef DEBUG_PRINT
        cout << "found a solution !" << endl;
#endif

        // if pattern is cyclic, test if last position is the same than first
        if ((params->cyclicCardPosition).at(warpId) == CYCLIC) {
          if ((frontHole == 0) && (backHole == (nbHoles-1))) {
#ifdef DEBUG_PRINT
            cout << "for moves n°" << i << ", card finishes in start position" << endl;
#endif
            /* ugly but simple : put nbcolor in solution, in ordre that thread model
               can compare with actual solution
               */

            solus = (quint16)j | ( (quint16)nbColor << 13);
            (possibleThreadings[i]).append(solus);
          }
#ifdef DEBUG_PRINT
          else {
            cout << "for moves n°" << i << ", card finishes in [" << (int)frontHole << "," << (int)backHole << "]" << endl;
          }
#endif
        }
        else {                    
          solus = (quint16)j | ( (quint16)nbColor << 13);
          (possibleThreadings[i]).append(solus);
        }
      }
    }
  }
  return 0;
}

void PatternGuesser::printPossibleThreading(quint16 threading) {

  int nbColor = (threading & 57344) >> 13;
  threading = threading & 8191;

  quint8 *allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];

  quint32 i;

  for(i=0;i<nbHoles;i++) {
    cout << (char)('A'-1+allPoss[threading*nbHoles+i]);
  }
  cout << endl;
}

int PatternGuesser::findBestSSolution() {

  quint32 i;
  int min = 1<<30;
  int idMin = -1;
  quint8 pick;  

  for(i=0;i<nbPossMoves;i++) {
    pick = MOVE(i,0);
    if ((PICK_MOUNT(pick) == S_MOUNT) && ((possibleThreadings[i]).isEmpty() == false)) {
      if (movesQuality[i] < min) {
        min = movesQuality[i];
        idMin = i;
      }
    }
  }  
  return idMin;
}

int PatternGuesser::findBestZSolution() {

  quint32 i;
  int min = 1<<30;
  int idMin = -1;
  quint8 pick;

  for(i=0;i<nbPossMoves;i++) {
    pick = MOVE(i,0);
    if ((PICK_MOUNT(pick) == Z_MOUNT) && ((possibleThreadings[i]).isEmpty() == false)) {
      if (movesQuality[i] < min) {
        min = movesQuality[i];
        idMin = i;
      }
    }
  }
  return idMin;
}


void PatternGuesser::printPossibleMoves() {

  quint32 i,j;
  quint8 m;

  for(i=0;i<nbPossMoves;i++) {
    cout << "n°" << i << " : ";
    for(j=0;j<patternLength;j++) {
      m = MOVE(i,j);
      if (PICK_MOUNT(m) == S_MOUNT) cout << "S";
      else cout << "Z";
      if (PICK_TWIST(m) == TWIST) cout << "T";
      if (PICK_MOVE(m) == FORWARD) cout << "F, ";
      else if (PICK_MOVE(m) == BACKWARD) cout << "B, ";
      else if (PICK_MOVE(m) == IDLE) cout << "I, ";
    }
    cout << endl;
  }
}

bool PatternGuesser::possibleMoveFromLast(quint8 currShape, quint8 lastPick, quint8 *pos1, quint8 *pos2) {

  bool ret = true;

  if (PICK_MOUNT(lastPick) == S_MOUNT) {
    if ((PICK_SHAPE(currShape) == PARA_RIGHT) || (PICK_SHAPE(currShape) == TRI_RIGHT)){
      *pos1 = SF;
      *pos2 = SI;
    }
    else if ((PICK_SHAPE(currShape) == PARA_LEFT) || (PICK_SHAPE(currShape) == TRI_LEFT)) {
      *pos1 = SB;
      /* IMPORTANT :
         guntram seems to be wrong assuming that a S mount with two xxx_left can't occur
         with the second being an idle.
         */
      //*pos2 = SB;
      *pos2 = SI;
    }
    else {
      cerr << "Cannot determine the orientation of the following pick" << endl;
      ret = false;
    }
  }
  else if (PICK_MOUNT(lastPick) == Z_MOUNT) {

    if ((PICK_SHAPE(currShape) == PARA_RIGHT) || (PICK_SHAPE(currShape) == TRI_RIGHT)){
      *pos1 = ZB;
      /* IMPORTANT :
         guntram seems to be wrong assuming that a Z mount with two xxx_right can't occur
         with the second being an idle.
         */
      //*pos2 = ZB;
      *pos2 = ZI;
    }
    else if ((PICK_SHAPE(currShape) == PARA_LEFT) || (PICK_SHAPE(currShape) == TRI_LEFT)) {
      *pos1 = ZF;
      *pos2 = ZI;      
    }
    else {
      cerr << "Cannot determine the orientation of the following pick" << endl;
      ret = false;
    }
  }
  return ret;
}



