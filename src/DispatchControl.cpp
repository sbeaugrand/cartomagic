#include "DispatchControl.hpp"
#include "InsertWarpDialog.hpp"
#include "RemoveWarpDialog.hpp"

DispatchControl::DispatchControl(ProjectParams *_params, PatternGuesser *_guesser) {
  params = _params;
  guesser = _guesser;
  design = NULL;

  setParamsConnections();
}

void DispatchControl::setMainWindow(MainWindow *_mainWindow) {
  mainWindow = _mainWindow;
}

void DispatchControl::setDesign(DesignWidget *_design) {

  design = _design;
  setDesignConnections(true);
}

void DispatchControl::unsetDesign() {  
  setDesignConnections(false);
  design = NULL;
}

void DispatchControl::setDesignConnections(bool state) {

  // if state = true connect all is needed
  if (state) {

    /* signals from the band widget that must be relayed to other models */
    connect(design->band, SIGNAL(currentColorChanged(quint8)),this,SLOT(slotChangeCurrentColor(quint8)));
    connect(design->band,SIGNAL(paletteChanged(quint8,QColor)),this,SLOT(slotChangePalette(quint8,QColor)));

    /* signals from the band model that must be relayed to other models */
    connect(design->band->model,SIGNAL(warpCompleted(int)),this,SLOT(slotWarpCompleted(int)));    
    connect(design->band->model,SIGNAL(warpUncompleted(int)),this,SLOT(slotWarpUncompleted(int)));

    /* signals from the moves model that must be relayed to other models */
    connect(design->moves->model,SIGNAL(refreshMove(int)),this,SLOT(slotWarpCompleted(int)));
    connect(design->moves->model,SIGNAL(moveCleared(int)),this,SLOT(slotMoveCleared(int)));
    connect(design->moves->model,SIGNAL(moveToggled(int,int)),this,SLOT(slotMoveChanged(int,int)));
    connect(design->moves->model,SIGNAL(moveGuessed(int)),this,SLOT(slotMoveGuessed(int)));

    /* signals from the moves control that must be relayed to other models */
    connect(design->moves->control,SIGNAL(mountClicked(int)),this,SLOT(slotToggleMount(int)));
    connect(design->moves->control,SIGNAL(cycleClicked(int)),this,SLOT(slotToggleCyclic(int)));

    /* signals from the moves widget that must be relayed to other models */
    connect(design->moves,SIGNAL(allowCyclicToggled()),this,SLOT(slotToggleAllowCyclic()));
    connect(design->moves,SIGNAL(guessIdleToggled()),this,SLOT(slotToggleGuessIdle()));
    connect(design->moves,SIGNAL(refreshClicked()),this,SLOT(slotRefresh()));

    /* signals from the threading widget that must be relayed to other models */
    connect(design->threading,SIGNAL(letteringOrderChanged()), this, SLOT(slotToggleLetteringOrder()));
    connect(design->threading,SIGNAL(letteringTypeChanged()), this, SLOT(slotToggleLetteringType()));

    /* signals from the threading model that must be relayed to other models */
    connect(design->threading->model,SIGNAL(holeColorChanged(int,quint8,quint8)),this,SLOT(slotChangeHoleColor(int,quint8,quint8)));

    /* signals from the guesser that must be relayed to the moves model and threading model */
    connect(guesser,SIGNAL(threadingChanged(int,QList<quint16>&,QList<quint16>&)),design->threading->model,SLOT(updateCardThreading(int,QList<quint16>&,QList<quint16>&)));
    connect(guesser,SIGNAL(movesChanged(int,quint8*,quint8*)),design->moves->model,SLOT(updateWarpMoves(int,quint8*,quint8*)));

  }
  else {
    /* signals from the band widget that must be relayed to other models */
    disconnect(design->band, SIGNAL(currentColorChanged(quint8)),0,0);
    disconnect(design->band,SIGNAL(paletteChanged(quint8,QColor)),0,0);

    /* signals from the band model that must be relayed to other models */
    disconnect(design->band->model,SIGNAL(warpCompleted(int)),0,0);
    disconnect(design->band->model,SIGNAL(warpUncompleted(int)),0,0);

    /* signals from the moves model that must be relayed to other models */
    disconnect(design->moves->model,SIGNAL(refreshMove(int)),0,0);
    disconnect(design->moves->model,SIGNAL(moveCleared(int)),0,0);
    disconnect(design->moves->model,SIGNAL(moveToggled(int,int)),0,0);
    disconnect(design->moves->model,SIGNAL(moveGuessed(int)),0,0);

    /* signals from the moves control that must be relayed to other models */
    disconnect(design->moves->control,SIGNAL(mountClicked(int)),0,0);
    disconnect(design->moves->control,SIGNAL(cycleClicked(int)),0,0);

    /* signals from the moves widget that must be relayed to other models */
    disconnect(design->moves,SIGNAL(allowCyclicToggled()),0,0);
    disconnect(design->moves,SIGNAL(guessIdleToggled()),0,0);
    disconnect(design->moves,SIGNAL(refreshClicked()),0,0);

    /* signals from the threading widget that must be relayed to other models */
    disconnect(design->threading,SIGNAL(letteringOrderChanged()), 0,0);
    disconnect(design->threading,SIGNAL(letteringTypeChanged()), 0,0);

    /* signals from the threading model that must be relayed to other models */
    disconnect(design->threading->model,SIGNAL(holeColorChanged(int,quint8,quint8)),0,0);

    /* signals from the guesser that must be relayed to the moves model and threading model */
    disconnect(guesser,SIGNAL(threadingChanged(int,QList<quint16>&,QList<quint16>&)),0,0);
    disconnect(guesser,SIGNAL(movesChanged(int,quint8*,quint8*)),0,0);

  }
}

