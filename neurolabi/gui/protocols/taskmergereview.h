#ifndef TASKMERGEREVIEW_H
#define TASKMERGEREVIEW_H

#include "protocols/taskreview.h"

class TaskMergeReview : public TaskReview
{
  Q_OBJECT
public:
  TaskMergeReview(QJsonObject json, ZFlyEmBody3dDoc *bodyDoc);

  // For use with TaskProtocolTaskFactory.
  static QString taskTypeStatic();
  static TaskMergeReview* createFromJson(QJsonObject json, ZFlyEmBody3dDoc *bodyDoc);

  virtual QString taskType() const override;
  virtual QString actionString() override;

protected:
  virtual std::string getOutputInstanceName(const ZDvidTarget &dvidTarget) const override;

  virtual QString labelForDoButton() const override;
  virtual QString labelForDontButton() const override;
  virtual QString labelForDoMajorButton() const override;

  virtual QString valueResultDo() const override;
  virtual QString valueResultDont() const override;
  virtual QString valueResultDoMajor() const override;
};

#endif // TASKMERGEREVIEW_H
