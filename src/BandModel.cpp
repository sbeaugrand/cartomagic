/*-==============================================================-

file : BandModel.cpp

creation date : 27/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include <QProgressDialog>
#include "BandModel.hpp"

BandModel::BandModel(ProjectParams *_params) : QObject() {

  params = _params;  

  QList<quint16> *lst;

  quint32 i,j;
  quint16 numHole;

  for(i=0;i<(params->bandWidth);i++) {
    lst = new QList<quint16>();

    for(j=0;j<(params->bandLength);j++) {
      if (j==0) {
        lst->append(VOID);
      }
      else {
        numHole = params->nbHoles-1 - ((j-1) % params->nbHoles);
        lst->append(numHole << HOLE_STEP);
      }
    }
    warps.append(lst);


    nbColoredPicks.append(0);
    selectedWarps.append(false);
    totallyLockedWarps.append(false);
    shapeLockedWarps.append(false);
  }
  allSelected = false;

}

BandModel::~BandModel() {

  clear();
}


void BandModel::clear() {

  QListIterator<QList<quint16> *> iter(warps);
  QList<quint16> *lst;

  while (iter.hasNext()) {
    lst = iter.next();
    lst->clear();
    delete lst;
  }
  selectedWarps.clear();
  totallyLockedWarps.clear();
  shapeLockedWarps.clear();
  nbColoredPicks.clear();

}


void BandModel::clearWarp(int warpId) {

  QList<quint16> *lst;
  quint32 i;

  lst = warps.at(warpId);
  for(i=0;i<(params->bandLength);i++) {
    lst->replace(i,VOID);
  }
  selectedWarps.replace(warpId,false);
  totallyLockedWarps.replace(warpId,false);
  shapeLockedWarps.replace(warpId,false);
  nbColoredPicks.replace(warpId,0);

  // signal to the view
  emit warpChanged(warpId);
}

void BandModel::selectWarp(int warpId) {
  bool sel = selectedWarps.at(warpId);
  sel = !sel;
  selectedWarps.replace(warpId,sel);

  // signal to the view
  emit selectionChanged(warpId);
}

void BandModel::selectAllWarps() {

  quint32 i;

  if (allSelected == false) {
    for(i=0;i<params->bandWidth;i++) {
      if (totallyLockedWarps.at(i) || shapeLockedWarps.at(i)) {
        selectedWarps.replace(i,false);
      }
      else {
        selectedWarps.replace(i,true);
      }
    }
    allSelected = true;
  }
  else {
    for(i=0;i<params->bandWidth;i++) {
      selectedWarps.replace(i,false);
    }
    allSelected = false;
  }
  // signal to the view
  emit selectionAllChanged();
}

int BandModel::nbSelected() {
  quint32 i;
  int nb = 0;

  for(i=0;i<params->bandWidth;i++) {
    if (selectedWarps.at(i) == true) {
      nb += 1;
    }
  }
  return nb;
}

void BandModel::totalLockWarp(int warpId) {
  bool lock = totallyLockedWarps.at(warpId);
  lock = !lock;
  totallyLockedWarps.replace(warpId,lock);

  //force deselection and shapeLock if locked
  if (lock) {
    selectedWarps.replace(warpId,false);
    shapeLockedWarps.replace(warpId,true);
  }

  // signal to the view
  emit lockChanged(warpId);
  emit selectionChanged(warpId);
}

void BandModel::shapeLockWarp(int warpId) {

  // do something only if the allLock is false

  if (! totallyLockedWarps.at(warpId)) {
    bool lock = shapeLockedWarps.at(warpId);
    lock = !lock;
    shapeLockedWarps.replace(warpId,lock);

    //force deselection if locked
    if (lock) {
      selectedWarps.replace(warpId,false);
    }

    // signal to the view
    emit lockChanged(warpId);
    emit selectionChanged(warpId);
  }
}

void BandModel::insertWarp(int warpId, bool sendSig) {

  QList<quint16> *lst;
  quint32 i;

  lst = new QList<quint16>();
  for(i=0;i<(params->bandLength);i++) {
    lst->append(0);
  }
  warps.insert(warpId,lst);
  selectedWarps.insert(warpId,false);
  totallyLockedWarps.insert(warpId,false);
  shapeLockedWarps.insert(warpId,false);
  nbColoredPicks.insert(warpId,0);

  // NB : setting params->bandWidth is done in insertWarpDialog

  if (sendSig) {
    emit allWarpsChanged();
  }

}

void BandModel::removeWarp(int warpId, bool sendSig) {
  QList<quint16> *lst;

  lst = warps.takeAt(warpId);
  lst->clear();
  delete lst;
  selectedWarps.removeAt(warpId);
  totallyLockedWarps.removeAt(warpId);
  shapeLockedWarps.removeAt(warpId);
  nbColoredPicks.removeAt(warpId);

  // NB : setting params->bandWidth is done in insertWarpDialog
  if (sendSig) {
    emit allWarpsChanged();
  }

}


void BandModel::swapWarp(int warpId1, int warpId2, bool twist) {

  warps.swap(warpId1, warpId2);
  selectedWarps.swap(warpId1, warpId2);
  totallyLockedWarps.swap(warpId1, warpId2);
  shapeLockedWarps.swap(warpId1, warpId2);
  nbColoredPicks.swap(warpId1, warpId2);

  if (twist) {
    twistWarp(warpId1);
    twistWarp(warpId2);

    // NB : twistWarp will inform the view to change
  }
  else {
    // signal to the view
    emit warpChanged(warpId1);
    // signal to the view
    emit warpChanged(warpId2);
  }
}

/* NOTE :
   This slot must be only called by the dispatch controller, that tests
   if the creation mode is FROMBAND. If it is not the case, this slot should never
   be called.
   */
void BandModel::copyWarp(int warpIdSrc, int warpIdDest, bool twist) {

  quint32 i;
  QList<quint16> *lstSrc;
  QList<quint16> *lstDest;
  bool uncomplete = false;

  lstSrc = warps.at(warpIdSrc);
  lstDest = warps.at(warpIdDest);

  lstDest->clear();

  for(i=0;i<(params->bandLength);i++) {
    lstDest->append(lstSrc->at(i));
  }

  if ( (nbColoredPicks.at(warpIdDest) == (params->bandLength)-1) && (nbColoredPicks.at(warpIdSrc) < (params->bandLength)-1) ) {
    uncomplete = true;
  }
  nbColoredPicks.replace(warpIdDest,nbColoredPicks.at(warpIdSrc));

  if (twist) {
    twistWarp(warpIdDest);
    /* NB : if the copied warp is completed, then the dest warp is also completed
       thus, twistWarp() will call translateCompleteWarp(). But, if the dest warp
       was completed and the src is not, the dest warp must be signaled uncompleted
    */
    if (uncomplete) {
      (params->isWarpComplete).replace(warpIdDest,false);
      emit warpUncompleted(warpIdDest);
    }
  }
  else {
    if (uncomplete) {
      (params->isWarpComplete).replace(warpIdDest,false);
      emit warpUncompleted(warpIdDest);
    }
    else {
      translateCompleteWarp(warpIdDest);
    }

    // signal to the view
    emit warpChanged(warpIdDest);
  }
}

void BandModel::colorWarp(int warpId) {
  quint32 i;
  QList<quint16> *lst;
  lst = warps.at(warpId);
  quint16 currPick;

  quint8 color;
  color = (params->currColor)+1;

  for(i=1;i<params->bandLength;i++) {
    currPick = lst->at(i);
    currPick = (currPick & NOCOLOR_MASK) | (color & COLOR_MASK);
    lst->replace(i,currPick);
  }

  emit warpChanged(warpId);

  nbColoredPicks.replace(warpId,(params->bandLength)-1);
  translateCompleteWarp(warpId);
}

/* twistWarp(warpId) : twist the shape of all picks of the warp n° warpId

   NOTES :
    - In BAND mode, this method is called when the user use the twist or
   the copy-and-twist functions.
   - In MOVES mode, this method is called when toggeling moutn or swapping in mirror mode
   In this case, the warps translation is useless


   */
