#ifndef FLYEMBODYINFODIALOG_H
#define FLYEMBODYINFODIALOG_H

#include <QDialog>
#include <QtGui>

#include "dvid/zdvidtarget.h"
#include "zjsonobject.h"

namespace Ui {
class FlyEmBodyInfoDialog;
}

class FlyEmBodyInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FlyEmBodyInfoDialog(QWidget *parent = 0);
    ~FlyEmBodyInfoDialog();

public slots:
  void dvidTargetChanged(ZDvidTarget target);

signals:
  void bodyActivated(uint64_t bodyId);
  void dataChanged(ZJsonValue object);
  void jsonLoadError(QString message);
  void loadCompleted();

private slots:
    void onCloseButton();
    void onRefreshButton();
    void activateBody(QModelIndex modelIndex);
    void updateModel(ZJsonValue object);
    void onJsonLoadError(QString message);
    void updateStatusLabel();
    void updateStatusAfterLoading();
    void filterUpdated(QString filterText);
    void applicationQuitting();    
    void onSaveColorFilter();

private:
    Ui::FlyEmBodyInfoDialog *ui;
    QStandardItemModel* m_bodyModel;
    QStandardItemModel* m_filterModel;
    QSortFilterProxyModel* m_bodyProxy;
    QSortFilterProxyModel* m_filterProxy;
    qlonglong m_totalPre;
    qlonglong m_totalPost;
    bool m_quitting;
    ZDvidTarget m_currentDvidTarget;
    QStandardItemModel* createModel(QObject*);
    QStandardItemModel* createFilterModel(QObject*);
    void setHeaders(QStandardItemModel*);
    bool isValidBookmarkFile(ZJsonObject object);
    bool dvidBookmarksPresent(ZDvidTarget target);
    bool bodies3Present(ZDvidTarget target);
    void importBookmarksDvid(ZDvidTarget target);
    void importBodiesDvid(ZDvidTarget target);
    void setStatusLabel(QString label);
    void clearStatusLabel();
    void init();
    void updateColorFilter(QString filter, QString oldFilter = "");
};

#endif // FLYEMBODYINFODIALOG_H
