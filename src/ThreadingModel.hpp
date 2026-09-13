/*-==============================================================-

file : ThreadingModel.hpp

creation date : 22/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __THREADINGMODEL_HPP__
#define __THREADINGMODEL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "BandCst.hpp"
#include "ProjectParams.hpp"

using namespace std;
using namespace Qt;

class ThreadingModel : public QObject {

  Q_OBJECT

public:
  ThreadingModel(ProjectParams *_params);
  ~ThreadingModel();

  QList<quint8 *> cardsInS; // the hole colors of each card if mounted in S
  QList<quint8 *> cardsInZ; // the hole colors of each card if mounted in Z
  QList<quint8 *> cardsForMoveMode;

private:
  ProjectParams *params;
  void clear();

  bool compareSColors(int cardId, quint16 threading); // compare current cardInS colors to the ones given by threading
  bool compareZColors(int cardId, quint16 threading);

public slots:
  void removeCard(int cardId, bool sendSig = false);
  void insertCard(int cardId, bool sendSig = false);
  void copyCard(int cardIdSrc, int cardIdDest);
  void swapCard(int cardIdSrc, int cardIdDest);

  void updateForNewNbHoles();  
  void updateForNewCreationMode();

  void updateCardThreading(int cardId, QList<quint16> & lstThreadingForS, QList<quint16> & lstThreadingForZ);
  void voidCardThreading(int cardId);

  void updateHoleColor(int cardId, quint8 holeId);

  QRect draw(QPainter *p); // the band is supposed to take all the place

signals:
  // signals for the view
  void threadingChanged(int cardId);
  void holeColorChanged(int cardId, quint8 holeId, quint8 color);
  void allThreadingsChanged();

  friend QDataStream &operator<<(QDataStream &out, const ThreadingModel &t);
  friend QDataStream &operator>>(QDataStream &in, ThreadingModel &t);

};
#endif // __THREADING_HPP__
