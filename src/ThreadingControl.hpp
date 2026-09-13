/*-==============================================================-

file : ThreadingControl.hpp

creation date : 01/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __THREADINGCONTROL_HPP__
#define __THREADINGCONTROL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "ProjectParams.hpp"
#include "ThreadingModel.hpp"
#include "ThreadingView.hpp"

using namespace std;
using namespace Qt;

class ThreadingControl : public QObject {

  Q_OBJECT

public:

  ThreadingControl(ProjectParams *_params, ThreadingModel *_model, ThreadingView *_view);

private slots:

  void slotSelectHole(int cardId, int holeId, bool rightButton);
  /*
  Slots that are connected to view signals
  */

  /*
     Slots that are connected to model signals
  */

  /*
     Slots that are connected to params signals
  */
  void nbHolesChanged();  

private:
  ProjectParams *params;
  ThreadingModel *model;
  ThreadingView *view;

};
#endif //__THREADINGCONTROL_HPP__