void DispatchControl::setParamsConnections() {

  connect(params,SIGNAL(guessWithIdlesSet()),this,SLOT(slotEnableGuessIdleMenu()));

}


/***************************************
      PRIVATE SLOTS SECTION
***************************************/

void DispatchControl::slotChangeCurrentColor(quint8 numCol) {
  params->setCurrentColor(numCol);
}

void DispatchControl::slotChangePalette(quint8 numCol, QColor c) {
  params->setPalette(numCol,c);
  design->band->updatePalette(numCol);
  design->band->view->updateView();
  design->threading->view->updateView();
}

void DispatchControl::slotEnableGuessIdleMenu() {

  if (params->guessWithIdles) {
    mainWindow->enableWarpActions(true,WARP_GUESSIDLE,OP_REM);    
  }
  else {
    mainWindow->enableWarpActions(true,WARP_GUESSIDLE,OP_ADD);
  }

  guesser->update();
}

void DispatchControl::slotWarpCompleted(int warpId) {
  guesser->autoGuessForWarp(warpId);
}

void DispatchControl::slotWarpUncompleted(int warpId) {
  design->moves->model->updateWarpMoves(warpId,NULL,NULL);
  design->threading->model->voidCardThreading(warpId);
}

void DispatchControl::slotMoveCleared(int warpId) {
  design->threading->model->voidCardThreading(warpId);
}

/* NOTE : must be called only in MOVES mode */
void DispatchControl::slotMoveChanged(int warpId, int moveId) {

  quint8 *lst;
  quint8 move;
  quint8 *thread;

  lst = design->moves->model->movesForMoveMode.at(warpId);
  thread = design->threading->model->cardsForMoveMode.at(warpId);

  move = lst[moveId];

  design->band->model->updateWarpFromMoves(warpId,lst,thread);

}

/* NOTE : must be called only in BAND mode */
void DispatchControl::slotMoveGuessed(int warpId) {

  quint8 *lst;  

  if ((params->mount).at(warpId) == S_MOUNT) {
    lst = design->moves->model->startInS.at(warpId);
  }
  else {
    lst = design->moves->model->startInZ.at(warpId);
  }
  design->band->model->updateLetteringFromMoves(warpId,lst);

}

