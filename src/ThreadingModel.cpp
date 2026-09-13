/*-==============================================================-

file : ThreadingModel.cpp

creation date : 27/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description :

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "ThreadingModel.hpp"

ThreadingModel::ThreadingModel(ProjectParams *_params) : QObject() {

  params = _params;

  quint8 *lst;
  quint32 i;

  for(i=0;i<(params->bandWidth);i++) {
    lst = new quint8[params->nbHoles];
    memset(lst,0,params->nbHoles);
    cardsInS.append(lst);
  }
  for(i=0;i<(params->bandWidth);i++) {
    lst = new quint8[params->nbHoles];
    memset(lst,0,params->nbHoles);
    cardsInZ.append(lst);
  }
  for(i=0;i<(params->bandWidth);i++) {
    lst = new quint8[params->nbHoles];
    memset(lst,0,params->nbHoles);
    cardsForMoveMode.append(lst);
  }
  //connect(params,SIGNAL(nbHolesChanged()),this,SLOT(updateForNewNbHoles()));
  //connect(params,SIGNAL(patternLengthChanged()),this,SLOT(updateForNewPatternLength()));
}

ThreadingModel::~ThreadingModel() {
  clear();
}


void ThreadingModel::clear() {

  quint8 *lst;
  QListIterator<quint8 *> iterS(cardsInS);

  while (iterS.hasNext()) {
    lst = iterS.next();    
    delete [] lst;
  }
  cardsInS.clear();

  QListIterator<quint8 *> iterZ(cardsInZ);

  while (iterZ.hasNext()) {
    lst = iterZ.next();    
    delete [] lst;
  }
  cardsInZ.clear();

  QListIterator<quint8 *> iterAny(cardsForMoveMode);

  while (iterAny.hasNext()) {
    lst = iterAny.next();
    delete [] lst;
  }
  cardsForMoveMode.clear();

}

void ThreadingModel::removeCard(int cardId, bool sendSig) {
  quint8 *lst;
  lst = cardsInS.at(cardId);
  delete [] lst;
  cardsInS.removeAt(cardId);

  lst = cardsInZ.at(cardId);
  delete [] lst;
  cardsInZ.removeAt(cardId);

  lst = cardsForMoveMode.at(cardId);
  delete [] lst;
  cardsForMoveMode.removeAt(cardId);


  if (sendSig) {
    emit allThreadingsChanged();
  }
}

void ThreadingModel::insertCard(int cardId, bool sendSig) {
  quint8 *lst;

  lst = new quint8[params->nbHoles];
  memset(lst,0,params->nbHoles);
  cardsInS.insert(cardId,lst);

  lst = new quint8[params->nbHoles];
  memset(lst,0,params->nbHoles);
  cardsInZ.insert(cardId,lst);  

  lst = new quint8[params->nbHoles];
  memset(lst,0,params->nbHoles);
  cardsForMoveMode.insert(cardId,lst);

  if (sendSig) {
    emit allThreadingsChanged();
  }

}

void ThreadingModel::copyCard(int cardIdSrc, int cardIdDest) {

  quint8 *lstAnySrc;
  quint8 *lstAnyDest;
  lstAnySrc = cardsForMoveMode.at(cardIdSrc);
  lstAnyDest = cardsForMoveMode.at(cardIdDest);
  memcpy(lstAnyDest,lstAnySrc,params->nbHoles);

  emit threadingChanged(cardIdDest);
}

void ThreadingModel::swapCard(int warpId1, int warpId2) {

  if (params->createMode == MODE_FROMBAND) {
    cardsInS.swap(warpId1,warpId2);
    cardsInZ.swap(warpId1,warpId2);
  }
  else if (params->createMode == MODE_FROMMOVES) {
    cardsForMoveMode.swap(warpId1,warpId2);
  }
  emit threadingChanged(warpId1);
  emit threadingChanged(warpId2);

}

void ThreadingModel::updateForNewCreationMode() {

  quint32 i,j;
  quint8 *lstS;
  quint8 *lstZ;
  quint8 *lstAny;

  if (params->createMode == MODE_FROMMOVES) {

    for(i=0;i<params->bandWidth;i++) {
      lstS = cardsInS.at(i);
      lstZ = cardsInZ.at(i);
      lstAny = cardsForMoveMode.at(i);

      for(j=0;j<params->nbHoles;j++) {

	if ((params->mount).at(i) == S_MOUNT) {
	  lstAny[j] = lstS[j];
	}
	else {
	  lstAny[j] = lstZ[j];
	}
      }
    }
  }
  else {
    for(i=0;i<params->bandWidth;i++) {
      lstS = cardsInS.at(i);
      lstZ = cardsInZ.at(i);
      lstAny = cardsForMoveMode.at(i);

      for(j=0;j<params->nbHoles;j++) {

	if ((params->mount).at(i) == S_MOUNT) {
	  lstS[j] = lstAny[j];
	  lstZ[j] = lstAny[params->nbHoles-1-j];
	}
	else {
	  lstZ[j] = lstAny[j];
	  lstS[j] = lstAny[params->nbHoles-1-j];
	}
      }
    }
  }
}

void ThreadingModel::updateForNewNbHoles() {


  emit allThreadingsChanged();
}

void ThreadingModel::voidCardThreading(int cardId) {
  quint8 *lstS;
  quint8 *lstZ;

  lstS = cardsInS.at(cardId);
  memset(lstS,0,params->nbHoles);
  lstZ = cardsInZ.at(cardId);
  memset(lstZ,0,params->nbHoles);

  // signal to the view
  emit threadingChanged(cardId);
}

bool ThreadingModel::compareSColors(int cardId, quint16 threading) {
  bool ret = true;
  quint8 *lstS = NULL;
  int i;

  // extract nbColor
  quint8 nbColor = (threading & 57344) >> 13;
  // remove nbColor from threading
  threading = threading & 8191;

  int nbHoles = params->nbHoles;
  quint8 *allPoss;
  quint8 *colors = (params->idToColors).at(cardId);
  allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];

  lstS = cardsInS.at(cardId);

  for(i=0;(i<nbHoles) && (ret);i++) {
    if (lstS[i] != colors[allPoss[threading*nbHoles+i]]) {
      ret = false;
    }
  }
  return ret;
}

bool ThreadingModel::compareZColors(int cardId, quint16 threading) {
  bool ret = true;
  quint8 *lstZ = NULL;
  int i;

  // extract nbColor
  quint8 nbColor = (threading & 57344) >> 13;
  // remove nbColor from threading
  threading = threading & 8191;

  int nbHoles = params->nbHoles;
  quint8 *allPoss;
  quint8 *colors = (params->idToColors).at(cardId);
  allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];

  lstZ = cardsInZ.at(cardId);

  for(i=0;(i<nbHoles) && (ret);i++) {
    if (lstZ[i] != colors[allPoss[threading*nbHoles+i]]) {
      ret = false;
    }
  }
  return ret;
}

void ThreadingModel::updateCardThreading(int cardId, QList<quint16> & lstThreadingForS, QList<quint16> & lstThreadingForZ) {

  int i;
  quint8 nbColor;
  quint8 *lstS;
  quint8 *lstZ;
  nbColor = (params->nbColors).at(cardId);
  lstS = cardsInS.at(cardId);
  lstZ = cardsInZ.at(cardId);

  // abnormal case but ...
  if (nbColor == 0) {
#ifdef DEBUG_PRINT
    cout << "pb : updateCardThreading is called when nbColor = 0" << endl;
#endif
    memset(lstS,0,params->nbHoles);
    memset(lstZ,0,params->nbHoles);
  }
  else {

    QList<quint16> possForS = lstThreadingForS;
    QList<quint16> possForZ = lstThreadingForZ;   
    int nbHoles = params->nbHoles;
    quint8 *allPoss = NULL;
    quint8 *idToColors = (params->idToColors).at(cardId);

    quint16 idThreadingForS,idThreadingForZ;

    bool similar;
    quint16 poss;

    similar = false;
    for(i=0;(i<possForS.size()) && (similar == false);i++) {
      poss = possForS.at(i);
      similar = compareSColors(cardId,poss);
    }
    if (similar) {      
      idThreadingForS = poss;
    }
    else if (possForS.isEmpty()) {
      idThreadingForS = 65535;
      memset(lstS,0,params->nbHoles);
    }
    else {
      idThreadingForS = possForS.at(0);
    }

    similar = false;
    for(i=0;(i<possForZ.size()) && (similar == false);i++) {
      poss = possForZ.at(i);
      similar = compareZColors(cardId,poss);
    }
    if (similar) {
      idThreadingForZ = poss;
    }
    else if (possForZ.isEmpty()) {
      idThreadingForZ = 65535;
      memset(lstZ,0,params->nbHoles);
    }
    else {
      idThreadingForZ = possForZ.at(0);
    }

    if (idThreadingForS != 65535) {

      nbColor = idThreadingForS >> 13;
      idThreadingForS = idThreadingForS & 8191;
      allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];

      for(i=0;i<nbHoles;i++) {
        lstS[i] = idToColors[allPoss[idThreadingForS*nbHoles+i]];
      }

      // update nbMaxColors
      (params->nbMaxColors).replace(cardId,nbColor);
    }


    if (idThreadingForZ != 65535) {

      nbColor = idThreadingForZ >> 13;
      idThreadingForZ = idThreadingForZ & 8191;
      allPoss = ((params->threadPerm)->holesToColor)[nbHoles-3][nbColor-1];

      for(i=0;i<nbHoles;i++) {
        lstZ[i] = idToColors[allPoss[idThreadingForZ*nbHoles+i]];
      }

      // update nbMaxColors
      (params->nbMaxColors).replace(cardId,nbColor);
    }

  }
  // signal to the view
  emit threadingChanged(cardId);
}

void ThreadingModel::updateHoleColor(int cardId, quint8 holeId) {
  quint8 *lst;
  quint8 lstColor[PALETTE_SIZE+1];
  quint8 numHole;  
  quint8 *idToColors;
  quint32 i;
  quint8 nbColor;

  lst = cardsForMoveMode.at(cardId);

  numHole = holeId;

  if (params->numberingType == CLOCKWISE) {

    /* since threadings are computed for anticlockwise and it is only
       the view that swap the thread, a click in the view in clockwise type
       means that in reality, the user has click on the swapped hole and not on the real
       hole
       */
    numHole = params->nbHoles-1-holeId;
  }

  // if there is a change
  if (lst[numHole] != params->currColor +1) {

    // setting the new color
    lst[numHole] = params->currColor +1;

    // update idToColor
    memset(lstColor,0,PALETTE_SIZE+1);
    idToColors = (params->idToColors).at(cardId);
    memset(idToColors,0,PALETTE_SIZE+1);

    for(i=0;i<params->nbHoles;i++) {
      lstColor[lst[i]] += 1;
    }
    nbColor = 0;
    for(i=1;i<=PALETTE_SIZE;i++) {
      if (lstColor[i] != 0) {
        nbColor+=1;
        /* idToColors contains the id+1 in params->palette of the color n° nbColor in the threading
           with nbColor >= 1 and <= params->nbHoles. It means that idToColors[0] is never changed and
           that we must put i in idToColors.
           */
        idToColors[nbColor] = i;
      }
    }
    (params->nbColors).replace(cardId,nbColor);
    (params->nbMaxColors).replace(cardId,nbColor);

    // signal to the view
    emit threadingChanged(cardId);

    // signal to the dispatcher
    /* NOTE : it finally calls updateHoleColor() in BandModel that search for picks
     with the hole letter that just been clicked. Thus, it is holeId and not numHole.
     Nevertheless, in clockwise type, the color is still in lst[numHole]
     */
    emit holeColorChanged(cardId, holeId, lst[numHole]);
  }
}



