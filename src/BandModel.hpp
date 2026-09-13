/*-==============================================================-

file : BandModel.hpp

creation date : 27/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDMODEL_HPP__
#define __BANDMODEL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandCst.hpp"
#include "ProjectParams.hpp"

using namespace std;
using namespace Qt;

class BandModel : public QObject {

  Q_OBJECT

public:

  BandModel(ProjectParams *_params);
  ~BandModel();

  /* each QList<quint16> represents a warp. The first element
     of this QList is a void pick that will be always void and
     cannot be selected for changing its color/shape
   */

  QList<QList<quint16> *> warps;
  QList<quint16> nbColoredPicks;
  QList<bool> selectedWarps;
  bool allSelected;
  QList<bool> totallyLockedWarps;
  QList<bool> shapeLockedWarps;

private:
  ProjectParams *params;
  void clear();  
  void colorPickCyclicOrFree(int warpId, int pickId, bool justTwisted = false);
  void colorPickHole(int warpId, int pickId, bool justTwisted = false);
  void checkLastPick(int warpId); // check if last picks are in correct position for files saved vs <0.3

public slots:
  
  void clearWarp(int warpId);
  void removeWarp(int warpId, bool sendSig = false);
  void insertWarp(int warpId, bool sendSig = false);
  void swapWarp(int warpId1, int warpId2, bool twist = false);
  void copyWarp(int warpIdSrc, int warpIdDest, bool twist = false);
  void colorWarp(int warpId);
  void twistWarp(int warpId);
  void setPattern(int warpId, quint16 baseShape, bool* whichIsTwisted); // set pattern from which pick is twisted
  void rollWarp(int warpId, quint32 nbRoll, bool toDown);

  /* to set the model when importing a gtt file */
  void setModel(const QList<QList<quint16> *> &_warps);

  void selectWarp(int warpId);
  void selectAllWarps();
  int nbSelected();
  void totalLockWarp(int warpId);
  void shapeLockWarp(int warpId);

  void twistPick(int warpId, int pickId);  

  void colorPick(int warpId, int pickId, bool justTwisted = false);

  void insertPicksAfter(int warpId, int pos, int nb );
  void removePicksAfter(int warpId, int pos, int nb );

  void findHoleLettersWithoutIdles(int warpId);
  void updateWarpFromMoves(int warpId, quint8 *moves, quint8 *thread, bool sendSig = true);
  void updateLetteringFromMoves(int warpId, quint8 *moves);
  void updateHoleColor(int warpId, quint8 numHole, quint8 color);

  void refresh(bool guess);
  void translateCompleteWarp(int warpId, bool sendSig = true);

  /* these two methods try to recreate a coherent band in case of
     patternLength or nbPatterns changed. The principle is to take
     picks from 1 to patternLength and to duplicate them nbPatterns
     times.
  */
  void updateForNewPatternLength(quint32 oldValue);
  void updateForNewNbPatterns(quint32 oldValue); // just add or remove some picks. No signal emitted

  QRect draw(QPainter *p); // the band is supposed to take all the place

signals:
  void warpChanged(int warpId);
  void letteringChanged(int warpId);
  void warpCompleted(int warpId);
  void warpUncompleted(int warpId);
  void allWarpsChanged();
  void selectionChanged(int warpId);
  void selectionAllChanged();
  void lockChanged(int warpId);


  friend QDataStream &operator<<(QDataStream &out, const BandModel &b);
  friend QDataStream &operator>>(QDataStream &in, BandModel &b);
};

#endif //__BANDMODEL_HPP__ 
