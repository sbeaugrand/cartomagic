/*-==============================================================-

file : BandControl.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __BANDCONTROL_HPP__
#define __BANDCONTROL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "BandModel.hpp"
#include "BandView.hpp"
#include "BandLockersView.hpp"
#include "BandSelectorsView.hpp"

using namespace std;
using namespace Qt;

class BandControl : public QObject {

  Q_OBJECT

public:

  BandControl(ProjectParams *_params, BandModel *_model, BandView *_view, BandLockersView *_lockers, BandSelectorsView *_selectors);

public slots:
  /*
     Slots that are called by "external" widget
  */
  void setMode(int _mode);

  void setModel(const QList<QList<quint16> *> &_warps);


private slots:

  /*
  Slots that are connected to view signals
  */
  void slotSelectPick(int warpId, int pickId, bool rightButton);

  /*
     Slots that are connected to model signals
  */
  //void warpCompleted(int warpId, quint8 *warp, int nbColor);

private:
  ProjectParams *params;
  BandModel *model;  
  BandView * view;
  BandLockersView *lockers;
  BandSelectorsView *selectors;
  int mode;

signals:
  void warpCompleted(int warpId);
  void warpUncompleted(int warpId);

  // signals that are relayed to the moves widget
  void warpSelected(int warpId);
  void allWarpsSelected();

};
#endif //__BANDCONTROL_HPP__