QDataStream &operator<<(QDataStream &out, const ThreadingModel &t) {

  QByteArray block;
  QDataStream toWrite(&block, QIODevice::WriteOnly);
  toWrite.setVersion(QDataStream::Qt_4_0);

  quint32 i,j;
  quint8 *threading;

  for(i=0;i<(t.params)->bandWidth;i++) {
    threading = t.cardsInS.at(i);
    for(j=0;j<(t.params)->nbHoles;j++) {
      toWrite << threading[j];
    }
    threading = t.cardsInZ.at(i);
    for(j=0;j<(t.params)->nbHoles;j++) {
      toWrite << threading[j];
    }
  }

  out << block;
  return out;
}

QDataStream &operator>>(QDataStream &in, ThreadingModel &t) {

  if (((t.params)->cmpVersionMaj >= 0) && ((t.params)->cmpVersionMin >= 4)) {

    quint32 blockSize;
    in.setVersion(QDataStream::Qt_4_0);

    in >> blockSize;

    quint32 i,j;
    quint8 *threading;
    for(i=0;i<(t.params)->bandWidth;i++) {
      threading = t.cardsInS.at(i);
      for(j=0;j<(t.params)->nbHoles;j++) {
	in >> threading[j];
      }
      threading = t.cardsInZ.at(i);
      for(j=0;j<(t.params)->nbHoles;j++) {
	in >> threading[j];
      }
    }
  }
  return in;
}