void BandModel::twistWarp(int warpId) {
  quint32 i;
  QList<quint16> *lst;
  lst = warps.at(warpId);
  quint16 currPick;
  quint16 shape = PARA_RIGHT;

  for(i=0;i<params->bandLength;i++) {
    currPick = lst->at(i);
    if (PICK_SHAPE(currPick) == PARA_LEFT) {
      shape = PARA_RIGHT<<SHAPE_STEP;
    }
    else if (PICK_SHAPE(currPick) == PARA_RIGHT) {
      shape = PARA_LEFT<<SHAPE_STEP;
    }
    else if (PICK_SHAPE(currPick) == TRI_LEFT) {
      shape = TRI_RIGHT<<SHAPE_STEP;
    }
    else if (PICK_SHAPE(currPick) == TRI_RIGHT) {
      shape = TRI_LEFT<<SHAPE_STEP;
    }
    currPick = shape | (currPick & NOSHAPE_MASK);
    lst->replace(i,currPick);
  }

  /*
  if (params->createMode == MODE_FROMBAND) {
    findHoleLettersWithoutIdles(warpId);
  }
  */

  emit warpChanged(warpId);

  if (params->createMode == MODE_FROMBAND) {
    translateCompleteWarp(warpId);
  }  
}

/* NB this slots reset all colors of warp to VOID
   baseShape determines the shape of the pick n° 0, not taking
   account if pick n°1 is twisted. It can be PARA_LEFT or PARA_RIGHT.

   => if baseShape is PARA_RIGHT and pick n°1 is not twisted (=XXX_RIGHT), nothing happens
   but if baseShape is PARA_RIGHT and n°1 is twisted (=XXX_LEFT), pick n°0 becomes PARA_LEFT
   */
void BandModel::setPattern(int warpId, quint16 baseShape, bool *whichIsTwisted) {

  quint32 i;
  QList<quint16> *lst;
  lst = warps.at(warpId);
  int idInWhich = 0;
  bool nextTwisted = false;

  // starting with last pick of the band
  if (whichIsTwisted[params->patternLength-1]) {
    nextTwisted = true;
    if (baseShape == PARA_RIGHT)  {
      lst->replace(params->bandLength-1,PARA_LEFT<<SHAPE_STEP);
    }
    else if (baseShape == PARA_LEFT)  {      
      lst->replace(params->bandLength-1,PARA_RIGHT<<SHAPE_STEP);
    }
    else {
      cerr << "twistPattern() : impossible case on last pick" << endl;
    }
  }
  else {
    nextTwisted = false;
    lst->replace(params->bandLength-1,baseShape<<SHAPE_STEP);
  }

  idInWhich = params->patternLength-2;
  for(i=params->bandLength-2;i>=1;i--) {
#ifdef DEBUG_PRINT
    cout << "pick n°" << i << " : " << whichIsTwisted[idInWhich] << endl;
#endif
    // next one is twisted
    if (nextTwisted) {

      // next and current are twisted => PARA_XXX
      if (whichIsTwisted[idInWhich]) {
        if (baseShape == PARA_RIGHT) {
          lst->replace(i,PARA_LEFT<<SHAPE_STEP);
        }
        else if (baseShape == PARA_LEFT) {
          lst->replace(i,PARA_RIGHT<<SHAPE_STEP);
        }
        else {
          cerr << "twistPattern() : impossible case on pick n°" << i << endl;
        }
      }
      // next is twisted but not current => TRI_XXX
      else {
        if (baseShape == PARA_RIGHT) {
          lst->replace(i,TRI_RIGHT<<SHAPE_STEP);
        }
        else if (baseShape == PARA_LEFT) {
          lst->replace(i,TRI_LEFT<<SHAPE_STEP);
        }
        else {
          cerr << "twistPattern() : impossible case on pick n°" << i << endl;
        }
      }
    }
    // next one is not twisted
    else {
      // next is not twisted and current is => TRI_XXX
      if (whichIsTwisted[idInWhich]) {
        if (baseShape == PARA_RIGHT) {
          lst->replace(i,TRI_LEFT<<SHAPE_STEP);
        }
        else if (baseShape == PARA_LEFT) {
          lst->replace(i,TRI_RIGHT<<SHAPE_STEP);
        }
        else {
          cerr << "twistPattern() : impossible case on pick n°" << i << endl;
        }
      }
      // next and cur are not twisted => PARA_XXX
      else {
        if (baseShape == PARA_RIGHT) {
          lst->replace(i,PARA_RIGHT<<SHAPE_STEP);
        }
        else if (baseShape == PARA_LEFT) {
          lst->replace(i,PARA_LEFT<<SHAPE_STEP);
        }
        else {
          cerr << "twistPattern() : impossible case on pick n°" << i << endl;
        }
      }
    }

    nextTwisted = whichIsTwisted[idInWhich];
    idInWhich = (idInWhich-1+params->patternLength)%params->patternLength;
  }

  if ((baseShape == PARA_RIGHT) && (whichIsTwisted[0])) {
    lst->replace(0,PARA_LEFT<<SHAPE_STEP);
  }
  else if ((baseShape == PARA_LEFT) && (whichIsTwisted[0])) {
    lst->replace(0,PARA_RIGHT<<SHAPE_STEP);
  }
  else {
    lst->replace(0,baseShape<<SHAPE_STEP);
  }

  checkLastPick(warpId); // change last pick if needed
  findHoleLettersWithoutIdles(warpId);
  nbColoredPicks.replace(warpId,0);
  (params->isWarpComplete).replace(warpId,false);
  emit warpUncompleted(warpId);

  emit warpChanged(warpId);
}

void BandModel::rollWarp(int warpId, quint32 nbRoll, bool toDown) {
  quint32 i,j;
  QList<quint16> *lst;
  lst = warps.at(warpId);
  quint16* rolled;
  int index;

  if (nbRoll < params->patternLength) {

    rolled = new quint16[params->patternLength];
    for(i=0;i<params->patternLength;i++) {
      if (toDown) {
        index = (i-nbRoll+params->patternLength)%params->patternLength;
      }
      else {
        index = (i+nbRoll)%params->patternLength;
      }
      rolled[index] = lst->at(i+1);
    }

    // plenty of special cases when nb of patterns = 1
    /*
    if (params->nbPatterns == 1) {

      // in case of pick  right and pick 1 left
      if ( ((PICK_SHAPE(rolled[0]) == PARA_RIGHT) || (PICK_SHAPE(rolled[0]) == TRI_RIGHT)) &&
           ((PICK_SHAPE(rolled[1]) == PARA_LEFT) || (PICK_SHAPE(rolled[1]) == TRI_LEFT)) ) {
        rolled[0] = (rolled[0] & NOSHAPE_MASK) | (TRI_RIGHT<<SHAPE_STEP);
      }
      // else the dual case
      else if ( ((PICK_SHAPE(rolled[1]) == PARA_RIGHT) || (PICK_SHAPE(rolled[1]) == TRI_RIGHT)) &&
                ((PICK_SHAPE(rolled[0]) == PARA_LEFT) || (PICK_SHAPE(rolled[0]) == TRI_LEFT)) ) {
        rolled[0] = (rolled[0] & NOSHAPE_MASK) | (TRI_LEFT<<SHAPE_STEP);
      }
    }
    */
    if ((PICK_SHAPE(rolled[0]) == PARA_RIGHT) || (PICK_SHAPE(rolled[0]) == TRI_RIGHT)) {
      lst->replace(0,PARA_RIGHT<<SHAPE_STEP);
    }
    else if ((PICK_SHAPE(rolled[0]) == PARA_LEFT) || (PICK_SHAPE(rolled[0]) == TRI_LEFT)) {
      lst->replace(0,PARA_LEFT<<SHAPE_STEP);
    }

    for(i=0;i<params->nbPatterns;i++) {
      for(j=0;j<params->patternLength;j++) {
        lst->replace(1+i*params->patternLength+j,rolled[j]);
      }
    }

    delete [] rolled;

    findHoleLettersWithoutIdles(warpId);
    emit warpChanged(warpId);

    translateCompleteWarp(warpId);
  }
}

void BandModel::colorPick(int warpId, int pickId, bool justTwisted) {

  if (params->coloringStyle == COLORING_HOLE) {
    colorPickHole(warpId,pickId,justTwisted);
  }
  else {
    colorPickCyclicOrFree(warpId,pickId,justTwisted);
  }
}

