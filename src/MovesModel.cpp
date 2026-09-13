/*-==============================================================-

file : MovesModel.cpp

creation date : 27/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description :

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "MovesModel.hpp"

MovesModel::MovesModel(ProjectParams *_params) : QObject() {

  params = _params;

  quint8 *lstS;
  quint8 *lstZ;
  quint8 *lstAny;
  quint32 i;

  for(i=0;i<(params->bandWidth);i++) {
    lstS = new quint8[params->patternLength];
    memset(lstS,NO_MOVE,params->patternLength);
    lstZ = new quint8[params->patternLength];
    memset(lstZ,NO_MOVE,params->patternLength);
    lstAny = new quint8[params->patternLength];
    memset(lstAny,NO_MOVE,params->patternLength);

    startInS.append(lstS);
    startInZ.append(lstZ);    
    movesForMoveMode.append(lstAny);

    selectedMoves.append(false);
    totallyLockedMoves.append(false);

  }
  allSelected = false;
}

MovesModel::~MovesModel() {  
  clear();
}


void MovesModel::clear() {

  QListIterator<quint8 *> iterS(startInS);
  quint8 * lst;
  while(iterS.hasNext()) {
    lst = iterS.next();
    delete [] lst;
  }
  startInS.clear();

  QListIterator<quint8 *> iterZ(startInZ);  
  while(iterZ.hasNext()) {
    lst = iterZ.next();
    delete [] lst;
  }
  startInZ.clear();

  QListIterator<quint8 *> iterAny(movesForMoveMode);
  while(iterAny.hasNext()) {
    lst = iterAny.next();
    delete [] lst;
  }
  movesForMoveMode.clear();

  selectedMoves.clear();
  totallyLockedMoves.clear();

}

void MovesModel::setCardSide(int warpId, int forWhich) {

  quint8 *lst = NULL;

  if (forWhich == FOR_S) {
    lst = startInS.at(warpId);
  }
  else if (forWhich == FOR_Z){
    lst = startInZ.at(warpId);
  }
  else if (forWhich == FOR_MOVES){
    lst = movesForMoveMode.at(warpId);
  }

  // if mvoes are void just return
  if (lst[0] == NO_MOVE) {
    return;
  }
  quint32 i;
  quint8 move;
  quint8 frontHole; // the number of the upper-front hole
  quint8 backHole; // the number of the  upper-back hole
  quint8 nbHoles = params->nbHoles;
  quint8 tmp;
  quint8 side = 0;

  bool isTwisted = false;

  // NOTE : the card side is always computed ofr anticlockwise holes lettering
  frontHole = 0;
  backHole = nbHoles-1;
  isTwisted = false;

  for (i=0;i<params->patternLength;i++) {

    move = lst[i];

    // no twist before current move
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

    lst[i] = (move & NOSIDE_MASK) | (side << SIDE_STEP);
  }
}

bool MovesModel::isVoidMove(int warpId) {

  quint8 *lstS;
  quint8 *lstZ;
  quint8 *lstAny;

  lstAny = movesForMoveMode.at(warpId);
  lstS = startInS.at(warpId);
  lstZ = startInZ.at(warpId);

  if ((params->createMode == MODE_FROMBAND) && (lstS[0] == NO_MOVE) && (lstZ[0] == NO_MOVE)) {
    return true;
  }
  if ((params->createMode == MODE_FROMMOVES) && (lstAny[0] == NO_MOVE)) {
    return true;
  }
  return false;
}

void MovesModel::selectMove(int warpId) {
  bool sel = selectedMoves.at(warpId);
  sel = !sel;
  selectedMoves.replace(warpId,sel);

  // signal to the view
  emit selectionChanged(warpId);
}

void MovesModel::selectAllMoves() {

  quint32 i;

  if (allSelected == false) {
    for(i=0;i<params->bandWidth;i++) {
      if (totallyLockedMoves.at(i)) {
        selectedMoves.replace(i,false);
      }
      else {
        selectedMoves.replace(i,true);
      }
    }
    allSelected = true;
  }
  else {
    for(i=0;i<params->bandWidth;i++) {
      selectedMoves.replace(i,false);
    }
    allSelected = false;
  }
  // signal to the view
  emit selectionAllChanged();
}

int MovesModel::nbSelected() {
  quint32 i;
  int nb = 0;

  for(i=0;i<params->bandWidth;i++) {
    if (selectedMoves.at(i) == true) {
      nb += 1;
    }
  }
  return nb;
}

void MovesModel::totalLockMove(int warpId) {
  bool lock = totallyLockedMoves.at(warpId);
  lock = !lock;
  totallyLockedMoves.replace(warpId,lock);

  //force deselection and shapeLock if locked
  if (lock) {
    selectedMoves.replace(warpId,false);
  }

  // signal to the view
  emit lockChanged(warpId);
  emit selectionChanged(warpId);
}

void MovesModel::removeWarp(int warpId, bool sendSig) {
  quint8* lst;
  lst = startInS.at(warpId);
  delete [] lst;
  lst = startInZ.at(warpId);
  delete [] lst;
  lst = movesForMoveMode.at(warpId);
  delete [] lst;
  startInS.removeAt(warpId);
  startInZ.removeAt(warpId);
  movesForMoveMode.removeAt(warpId);
  selectedMoves.removeAt(warpId);
  totallyLockedMoves.removeAt(warpId);

  if (sendSig == true) {
    emit allMovesChanged();
  }
}

void MovesModel::insertWarp(int warpId, bool sendSig) {

  quint8 *lstS;
  quint8 *lstZ;
  quint8 *lstAny;  

  lstS = new quint8[params->patternLength];
  memset(lstS,NO_MOVE,params->patternLength);
  lstZ = new quint8[params->patternLength];
  memset(lstZ,NO_MOVE,params->patternLength);
  lstAny = new quint8[params->patternLength];
  memset(lstAny,NO_MOVE,params->patternLength);

  startInS.insert(warpId,lstS);
  startInZ.insert(warpId,lstZ);
  movesForMoveMode.insert(warpId,lstAny);
  selectedMoves.insert(warpId,false);
  totallyLockedMoves.insert(warpId,false);

  if (sendSig == true) {
    emit allMovesChanged();
  }
}

void MovesModel::copyWarp(int warpIdSrc, int warpIdDest) {

  quint8 *lstAnySrc;
  quint8 *lstAnyDest;

  lstAnySrc = movesForMoveMode.at(warpIdSrc);
  lstAnyDest = movesForMoveMode.at(warpIdDest);
  memcpy(lstAnyDest,lstAnySrc,params->patternLength);
  // signal to the view that a move changed
  emit moveChanged(warpIdDest);
}


void MovesModel::toggleMove(int warpId, int moveId) {

  quint8 *lstAny = movesForMoveMode.at(warpId);
  quint8 move = lstAny[moveId];

  if (PICK_MOVE(move) == FORWARD) {
    move = (move & NOMOVE_MASK) | (BACKWARD << MOVE_STEP);
  }
  else if (PICK_MOVE(move) == BACKWARD) {
    move = (move & NOMOVE_MASK) | (FORWARD << MOVE_STEP);
  }
  else if (PICK_MOVE(move) == IDLE) {
    move = (move & NOMOVE_MASK) | (FORWARD << MOVE_STEP);
  }
  lstAny[moveId] = move;

  setCardSide(warpId,FOR_MOVES);

  // signal to the view that a move changed
  emit moveChanged(warpId);

  // signal to dispatcher that a move changed
  emit moveToggled(warpId, moveId);
}

void MovesModel::putIdle(int warpId, int moveId) {

  quint8 *lstAny = movesForMoveMode.at(warpId);
  quint8 move = lstAny[moveId];

  move = (move & NOMOVE_MASK) | (IDLE << MOVE_STEP);

  lstAny[moveId] = move;
  setCardSide(warpId,FOR_MOVES);

  // signal to the view that a move changed
  emit moveChanged(warpId);

  // signal to dispatcher that a move changed
  emit moveToggled(warpId, moveId);
}

void MovesModel::updateForNewAllowCyclic() {

  quint32 i;
  bool changes = false;
  // if no cyclic is allowed just refresh the view
  if (params->allowNoCyclic == true) {
    emit allMovesChanged();
  }
  else {
    // search for non cyclic warps and guess them again with cylic
    for(i=0;i<params->bandWidth;i++) {
      if ((params->cyclicCardPosition).at(i) == NOCYCLIC) {
        (params->cyclicCardPosition).replace(i,CYCLIC);
        changes = true;
        emit refreshMove(i);
      }
    }
    // if no changes, have to refresh the view to suppress the cyclic icons
    if (! changes) {
      emit allMovesChanged();
    }
  }
}

void MovesModel::updateForNewGuessIdle() {


  quint32 i,j;
  bool stop;
  quint8 *lstS;
  quint8 *lstZ;
  quint8 pick;
  bool changes = false;

  if (params->guessWithIdles == false) {
    // search for moves with idles and void them

    for(i=0;i<params->bandWidth;i++) {
      lstS = startInS.at(i);      
      lstZ = startInZ.at(i);
      stop = false;
      for(j=0;(j<params->patternLength) && (!stop);j++) {
        pick = lstS[j];        
        if (PICK_MOVE(pick) == IDLE) stop = true;
      }
      if (stop) {
        memset(lstS,NO_MOVE,params->patternLength);
        memset(lstZ,NO_MOVE,params->patternLength);
        changes = true;        
        emit moveCleared(i);
      }
    }

    if (changes) {
      emit allMovesChanged();
    }
  }
}

void MovesModel::updateForNewCreationMode() {

  quint32 i,j;
  quint8 *lstS;
  quint8 *lstZ;
  quint8 *lstIdem;
  quint8 *lstInv;
  quint8 *lstAny;  
  int forWhich;

  // if FROMMOVES is the new mode : copy S or Z in Any
  if (params->createMode == MODE_FROMMOVES) {

    for(i=0;i<params->bandWidth;i++) {

      lstS = startInS.at(i);
      lstZ = startInZ.at(i);
      lstAny = movesForMoveMode.at(i);

      for(j=0;j<params->patternLength;j++) {

        if ((params->mount).at(i) == S_MOUNT) {
          lstAny[j] = lstS[j];
        }
        else {
          lstAny[j] = lstZ[j];
        }
      }

      if (!isVoidMove(i)) {
        setCardSide(i,FOR_MOVES);

        // signal to the band model to update the band so that real hole letters are computed
        emit moveToggled(i,0);
      }
    }
  }
  // if FROMBAND is the new mode : copy Any in S and Z
  else {

    for(i=0;i<params->bandWidth;i++) {

      if ((params->mount).at(i) == S_MOUNT) {
	lstIdem = startInS.at(i);
	lstInv = startInZ.at(i);
	forWhich = FOR_Z;
      }
      else {
	lstIdem = startInZ.at(i);
	lstInv = startInS.at(i);
	forWhich = FOR_S;
      }
      lstAny = movesForMoveMode.at(i);

      for(j=0;j<params->patternLength;j++) {
	lstIdem[j] = lstAny[j];

	if (lstAny[j] == NO_MOVE) {
	  lstInv[j] = NO_MOVE;
	}
	else if (PICK_MOVE(lstAny[j]) == FORWARD) {
	  lstInv[j] = (lstAny[j] & NOMOVE_MASK) | (BACKWARD << MOVE_STEP);
	}
	else if (PICK_MOVE(lstAny[j]) == BACKWARD) {
	  lstInv[j] = (lstAny[j] & NOMOVE_MASK) | (FORWARD << MOVE_STEP);
	}
	else if (PICK_MOVE(lstAny[j]) == IDLE) {
	  lstInv[j] = lstAny[j];
	}
      }
      setCardSide(i,forWhich);
    }
  }
}

/* NB : rollWarp() is called only in FROMMOVES mode */
void MovesModel::rollWarp(int warpId, quint32 nbRoll, bool toDown) {

  quint32 i;
  quint8 *lstAny;
  quint8 *lstDec;
  int decal;

  if (toDown) {
    decal = -nbRoll;
  }
  else {
    decal = nbRoll;
  }
  lstAny = movesForMoveMode.at(warpId);
  lstDec = new quint8[params->patternLength];
  for(i=0;i<params->patternLength;i++) {
    lstDec[(i+decal+params->patternLength)%params->patternLength] = lstAny[i];
  }
  movesForMoveMode.replace(warpId,lstDec);
  delete [] lstAny;
  setCardSide(warpId,FOR_MOVES);

  // signal to the view that a move changed
  emit moveChanged(warpId);

  // signal to dispatcher that a move changed
  emit moveToggled(warpId, 0);
}

