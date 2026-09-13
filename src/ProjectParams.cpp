/*-==============================================================-

file : ProjectParams.cpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#include "ProjectParams.hpp"

quint8 ProjectParams::versionMaj = VERSION_MAJ;
quint8 ProjectParams::versionMin = VERSION_MIN;
quint8 ProjectParams::revision = REVISION;

ProjectParams::ProjectParams() {
  threadPerm = new CardThreading();
  clear();
}

ProjectParams::~ProjectParams() {
  quint8* lst;

  delete threadPerm;
  if (! completedWarps.isEmpty()) {

    QListIterator<quint8 *> iter(completedWarps);
    // first, clear completedWarps
    while (iter.hasNext()) {
      lst = iter.next();
      if (lst != NULL) delete [] lst;
    }
    completedWarps.clear();
    isWarpComplete.clear();
    nbColors.clear();
    nbMaxColors.clear();
    mount.clear();
    cyclicCardPosition.clear();
  }

  if (! idToColors.isEmpty()) {
    QListIterator<quint8 *> iter(idToColors);
    while (iter.hasNext()) {
      lst = iter.next();
      if (lst != NULL) delete [] lst;
    }
    idToColors.clear();
  }
}

void ProjectParams::clear() {
  bandWidth = 1; // 1 to avoid a null allocation during setup
  patternLength = 1; // 1 to avoid a null allocation during setup
  nbPatterns = 3;
  nbHoles = 1; // 1 to avoid a null allocation during setup
  name = "untitled";
  mustBeSaved = false;
  numberingType = ANTICLOCKWISE;
  numberingOrder = ASCENDING;

  allowNoCyclic = false;

  palette.clear();
  // add 9 predefined colors
  palette.append(Qt::white);
  palette.append(Qt::black);
  palette.append(Qt::red);
  palette.append(Qt::green);
  palette.append(Qt::blue);
  palette.append(Qt::cyan);
  palette.append(Qt::magenta);
  palette.append(Qt::yellow);
  palette.append(Qt::gray);
  // add 7 others colors to reach 16
  palette.append(Qt::white);
  palette.append(Qt::white);
  palette.append(Qt::white);
  palette.append(Qt::white);
  palette.append(Qt::white);
  palette.append(Qt::white);
  palette.append(Qt::white);

  currColor = 0;

  pickWidth = 20;
  pickBaseLength = 25;
  pickTotalLength = 50;

  moveWidth = 20;
  threadWidth = 20;
  movesVertSep.clear(); /* the list will be filled the first time band is printed
                           or when the associated options is set */
  printVertSep = true;

  createMode = MODE_FROMBAND;
  // settup coloring style to default value
  coloringStyle = COLORING_CYCLIC;
}

void ProjectParams::setPickAppearence(int _pickWidth, int _pickBaseLength, int _pickTotalLength) {
  pickWidth = _pickWidth;
  pickBaseLength = _pickBaseLength;
  pickTotalLength = _pickTotalLength;
}

void ProjectParams::setMovesVertSep(int gap) {
  movesVertSep.clear();  
  quint32 i;
  for(i=gap;i<bandWidth;i+=gap) {
    movesVertSep.append(i);
  }
}

void ProjectParams::setMovesVertSep(QList<int> &lst) {
  movesVertSep.clear();
  movesVertSep = lst;
}

void ProjectParams::print() {

}

void ProjectParams::setName(const QString &_name) {
  name = _name;
}

void ProjectParams::setBandWidth(int _bandWidth) {

  bandWidth = _bandWidth;

  isWarpHoleNumber.clear();
  nbColors.clear();
  nbMaxColors.clear();
  mount.clear();
  cyclicCardPosition.clear();

  quint32 i;
  for(i=0;i<bandWidth;i++) {
    isWarpHoleNumber.append(false);
    nbColors.append(0);
    nbMaxColors.append(0);
    mount.append(S_MOUNT);
    cyclicCardPosition.append(CYCLIC);
  }

}

void ProjectParams::insertWarp(int index) {

  quint8* lst;
  lst = new quint8[bandLength];
  memset(lst,0,bandLength);
  completedWarps.insert(index,lst);
  isWarpComplete.insert(index,false);
  isWarpHoleNumber.insert(index,false);
  nbColors.insert(index,0);
  nbMaxColors.insert(index,0);
  mount.insert(index,S_MOUNT);
  cyclicCardPosition.insert(index,CYCLIC);

  lst = new quint8[PALETTE_SIZE+1];
  memset(lst,0,PALETTE_SIZE+1);
  idToColors.insert(index,lst);

  bandWidth = bandWidth + 1;  
}

