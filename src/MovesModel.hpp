/*-==============================================================-

file : MovesModel.hpp

creation date : 22/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __MOVESMODEL_HPP__
#define __MOVESMODEL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandCst.hpp"
#include "ProjectParams.hpp"

#define FOR_S 0
#define FOR_Z 1
#define FOR_MOVES 2


using namespace std;
using namespace Qt;

class MovesModel : public QObject {

  Q_OBJECT

public:
  MovesModel(ProjectParams *_params);
  ~MovesModel();

  /*
    startInS and startInZ elements are quint8* array that are copied from
    solutions found by guesser. They could be only referenced since the
    guesser->possibleMoves pointer to the solution is passed as
    a signal parameter and will be valid until it changes. But, since the solutions
    are always found for an S mount, the Z mount moves must be computed and stored somewhere
    Finally, it is more practical to also copy the S solution.
    */
  QList<quint8 *> startInS;
  QList<quint8 *> startInZ;  

  /*
    movesForMoveMode contains quint8* arrays that are copied from startInS or startInZ, depending
    on the card mount, when the creation mode changes from FROMBAND to FROMMOVES.
    In this mode, it is used by the view to display moves and idles    
    */
  QList<quint8 *> movesForMoveMode;

  QList<bool> selectedMoves;
  bool allSelected;
  QList<bool> totallyLockedMoves;


private:
  ProjectParams *params;
  void clear();
  void setCardSide(int warpId, int forWhich); // forWhich = 0 : set card side for S and = 1 : for Z, and = 2 : for moves

public slots:
  void removeWarp(int warpId, bool sendSig = false);
  void insertWarp(int warpId, bool sendSig = false);
  void copyWarp(int warpIdSrc, int warpIdDest);
  void swapWarp(int warpId1, int warpId2);
  void rollWarp(int warpId, quint32 nbRoll, bool toDown);

  void setPattern(int warpId, bool* fb); // set pattern from a serie of F/B moves

  void updateForNewAllowCyclic();
  void updateForNewGuessIdle();
  void updateForNewCreationMode();

  void updateForNewPatternLength();
  void updateWarpMoves(int warpId, quint8* movesForS, quint8* movesForZ);  

  void refreshMoves();

  void selectMove(int warpId);
  void selectAllMoves();
  int nbSelected();
  void totalLockMove(int warpId);

  bool isVoidMove(int warpId);

  /* slots connected to control when view is clicked */
  void toggleMove(int warpId, int moveId);
  void putIdle(int warpId, int moveId);

  QRect draw(QPainter *p); // the band is supposed to take all the place
  //QRect drawSides(QPainter *p); // the band is supposed to take all the place

signals:
  // signals for the view
  void moveChanged(int warpId);
  void allMovesChanged();
  /* signal for the control that will relay it
     to the guesser
  */
  void refreshMove(int warpId);
  void moveCleared(int warpId);

  void selectionChanged(int warpId);
  void selectionAllChanged();
  void lockChanged(int warpId);

  // singal to the control, relayed to band model
  void moveToggled(int warpId, int moveId);  
  void moveGuessed(int warpId);

  friend QDataStream &operator<<(QDataStream &out, const MovesModel &m);
  friend QDataStream &operator>>(QDataStream &in, MovesModel &m);


};
#endif // __MOVESMODEL_HPP__
