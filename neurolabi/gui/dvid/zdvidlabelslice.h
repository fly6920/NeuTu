#ifndef ZDVIDLABELSLICE_H
#define ZDVIDLABELSLICE_H

#include "zstackobject.h"
#include "zdvidtarget.h"
#include "zobject3dscan.h"
#include "zobject3dscanarray.h"
#include "zstackviewparam.h"
#include "zobjectcolorscheme.h"

class ZFlyEmBodyMerger;
class QColor;

class ZDvidLabelSlice : public ZStackObject
{
public:
  ZDvidLabelSlice();
  ZDvidLabelSlice(int maxWidth, int maxHeight);

  void setMaxSize(int maxWidth, int maxHeight);

  void update(const ZStackViewParam &viewParam);
  void update(int z);
  void update();

  void display(ZPainter &painter, int slice, EDisplayStyle option) const;

  const std::string& className() const;

  inline void setDvidTarget(const ZDvidTarget &target) {
    m_dvidTarget = target;
  }

  bool hit(double x, double y, double z);

  void selectHit(bool appending = false);
  void deselectAll();
  void toggleHitSelection(bool appending = false);
  void clearSelection();

  inline const std::set<uint64_t>& getSelected() const {
    return m_selectedSet;
  }

  void setBodyMerger(ZFlyEmBodyMerger *bodyMerger);
  void updateLabelColor();

  const ZObjectColorScheme& getColorScheme() const {
    return m_objColorSheme;
  }

  QColor getColor(uint64_t label) const;
  QColor getColor(int64_t label) const;

  uint64_t getMappedLabel(const ZObject3dScan &obj) const;

private:
  inline const ZDvidTarget& getDvidTarget() const { return m_dvidTarget; }
  void assignColorMap();
  void forceUpdate(const ZStackViewParam &viewParam);
  //void updateLabel(const ZFlyEmBodyMerger &merger);
  void init(int maxWidth, int maxHeight);

private:
  ZDvidTarget m_dvidTarget;
  ZObject3dScanArray m_objArray;
  ZStackViewParam m_currentViewParam;
  ZObjectColorScheme m_objColorSheme;
  uint64_t m_hitLabel;
  std::set<uint64_t> m_selectedSet;
  ZFlyEmBodyMerger *m_bodyMerger;

  int m_maxWidth;
  int m_maxHeight;
};

#endif // ZDVIDLABELSLICE_H