void ProjectParams::removeWarp(int index) {

  quint8* lst;
  lst = completedWarps.at(index);
  delete [] lst;
  completedWarps.removeAt(index);
  isWarpComplete.removeAt(index);
  isWarpHoleNumber.removeAt(index);
  nbColors.removeAt(index);
  nbMaxColors.removeAt(index);
  mount.removeAt(index);
  cyclicCardPosition.removeAt(index);
  lst = idToColors.at(index);
  delete [] lst;
  idToColors.removeAt(index);

  bandWidth = bandWidth - 1;  
}

/* NOTE :

   During the creation of a new band (from New or import GTT menu), setting patternLength is the
   last thing done. In this case, sendSig is always false.

   But, if the band is alreay set-up, then sendSig is always true if the patternLength changes.

  */
void ProjectParams::setPatternLength(int _patternLength) {

  patternLength = _patternLength;
  setBandLength();
  updateCompletedWarps();
  updateIdToColor();
  setGuessWithIdles();
}

/* NOTE :
   this method is called during creation/opening of a band, then
   the design widget is not yet created, thus MovesWidget will not
   receive the guessIdleSet() signal

   */

void ProjectParams::setGuessWithIdles() {

  if (patternLength > LIM_PATTERN_LENGTH) {
    guessWithIdles = false;
  }
  else {
    guessWithIdles = true;
  }

  /* this signal is caugth by the main window to (des)activate the associated menu item
     and by guesser to update its arrays
     */
  emit guessWithIdlesSet();

}

void ProjectParams::setNbPatterns(int _nbPatterns, bool update) {

  quint32 oldValue = nbPatterns;
  nbPatterns = _nbPatterns;
  setBandLength();  

  if (update) {
    /* this case is when user changes nb patterns after the inital setup of the band
       => completedWarps must be updated
    */
    updateCompletedWarps();    
  }
}

void ProjectParams::setNbHoles(int _nbHoles) {
  nbHoles = _nbHoles;  
}

void ProjectParams::setBandLength() {
  bandLength = 1+nbPatterns*patternLength;
}


void ProjectParams::updateCompletedWarps() {

  quint8* lst;
  if (! completedWarps.isEmpty()) {

    QListIterator<quint8 *> iter(completedWarps);    
    // first, clear completedWarps
    while (iter.hasNext()) {
      lst = iter.next();
      if (lst != NULL) delete [] lst;
    }
    completedWarps.clear();
    isWarpComplete.clear();
  }

  // secondly, create the completedWarps  
  quint32 i;
  for(i=0;i<bandWidth;i++) {
    /* set the size to bandLength in case there are some warps
       that must be guessed without cylic positions of cards
    */
    lst = new quint8[bandLength];
    memset(lst,0,bandLength);
    completedWarps.append(lst);
    isWarpComplete.append(false);
  }  
}

void ProjectParams::updateIdToColor() {

  quint8* lst;
  if (! idToColors.isEmpty()) {
    QListIterator<quint8 *> iter(idToColors);    
    while (iter.hasNext()) {
      lst = iter.next();
      if (lst != NULL) delete [] lst;
    }
    idToColors.clear();
  }
  quint32 i;
  for(i=0;i<bandWidth;i++) {

    /* NB : normally, each idToColor should be nbHoles long
         since it is impossible to have more than nbHoles colors
         for a card. But, since the user can colorize a warp with
         at most PALETTE_SIZE colors (depending on patternLength)
         it is more practical to have idToColor length equals to that
         value. Tests on nbColors and nbHoles are done when a warp is
         completed and the guesser is called.
        */
    lst = new quint8[PALETTE_SIZE+1];
    memset(lst,0,PALETTE_SIZE+1);
    idToColors.append(lst);
  }

}

void ProjectParams::setPalette(int colorId, QColor c) {
  palette.replace(colorId,c);  
}

void ProjectParams::setCurrentColor(quint8 colorId) {
  currColor = colorId;
}

void ProjectParams::toggleWarpMount(int warpId) {

  quint8 m = mount.at(warpId);
  if(m == S_MOUNT) mount.replace(warpId,Z_MOUNT);
  else if(m == Z_MOUNT) mount.replace(warpId,S_MOUNT);

}