/* NOTES :
   - setPattern() is called only in FROMMOVES mode
   - setPattern() uses fb array in which true means B and false means F
   */
void MovesModel::setPattern(int warpId, bool *fb) {

  quint8 *lstAny;
  lstAny = movesForMoveMode.at(warpId);
  quint32 i;
  for(i=0;i<params->patternLength;i++) {
    if (fb[i]) {
      lstAny[i] = (lstAny[i] & NOMOVE_MASK) | (BACKWARD << MOVE_STEP);
    }
    else {
      lstAny[i] = (lstAny[i] & NOMOVE_MASK) | (FORWARD << MOVE_STEP);
    }
  }

  setCardSide(warpId,FOR_MOVES);
  // signal to the view that a move changed
  emit moveChanged(warpId);

  // signal to dispatcher that a move changed
  emit moveToggled(warpId, 0);

}

void MovesModel::refreshMoves() {

  quint32 i;
  quint8 *lstS;
  quint8 *lstZ;

  // search for void moves and guess for them if the warp is completed
  for(i=0;i<params->bandWidth;i++) {
    lstS = startInS.at(i);
    lstZ = startInZ.at(i);
    if ((lstS[0] == NO_MOVE) || (lstZ[0] == NO_MOVE)) {
      if ((params->isWarpComplete).at(i) == true)
        emit refreshMove(i);
    }
  }
}