void BandModel::colorPickHole(int warpId, int pickId, bool justTwisted) {

  quint32 i;
  bool changed = false;
  QList<quint16> *lst;
  lst = warps.at(warpId);

  quint16 currPick;
  quint16 currHole;
  quint16 pickIdHole;
  bool *tabPick = new bool[params->bandLength];

  currPick = lst->at(pickId);
  if ((params->mount).at(warpId) == S_MOUNT) {
    pickIdHole = PICK_HOLE(currPick);
  }
  else {
    pickIdHole = params->nbHoles-1-PICK_HOLE(currPick);
  }

  /* IMPORTANT :
     in order to colorize all pick with the same hole letter, we just have to
     search for the same pick associated to this hole. Indeed, even if there are
     some tirangles below or upper the current pick, they are always associated to
     the same hole
     */

  tabPick[0] = false;
  for(i=1;i<params->bandLength;i++) {

    currPick = lst->at(i);
    if ((params->mount).at(warpId) == S_MOUNT) {
      currHole = PICK_HOLE(currPick);
    }
    else {
      currHole = params->nbHoles-1-PICK_HOLE(currPick);
    }

    if (currHole == pickIdHole) {
      tabPick[i] = true;
    }
    else {
      tabPick[i] = false;
    }
  }

  quint8 color;
  if (justTwisted) {
    color = PICK_COLOR(lst->at(pickId));
  }
  else {
    color = (params->currColor)+1;
  }

  // changing colors
  quint8 nbNonVoid = 0;
  for(i=1;i<params->bandLength;i++) {

    currPick = lst->at(i);
    if (tabPick[i]) {
      // first, changing color if needed
      if (PICK_COLOR(currPick) != color) {
        currPick = (currPick & NOCOLOR_MASK) | (color & COLOR_MASK);
        lst->replace(i,currPick);
        changed = true;
      }
      // then testing if it's void or not
      if (PICK_COLOR(currPick) != VOID) {
        nbNonVoid += 1;
      }
    }
    else if (PICK_COLOR(currPick) != VOID) {
      nbNonVoid += 1;
    }
  }

  delete [] tabPick;

  nbColoredPicks.replace(warpId,nbNonVoid);
  translateCompleteWarp(warpId);

  if ((changed) || (justTwisted)) {
    // signal to the view
    emit warpChanged(warpId);
  }
}

void BandModel::colorPickCyclicOrFree(int warpId, int pickId, bool justTwisted) {

  quint32 i,j,k;
  quint32 searchEnd;
  int id;
  bool changed = false;
  QList<quint16> *lst;
  lst = warps.at(warpId);
  quint16 currPick;
  int *tabPick = new int[params->bandLength];


  /* IMPORTANT :
     each time a pick is selected to be colorized, there may be some
     preceding and following pick that have a triangle shape. Then, all
     must be colorized with the current color.

     This process is done cyclically over all the band or not, depending
     on the coloring style
     */

  for(i=0;i<params->bandLength;i++) {
    tabPick[i] = 0;
  }

  quint8 refId; /* the reference pick id : may be the chosen pick
                 or th ecorresponding pick in the first pattern */
  if (params->coloringStyle == COLORING_CYCLIC) {

    refId = 1+((pickId-1)%(params->patternLength));
    searchEnd = params->nbPatterns;
  }
  else {
    refId = pickId;
    searchEnd = 1;
  }

  for(i=0;i<searchEnd;i++) {

    tabPick[refId] += 1;
#ifdef DEBUG_PRINT
    cout << "adding pick n°" << (int)refId << " for change" << endl;
#endif
    // detecting triangles below current pick
    j = refId-1;
    bool stop = false;
    while ((j>0) && (!stop)) {
      currPick = lst->at(j);
      if ((PICK_SHAPE(currPick) == TRI_LEFT) || (PICK_SHAPE(currPick) == TRI_RIGHT)) {

        if (params->coloringStyle == COLORING_CYCLIC) {
          // adding pick at j and all the corresponding pick in all patterns
          id = 1+((j-1)%(params->patternLength));
          for(k=0;k<params->nbPatterns;k++) {
            tabPick[id+k*(params->patternLength)] += 1;
#ifdef DEBUG_PRINT
            cout << "adidng pick n°" << id << " for change" << endl;
#endif
          }
        }
        else {
          // just add pick n° j
          tabPick[j] += 1;
        }
        j = j-1;
      }
      else {
        stop = true;
      }
    }
    // detecting triangles above current pick if it is a triangle itself
    currPick = lst->at(refId);
    if ((PICK_SHAPE(currPick) == TRI_LEFT) || (PICK_SHAPE(currPick) == TRI_RIGHT)) {
      j = refId+1;
      stop = false;
      while ((j<(params->bandLength)) && (!stop)) {
        currPick = lst->at(j);
        if ((PICK_SHAPE(currPick) == TRI_LEFT) || (PICK_SHAPE(currPick) == TRI_RIGHT)) {

          if (params->coloringStyle == COLORING_CYCLIC) {
            // adding pick at j and all the corresponding pick in all patterns
            id = 1+((j-1)%(params->patternLength));
            for(k=0;k<params->nbPatterns;k++) {
              tabPick[id+k*(params->patternLength)] += 1;
#ifdef DEBUG_PRINT
              cout << "adding pick n°" << id << " for change" << endl;
#endif
            }
          }
          else {
            tabPick[j] += 1;
          }
          j = j+1;
        }
        else {
          stop = true;
        }
      }
      // if end of band not reached, also add the first pick that is not a triangle
      if (stop) {
        if (params->coloringStyle == COLORING_CYCLIC) {
          // adding pick at j and all the corresponding pick in all patterns
          id = 1+((j-1)%(params->patternLength));
          for(k=0;k<params->nbPatterns;k++) {
            tabPick[id+k*(params->patternLength)] += 1;
#ifdef DEBUG_PRINT
            cout << "adding pick n°" << id << " for change" << endl;
#endif
          }
        }
        else {
          tabPick[j] += 1;
        }
      }
    }
    refId += (params->patternLength);
  }

  quint8 color;
  if (justTwisted) {
    color = PICK_COLOR(lst->at(pickId));
#ifdef DEBUG_PRINT
    cout << "twist -> taking color of curr pick n°" << pickId << " = " << (int)color << endl;
#endif
  }
  else {
    color = (params->currColor)+1;
  }

  quint8 nbNonVoid = 0;

  for(i=1;i<params->bandLength;i++) {

    currPick = lst->at(i);    
    if (tabPick[i] > 0) {      
      // first, changing color if needed
      if (PICK_COLOR(currPick) != color) {
        currPick = (currPick & NOCOLOR_MASK) | (color & COLOR_MASK);
        lst->replace(i,currPick);
        changed = true;
      }
      // then testing if it's void or not
      if (PICK_COLOR(currPick) != VOID) {
        nbNonVoid += 1;
      }
    }
    else if (PICK_COLOR(currPick) != VOID) {
      nbNonVoid += 1;
    }
  }

  delete [] tabPick;

  nbColoredPicks.replace(warpId,nbNonVoid);
  translateCompleteWarp(warpId);

  if ((changed) || (justTwisted)) {
    // signal to the view
    emit warpChanged(warpId);
  }
}

/* NB : this method does not call an update on the view
  since a twist may lead to color changes. Instead, il calls
  colorPick() with the 3rd parameter to true, to force the
  update in case of there are no changes in colors */

