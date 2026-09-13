/*-==============================================================-

file : DispatchControl.hpp

creation date : 07/07/2011

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : an event dispatcher that relay some signals between widget, i.e. the MainWindow 
and the BandControl, the Guesser and the BandWidget, ...

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __DISPATCHCONTROL_HPP__
#define __DISPATCHCONTROL_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "MainWindow.hpp"
class MainWindow;
#include "ProjectParams.hpp"
#include "DesignWidget.hpp"
#include "PatternGuesser.hpp"

using namespace std;
using namespace Qt;

class DispatchControl : public QObject {

  Q_OBJECT

public:

  DispatchControl(ProjectParams *_params, PatternGuesser *_guesser);

  /* this method must be called after the creation of design widget */
  void setDesign(DesignWidget *_design);
  /* this method must be called after the deletion of design widget */
  void unsetDesign();
/* this method must be called after the creation of the main window */
  void setMainWindow(MainWindow *_mainWindow);

public slots:

  /* slot called by the different dialogs, executed after triggering a menu item
     In fact, they are called directly and not by a connection with a signal.
  */
  void insertWarp(int index, bool sendSig);
  void removeWarp(int index, bool sendSig);
  void copyWarp(int warpIdSrc, int warpIdDest, bool twist = false);
  void copyMoves(int warpIdSrc, int warpIdDest, bool twist = false, bool copyThreading = true);
  void swapWarp(int warpIdSrc, int warpIdDest, bool twist = false);
  void twistWarp(int warpId);
  void colorWarp(int index);
  void rollWarp(int warpId, quint32 nbRoll, bool toDown);
  void setWarpLettering(int warpId, bool state);

  void setPattern(int warpId, quint16 baseShape, bool* whichIsTwisted); // set pattern from which pick is twisted

  void setPatternLength(int patternLength);
  void setNbPatterns(int nbPatterns);

  void showLetterginWarning();

  /* slots that are called (directly in fact) by the main window */
  void toggleCreationMode();

  /* slots to force repainting the widgets, if needed */
  void repaintBand();
  void repaintMoves();
  void repaintThreading();

private slots:

  /* NOTE :
     private slots are connected by dispatcher itself, almost after design has been set
  */

  /* slots that are connected to band widget signals */
  void slotChangeCurrentColor(quint8 numCol);
  void slotChangePalette(quint8 numCol, QColor c);

  /* slots that are connected to band/moves model signals */
  void slotWarpCompleted(int warpId);
  void slotWarpUncompleted(int warpId);
  void slotMoveCleared(int warpId);
  void slotMoveChanged(int warpId, int moveId); // NB this slot does sth only in FROMMOVES mode  
  void slotMoveGuessed(int warpId); // NB this slot does sth only in FROMBAND mode

  /* slots that are connected to moves widget signals */
  void slotToggleGuessIdle();
  void slotToggleAllowCyclic();
  void slotRefresh();

  /* slots that are connected to moves control */
  void slotToggleMount(int warpId);
  void slotToggleCyclic(int warpId);

  /* slots that are connected to threading widget signals */
  void slotToggleLetteringType();
  void slotToggleLetteringOrder();

  /* slots that are connected to threading model signals */
  void slotChangeHoleColor(int warpId, quint8 holeId, quint8 color);

  /* slots that are connected to params signals */
  void slotEnableGuessIdleMenu();

private:
  MainWindow *mainWindow;
  ProjectParams *params;
  DesignWidget *design;
  PatternGuesser *guesser;

  void setDesignConnections(bool state);
  void setParamsConnections();
  
};
#endif //__DISPATCHCONTROL_HPP__