void MovesModel::updateForNewPatternLength() {

  startInS.clear(); // clear the existing moves
  startInZ.clear(); // clear the existing moves
  movesForMoveMode.clear();

  // create the moves arrays ith new patternLength
  quint8 *lstS;
  quint8 *lstZ;
  quint8 *lstAny;
  quint32 i;

  for(i=0;i<(params->bandWidth);i++) {
    lstS = new quint8[params->patternLength];
    memset(lstS,NO_MOVE,params->patternLength);
    lstZ = new quint8[params->patternLength];
    memset(lstZ,NO_MOVE,params->patternLength);
    lstAny = new quint8[params->patternLength];
    memset(lstAny,NO_MOVE,params->patternLength);

    startInS.append(lstS);
    startInZ.append(lstZ);
    movesForMoveMode.append(lstAny);

    // signal to the control, relayed to the threading to clear cards
    emit moveCleared(i);
  }

  emit allMovesChanged();
}


void MovesModel::updateWarpMoves(int warpId, quint8 *movesForS,quint8 *movesForZ) {

  quint32 i;
  quint8 *lstS = startInS.at(warpId);
  quint8 *lstZ = startInZ.at(warpId);

  if (movesForS == NULL) {
    memset(lstS,NO_MOVE,params->patternLength);
  }
  else {   
    for(i=0;i<params->patternLength;i++) {
      lstS[i] = movesForS[i];
    }
  }

  if (movesForZ == NULL) {
    memset(lstZ,NO_MOVE,params->patternLength);
  }
  else {
    for(i=0;i<params->patternLength;i++) {
      lstZ[i] = movesForZ[i];
    }
  }
  emit moveChanged(warpId);

  // signal to the control and relayed to band model
  emit moveGuessed(warpId);
}