void ProjectParams::toggleCyclicPattern(int warpId) {

  quint8 c = cyclicCardPosition.at(warpId);
  if(c == CYCLIC) cyclicCardPosition.replace(warpId,NOCYCLIC);
  else if(c == NOCYCLIC) cyclicCardPosition.replace(warpId,CYCLIC);

}

void ProjectParams::toggleAllowNoCyclic() {

  allowNoCyclic = !allowNoCyclic;

}

void ProjectParams::toggleGuessIdles() {
  guessWithIdles = !guessWithIdles;
}

void ProjectParams::toggleLetteringType() {
  if (numberingType == CLOCKWISE) {
    numberingType = ANTICLOCKWISE;
  }
  else {
    numberingType = CLOCKWISE;
  }  
}

void ProjectParams::toggleNumberingOrder() {
  if (numberingOrder == DESCENDING) {
    numberingOrder = ASCENDING;
  }
  else {
    numberingOrder = DESCENDING;
  }
}

void ProjectParams::toggleCreationMode() {
  if (createMode == MODE_FROMBAND) {
    createMode = MODE_FROMMOVES;
  }
  else {
    createMode = MODE_FROMBAND;
  }
}

void ProjectParams::setWarpMount(int warpId, quint8 ori) {

  if(ori == S_MOUNT) mount.replace(warpId,S_MOUNT);
  else if(ori == Z_MOUNT) mount.replace(warpId,Z_MOUNT);

}

void ProjectParams::setWarpLettering(int warpId, bool state) {
  bool curState = isWarpHoleNumber.at(warpId);

  if (state != curState) {
    isWarpHoleNumber.replace(warpId,state);
  }
}

bool ProjectParams::isWarpLettering() {

  bool ret = false;
  quint32 i;
  for(i=0;(i<bandWidth) && (ret == false);i++) {
    if (isWarpHoleNumber.at(i) == true) {
      ret = true;
    }
  }

  return ret;
}

void ProjectParams::setColoringStyle(quint8 style) {

  coloringStyle = style;
}

void ProjectParams::swapWarp(int warpId1, int warpId2) {

  completedWarps.swap(warpId1,warpId2);
  isWarpComplete.swap(warpId1,warpId2);
  idToColors.swap(warpId1,warpId2);
  nbColors.swap(warpId1,warpId2);
  nbMaxColors.swap(warpId1,warpId2);
  mount.swap(warpId1,warpId2);
  cyclicCardPosition.swap(warpId1,warpId2);
  isWarpHoleNumber.swap(warpId1,warpId2);

}

QDataStream &operator<<(QDataStream &out, const ProjectParams &p)// throw(int){
{
  QByteArray block;
  QDataStream toWrite(&block, QIODevice::WriteOnly);
  toWrite.setVersion(QDataStream::Qt_4_0);

  QString magic(MAGIC_STRING);
  toWrite << magic;
  toWrite << VERSION_MAJ;
  toWrite << VERSION_MIN;  

  toWrite << p.name;
  toWrite << p.bandWidth;
  toWrite << p.patternLength;
  toWrite << p.nbPatterns;
  toWrite << p.bandLength;
  toWrite << p.nbHoles;

  toWrite << p.pickWidth;
  toWrite << p.pickBaseLength;
  toWrite << p.pickTotalLength;

  toWrite << p.palette;
  toWrite << p.mount; // saved in ordre to retrieve the same S/Z arrangement

  toWrite << p.numberingType;
  toWrite << p.numberingOrder;

  /* added in version 0.2 */
  toWrite << p.allowNoCyclic;
  toWrite << p.cyclicCardPosition;
  toWrite << p.isWarpComplete;

  /* added in version 0.4 */
  toWrite << p.moveWidth;
  toWrite << p.threadWidth;
  toWrite << p.movesVertSep;
  toWrite << p.printVertSep;
  toWrite << p.nbColors;
  toWrite << p.nbMaxColors;
  toWrite << p.isWarpHoleNumber;
  toWrite << p.isWarpComplete;
  quint8 *tab;
  quint32 i,j;
  for(i=0;i<p.bandWidth;i++) {
    tab = (p.completedWarps).at(i);
    for(j=0;j<p.bandLength;j++) {
      toWrite << tab[j];
    }
  }
  for(i=0;i<p.bandWidth;i++) {
    tab = (p.idToColors).at(i);
    for(j=0;j<PALETTE_SIZE+1;j++) {
      toWrite << tab[j];
    }
  }
  /* NB : other attributes completedWarps, idToColor, nbColors will be
     updated after a load wia a refresh slot call
     */
  out << block;
  return out;
}

