#include "zflyemproofdocmenufactory.h"
#include "zstackpresenter.h"
#include "zactionfactory.h"

ZFlyEmProofDocMenuFactory::ZFlyEmProofDocMenuFactory()
{
#ifdef _DEBUG_
  std::cout << "ZFlyEmProofDocMenuFactory constructed" << std::endl;
#endif
}

QMenu* ZFlyEmProofDocMenuFactory::makeBodyContextMenu(
    ZStackPresenter *presenter, QWidget */*parentWidget*/, QMenu *menu)
{
  if (menu == NULL) {
    menu = new QMenu(NULL);
  }

  QAction *action = presenter->getAction(
        ZActionFactory::ACTION_BODY_SPLIT_START);
  menu->addAction(action);

  menu->addAction(presenter->getAction(ZActionFactory::ACTION_BODY_DECOMPOSE));

  menu->addAction(presenter->getAction(
                    ZActionFactory::ACTION_BODY_ANNOTATION));

  menu->addAction(presenter->getAction(ZActionFactory::ACTION_BODY_CHECKOUT));
  menu->addAction(presenter->getAction(ZActionFactory::ACTION_BODY_CHECKIN));

  if (isAdmin()) {
    menu->addAction(presenter->getAction(
                      ZActionFactory::ACTION_BODY_FORCE_CHECKIN));
  }

  return menu;
}

QMenu* ZFlyEmProofDocMenuFactory::makeSynapseContextMenu(
    ZStackPresenter *presenter, QWidget */*parentWidget*/, QMenu *menu)
{
  if (menu == NULL) {
    menu = new QMenu(NULL);
  }

  QAction *action = presenter->getAction(
        ZActionFactory::ACTION_SYNAPSE_ADD);
  menu->addAction(action);

  menu->addAction(presenter->getAction(ZActionFactory::ACTION_SYNAPSE_DELETE));

  menu->addAction(presenter->getAction(
                    ZActionFactory::ACTION_SYNAPSE_MOVE));

  return menu;
}
