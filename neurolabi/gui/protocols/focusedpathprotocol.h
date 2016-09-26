#ifndef FOCUSEDPATHPROTOCOL_H
#define FOCUSEDPATHPROTOCOL_H

#include "zjsonobject.h"

#include "protocoldialog.h"

namespace Ui {
class FocusedPathProtocol;
}

class FocusedPathProtocol : public ProtocolDialog
{
    Q_OBJECT

public:
    explicit FocusedPathProtocol(QWidget *parent = 0);
    ~FocusedPathProtocol();

signals:
    void protocolCompleting();
    void protocolExiting();
    void requestSaveProtocol(ZJsonObject data);

public slots:
    void loadDataRequested(ZJsonObject data);

private slots:
    void onExitButton();
    void onCompleteButton();

private:
    static const std::string KEY_VERSION;
    static const int fileVersion;

    Ui::FocusedPathProtocol *ui;
    bool initialize();
    void saveState();
};

#endif // FOCUSEDPATHPROTOCOL_H