QRect ThreadingModel::draw(QPainter *p) {

  //QPrinter *printer = (QPrinter *)(p->device());
  QPaintDevice *pd= p->device();

  QFont fontNum("Arial",7);
  p->setFont(fontNum);
  QFontMetrics fmNum = p->fontMetrics();
  int numWidth = fmNum.width("55");
  int numHeight = fmNum.height();

  QFont fontSZ("Arial",10);
  p->setFont(fontSZ);
  QFontMetrics fmSZ = p->fontMetrics();
  int szWidth = fmSZ.width("Z");
  int szHeight = fmSZ.height();

  //int dpi = printer->resolution();
  int dpi = pd->logicalDpiX();
  //QRect paperSize = printer->paperRect();
  QRect paperSize(0,0,pd->width(),pd->height());

  int marginUp = (int)(((params->exportMarginUp)*dpi)/2.54);

  int marginLeft = (int)(((params->exportMarginLeft)*dpi)/2.54);
  int marginRight = (int)(((params->exportMarginRight)*dpi)/2.54);
  int space = (int)(((params->exportSpace)*dpi)/2.54);

  int drawWidth = paperSize.width() - marginLeft - marginRight;

  int threadWidthPrinted = (int)(((params->exportThreadWidth)*dpi)/2.54);

  int totalWidth = (params->bandWidth)*threadWidthPrinted;

  double ratio = 1.0;

  if (totalWidth > (drawWidth-szWidth-space)) {
    ratio = (double)(drawWidth-szWidth-space)/(double)totalWidth;
  }
  threadWidthPrinted = (int)(threadWidthPrinted*ratio);

  // adjust font size;
  int fontSize = fontNum.pointSize();
  while ((fontSize > 0) && (numHeight > threadWidthPrinted)) {
    fontSize -=1;
    fontNum.setPointSize(fontSize);
    p->setFont(fontNum);
    fmNum = p->fontMetrics();
    numHeight = fmNum.height();
    numWidth = fmNum.width("55");
  }
  fontSize = fontSZ.pointSize();
  while ((fontSize > 0) && ((szHeight > threadWidthPrinted) ||(szWidth > threadWidthPrinted))) {
    fontSize -=1;
    fontSZ.setPointSize(fontSize);
    p->setFont(fontSZ);
    fmSZ = p->fontMetrics();
    szHeight = fmSZ.height();
    szWidth = fmSZ.width("55");
  }

  int totalHeight = numHeight+space+(int)(ratio*params->nbHoles*threadWidthPrinted)+space+szHeight;

  QPen pen(Qt::black);
  pen.setWidth(2);
  p->setPen(pen);
  QBrush coloredPick(Qt::black);

  quint8* lstHoles;
  quint8 holeColor;
  QString num;
  int x,y,xStart,yStart;
  quint32 i,j;

  // paint hole numbering
  pen.setWidth(1);
  p->setPen(pen);

  p->setFont(fontSZ);
  char firstHole = 'A';
  QString holeName("A");
  if (params->numberingOrder == DESCENDING) {
    firstHole += params->nbHoles-1;
    holeName = firstHole;
  }
  yStart = marginUp+numHeight+space;
  xStart = marginLeft;
  for(i=0;i<(params->nbHoles);i++) {
    x = xStart;
    y = yStart + (i+1)*threadWidthPrinted;
    p->drawText(x,y-(threadWidthPrinted-szHeight)/2,holeName);
    if (params->numberingOrder == ASCENDING) {
      firstHole += 1;
    }
    else {
      firstHole -= 1;
    }
    holeName = firstHole;
  }

  // paint numbers, thread and S/Z
  yStart = marginUp;
  xStart = marginLeft+szWidth+space;
  for(i=0;i<params->bandWidth;i++) {

    pen.setWidth(1);
    p->setPen(pen);
    p->setFont(fontNum);
    // paint warp umber
    x = xStart + i*threadWidthPrinted;
    y = yStart + numHeight;
    num.setNum(i+1);
    p->drawText(x+(threadWidthPrinted-numWidth)/2,y,num);

    y += space;

    if ((params->mount).at(i) == S_MOUNT) {
      lstHoles = cardsInS.at(i);
    }
    else {
      lstHoles = cardsInZ.at(i);
    }

    pen.setWidth(2);
    p->setPen(pen);

    for(j=0;j<params->nbHoles;j++) {
      // if numberingType is anti-clockwise lstHoles is in the correct order
      if (params->numberingType == ANTICLOCKWISE) {

	// guntram like : anti-clockwise and from A to D numbering
	if (params->numberingOrder == ASCENDING) {
	  holeColor = lstHoles[j];
	}
	// semi guntram : anti-clockwise but from D to A numbering
	else {
	  holeColor = lstHoles[params->nbHoles-1-j];
	}
      }
      else {
	// not lik guntram but get the same
	if (params->numberingOrder == DESCENDING) {
	  holeColor = lstHoles[j];
	}
	else {
	  holeColor = lstHoles[params->nbHoles-1-j];
	}
      }
      if (holeColor == 0) {
	p->setBrush(Qt::Dense7Pattern);
      }
      else {
	coloredPick.setColor((params->palette).at(holeColor-1));
	p->setBrush(coloredPick);
      }
      p->drawRect(x,y,threadWidthPrinted,threadWidthPrinted);

      y += threadWidthPrinted;
    }
    y += space;

    pen.setWidth(1);
    p->setPen(pen);

    // paint S/Z
    p->setFont(fontSZ);
    if ((params->mount).at(i) == S_MOUNT) {
      p->drawText(x+(threadWidthPrinted-szWidth)/2,y+szHeight,"S");
    }
    else {
      p->drawText(x+(threadWidthPrinted-szWidth)/2,y+szHeight,"Z");
    }
  }

  pen.setWidth(0);
  p->setPen(pen);

  if (params->numberingOrder == DESCENDING) {

    yStart += (params->nbHoles)*threadWidthPrinted + space + szHeight + space+ 2*threadWidthPrinted;
    xStart = marginLeft;
    p->setPen(QPen(Qt::red));
    fontSZ.setWeight(QFont::Bold);
    p->drawText(xStart,yStart,tr("CAUTION !"));
    xStart += fmSZ.width(tr("CAUTION !"));
    p->setPen(pen);
    fontSZ.setWeight(QFont::Normal);
    p->drawText(xStart+space,yStart,tr("card letterings are in decreasing order."));

    yStart += 2*threadWidthPrinted;
    xStart = marginLeft;
    totalHeight += space+4*threadWidthPrinted;
  }
  else {
    yStart += (params->nbHoles)*threadWidthPrinted + space + szHeight + space + 3*threadWidthPrinted;
    xStart = marginLeft;
    totalHeight += space+3*threadWidthPrinted;
  }
  int imgWidth = (int)(5.0*dpi/2.54);
  if (params->numberingType == CLOCKWISE) {
    QString txt = tr("card holes are numbered in a clockwise fashion, with A starting up and toward the weaver.");
    QImage imgType = QImage(":/icons/card-mount-hor-big.png").scaledToWidth(imgWidth);
    p->drawImage(xStart,yStart,imgType);
    p->drawText(xStart+imgWidth+space,yStart+3*szHeight,txt);
    totalHeight += imgType.height();
    if (totalWidth < xStart+imgWidth+space+fmSZ.width(txt)) {
      totalWidth = xStart+imgWidth+space+fmSZ.width(txt);
    }
  }
  else {
    QString txt=tr("card holes are numbered in a anti-clockwise fashion, with A starting up and toward unwoven warps (like guntram).");
    QImage imgType = QImage(":/icons/card-mount-antihor-big.png").scaledToWidth(imgWidth);
    p->drawImage(xStart,yStart,imgType);
    p->drawText(xStart+imgWidth+space,yStart+3*szHeight,txt);
    totalHeight += imgType.height();
    if (totalWidth < xStart+imgWidth+space+fmSZ.width(txt)) {
      totalWidth = xStart+imgWidth+space+fmSZ.width(txt);
    }
  }

  QRect boundingBox(marginLeft,marginUp,0,0);

  totalWidth = (int)(ratio*totalWidth) + szWidth + space + 1;
  boundingBox.setHeight(totalHeight);
  boundingBox.setWidth(totalWidth);

  return boundingBox;
}