void BandModel::twistPick(int warpId, int pickId) {

  quint32 i;
  quint16 lastPick;
  quint16 currPick;
  quint8 refId,refIdLast;


  QList<quint16> *lst;
  lst = warps.at(warpId);

  /* NB : in any case, pick n°0 cannot be picked
     but can be changed by twists on pick n°1.
     Thus, there is always a 'last pick"
  */
  refId = 1+((pickId-1)%(params->patternLength));
  refIdLast = refId-1;

  for(i=0;i<params->nbPatterns;i++) {

    currPick = lst->at(refId);
    lastPick = lst->at(refIdLast);

    /* special case pick n°1 is selected :
        - change pick n° 0
        - change the last pick of the band -> set lastPick to bandLength
    */
    if (refId == 1) {
      if (PICK_SHAPE(currPick) == PARA_RIGHT) {        
        //currPick = (currPick & COLOR_MASK) | (TRI_LEFT<< SHAPE_STEP);
        lastPick = PARA_LEFT<< SHAPE_STEP;
      }
      else if (PICK_SHAPE(currPick) == PARA_LEFT) {
        //currPick = (currPick & COLOR_MASK) | (TRI_RIGHT<< SHAPE_STEP);
        lastPick = PARA_RIGHT<< SHAPE_STEP;
      }
      else if (PICK_SHAPE(currPick) == TRI_RIGHT) {
        //currPick = (currPick & COLOR_MASK) | (PARA_LEFT<< SHAPE_STEP);
        lastPick = PARA_LEFT<< SHAPE_STEP;
      }
      else if (PICK_SHAPE(currPick) == TRI_LEFT) {
        //currPick = (currPick & COLOR_MASK) | (PARA_RIGHT<< SHAPE_STEP);
        lastPick = PARA_RIGHT<< SHAPE_STEP;
      }
      lst->replace(0,lastPick);

      // now change lastPick to the last pick id in the band
      refIdLast = params->bandLength - 1;
      lastPick = lst->at(refIdLast);
    }

    /* 8 possible cases, just change shape, color will be changed
       by calling colorPick */

    if ( (PICK_SHAPE(lastPick) == PARA_RIGHT) &&
         (PICK_SHAPE(currPick) == PARA_RIGHT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (TRI_LEFT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (TRI_RIGHT<< SHAPE_STEP);
    }
    else if ( (PICK_SHAPE(lastPick) == PARA_LEFT) &&
              (PICK_SHAPE(currPick) == PARA_LEFT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (TRI_RIGHT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (TRI_LEFT<< SHAPE_STEP);
    }
    // the opposite of the 2 preceding
    else if ( (PICK_SHAPE(lastPick) == TRI_RIGHT) &&
              (PICK_SHAPE(currPick) == TRI_LEFT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (PARA_RIGHT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_RIGHT<< SHAPE_STEP);
    }
    else if ( (PICK_SHAPE(lastPick) == TRI_LEFT) &&
              (PICK_SHAPE(currPick) == TRI_RIGHT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (PARA_LEFT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_LEFT<< SHAPE_STEP);
    }

    else if ( (PICK_SHAPE(lastPick) == TRI_RIGHT) &&
              (PICK_SHAPE(currPick) == PARA_LEFT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (TRI_RIGHT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_RIGHT<< SHAPE_STEP);
    }
    else if ( (PICK_SHAPE(lastPick) == TRI_LEFT) &&
              (PICK_SHAPE(currPick) == PARA_RIGHT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (TRI_LEFT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_LEFT<< SHAPE_STEP);
    }
    // the opposite of the 2 preceding
    else if ( (PICK_SHAPE(lastPick) == PARA_RIGHT) &&
              (PICK_SHAPE(currPick) == TRI_RIGHT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (PARA_LEFT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (TRI_RIGHT<< SHAPE_STEP);
    }
    else if ( (PICK_SHAPE(lastPick) == PARA_LEFT) &&
              (PICK_SHAPE(currPick) == TRI_LEFT) ) {

      currPick = (currPick & NOSHAPE_MASK) | (PARA_RIGHT<< SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (TRI_LEFT<< SHAPE_STEP);
    }
    lst->replace(refId,currPick);
    lst->replace(refIdLast,lastPick);

    refId += (params->patternLength);
    refIdLast = refId-1;
  }

  if (params->createMode == MODE_FROMBAND) {
    findHoleLettersWithoutIdles(warpId);
    colorPick(warpId,pickId,true);
  }
}

void BandModel::checkLastPick(int warpId) {

  /* CAUTION : this function always find letters for an S mount first pick
    */
  QList<quint16> *lst = warps.at(warpId);
  quint16 firstPick,lastPick,pick;
  quint32 i;

  /* simple case : several patterns : just take
     the pick id = patternLength and assign its shape to
     the last pick
     */
  if (params->nbPatterns > 1) {
    lastPick = lst->at(params->bandLength-1);
    pick = lst->at(params->patternLength);
    lastPick = (lastPick & NOSHAPE_MASK) | (PICK_SHAPE(pick) << SHAPE_STEP);
    lst->replace(params->bandLength-1,lastPick);
  }
  else {

    /* have to check the first pick shape */
    firstPick = lst->at(1);
    lastPick = lst->at(params->bandLength-1);

    if ( (PICK_SHAPE(firstPick) == PARA_RIGHT) || (PICK_SHAPE(firstPick) == TRI_RIGHT)) {

      if ( (PICK_SHAPE(lastPick) == PARA_RIGHT) || (PICK_SHAPE(lastPick) == TRI_RIGHT)) {
        lastPick = (lastPick & NOSHAPE_MASK) | (PARA_RIGHT << SHAPE_STEP);
        lst->replace(params->bandLength-1,lastPick);
      }
      else if (PICK_SHAPE(lastPick) == PARA_LEFT) {
        lastPick = (lastPick & NOSHAPE_MASK) | (TRI_LEFT << SHAPE_STEP);
        lst->replace(params->bandLength-1,lastPick);
      }
    }
    else if ( (PICK_SHAPE(firstPick) == PARA_LEFT) || (PICK_SHAPE(firstPick) == TRI_LEFT)) {

      if ( (PICK_SHAPE(lastPick) == PARA_LEFT) || (PICK_SHAPE(lastPick) == TRI_LEFT)) {
        lastPick = (lastPick & NOSHAPE_MASK) | (PARA_LEFT << SHAPE_STEP);
        lst->replace(params->bandLength-1,lastPick);
      }
      else if (PICK_SHAPE(lastPick) == PARA_RIGHT) {
        lastPick = (lastPick & NOSHAPE_MASK) | (TRI_RIGHT << SHAPE_STEP);
        lst->replace(params->bandLength-1,lastPick);
      }
    }
  }
}


void BandModel::findHoleLettersWithoutIdles(int warpId) {

  QList<quint16> *lst = warps.at(warpId);
  quint16 pick;
  quint32 i;
  quint16 numHoleS;
  quint16 numLastHoleS;
  quint8 lastOri;
  quint8 curOri;
  bool isForwardS;
  quint32 nbHole = params->nbHoles;

  pick = lst->at(1);    
  if ( (PICK_SHAPE(pick) == PARA_LEFT) || ( PICK_SHAPE(pick) == TRI_LEFT) ) {
    lastOri = 0;
    isForwardS = false;    
  }
  else {
    lastOri = 1;
    isForwardS = true;
  }

  if (params->numberingType == ANTICLOCKWISE) {

    if (isForwardS) {
      numHoleS = nbHole-1;
    }
    else {
      numHoleS = 0;
    }
  }
  else {
    if (isForwardS) {
      numHoleS = 0;
    }
    else {
      numHoleS = nbHole-1;
    }
  }

  if (params->mount.at(warpId) == S_MOUNT) {
    pick = (numHoleS << HOLE_STEP) | (pick & NOHOLE_MASK);
  }
  else {
    pick = ((nbHole-1-numHoleS) << HOLE_STEP) | (pick & NOHOLE_MASK);
  }

  lst->replace(1,pick);
  numLastHoleS = numHoleS;  

  for(i=2;i<params->bandLength;i++) {
    pick = lst->at(i);
    if ( (PICK_SHAPE(pick) == PARA_LEFT) || ( PICK_SHAPE(pick) == TRI_LEFT) ) {
      curOri = 0;
    }
    else {
      curOri = 1;
    }
    // if no change of orientation, then no change in turning direction
    if (lastOri == curOri) {

      if (isForwardS) {
        if (params->numberingType == ANTICLOCKWISE) {
          numHoleS = (numHoleS-1 + nbHole) % nbHole;
        }
        else {
          numHoleS = (numHoleS+1) % nbHole;
        }
      }
      else {
        if (params->numberingType == ANTICLOCKWISE) {
          numHoleS = (numHoleS+1) % nbHole;
        }
        else {
          numHoleS = (numHoleS-1 + nbHole) % nbHole;
        }
      }
    }
    else { // change of orientation : change of turning direction, and keeping the last hole letter
      isForwardS = !isForwardS;
      numHoleS = numLastHoleS;
    }

    if (params->mount.at(warpId) == S_MOUNT) {
      pick = (numHoleS << HOLE_STEP) | (pick & NOHOLE_MASK);
    }
    else {
      pick = ((nbHole-1-numHoleS) << HOLE_STEP) | (pick & NOHOLE_MASK);
    }

    lst->replace(i,pick);
    lastOri = curOri;
    numLastHoleS = numHoleS;    
  }
}

void BandModel::translateCompleteWarp(int warpId, bool sendSig) {  

  if (params->createMode == MODE_FROMMOVES) {
    cerr << "Abnormal case : call to translateCompleteWarp in MOVES mode" << endl;
    return;
  }

  // if warp colored pick != bandLength, do nothing
  if (nbColoredPicks.at(warpId) != (params->bandLength)-1) {
    return;
  }
  quint8* warp;
  quint8* idToColors;
  quint32 nbColor;
  quint8 oldIdToColors[PALETTE_SIZE+1];
  quint32 oldNbColor;
  quint32 i;

  quint16 pick;

  warp = (params->completedWarps).at(warpId);
  (params->isWarpComplete).replace(warpId,true);
  memset(warp,0,params->bandLength);
  idToColors = (params->idToColors).at(warpId);  
  oldNbColor = (params->nbMaxColors).at(warpId);
  for(i=0;i<oldNbColor;i++) {
    oldIdToColors[i+1] = idToColors[i+1];
  }  

  QList<quint16> *lst = warps.at(warpId);
  quint8 lstColor[PALETTE_SIZE+1];
  memset(lstColor,0,PALETTE_SIZE+1);  
  memset(idToColors,0,PALETTE_SIZE+1);

  // find all colors on warp
  for(i=1;i<params->bandLength;i++) {
    pick = lst->at(i);
    lstColor[PICK_COLOR(pick)] += 1;
  }
  /*
    since PICK_COLOR(pick) = 0 means that the pick is not colorized (= void)
    lstColor[0] is never changed.
    */
  QHash<quint8,quint8> colorToId; // to retrieve the id in 1,2... with the id in palette
  nbColor = 0;
  for(i=1;i<=PALETTE_SIZE;i++) {
    if (lstColor[i] != 0) {      
      nbColor+=1;
      /* idToColors contains the id+1 in params->palette of the color n° nbColor in the threading
         with nbColor >= 1 and <= params->nbHoles. It means that idToColors[0] is never changed and
         that we must put i in idToColors.
         */
      idToColors[nbColor] = i;
      colorToId.insert(i,nbColor);
    }
  }

  (params->nbColors).replace(warpId,nbColor);
#ifdef DEBUG_PRINT
  cout << "warp n°" << warpId << " contains " << nbColor << " colors" << endl;
#endif  

  // fulfill with existing refs in precedent idToColors
  if (nbColor < oldNbColor) {

    // set currently found refs to 1
    for(i=1;i<=PALETTE_SIZE;i++) {
      if (lstColor[i] != 0) {
        lstColor[i] = 1;
      }
    }
    // find old colors that are not already in the list
    for(i=0;i<oldNbColor;i++) {
      if (lstColor[oldIdToColors[i+1]] == 0) {
        lstColor[oldIdToColors[i+1]] = 2;
      }
    }
    // add old colors
    for(i=1;i<=PALETTE_SIZE;i++) {      
      if ((nbColor < params->nbHoles) && (lstColor[i] == 2)) {
        nbColor+=1;
        idToColors[nbColor] = i;
      }
    }
  }  
  (params->nbMaxColors).replace(warpId,nbColor);

  // translating
  for(i=0;i<params->bandLength-1;i++) {
    pick=lst->at(i+1);
    warp[i] = colorToId.value(PICK_COLOR(pick)) | (pick & SHAPE_MASK);
  }

  // do the guess only when in FROMBAND mode
  if ((params->createMode == MODE_FROMBAND) && (nbColor <= params->nbHoles) && (sendSig)) {
    emit warpCompleted(warpId);
  }
}

void BandModel::refresh(bool guess) {
  quint32 i;
  quint32 nb;

  if (guess) { // if guesser is called, show a progression bar
    nb = 0;
    for(i=0;i<params->bandWidth;i++) {
      if (nbColoredPicks.at(i) == (params->bandLength)-1) {
        nb += 1;
      }
    }

    QProgressDialog progress(tr("Computing solutions..."), "", 0, nb);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButton(NULL);

    nb = 0;
    for(i=0;i<params->bandWidth;i++) {
      if (nbColoredPicks.at(i) == (params->bandLength)-1) {
        translateCompleteWarp(i,guess);
        progress.setValue(nb++);
      }
    }
    progress.setValue(nb);
  }
  else { // just translate the warps (used during opening v>0.4 file) and do not guess
    for(i=0;i<params->bandWidth;i++) {      
      translateCompleteWarp(i,false);
    }
  }

  emit allWarpsChanged();
}

/* NOTE :
   - updateHoleColor() is called only in FROMMOVES mode
   */
void BandModel::updateHoleColor(int warpId, quint8 numHole, quint8 color) {

  QList<quint16> *lst = warps.at(warpId);
  quint16 pick;
  quint32 i;

  for(i=1;i<params->bandLength;i++) {
    pick = lst->at(i);

    if (PICK_HOLE(pick) == numHole) {
      pick = (pick & NOCOLOR_MASK) | (color << COLOR_STEP);
      lst->replace(i,pick);
    }
  }

  emit warpChanged(warpId);
}

void BandModel::updateWarpFromMoves(int warpId, quint8 *moves, quint8 *thread, bool sendSig) {

  QList<quint16> *lst = warps.at(warpId);
  quint16 pick;  
  quint16 lastPick = 0;
  quint8 move;
  quint8 color;
  quint32 i,j;
  bool invFirst = false;
  quint16 numHole = 0;
  quint16 numLastHole;
  quint16 currHole;
  quint8 lastOri = FORWARD;
  quint32 nbHole = params->nbHoles;
  quint32 nbColored = 0;

  quint8 lastShape = 0; // shape of last pick : 0 = left, 1 = right

  // first supposing that numeringType is anticlockwise  
  lst->clear(); // clear pick list
  // set pick n°0 and n°1

  /* CAUTION : have to check for a very particular case.
     Indeed, when the first move is idle and for ex, card is mounted S, then
     the default is to have a PARA_RIGHT. But, if the last move of the pattern
     is backward, or if there are some idles at the end, the last not-idle move is
     backward, then the first pick must be chosen as PARA_LEFT in order to have a
     correct alignment of the first and last picks of the band.
     */
  lst->clear();
  move = moves[0];
  if (PICK_MOVE(move) == FORWARD){

    if ((params->mount).at(warpId) == S_MOUNT) {
      pick = (PARA_RIGHT << SHAPE_STEP);
      lastShape = 1;
    }
    else {
      pick = (PARA_LEFT << SHAPE_STEP);
      lastShape = 0;
    }
    numHole = nbHole-1;

    // setting lastOri for next pick
    lastOri = FORWARD;
  }
  else if (PICK_MOVE(move) == BACKWARD) {

    if ((params->mount).at(warpId) == Z_MOUNT) {
      pick = (PARA_RIGHT << SHAPE_STEP);
      lastShape = 1;
    }
    else {
      pick = (PARA_LEFT << SHAPE_STEP);
      lastShape = 0;
    }
    numHole = 0;
    // setting lastOri for next pick
    lastOri = BACKWARD;
  }
  else if ((PICK_MOVE(move) == IDLE)){

    // check for the last move if its backward or forward.
    invFirst = false;
    j = params->patternLength-1;
    while ( (j>=1) && (PICK_MOVE(moves[j]) == IDLE)) {
      j -= 1;
    }
    if ((j>=1) && (PICK_MOVE(moves[j]) == BACKWARD)) {      
      invFirst = true;
    }

    if ((params->mount).at(warpId) == S_MOUNT) {

      pick = (PARA_RIGHT << SHAPE_STEP);
      lastShape = 1;

    }
    else {

      pick = (PARA_LEFT << SHAPE_STEP);
      lastShape = 0;

    }
    numHole = 0;
    // setting lastOri for next pick
    lastOri = FORWARD;
  }
  /* all is computed for anticlockwise, thus color is found with numHole and not cuurHole
   */
  color = thread[numHole];

  // if numbering is clockwise exchange 0<->nbHole-1, 1<->nbHole-2, ...
  if (params->numberingType == CLOCKWISE) {
    currHole = nbHole-1-numHole;
  }
  else {
    currHole = numHole;
  }


  lst->append(pick); // for pick 0
  pick = pick | (color << COLOR_STEP) | (currHole << HOLE_STEP);
  if (color != VOID) {
    nbColored += 1;
  }
  lst->append(pick); // for pick 1

  numLastHole = numHole;

  for(i=2;i<=params->bandLength;i++) {

    move = moves[(i-1)%params->patternLength];
    lastPick = lst->at(i-1);

    if (PICK_MOVE(move) == FORWARD) {

      if (lastOri == FORWARD) {
        numHole = (numLastHole-1 + nbHole) % nbHole;
        if (lastShape == 0) {
          pick = (PARA_LEFT << SHAPE_STEP);
        }
        else {
          pick = (PARA_RIGHT << SHAPE_STEP);
        }
      }
      else if (lastOri == BACKWARD) {
        numHole = numLastHole;        
        if (lastShape == 0) {
          pick = (PARA_RIGHT << SHAPE_STEP);
          lastShape = 1;
        }
        else {
          pick = (PARA_LEFT << SHAPE_STEP);
          lastShape = 0;
        }

        // change of orientation -> change last pick shape to triangle
        if (PICK_SHAPE(lastPick) == PARA_RIGHT) {
          lastPick = (lastPick & NOSHAPE_MASK) | (TRI_RIGHT << SHAPE_STEP);
        }
        else if (PICK_SHAPE(lastPick) == PARA_LEFT) {
          lastPick = (lastPick & NOSHAPE_MASK) | (TRI_LEFT << SHAPE_STEP);
        }
        lst->replace(i-1,lastPick);
      }

      // set lastOri for next move
      lastOri = FORWARD;
    }
    else if (PICK_MOVE(move) == BACKWARD) {

      if (lastOri == BACKWARD) {
        numHole = (numLastHole+1) % nbHole;
        if (lastShape == 0) {
          pick = (PARA_LEFT << SHAPE_STEP);
        }
        else {
          pick = (PARA_RIGHT << SHAPE_STEP);
        }
      }
      else if (lastOri == FORWARD) {
        numHole = numLastHole;
        if (lastShape == 0) {
          pick = (PARA_RIGHT << SHAPE_STEP);
          lastShape = 1;
        }
        else {
          pick = (PARA_LEFT << SHAPE_STEP);
          lastShape = 0;
        }

        // change of orientation -> change last pick shape to triangle
        if (PICK_SHAPE(lastPick) == PARA_RIGHT) {
          lastPick = (lastPick & NOSHAPE_MASK) | (TRI_RIGHT << SHAPE_STEP);
        }
        else if (PICK_SHAPE(lastPick) == PARA_LEFT) {
          lastPick = (lastPick & NOSHAPE_MASK) | (TRI_LEFT << SHAPE_STEP);
        }
        lst->replace(i-1,lastPick);
      }
      // set lastOri for next move
      lastOri = BACKWARD;
    }
    else if (PICK_MOVE(move) == IDLE) {

      numHole = numLastHole;
      // lastOri does not change
    }
    /* all is computed for anticlockwise, thus color is found with numHole and not currHole
       */
    color = thread[numHole];

    // if numbering is clockwise exchange 0<->nbHole-1, 1<->nbHole-2, ...
    if (params->numberingType == CLOCKWISE) {
      currHole = nbHole-1-numHole;
    }
    else {
      currHole = numHole;
    }

    pick = pick | (color << COLOR_STEP) | (currHole << HOLE_STEP);
    if (i<params->bandLength) {
      lst->append(pick);
      if (color != VOID) {
        nbColored += 1;
      }
    }
    numLastHole = numHole;
  }

  // if needed, inverse the first pick
  if (invFirst) {
    pick = lst->at(1);
    lastPick = lst->at(0);
    if (PICK_SHAPE(pick) == PARA_LEFT) {
      pick = (pick & NOSHAPE_MASK) | (TRI_RIGHT << SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_RIGHT << SHAPE_STEP);
    }
    else if (PICK_SHAPE(pick) == PARA_RIGHT) {
      pick = (pick & NOSHAPE_MASK) | (TRI_LEFT << SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_LEFT << SHAPE_STEP);
    }
    else if (PICK_SHAPE(pick) == TRI_LEFT) {
      pick = (pick & NOSHAPE_MASK) | (PARA_RIGHT << SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_RIGHT << SHAPE_STEP);
    }
    else if (PICK_SHAPE(pick) == TRI_RIGHT) {
      pick = (pick & NOSHAPE_MASK) | (PARA_LEFT << SHAPE_STEP);
      lastPick = (lastPick & NOSHAPE_MASK) | (PARA_LEFT << SHAPE_STEP);
    }
    lst->replace(0,lastPick);
    lst->replace(1,pick);
  }

  nbColoredPicks.replace(warpId,nbColored);
  if (nbColored == params->bandLength-1) {
    params->isWarpComplete.replace(warpId,true);
  }
  else {
    params->isWarpComplete.replace(warpId,false);
  }

  if (sendSig) {
    emit warpChanged(warpId);
  }
}


void BandModel::updateLetteringFromMoves(int warpId, quint8 *moves) {

  QList<quint16> *lst = warps.at(warpId);
  quint16 pick;
  quint16 lastPick = 0;
  quint8 move;  
  quint32 i;
  quint16 numHole = 0;
  quint16 numLastHole;
  quint16 currHole;
  quint8 lastOri = FORWARD;
  quint32 nbHole = params->nbHoles;
  quint8 usedHoles[6]; // 6 is amximum of nb holes

  memset(usedHoles,0,6);
  move = moves[0];
  if (move == NO_MOVE) {
    findHoleLettersWithoutIdles(warpId);
  }
  else {
    if (PICK_MOVE(move) == FORWARD){
      numHole = nbHole-1;
      // setting lastOri for next pick
      lastOri = FORWARD;
    }
    else if (PICK_MOVE(move) == BACKWARD) {
      numHole = 0;
      // setting lastOri for next pick
      lastOri = BACKWARD;
    }
    else if ((PICK_MOVE(move) == IDLE)){
      numHole = 0;
      // setting lastOri for next pick
      lastOri = FORWARD;
    }

    // if numbering is clockwise exchange 0<->nbHole-1, 1<->nbHole-2, ...
    if (params->numberingType == CLOCKWISE) {
      currHole = nbHole-1-numHole;
    }
    else {
      currHole = numHole;
    }

    pick = lst->at(1);
    pick = (pick & NOHOLE_MASK) | (currHole << HOLE_STEP);
    lst->replace(1,pick);
    usedHoles[currHole] = 1;

    numLastHole = numHole;

    for(i=2;i<params->bandLength;i++) {

      move = moves[(i-1)%params->patternLength];
      lastPick = lst->at(i-1);

      if (PICK_MOVE(move) == FORWARD) {

        if (lastOri == FORWARD) {
          numHole = (numLastHole-1 + nbHole) % nbHole;
        }
        else if (lastOri == BACKWARD) {
          numHole = numLastHole;
        }
        // set lastOri for next move
        lastOri = FORWARD;
      }
      else if (PICK_MOVE(move) == BACKWARD) {

        if (lastOri == BACKWARD) {
          numHole = (numLastHole+1) % nbHole;
        }
        else if (lastOri == FORWARD) {
          numHole = numLastHole;
        }
        // set lastOri for next move
        lastOri = BACKWARD;
      }
      else if (PICK_MOVE(move) == IDLE) {
        numHole = numLastHole;
        // lastOri does not change
      }

      // if numbering is clockwise exchange 0<->nbHole-1, 1<->nbHole-2, ...
      if (params->numberingType == CLOCKWISE) {
        currHole = nbHole-1-numHole;
      }
      else {
        currHole = numHole;
      }

      usedHoles[currHole] = 1;

      pick = lst->at(i);
      pick = (pick & NOHOLE_MASK) | (currHole << HOLE_STEP);
      lst->replace(i,pick);
      numLastHole = numHole;
    }
  }  

  emit letteringChanged(warpId);
}


void BandModel::updateForNewPatternLength(quint32 oldValue) {

  /* two cases :
     - the pattern length decreases : have to cut some picks (only the last ?)
     - the pattern length increases : have to put some void picks
     */

  quint32 i,j,gap;
  QList<quint16> *lst;
  quint16 pick;
  quint32 nbColored;

  for(i=0;i<params->bandWidth;i++) {


    if (oldValue < params->patternLength) {

      gap = params->patternLength - oldValue;

      for(j=0;j<params->nbPatterns;j++) {
        insertPicksAfter(i,(j+1)*oldValue + (j*gap), gap);
      }
    }

    else {

      gap = oldValue - params->patternLength;

      for(j=0;j<params->nbPatterns;j++) {
        removePicksAfter(i,(j+1)*oldValue-gap - (j*gap), gap);
      }
    }
    checkLastPick(i);
    findHoleLettersWithoutIdles(i);

    /* in case of increasing pattern length, some void picks are inserted
       at the end of each pattern. Depending on the coloring of the last and first picks of
       each pattern, there may be some changes for the number of colored picks. Idem when some picks
       are removed.
       */
    // update number of colored picks
    lst = warps.at(i);
    nbColored = 0;
    for(j=1;j<params->bandLength;j++) {
      pick = lst->at(j);
      if (PICK_COLOR(pick) != VOID) nbColored += 1;
    }
    nbColoredPicks.replace(i,nbColored);
    /* even if picks are complete, do no guess in ordre to be sure that
       moves and threading are voided */
    translateCompleteWarp(i,false);
  }
  // signal to the view
  emit allWarpsChanged();
}

/* removePicksAfter() : remove nb picks after the pick n° pos
   pick n° pos is not removed.
  */
void BandModel::removePicksAfter(int warpId, int pos, int nb ) {

  quint16 currPick;
  quint16 nextPick;
  QList<quint16> *lst;
  int oldSize;
  int i;  

  lst = warps.at(warpId);
  oldSize = lst->size();

  if (pos+nb+1 < oldSize) {

    currPick = lst->at(pos);
    nextPick = lst->at(pos+nb+1);
    if (PICK_SHAPE(currPick) == PARA_RIGHT) {
      if ((PICK_SHAPE(nextPick) == PARA_LEFT) || (PICK_SHAPE(nextPick) == TRI_LEFT)) {
        currPick = (currPick & NOSHAPE_MASK) | (TRI_RIGHT<<SHAPE_STEP);
        lst->replace(pos,currPick);
      }
    }
    else if (PICK_SHAPE(currPick) == TRI_RIGHT) {
      if ((PICK_SHAPE(nextPick) == PARA_RIGHT) || (PICK_SHAPE(nextPick) == TRI_RIGHT)) {
        currPick = (currPick & NOSHAPE_MASK) | (PARA_RIGHT<<SHAPE_STEP);
        lst->replace(pos,currPick);
      }
    }
    else if (PICK_SHAPE(currPick) == PARA_LEFT) {
      if ((PICK_SHAPE(nextPick) == PARA_RIGHT) || (PICK_SHAPE(nextPick) == TRI_RIGHT)) {
        currPick = (currPick & NOSHAPE_MASK) | (TRI_LEFT<<SHAPE_STEP);
        lst->replace(pos,currPick);
      }
    }
    else if (PICK_SHAPE(currPick) == TRI_LEFT) {
      if ((PICK_SHAPE(nextPick) == PARA_LEFT) || (PICK_SHAPE(nextPick) == TRI_LEFT)) {
        currPick = (currPick & NOSHAPE_MASK) | (PARA_LEFT<<SHAPE_STEP);
        lst->replace(pos,currPick);
      }
    }
  }
  for(i=0;i<nb;i++) {
    lst->removeAt(pos+1);
  }
}

/* insertPicksAfter() : insert nb void picks after the pick n° pos
  */
void BandModel::insertPicksAfter(int warpId, int pos, int nb ) {

  quint16 currPick;
  QList<quint16> *lst;
  int i;
  bool isRight = true;
  lst = warps.at(warpId);
  currPick = lst->at(pos);
  if ((PICK_SHAPE(currPick) == PARA_LEFT) || (PICK_SHAPE(currPick) == TRI_RIGHT)) {
    isRight = false;
  }
  for(i=0;i<nb;i++) {
    if (isRight) {
      lst->insert(pos+1,PARA_RIGHT<<SHAPE_STEP);
    }
    else {
      lst->insert(pos+1,PARA_LEFT<<SHAPE_STEP);
    }
  }  
}

void BandModel::updateForNewNbPatterns(quint32 oldValue) {

  /* two cases :
     - nb pattern ldecreases : have to cut the last patterns
     - nb pattern length increases : have to put new patterns.
     */

  quint32 i,j,gap;
  QList<quint16> *lst;
  quint16 pick;
  int pos;
  quint32 nbColored;

  for(i=0;i<params->bandWidth;i++) {

    lst = warps.at(i);

    if (oldValue > params->nbPatterns) {
      gap = oldValue - params->nbPatterns;
      pos = params->nbPatterns * params->patternLength +1;

      for(j=0;j<gap*params->patternLength;j++) {
        lst->removeAt(pos);
      }
    }    
    else {
      gap = params->nbPatterns - oldValue;

      for(j=0;j<gap*params->patternLength;j++) {
        pick = lst->at((j%params->patternLength)+1);
        lst->append(pick);
      }      
    }
    findHoleLettersWithoutIdles(i);

    // update number of colored picks
    nbColored = 0;
    for(j=1;j<params->bandLength;j++) {
      pick = lst->at(j);
      if (PICK_COLOR(pick) != VOID) nbColored += 1;
    }
    nbColoredPicks.replace(i,nbColored);
  }
}

void BandModel::setModel(const QList<QList<quint16> *> &_warps) {

  QListIterator<QList<quint16> *> iter(warps);
  QList<quint16> *lst;
  quint8 pick;
  int nbColored;
  quint32 i,j;

  while (iter.hasNext()) {
    lst = iter.next();
    lst->clear();
    delete lst;
  }
  warps.clear();
  warps = _warps;
#ifdef DEBUG_PRINT
  cout << "warps <- import, nb warps : " << warps.size() << ", bandwidth : " << params->bandWidth << ", pattern length : " << params->patternLength << ", bandlength : " << params->bandLength << endl;
#endif
  for(i=0;i<params->bandWidth;i++) {
    lst = warps.at(i);
    checkLastPick(i);
    nbColored = 0;
    for(j=1;j<params->bandLength;j++) {
      pick = lst->at(j);
      if (PICK_COLOR(pick) != VOID) nbColored += 1;
    }
    nbColoredPicks.replace(i,nbColored);
  }

}


QDataStream &operator<<(QDataStream &out, const BandModel &b) {

  QByteArray block;
  QDataStream toWrite(&block, QIODevice::WriteOnly);
  toWrite.setVersion(QDataStream::Qt_4_0);

  quint32 i;
  QList<quint16> *lst;
  for(i=0;i<(b.params)->bandWidth;i++) {
    lst = b.warps.at(i);
    toWrite << *lst;
  }  
  toWrite << b.nbColoredPicks;

  out << block;
  return out;
}

QDataStream &operator>>(QDataStream &in, BandModel &b) {
  quint32 blockSize;
  in.setVersion(QDataStream::Qt_4_0);
  QString txt;  

  in >> blockSize;

  quint32 i;
  int j;
  QList<quint16> *lst;
  QList<quint16> lstRead;
  QList<quint8> lstOldRead;

  for(i=0;i<(b.params)->bandWidth;i++) {

    if ((b.params->cmpVersionMaj == 0) && (b.params->cmpVersionMin < 3)) {

#ifdef DEBUG_PRINT
      cout << "file version < 0.3 : need to convert quint8 warp value to quint6" << endl;
#endif
      in >> lstOldRead;
      // convert old to new
      lstRead.clear();
      for(j=0;j<lstOldRead.size();j++) {        
        lstRead.append(lstOldRead.at(j));
      }
    }
    else {
      in >> lstRead;
    }
    lst = b.warps.at(i);
    lst->clear();
    *lst = lstRead;

    b.checkLastPick(i);
    b.findHoleLettersWithoutIdles(i);
  }


  QList<quint16> lstColPick;
  QList<quint8> lstOldColPick;

  if ((b.params->cmpVersionMaj == 0) && (b.params->cmpVersionMin < 3)) {
    in >> lstOldColPick;
    lstColPick.clear();
    for(j=0;j<lstOldColPick.size();j++) {
      lstColPick.append(lstOldColPick.at(j));
    }
  }
  else {
    in >> lstColPick;
  }


  b.nbColoredPicks.clear();
  b.nbColoredPicks = lstColPick;

  return in;

}

QRect BandModel::draw(QPainter *p) {

  //QPrinter *printer = (QPrinter *)(p->device());
  QPaintDevice *pd= p->device();

  QFont fontNum("Arial",7);
  p->setFont(fontNum);
  QFontMetrics fmNum = p->fontMetrics();
  int numWidth = fmNum.width("55");
  int numHeight = fmNum.height();

  //int dpi = printer->resolution();
  int dpi = pd->logicalDpiX();
  //QRect paperSize = printer->paperRect();
  QRect paperSize(0,0,pd->width(),pd->height());

  int marginUp = (int)(((params->exportMarginUp)*dpi)/2.54);
  int marginBottom = (int)(((params->exportMarginBottom)*dpi)/2.54);
  int marginLeft = (int)(((params->exportMarginLeft)*dpi)/2.54);
  int marginRight = (int)(((params->exportMarginRight)*dpi)/2.54);
  int space = (int)(((params->exportSpace)*dpi)/2.54);

  int drawHeight = paperSize.height() - marginUp - marginBottom;
  int drawWidth = paperSize.width() - marginLeft - marginRight;

  int pickWidthPrinted = (int)(((params->exportPickWidth)*dpi)/2.54);
  int pickBaseLengthPrinted = (int)(((params->exportPickBaseLength)*dpi)/2.54);
  int pickTotalLengthPrinted = (int)(((params->exportPickTotalLength)*dpi)/2.54);  

  int totalLength = (params->bandLength-1)*pickBaseLengthPrinted+pickTotalLengthPrinted;
  int totalWidth = (params->bandWidth)*pickWidthPrinted;

  double ratioLength = 1.0;
  double ratioWidth = 1.0;
  if (totalLength > (drawWidth-numWidth-space)) {
    ratioLength = (double)(drawWidth-numWidth-space)/(double)totalLength;
  }
  if (totalWidth > drawHeight) {
    ratioWidth = (double)drawHeight/(double)totalWidth;
  }
  double ratio;
  if (ratioLength < ratioWidth) {
    ratio = ratioLength;
  }
  else {
    ratio = ratioWidth;
  }
  pickWidthPrinted = (int)(pickWidthPrinted*ratio);
  pickBaseLengthPrinted = (int)(pickBaseLengthPrinted*ratio);
  pickTotalLengthPrinted = (int)(pickTotalLengthPrinted*ratio);

  int fontSize = fontNum.pointSize();
  while ((fontSize > 0) && (numHeight > pickWidthPrinted)) {
    fontSize -=1;
    fontNum.setPointSize(fontSize);
    p->setFont(fontNum);
    fmNum = p->fontMetrics();
    numHeight = fmNum.height();    
  }

  QPen pen(Qt::black);
  pen.setWidth(2);
  p->setPen(pen);
  QBrush coloredPick(Qt::black);

  QList<quint16> *lstPick;
  quint16 pick;
  QString num;
  int x,y;
  quint32 i,j;
  for(i=0;i<params->bandWidth;i++) {

    // paint number
    x = marginLeft;
    y = marginUp + i*pickWidthPrinted;
    num.setNum(i+1);    
    p->drawText(x,y+numHeight-(pickWidthPrinted-numHeight)/2, num);

    x = x+numWidth+space;
    lstPick = warps.at(i);
    pick = lstPick->at(0);
    // paint first void pick
    QPainterPath firstPath;
    if ((PICK_SHAPE(pick) == PARA_RIGHT) || (PICK_SHAPE(pick) == TRI_RIGHT)) {
      firstPath.moveTo(x,y);
      firstPath.lineTo(x+pickBaseLengthPrinted,y);
      firstPath.lineTo(x+pickTotalLengthPrinted,y+pickWidthPrinted);
      firstPath.lineTo(x,y+pickWidthPrinted);
      firstPath.lineTo(x,y);
      firstPath.closeSubpath();
    }
    else if ((PICK_SHAPE(pick) == PARA_LEFT) || (PICK_SHAPE(pick) == TRI_LEFT)) {
      firstPath.moveTo(x,y+pickWidthPrinted);
      firstPath.lineTo(x+pickBaseLengthPrinted,y+pickWidthPrinted);
      firstPath.lineTo(x+pickTotalLengthPrinted,y);
      firstPath.lineTo(x,y);
      firstPath.lineTo(x,y+pickWidthPrinted);
      firstPath.closeSubpath();
    }
    p->setBrush(Qt::CrossPattern);
    p->drawPath(firstPath);

    x = x+pickBaseLengthPrinted;

    for(j=1;j<params->bandLength;j++) {      
      pick = lstPick->at(j);
      QPainterPath pickPath;
      if (PICK_SHAPE(pick) == PARA_RIGHT) {
        pickPath.moveTo(x,y);
        pickPath.lineTo(x+pickBaseLengthPrinted,y);
        pickPath.lineTo(x+pickTotalLengthPrinted,y+pickWidthPrinted);
        pickPath.lineTo(x+pickTotalLengthPrinted-pickBaseLengthPrinted,y+pickWidthPrinted);
        pickPath.lineTo(x,y);
        pickPath.closeSubpath();
      }
      else if (PICK_SHAPE(pick) == PARA_LEFT) {
        pickPath.moveTo(x,y+pickWidthPrinted);
        pickPath.lineTo(x+pickBaseLengthPrinted,y+pickWidthPrinted);
        pickPath.lineTo(x+pickTotalLengthPrinted,y);
        pickPath.lineTo(x+pickTotalLengthPrinted-pickBaseLengthPrinted,y);        
        pickPath.lineTo(x,y+pickWidthPrinted);       
        pickPath.closeSubpath();
      }
      else if (PICK_SHAPE(pick) == TRI_RIGHT) {
        pickPath.moveTo(x,y);
        pickPath.lineTo(x+pickTotalLengthPrinted,y);
        pickPath.lineTo(x+pickTotalLengthPrinted/2,y+pickWidthPrinted);
        pickPath.lineTo(x,y);
        pickPath.closeSubpath();
      }
      else if (PICK_SHAPE(pick) == TRI_LEFT) {
        pickPath.moveTo(x,y+pickWidthPrinted);
        pickPath.lineTo(x+pickTotalLengthPrinted,y+pickWidthPrinted);
        pickPath.lineTo(x+pickTotalLengthPrinted/2,y);
        pickPath.lineTo(x,y+pickWidthPrinted);
        pickPath.closeSubpath();
      }
      if (PICK_COLOR(pick) == VOID ) {
        p->setBrush(Qt::CrossPattern);
      }
      else {
        coloredPick.setColor((params->palette).at(PICK_COLOR(pick)-1));
        p->setBrush(coloredPick);
      }
      p->drawPath(pickPath);
      x = x+pickBaseLengthPrinted;
    }
  }

  QRect boundingBox(marginLeft,marginUp,0,0);
  totalLength = (int)(ratio*totalLength) + numWidth + space +1;
  totalWidth = (int)(ratio*totalWidth)+1;

  boundingBox.setWidth(totalLength);
  boundingBox.setHeight(totalWidth);

  return boundingBox;
}