void MovesModel::swapWarp(int warpId1, int warpId2) {

  selectedMoves.swap(warpId1,warpId2);
  totallyLockedMoves.swap(warpId1,warpId2);

  if (params->createMode == MODE_FROMMOVES) {
    movesForMoveMode.swap(warpId1,warpId2);
  }
  else if (params->createMode == MODE_FROMBAND) {

    startInS.swap(warpId1,warpId2);
    startInZ.swap(warpId1,warpId2);
  }
  emit moveChanged(warpId1);
  emit moveChanged(warpId2);

}

QDataStream &operator<<(QDataStream &out, const MovesModel &m) {

  QByteArray block;
  QDataStream toWrite(&block, QIODevice::WriteOnly);
  toWrite.setVersion(QDataStream::Qt_4_0);

  quint32 i,j;
  quint8 *moves;

  for(i=0;i<(m.params)->bandWidth;i++) {
    moves = m.startInS.at(i);
    for(j=0;j<(m.params)->patternLength;j++) {
      toWrite << moves[j];
    }
    moves = m.startInZ.at(i);
    for(j=0;j<(m.params)->patternLength;j++) {
      toWrite << moves[j];
    }
  }

  out << block;
  return out;
}

QDataStream &operator>>(QDataStream &in, MovesModel &m) {

  if (((m.params)->cmpVersionMaj >= 0) && ((m.params)->cmpVersionMin >= 4)) {

    quint32 blockSize;
    in.setVersion(QDataStream::Qt_4_0);

    in >> blockSize;

    quint32 i,j;
    quint8 *moves;
    for(i=0;i<(m.params)->bandWidth;i++) {
      moves = m.startInS.at(i);
      for(j=0;j<(m.params)->patternLength;j++) {
        in >> moves[j];
      }
      moves = m.startInZ.at(i);
      for(j=0;j<(m.params)->patternLength;j++) {
        in >> moves[j];
      }
    }
  }
  return in;
}