void DispatchControl::slotToggleAllowCyclic() {

  params->toggleAllowNoCyclic();
  design->moves->model->updateForNewAllowCyclic();
}

void DispatchControl::slotToggleGuessIdle() {

  params->toggleGuessIdles();
  slotEnableGuessIdleMenu();  
}

void DispatchControl::slotRefresh() {
  design->band->model->refresh(true);
}

void DispatchControl::slotToggleCyclic(int warpId) {

  params->toggleCyclicPattern(warpId);
  design->moves->szView->updateMove(warpId);
  slotWarpCompleted(warpId);
}

void DispatchControl::slotToggleMount(int warpId) {
  params->toggleWarpMount(warpId);
  if (params->createMode == MODE_FROMBAND) {
    //design->band->model->toggleHoleLetters(warpId);
    //design->band->view->updateLettering(warpId);
    slotMoveGuessed(warpId);
    design->threading->view->updateCard(warpId);
    design->moves->view->updateMove(warpId);
    design->moves->szView->updateMove(warpId);
  }
  else {
    design->band->model->twistWarp(warpId);
    design->threading->view->updateCard(warpId);
    design->moves->szView->updateMove(warpId);
  }
}

void DispatchControl::slotToggleLetteringOrder() {
  params->toggleNumberingOrder();
  design->threading->view->updateView();
}

void DispatchControl::slotToggleLetteringType() {
  params->toggleLetteringType();
  quint32 i;
  for(i=0;i<params->bandWidth;i++) {
    slotMoveGuessed(i);
  }  
  design->threading->view->updateView();
}

void DispatchControl::slotChangeHoleColor(int warpId, quint8 holeId, quint8 color) {
  design->band->model->updateHoleColor(warpId,holeId,color);
}

/***************************************
      PUBLIC SLOTS SECTION
***************************************/
void DispatchControl::toggleCreationMode() {

  params->toggleCreationMode();
  showLetterginWarning();
  design->updateForNewCreationMode();
}

void DispatchControl::insertWarp(int index, bool sendSig) {
  params->insertWarp(index);

  design->band->model->insertWarp(index, sendSig);
  design->moves->model->insertWarp(index, sendSig);
  design->threading->model->insertCard(index, sendSig);
  if (sendSig) {
    design->band->resizeScrollArea();
    design->moves->resizeScrollArea();
    design->threading->resizeScrollArea();
  }
}

void DispatchControl::removeWarp(int index, bool sendSig) {
  params->removeWarp(index);
  design->band->model->removeWarp(index,sendSig);
  design->moves->model->removeWarp(index,sendSig);
  design->threading->model->removeCard(index,sendSig);

}

void DispatchControl::copyWarp(int warpIdSrc, int warpIdDest, bool twist) {
  design->band->model->copyWarp(warpIdSrc,warpIdDest,twist);
}

void DispatchControl::swapWarp(int warpIdSrc, int warpIdDest, bool twist) {

  params->swapWarp(warpIdSrc,warpIdDest);
  if (params->createMode == MODE_FROMMOVES) {
    if (twist) {
      // have to toggle mount in order to keep the same moves but twist on the warps
      params->toggleWarpMount(warpIdSrc);
      params->toggleWarpMount(warpIdDest);
    }
  }
  design->moves->model->swapWarp(warpIdSrc,warpIdDest);
  design->threading->model->swapCard(warpIdSrc,warpIdDest);
  /* swap on band after moves/threading so that if there is a twist, the guesser will be called
     and new moves/threading will be shown
     */
  design->band->model->swapWarp(warpIdSrc,warpIdDest,twist);
}

void DispatchControl::copyMoves(int warpIdSrc, int warpIdDest, bool twist, bool copyThreading) {

  // copy moves only if they are not void => params.isComplete is mandatory to true
  if (! design->moves->model->isVoidMove(warpIdSrc)) {
    if (twist) {
      if ( (params->mount).at(warpIdSrc) == (params->mount).at(warpIdDest)) {
        params->toggleWarpMount(warpIdDest);
      }
    }        
    design->moves->model->copyWarp(warpIdSrc,warpIdDest);

    if (copyThreading) {
      design->threading->model->copyCard(warpIdSrc,warpIdDest);
    }

    // update the band
    slotMoveChanged(warpIdDest,0);
  }
}