QDataStream &operator>>(QDataStream &in, ProjectParams &p)// throw(int){
{
  quint32 blockSize;

  in.setVersion(QDataStream::Qt_4_0);
  QString txt;
  QList<QString> lstStr;

  in >> blockSize;

  in >> txt;
  if (txt != MAGIC_STRING) {
    throw(1);
  }
  in >> p.cmpVersionMaj;
  in >> p.cmpVersionMin;

#ifdef DEBUG_PRINT
  cout << "file version : " << (int)(p.cmpVersionMaj) << "." << (int)(p.cmpVersionMin) << endl;
#endif

  in >> txt;
  p.setName(txt);
  in >> p.bandWidth;
  in >> p.patternLength;

  in >> p.nbPatterns;
  in >> p.bandLength;
  in >> p.nbHoles;

  in >> p.pickWidth;
  in >> p.pickBaseLength;
  in >> p.pickTotalLength;

  // updating all lists with new params
  p.setBandLength();  
  /* these 2 lists will be maybe filled if file is >= v0.4
     Note that calling these methods will clear some arrays like mount, palette, ...
     so they must be called prior their reading
     */
  p.updateCompletedWarps();
  p.updateIdToColor();

  QList<QColor> lstColor;

  in >> lstColor;
  p.palette.clear();
  p.palette = lstColor;

  QList<quint8> lstMount;

  in >> lstMount;
  p.mount.clear();
  p.mount = lstMount;

  in >> p.numberingType;
  in >> p.numberingOrder;

  if ((p.cmpVersionMaj >= 0) && (p.cmpVersionMin >= 2)) {

    in >> p.allowNoCyclic;

    QList<quint8> lstCyclic;
    in >> lstCyclic;
    p.cyclicCardPosition.clear();
    p.cyclicCardPosition = lstCyclic;

    QList<bool> lstComp;
    in >> lstComp;
    p.isWarpComplete.clear();
    p.isWarpComplete = lstComp;
  }

  if ((p.cmpVersionMaj >= 0) && (p.cmpVersionMin >= 4)) {
    in >> p.moveWidth;
    in >> p.threadWidth;
    in >> p.movesVertSep;
    in >> p.printVertSep;
    in >> p.nbColors;
    in >> p.nbMaxColors;
    in >> p.isWarpHoleNumber;
    in >> p.isWarpComplete;
    quint8 *tab;
    quint32 i,j;
    for(i=0;i<p.bandWidth;i++) {
      tab = (p.completedWarps).at(i);
      for(j=0;j<p.bandLength;j++) {
        in >> tab[j];
      }
    }
    for(i=0;i<p.bandWidth;i++) {
      tab = (p.idToColors).at(i);
      for(j=0;j<PALETTE_SIZE+1;j++) {
        in >> tab[j];
      }
    }
  }
  p.setGuessWithIdles();  

  return in;

}

void ProjectParams::setExportForPrinter() {
  exportMarginUp = PRINT_MARGIN_UP;
  exportMarginBottom = PRINT_MARGIN_BOTTOM;
  exportMarginLeft = PRINT_MARGIN_LEFT;
  exportMarginRight = PRINT_MARGIN_RIGHT;
  exportSpace = PRINT_SPACE;
  exportPickWidth = PRINT_PICK_WIDTH;
  exportPickBaseLength = PRINT_PICK_BASELENGTH;
  exportPickTotalLength = PRINT_PICK_TOTALLENGTH;
  exportMoveWidth = PRINT_MOVE_WIDTH;
  exportTwistWidth = PRINT_TWIST_WIDTH;
  exportThreadWidth = PRINT_THREAD_WIDTH;
}

void ProjectParams::setExportForImage() {
  exportMarginUp = IMG_MARGIN_UP;
  exportMarginBottom = IMG_MARGIN_BOTTOM;
  exportMarginLeft = IMG_MARGIN_LEFT;
  exportMarginRight = IMG_MARGIN_RIGHT;
  exportSpace = IMG_SPACE;
  exportPickWidth = IMG_PICK_WIDTH;
  exportPickBaseLength = IMG_PICK_BASELENGTH;
  exportPickTotalLength = IMG_PICK_TOTALLENGTH;
  exportMoveWidth = IMG_MOVE_WIDTH;
  exportTwistWidth = IMG_TWIST_WIDTH;
  exportThreadWidth = IMG_THREAD_WIDTH;
}