/*
QRect MovesModel::drawMoves(QPainter *p) {

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

  int moveWidthPrinted = (int)(((params->exportMoveWidth)*dpi)/2.54);

  bool isPortrait = false;

  int totalHeight = 0;
  int totalWidth = 0;

  if (params->patternLength < params->bandWidth) {
    totalHeight = (params->patternLength)*moveWidthPrinted;
    totalWidth = (params->bandWidth)*moveWidthPrinted;
  }
  else {
    totalWidth = (params->patternLength)*moveWidthPrinted;
    totalHeight = (params->bandWidth)*moveWidthPrinted;
    isPortrait = true;
  }

  double ratioHeight = 1.0;
  double ratioWidth = 1.0;

  if (isPortrait) {
    if (totalHeight > (drawHeight-numWidth-space-moveWidthPrinted-space)) {
      ratioHeight = (double)(drawHeight-numWidth-space-moveWidthPrinted-space)/(double)totalHeight;
    }
    if (totalWidth > (drawWidth-numHeight-space)) {
      ratioWidth = (double)(drawWidth-numHeight-space)/(double)totalWidth;
    }
  }
  else {
    if (totalHeight > (drawHeight-numHeight-space-moveWidthPrinted-space)) {
      ratioHeight = (double)(drawHeight-numHeight-space-moveWidthPrinted-space)/(double)totalHeight;
    }
    if (totalWidth > (drawWidth-numWidth-space)) {
      ratioWidth = (double)(drawWidth-numWidth-space)/(double)totalWidth;
    }
  }
  double ratio;
  if (ratioHeight < ratioWidth) {
    ratio = ratioHeight;
  }
  else {
    ratio = ratioWidth;
  }
  moveWidthPrinted = (int)(moveWidthPrinted*ratio);
  int twistWidthPrinted = (int)((ratio*(params->exportTwistWidth)*dpi)/2.54);

  // adjust font size;
  int fontSize = fontNum.pointSize();
  while ((fontSize > 0) && (numHeight > moveWidthPrinted)) {
    fontSize -=1;
    fontNum.setPointSize(fontSize);
    p->setFont(fontNum);
    fmNum = p->fontMetrics();
    numHeight = fmNum.height();
    numWidth = fmNum.width("55");    
  }

  if (isPortrait) {
    totalWidth = (int)(ratio*totalWidth) + numHeight + space + 1;
    totalHeight = (int)(ratio*totalHeight) + numWidth + space + 1;
  }
  else {
    totalWidth = (int)(ratio*totalWidth) + numWidth + space + 1;
    totalHeight = (int)(ratio*totalHeight) + numHeight + space + 1;
  }

  QPen pen(Qt::black);
  pen.setWidth(2);
  p->setPen(pen);
  QBrush coloredPick(Qt::black);

  quint8 *lstMoves;
  quint8 pick;
  QString num;
  int x,y,xStart,yStart;
  quint32 i,j;

  // paint legend
  QString fw = tr("Forward");
  QString bw = tr("Backward");
  QString id = tr("Idle");
  QString tw = tr("F/B/I + Twist");
  QString no = tr("Nothing defined [= conception problem]");
  int fWidth = fmNum.width(fw);
  int bWidth = fmNum.width(bw);
  int iWidth = fmNum.width(id);
  int tWidth = fmNum.width(tw);
  int nWidth = fmNum.width(no);

  x = marginLeft;
  y = paperSize.height()-marginBottom-moveWidthPrinted;

  coloredPick.setColor(Qt::white);
  p->setBrush(coloredPick);  
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,fw);
  x += (fWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::black);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,bw);
  x += (bWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::green);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,id);
  x += (iWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::white);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  coloredPick.setColor(Qt::red);
  p->setBrush(coloredPick);
  p->drawRect(x+(moveWidthPrinted-twistWidthPrinted)/2,y+(moveWidthPrinted-twistWidthPrinted)/2,twistWidthPrinted,twistWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,tw);
  x += (tWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::lightGray);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,no);

  if (totalWidth < x+nWidth) {
    totalWidth = x+nWidth;
  }

  // paint pick number
  p->save();

  if (isPortrait) {
    xStart = marginLeft+numHeight+space;
    yStart = paperSize.height() - marginBottom - moveWidthPrinted - space - totalHeight ;
    p->translate(xStart,yStart);
    p->rotate(90.0);
  }
  else {
    xStart = marginLeft;
    yStart = paperSize.height() - marginBottom - moveWidthPrinted - space - numHeight - space;
    p->translate(xStart,yStart);
  }
  for(i=0;i<params->patternLength;i++) {
    x= 0;
    y = -i*moveWidthPrinted;
    num.setNum(i+1);
    p->drawText(x,y-(moveWidthPrinted-numHeight)/2, num);
  }
  p->restore();

  // paint warp number
  p->save();

  if (isPortrait) {
    xStart = marginLeft;
    yStart = paperSize.height() - marginBottom - moveWidthPrinted - space - totalHeight + numWidth + space ;
    p->translate(xStart,yStart);
    p->rotate(90.0);
  }
  else {
    xStart = marginLeft + numWidth + space;
    yStart = paperSize.height() - marginBottom - moveWidthPrinted - space;
    p->translate(xStart,yStart);
  }

  for(i=0;i<params->bandWidth;i++) {

    x = i*moveWidthPrinted;
    y = 0;
    num.setNum(i+1);
    p->drawText(x+(moveWidthPrinted-numWidth)/2,y,num);
  }
  p->restore();

  // paint move squares

  p->save();
  if (isPortrait) {
    xStart = marginLeft + numHeight+space;
    yStart = paperSize.height() - marginBottom - moveWidthPrinted - space - totalHeight + numWidth + space ;
    p->translate(xStart,yStart);
    p->rotate(90.0);
  }
  else {
    xStart = marginLeft + numWidth + space;
    yStart = paperSize.height() - marginBottom - moveWidthPrinted - space - numHeight - space;
    p->translate(xStart,yStart);
  }

  for(i=0;i<params->bandWidth;i++) {

    if ((params->mount).at(i) == S_MOUNT) {
      lstMoves = startInS.at(i);
    }
    else {
      lstMoves = startInZ.at(i);
    }

    for(j=0;j<(params->patternLength);j++) {

      pick = lstMoves[j];

      if (pick == NO_MOVE) {
        p->setBrush(Qt::lightGray);
      }
      else if (PICK_MOVE(pick) == FORWARD){
        coloredPick.setColor(Qt::white);
        p->setBrush(coloredPick);
      }
      else if (PICK_MOVE(pick) == BACKWARD){
        coloredPick.setColor(Qt::black);
        p->setBrush(coloredPick);
      }
      else if (PICK_MOVE(pick) == IDLE){
        coloredPick.setColor(Qt::green);
        p->setBrush(coloredPick);
      }
      pen.setWidth(2);
      p->setPen(pen);

      p->drawRect(i*moveWidthPrinted,-(j+1)*moveWidthPrinted,moveWidthPrinted,moveWidthPrinted);

      if ((pick != NO_MOVE) && (PICK_TWIST(pick) == TWIST)) {
        pen.setWidth(1);
        p->setPen(pen);
        coloredPick.setColor(Qt::red);
        p->setBrush(coloredPick);
        p->drawRect(i*moveWidthPrinted+(moveWidthPrinted-twistWidthPrinted)/2,-(j+1)*moveWidthPrinted+(moveWidthPrinted-twistWidthPrinted)/2,twistWidthPrinted,twistWidthPrinted);
      }
    }
  }
  p->restore();

  QRect boundingBox(marginLeft,marginUp,0,0);

  totalHeight += moveWidthPrinted + space + 20;

  boundingBox.setTop(paperSize.height()-marginBottom-totalHeight+10);
  boundingBox.setHeight(totalHeight);
  boundingBox.setWidth(totalWidth);

  return boundingBox;  
}
*/