void DispatchControl::colorWarp(int warpId) {
  design->band->model->colorWarp(warpId);
}

void DispatchControl::rollWarp(int warpId, quint32 nbRoll, bool toDown) {
  if (params->createMode == MODE_FROMBAND) {
    design->band->model->rollWarp(warpId,nbRoll,toDown);
  }
  else if (params->createMode == MODE_FROMMOVES) {
    design->moves->model->rollWarp(warpId,nbRoll,toDown);    
  }
}

void DispatchControl::twistWarp(int warpId) {

  if (params->createMode == MODE_FROMBAND) {
    design->band->model->twistWarp(warpId);
  }
  else if (params->createMode == MODE_FROMMOVES) {
    slotToggleMount(warpId);
  }
}

void DispatchControl::setWarpLettering(int warpId, bool state) {
  bool curState = (params->isWarpHoleNumber).at(warpId);

  if (state != curState) {
    params->setWarpLettering(warpId,state);
    design->band->view->updateLettering(warpId);
  }
}

void DispatchControl::setPattern(int warpId, quint16 baseShape, bool *whichIsTwisted) {
  if (params->createMode == MODE_FROMBAND) {
    design->band->model->setPattern(warpId,baseShape,whichIsTwisted);
  }
  else if (params->createMode == MODE_FROMMOVES) {
    if (baseShape == PARA_LEFT) {
      params->setWarpMount(warpId,Z_MOUNT);
    }
    else {
      params->setWarpMount(warpId,S_MOUNT);
    }
    design->moves->model->setPattern(warpId,whichIsTwisted);
  }
}

void DispatchControl::setNbPatterns(int nbPatterns) {

  quint32 oldValue = params->nbPatterns;
  quint32 i;
  quint8 *lst;
  quint8 *thread;

  if (oldValue != nbPatterns) {
    params->setNbPatterns(nbPatterns,true);

    // add or remove some picks on the band
    design->band->model->updateForNewNbPatterns(oldValue);    

    // if moves are not void, crush with warps with one deduced from moves
    for(i=0;i<params->bandWidth;i++) {

      // translate the warp in order to have correct data in params structures
      design->band->model->translateCompleteWarp(i,false);

      // if moves are not void, crush with warps with one deduced from moves
      if (! design->moves->model->isVoidMove(i)) {        
        if ((params->mount).at(i) == S_MOUNT) {
          lst = design->moves->model->startInS.at(i);
          thread = design->threading->model->cardsInS.at(i);
        }
        else {
          lst = design->moves->model->startInZ.at(i);
          thread = design->threading->model->cardsInZ.at(i);
        }        
        design->band->model->updateWarpFromMoves(i,lst,thread,false);
      }
    }

    // update view in order to create new pathes
    design->band->view->updateView();
  }
}


void DispatchControl::setPatternLength(int patternLength) {
  quint32 oldValue = params->patternLength;
  params->setPatternLength(patternLength);

  design->moves->model->updateForNewPatternLength();
  design->band->model->updateForNewPatternLength(oldValue);
}

void DispatchControl::showLetterginWarning() {
  if ( (params->createMode == MODE_FROMMOVES) || ((params->createMode == MODE_FROMBAND) && (! params->isWarpLettering())) ){
    design->band->warnLettering->setVisible(false);
    design->band->labLettering->setVisible(false);
  }
  else {
    design->band->warnLettering->setVisible(true);
    design->band->labLettering->setVisible(true);
  }
}

void DispatchControl::repaintBand() {
  design->band->view->updateView();
  design->band->lockers->updateView();
  design->band->selectors->updateView();
}

void DispatchControl::repaintMoves() {
  design->moves->view->updateView();
  design->moves->lockers->updateView();
  design->moves->selectors->updateView();
}

void DispatchControl::repaintThreading() {
  design->threading->view->updateView();
}
