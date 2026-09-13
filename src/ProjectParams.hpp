/*-==============================================================-

file : ProjectParams.hpp

creation date : 31/01/2012

author : S. Domas (sdomas@iut-bm.univ-fcomte.fr)

description : 

supp. infos : saved in UTF-8 [éè]

-==============================================================-*/
#ifndef __PROJECTPARAMS_HPP__
#define __PROJECTPARAMS_HPP__

#include <iostream>
#include <fstream>

#include <QtCore>
#include <QtGui>

#include "CardThreading.hpp"

#define MAGIC_STRING "cartomagic"
#define VERSION_MAJ (quint8)0 // major version code
#define VERSION_MIN (quint8)4 // minor version number
#define REVISION (quint8)1 // revision number of current version

#define MODE_FROMBAND (quint8)1 // create the band by clicking on the band view
#define MODE_FROMMOVES (quint8)2 // create the band by clicking on the moves view

#define BAD_MAGIC_STRING 1

#define LIM_PATTERN_LENGTH 20 // limit of pattern length while guessing with idles

#define PALETTE_SIZE 16

#define CLOCKWISE (quint8)1
#define ANTICLOCKWISE (quint8)2

#define DESCENDING (quint8)1
#define ASCENDING (quint8)2

#define CYCLIC (quint8)1
#define NOCYCLIC (quint8)2

// defines that condition the drawing mode
#define MODE_COLOR 0 // left click changes color, right click changes pick shape
#define MODE_TWIST 1 // right click changes color, left click changes pick shape

// defines for coloring mode
#define COLORING_CYCLIC (quint8)0 // coloring pick cyclically over patterns
#define COLORING_FREE (quint8)1 // coloring not cyclically
#define COLORING_HOLE (quint8)2 // coloring all the pick with the same hole letter with the same color (= nearly GTT)


// dimensions for printer exporting
#define PRINT_MARGIN_UP 1.0
#define PRINT_MARGIN_BOTTOM 1.0
#define PRINT_MARGIN_LEFT 1.0
#define PRINT_MARGIN_RIGHT 1.0
#define PRINT_SPACE 0.2

#define PRINT_PICK_WIDTH 0.5
#define PRINT_PICK_BASELENGTH 0.625
#define PRINT_PICK_TOTALLENGTH 1.25
#define PRINT_MOVE_WIDTH 0.5
#define PRINT_TWIST_WIDTH 0.2
#define PRINT_THREAD_WIDTH 0.5

// dimensions for jpg exporting
#define IMG_MARGIN_UP 0.0
#define IMG_MARGIN_BOTTOM 0.0
#define IMG_MARGIN_LEFT 0.0
#define IMG_MARGIN_RIGHT 0.0
#define IMG_SPACE 0.2

#define IMG_PICK_WIDTH 0.5
#define IMG_PICK_BASELENGTH 0.625
#define IMG_PICK_TOTALLENGTH 1.25
#define IMG_MOVE_WIDTH 0.5
#define IMG_TWIST_WIDTH 0.2
#define IMG_THREAD_WIDTH 0.5

using namespace std;
using namespace Qt;

class ProjectParams : public QObject {

  Q_OBJECT

public:

  ProjectParams();
  ~ProjectParams();

  void clear();
  void print();

  /* params for the very first version */

  quint32 bandWidth;
  quint32 patternLength;
  quint32 nbPatterns;
  quint32 bandLength;
  quint32 nbHoles;

  QString name;

  QList<QColor> palette;
  quint8 currColor; // the id of current color in palette

  /* ugly thing to declare this array here, but it is
  more practicle since it is shared by BandModel and PatternGuesser
  */
  QList<quint8 *> completedWarps;
  QList<bool> isWarpComplete; // tells if the warp is completed or not NB : from version 0.2
  QList<quint8 *> idToColors; // quint8 * matches an id 0,1, ... to color index in palette
  QList<quint8> nbColors; // numbers of colors on each warp in the band
  QList<quint8> nbMaxColors; // maximum numbers of colors to guess each warp
  QList<quint8> mount; // tells if the warp is mount initially in S or Z