QRect MovesModel::draw(QPainter *p) {

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

  int moveWidthPrinted = (int)(((params->exportMoveWidth)*dpi)/2.54);

  /* if there are some warps that need to be not cyclic, the sides
     are shown for the all band length and not just the pattern length
     */
  quint32 i,j;
  bool isNotCyclic = false;
  for(i=0;(i<params->bandWidth) && (isNotCyclic == false);i++) {
    if (params->cyclicCardPosition.at(i) == NOCYCLIC) {
      isNotCyclic = true;
    }
  }

  bool isPortrait = false;
  /* in case of really long patterns, the moves are shown in portrait mode
     else in landscape (legend is always in landscape
     */
  int totalHeight = 0;
  int totalWidth = 0;

  if (!isNotCyclic) {
    if (params->patternLength < params->bandWidth) {
      totalHeight = (params->patternLength)*moveWidthPrinted;
      totalWidth = (params->bandWidth)*moveWidthPrinted;
    }
    else {
      totalWidth = (params->patternLength)*moveWidthPrinted;
      totalHeight = (params->bandWidth)*moveWidthPrinted;
      isPortrait = true;
    }
  }
  else {
    if ((params->bandLength-1) < params->bandWidth) {
      totalHeight = (params->bandLength -1)*moveWidthPrinted;
      totalWidth = (params->bandWidth)*moveWidthPrinted;
    }
    else {
      totalWidth = (params->bandLength-1)*moveWidthPrinted;
      totalHeight = (params->bandWidth)*moveWidthPrinted;
      isPortrait = true;
    }

  }

  int imgWidth = (int)(2.0*dpi/2.54);
  QImage imgType;
  if (params->numberingType == CLOCKWISE) {    
    imgType = QImage(":/icons/card-side-hor-big.png").scaledToWidth(imgWidth);
  }
  else {
    imgType = QImage(":/icons/card-side-antihor-big.png").scaledToWidth(imgWidth);
  }
  int imgHeight = imgType.height();

  double ratioHeight = 1.0;
  double ratioWidth = 1.0;
  /* in case of really long patterns, the moves are shown in portrait mode
     else in landscape
     */
  if (isPortrait) {
    if (totalHeight > (drawHeight-imgHeight-space - space - numWidth)) {
      ratioHeight = (double)(drawHeight-numWidth-space-imgHeight-space)/(double)totalHeight;
    }
    if (totalWidth > (drawWidth-numHeight-space)) {
      ratioWidth = (double)(drawWidth-numHeight-space)/(double)totalWidth;
    }
  }
  else {
    if (totalHeight > (drawHeight-numHeight-space-imgHeight-space)) {
      ratioHeight = (double)(drawHeight-numHeight-space-imgHeight-space)/(double)totalHeight;
    }
    if (totalWidth > (drawWidth-numWidth-space)) {
      ratioWidth = (double)(drawWidth-numWidth-space)/(double)totalWidth;
    }
  }
  double ratio;
  if (ratioHeight < ratioWidth) {
    ratio = ratioHeight;
  }
  else {
    ratio = ratioWidth;
  }
  moveWidthPrinted = (int)(moveWidthPrinted*ratio);

  // adjust font size;
  int fontSize = fontNum.pointSize();
  while ((fontSize > 0) && (numHeight > moveWidthPrinted)) {
    fontSize -=1;
    fontNum.setPointSize(fontSize);
    p->setFont(fontNum);
    fmNum = p->fontMetrics();
    numHeight = fmNum.height();
    numWidth = fmNum.width("55");
  }

  if (isPortrait) {
    totalWidth = (int)(ratio*totalWidth) + numHeight + space + 1;
    totalHeight = (int)(ratio*totalHeight) + numWidth + space + 1;
  }
  else {
    totalWidth = (int)(ratio*totalWidth) + numWidth + space + 1;
    totalHeight = (int)(ratio*totalHeight) + numHeight + space + 1;
  }

  QPen pen(Qt::black);
  pen.setWidth(2);
  p->setPen(pen);
  QBrush coloredPick(Qt::black);

  quint8 *lstMoves;
  quint8 pick;
  QString num;
  int x,y,xStart,yStart;  

  // paint image and legend
  x = marginLeft;
  y = paperSize.height()-marginBottom-imgHeight;
  p->drawImage(x,y,imgType);

  x += imgType.width()+space;
  y += imgHeight;
  QString txtNumbering = tr("Side A-B is numbered 1, side B-C is numbered 2, and so on.");
  int sWidth = fmNum.width(txtNumbering);
  p->drawText(x,y,txtNumbering);
  x += sWidth + space;
  y -= moveWidthPrinted;

  QString fw = tr("Forward");
  QString bw = tr("Backward");
  QString id = tr("Idle");
  QString un = tr("Undefined");
  int fWidth = fmNum.width(fw);
  int bWidth = fmNum.width(bw);
  int iWidth = fmNum.width(id);
  int uWidth = fmNum.width(un);

  coloredPick.setColor(Qt::white);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,fw);
  x += (fWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::gray);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,bw);
  x += (bWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::black);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,id);
  x += (iWidth + 2*moveWidthPrinted);

  coloredPick.setColor(Qt::white);
  p->setBrush(coloredPick);
  p->drawRect(x,y,moveWidthPrinted,moveWidthPrinted);
  p->drawLine(x,y,x+moveWidthPrinted,y+moveWidthPrinted);
  p->drawLine(x+moveWidthPrinted,y,x,y+moveWidthPrinted);
  x += moveWidthPrinted+space;
  p->drawText(x,y+moveWidthPrinted,un);
  x += (uWidth + moveWidthPrinted);

  if (totalWidth < x) {
    totalWidth = x;
  }

  quint32 end;
  if (isNotCyclic) {
    end = params->bandLength -1;
  }
  else {
    end = params->patternLength;
  }

  // paint pick number
  p->save();

  if (isPortrait) {
    xStart = marginLeft+numHeight+space;
    yStart = paperSize.height() - marginBottom - imgHeight - space - totalHeight ;
    p->translate(xStart,yStart);
    p->rotate(90.0);
  }
  else {
    xStart = marginLeft;
    yStart = paperSize.height() - marginBottom - imgHeight - space - numHeight - space;
    p->translate(xStart,yStart);
  }

  for(i=0;i<end;i++) {
    x= 0;
    y = -i*moveWidthPrinted;
    num.setNum(i+1);
    p->drawText(x,y-(moveWidthPrinted-numHeight)/2, num);
  }
  p->restore();

  // paint warp number
  p->save();

  if (isPortrait) {
    xStart = marginLeft;
    yStart = paperSize.height() - marginBottom - imgHeight - space - totalHeight + numWidth + space ;
    p->translate(xStart,yStart);
    p->rotate(90.0);
  }
  else {
    xStart = marginLeft + numWidth + space;
    yStart = paperSize.height() - marginBottom - imgHeight - space;
    p->translate(xStart,yStart);
  }

  for(i=0;i<params->bandWidth;i++) {

    x = i*moveWidthPrinted;
    y = 0;
    num.setNum(i+1);
    p->drawText(x+(moveWidthPrinted-numWidth)/2,y,num);
  }
  p->restore();

  // paint side squares and bold lines
  quint8 side;
  quint8 decal;
  QString txtSide;
  p->save();
  if (isPortrait) {
    xStart = marginLeft + numHeight+space;
    yStart = paperSize.height() - marginBottom - imgHeight - space - totalHeight + numWidth + space ;
    p->translate(xStart,yStart);
    p->rotate(90.0);
  }
  else {
    xStart = marginLeft + numWidth + space;
    yStart = paperSize.height() - marginBottom - imgHeight - space - numHeight - space;
    p->translate(xStart,yStart);
  }

  for(i=0;i<params->bandWidth;i++) {

    if ((params->mount).at(i) == S_MOUNT) {
      lstMoves = startInS.at(i);
    }
    else {
      lstMoves = startInZ.at(i);
    }
    decal = 0;
    for(j=0;j<end;j++) {

      pick = lstMoves[j % params->patternLength];

      if ((pick == NO_MOVE) || (PICK_MOVE(pick) == FORWARD)) {
        coloredPick.setColor(Qt::white);
      }
      else if (PICK_MOVE(pick) == BACKWARD) {
        coloredPick.setColor(Qt::gray);
      }
      else if (PICK_MOVE(pick) == IDLE) {
        coloredPick.setColor(Qt::black);
      }
      p->setBrush(coloredPick);

      pen.setWidth(2);
      p->setPen(pen);

      p->drawRect(i*moveWidthPrinted,-(j+1)*moveWidthPrinted,moveWidthPrinted,moveWidthPrinted);
      if (pick == NO_MOVE) {
        p->drawLine(i*moveWidthPrinted,-(j+1)*moveWidthPrinted,(i+1)*moveWidthPrinted,-j*moveWidthPrinted);
        p->drawLine((i+1)*moveWidthPrinted,-(j+1)*moveWidthPrinted,i*moveWidthPrinted,-j*moveWidthPrinted);
      }

      side = PICK_SIDE(pick);
      if (side != 0) {

        if (params->numberingType == CLOCKWISE) {
          // must change the side number as if the cards where all twisted
          if (side != params->nbHoles) {
            side = params->nbHoles - side;
          }
        }        
        // apply decal, which is <= 0
        side = (params->nbHoles + side + decal) % params->nbHoles;
        if (side == 0) {
          side = params->nbHoles;
        }
        txtSide.setNum(side);
        p->drawText(i*moveWidthPrinted + ((moveWidthPrinted-numWidth)/2),(-j*moveWidthPrinted) - ((moveWidthPrinted-numHeight)/2),txtSide);
      }
      if ((j+1)%params->patternLength == 0) {
        decal = side - params->nbHoles;
      }
    }

    // paint bold lines

    if ( (params->printVertSep) && ((params->movesVertSep).contains(i)) ) {
      pen.setWidth(8);
      p->setPen(pen);
      p->drawLine(i*moveWidthPrinted,0,i*moveWidthPrinted,-end*moveWidthPrinted);
    }
  }
  if (params->printVertSep) {
    pen.setWidth(8);
    p->setPen(pen);
    p->drawLine(0,0,0,-end*moveWidthPrinted);
    p->drawLine(params->bandWidth*moveWidthPrinted,0,params->bandWidth*moveWidthPrinted,-end*moveWidthPrinted);
    p->drawLine(0,0,params->bandWidth*moveWidthPrinted,0);
    p->drawLine(0,-end*moveWidthPrinted,params->bandWidth*moveWidthPrinted,-end*moveWidthPrinted);
  }


  p->restore();

  QRect boundingBox(marginLeft,marginUp,0,0);

  totalHeight += imgHeight + space + 20;

  boundingBox.setTop(paperSize.height()-marginBottom-totalHeight+10);
  boundingBox.setHeight(totalHeight);
  boundingBox.setWidth(totalWidth);

  return boundingBox;
}