  QList<bool> isWarpHoleNumber; // tells if hole numbering shwos up on warps or not


  quint8 numberingType; // tells if cards are numbered (anti)clockwise
  quint8 numberingOrder; // tells if cards are numbered downward or forward

  int pickWidth; // determine the visual size of each pick
  int pickBaseLength;
  int pickTotalLength;

  int moveWidth;
  int threadWidth;
  QList<int> movesVertSep; /* a list where vertical separators appear when printing moves
                              e.g 1,4,7 means that a bold line appears on the left on moves of
                              warp 1 (couting from 0), on the left of warp 4, ...
                              */
  bool printVertSep;

  bool guessWithIdles;

  /* parameters introduced in version 0.2 */
  QList<quint8> cyclicCardPosition; // tells if the pick sequence is cyclic or not
  bool allowNoCyclic;

  quint8 coloringStyle; // style to colorize the picks (see defines of COLORING_XXXX)

  CardThreading *threadPerm;

  bool mustBeSaved; // true if modified since last save

  /* since v0.4 */
  quint8 createMode; // creation mode : click ont the band or click on the moves

  /* dimension parameters used for exporting to printer and jpg */
  double exportMarginUp;
  double exportMarginBottom;
  double exportMarginLeft;
  double exportMarginRight;
  double exportSpace;
  double exportPickWidth;
  double exportPickBaseLength;
  double exportPickTotalLength;
  double exportMoveWidth;
  double exportTwistWidth;
  double exportThreadWidth;
  double exportResolution;

  /* # version when reading a project file. Needed since the BandModel
       must know the file version to read quint8 or quint16 */
  quint8 cmpVersionMaj;
  quint8 cmpVersionMin;

  static quint8 versionMaj;
  static quint8 versionMin;
  static quint8 revision;

private:
  void setBandLength();
  void updateCompletedWarps();
  void updateIdToColor();
  void setGuessWithIdles();

public slots:

  void setName(const QString &_name);
  // setBandWidth should be called only when the project is setup
  void setBandWidth(int _bandWidth);
  void setPatternLength(int _patternLength);
  void setNbPatterns(int _nbPatterns, bool update = false); // update is true is nbPatterns has already been set and just updates
  void setNbHoles(int _nbHoles); // idem
  void setPalette(int colorId, QColor c);
  void setCurrentColor(quint8 colorId);
  void insertWarp(int index);
  void removeWarp(int index);

  /* toggleWarpMount() ; if S it becomes Z and if Z -> S */
  void toggleWarpMount(int warpId);
  void setWarpMount(int warpId, quint8 ori);

  /* toggleCyclicPattern() ; if a pattern must be considered as cyclic or not */
  void toggleCyclicPattern(int warpId);

  void toggleAllowNoCyclic();

  void swapWarp(int warpId1, int warpId2);

  void toggleGuessIdles();

  void toggleLetteringType();
  void toggleNumberingOrder();

  void toggleCreationMode();

  /* setWarpLettering() ; if a warp must be shown with hole snumer on picks */
  void setWarpLettering(int warpId, bool state);
  bool isWarpLettering(); // return false if none is selected to show lettering

  /* */
  void setColoringStyle(quint8 style);

  void setExportForPrinter();
  void setExportForImage();

  void setPickAppearence(int _pickWidth, int _pickBaseLength, int _pickTotalLength);

  void setMovesVertSep(int gap);
  void setMovesVertSep(QList<int> &lst);


signals:
  void bandWidthChanged(); // just used by widget to resize scroll areas
  void nbHolesChanged();  
  void guessWithIdlesSet();  


  friend QDataStream &operator<<(QDataStream &out, const ProjectParams &p);// throw(int);
  friend QDataStream &operator>>(QDataStream &in, ProjectParams &p);// throw(int);

};

#endif //__PROJECTPARAMS_HPP__ 
