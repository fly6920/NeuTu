
# #####################################################################
# Automatically generated by qmake (2.01a) Mon Dec 15 16:16:33 2008
# #####################################################################
TEMPLATE = app

contains(TEMPLATE, app) {
  DEFINES += _QT_APPLICATION_
} else {
  CONFIG += staticlib
}

CONFIG(neu3) {
  DEFINES += _NEU3_
}

CONFIG(neu3) | CONFIG(neutu) | CONFIG(flyem)  { #flyem CONFIG is an outdated option
  DEFINES *= _FLYEM_
} else {
  DEFINES += _NEUTUBE_
}

DEFINES *= _ENABLE_LOWTIS_
DEFINES += DRACO_ATTRIBUTE_DEDUPLICATION_SUPPORTED
DEFINES += _GLIBCXX_USE_CXX11_ABI=0

#This may result in many 'Cannot create directory' warnings. Just ignore it.
CONFIG += object_parallel_to_source
message("Objs dir: $${OBJECTS_DIR}")

#DEFINES+=_CLI_VERSION
win32 {
    QMAKE_CXXFLAGS += /bigobj #/GL # Enables whole program optimization.
    #QMAKE_LFLAGS += /LTCG # Link-time Code Generation

    DEFINES += _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN

    QMAKE_CXXFLAGS += /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
    QMAKE_CXXFLAGS += /wd4244 # 'argument' : conversion from 'type1' to 'type2', possible loss of data
    QMAKE_CXXFLAGS += /wd4305 # 'identifier' : truncation from 'type1' to 'type2'
    QMAKE_CXXFLAGS += /wd4819 # The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss.
    QMAKE_CXXFLAGS += /utf-8  # https://blogs.msdn.microsoft.com/vcblog/2016/02/22/new-options-for-managing-character-sets-in-the-microsoft-cc-compiler/

    DEPLOYMENT_COMMAND = $$PWD/deploy_win.bat $(QMAKE) $$OUT_PWD

    CONFIG(release, debug|release):!isEmpty(DEPLOYMENT_COMMAND) {
        QMAKE_POST_LINK += $$DEPLOYMENT_COMMAND
    }
}

app_name = neutu

CONFIG(debug, debug|release) {
    app_name = neuTube_d
    CONFIG(neu3) {
        app_name = neu3_d
    } else {
      contains(DEFINES, _FLYEM_) {
        app_name = neutu_d
      }
    }
    DEFINES += _DEBUG_ _ADVANCED_ PROJECT_PATH=\"\\\"$$PWD\\\"\"
} else {
#    CONFIG(debug_info) {
#      QMAKE_CXXFLAGS += -g
#    }
    app_name = neuTube
    CONFIG(neu3) {
      app_name = neu3
    } else {
      CONFIG(flyem) | CONFIG(neutu) {
        app_name = neutu
      }
    }
}

include(extratarget.pri)

message("Neurolabi target: $$neurolabi.target")

CONFIG(force_link) {
  PRE_TARGETDEPS += neurolabi
}

TARGET = $$app_name

CONFIG(force_link) {
  QMAKE_POST_LINK += $$quote(echo "making config"; make app_config;)
}

unix {
  # suppress warnings from 3rd party library, works for gcc and clang
  QMAKE_CXXFLAGS += -isystem ../gui/ext
} else {
  INCLUDEPATH += ../gui/ext
}

CONFIG += rtti exceptions
CONFIG += static_gtest

include(extlib.pri)

QT += printsupport
qtHaveModule(webenginewidgets) {
  QT += webenginewidgets
  DEFINES += _USE_WEBENGINE_
}

DEFINES += _QT_GUI_USED_ HAVE_CONFIG_H _ENABLE_DDP_ _ENABLE_WAVG_

#Machine information
HOSTNAME = $$system(echo $HOSTNAME)
USER = $$system(echo $USER)
HOME = $$system(echo $HOME)
GIT = $$system(which git)

contains(GIT, .*git) {
  COMMIT_HASH = $$system("git log --pretty=format:\"%H %p\" -1 | sed s/' '/_/g")
  DEFINES += _CURRENT_COMMIT_=\"\\\"$$COMMIT_HASH\\\"\"
  message($$COMMIT_HASH)
}

message("Defines: $${DEFINES}")

include(add_itk.pri)

#Qt4 (Obsolete)
isEqual(QT_MAJOR_VERSION,4) {
  QT += opengl xml network
  warning("Obsolete setting: Qt 4")
}

#Qt5
isEqual(QT_MAJOR_VERSION,5) | greaterThan(QT_MAJOR_VERSION,5) {
    isEqual(QT_MAJOR_VERSION,5) {
      lessThan(QT_MINOR_VERSION,4) {
        message("Unstable Qt version $${QT_VERSION}.")
        error("Use at least Qt 5.4.0.")
      }
    }
    message("Qt 5")
    QT += concurrent gui widgets network xml
    DEFINES += _QT5_
    CONFIG *= c++11
}

CONFIG(c++11) {
  message(Using C++11)
  DEFINES += _CPP11_
  unix {
    QMAKE_CXXFLAGS += -std=c++11
    macx {
      QMAKE_CXXFLAGS += -stdlib=libc++
    }
  }
}

equals(SANITIZE_BUILD, "thread") {
  QMAKE_CXXFLAGS += -fsanitize=thread
  QMAKE_LFLAGS += -fsanitize=thread
  DEFINES += SANITIZE_THREAD
}

equals(SANITIZE_BUILD, "address") {
  QMAKE_CXXFLAGS += -fsanitize=address
  QMAKE_LFLAGS += -fsanitize=address
}

unix {
    QMAKE_CXXFLAGS += -Wno-deprecated
    macx {
#        DEFINES += _NEUTUBE_MAC_
        LIBS += -framework AppKit -framework IOKit \
            -framework ApplicationServices \
            -framework CoreFoundation

        CONFIG(neutu) | CONFIG(flyem) {
            ICON += images/neutu.icns
        } else {
            CONFIG(neu3) {
                ICON += images/neu3.icns
            } else {
                ICON = images/app.icns
            }
        }
#        QMAKE_INFO_PLIST = images/Info.plist
        QMAKE_CXXFLAGS += -m64

        CONFIG(autotarget) {
          OSX_VERSION = $$system(sw_vers -productVersion)
          message("Mac OS X $$OSX_VERSION")
          MAC_VERSION_NUMBER = $$split(OSX_VERSION, .)
          OSX_MAJOR_VERSION = $$member(MAC_VERSION_NUMBER, 0)
          OSX_MINOR_VERSION = $$member(MAC_VERSION_NUMBER, 1)
          !isEqual(OSX_MAJOR_VERSION, 10) {
            error("Could not recognize OSX version")
          }

          OSX_COM_VER = $${OSX_MAJOR_VERSION}.$${OSX_MINOR_VERSION}
          QMAKE_MACOSX_DEPLOYMENT_TARGET = $$OSX_COM_VER
          message("Deployment target: $$QMAKE_MACOSX_DEPLOYMENT_TARGET")

          greaterThan(OSX_MINOR_VERSION, 8) {
            CONFIG(libstdc++) {
              message("Using libstdc++")
            } else {
              LIBS -= -lstdc++
              QMAKE_CXXFLAGS += -stdlib=libc++
            }

            QMAKE_MAC_SDK = macosx$${OSX_COM_VER}
            message("SDK: $$QMAKE_MAC_SDK")
          }
        } else {
          message("No auto mac version check")
          CONFIG(c++11) {
            isEqual(QT_MAJOR_VERSION,4) {
              message("Forcing deployment target: ")
              QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
            }
          }
        }

        isEqual(OSX_MINOR_VERSION, 11) {
          message("Forcing 10.12 SDK on xcode8: ")
          QMAKE_MAC_SDK = macosx10.12
          message("SDK: $$QMAKE_MAC_SDK")
        }

        doc.files = doc
        doc.path = Contents/MacOS
        QMAKE_BUNDLE_DATA += doc
#        config.files = config.xml
#        config.path = Contents/MacOS
#        QMAKE_BUNDLE_DATA += config
    } else {
        DEFINES += _LINUX_
        LIBS += -lX11 -lm -lpthread -lrt -lGLU -lstdc++

        message(Checking arch...)
        contains(QMAKE_HOST.arch, x86_64) {
            message($$QMAKE_HOST.arch)
            QMAKE_CXXFLAGS += -m64
        }
        QMAKE_CXXFLAGS += -fext-numeric-literals -msse3
        contains(DEFINES, _FLYEM_) {
            RC_FILE = images/app2.icns
        } else {
            RC_FILE = images/app.icns
        }

    } #macx
}

win32 {
  RC_FILE = images/app.rc
}

include (gui_free.pri)
include(test/test.pri)
include(sandbox/sandbox.pri)
include(command/command.pri)
include(trace/trace.pri)
include(neutuse/neutuse.pri)
include(service/service.pri)
include(logging/logging.pri)
include(mvc/mvc.pri)
include(vis3d.pri)
include(qt/qt.pri)
include(widgets/widgets.pri)
include(flyem/flyem.pri)

# Input
win32 {
  SOURCES += $$PWD/ext/sys/VideoMemoryWin.cpp \
      $$PWD/ext/sys/VidMemViaD3D9.cpp \
      $$PWD/ext/sys/VidMemViaDDraw.cpp \
      $$PWD/ext/sys/VidMemViaDxDiag.cpp
}

unix {
  macx {
      SOURCES += $$PWD/ext/sys/VideoMemoryMac.cpp
  } else {
      SOURCES += $$PWD/ext/sys/VideoMemoryLinux.cpp
  }
}

RESOURCES = gui.qrc

HEADERS += mainwindow.h \
    zimage.h \
    zslider.h \
    plotsettings.h \
    plotter.h \
    zinteractivecontext.h \
    ztraceproject.h \
    dialogs/channeldialog.h \
    zpunctum.h \
    dialogs/settingdialog.h \
    dialogs/frameinfodialog.h \
    dialogs/traceoutputdialog.h \
    dialogs/bcadjustdialog.h \
    dialogs/zeditswcdialog.h \
    dialogs/cannyedgedialog.h \
    dialogs/medianfilterdialog.h \
    dialogs/distancemapdialog.h \
    dialogs/regionexpanddialog.h \
    dialogs/neuroniddialog.h \
    dialogs/diffusiondialog.h \
    dialogs/connectedthresholddialog.h \
    dialogs/flyemskeletonizationdialog.h \
    dialogs/parameterdialog.h \
    dialogs/pixelquerydialog.h \
    dialogs/zrescaleswcdialog.h \
    dialogs/swcsizedialog.h \
    dialogs/swcskeletontransformdialog.h \
    dialogs/swctypedialog.h \
    dialogs/mexicanhatdialog.h \
    zsinglechannelstack.h \
    zrandom.h \
    zlocsegchainconn.h \
    zobjsitem.h \
    zobjsmodel.h \
    zdirectionaltemplatechain.h \
    zcolormap.h \
    zclickablelabel.h \
    zcolormapeditor.h \
    zselectfilewidget.h \
    zcolormapwidgetwitheditorwindow.h \
    zspinbox.h \
    zcombobox.h \
    zspinboxwithslider.h \
    zeventlistenerparameter.h \
    zspanslider.h \
    ztest.h \
    zglmutils.h \
    znormcolormap.h \
    zlocsegchain.h \
    zcurve.h \
    zxmldoc.h \
    zintmap.h \
    z3dgraph.h \
    zprocessprogressbase.h \
    zopencv_header.h \
    zhdf5writer.h \
    zmoviescene.h \
    zmovieactor.h \
    zswcmovieactor.h \
    zmoviemaker.h \
    zmoviephotographer.h \
    zmoviescript.h \
    zpunctamovieactor.h \
    zstackmovieactor.h \
    zmoviesceneclipper.h\
    zmoviecamera.h \
    flyemdataform.h \
    zswcobjsmodel.h \
    zpunctaobjsmodel.h \
    zobjsmanagerwidget.h \
    zmoviescriptgenerator.h \
    zmoviestage.h \
    dialogs/zalphadialog.h \
    zqtbarprogressreporter.h \
    zstackdoccommand.h \
    zcursorstore.h \
    zmessagereporter.h \
    zqtmessagereporter.h \
    zstroke2d.h \
    neutube.h \
    zreportable.h \
    dialogs/helpdialog.h \
    zswcnodeobjsmodel.h \
    zstackstatistics.h \
    biocytin/biocytin.h \
    biocytin/zstackprojector.h \
    dialogs/projectiondialog.h \
    zstackviewlocator.h \
    dialogs/startsettingdialog.h \
    zstackreadthread.h \
    zswccolorscheme.h \
    dialogs/moviedialog.h \
    zpunctumio.h \
    zstatisticsutils.h \
    zswcrangeanalyzer.h \
    dialogs/informationdialog.h \
    zswcnodezrangeselector.h \
    zswcnodecompositeselector.h \
    zswcnodeellipsoidrangeselector.h \
    dialogs/flyemdataquerydialog.h \
    dialogs/flyemdataprocessdialog.h \
    dialogs/autosaveswclistdialog.h \
    zswcfilelistmodel.h \
    zswcglobalfeatureanalyzer.h \
    zcommandline.h \
    zswclocationanalyzer.h \
    biocytin/zbiocytinfilenameparser.h \
    dialogs/diagnosisdialog.h \
    zerror.h \
    zhistogram.h \
    zswcgenerator.h \
    zpaintbundle.h \
    dialogs/flyemdataexportdialog.h \
    zflyemqueryview.h \
    dialogs/flyemgeosearchdialog.h \
    zqslogmessagereporter.h \
    dialogs/flyemgeofilterdialog.h \
    zactionactivator.h \
    zswccurvaturefeatureanalyzer.h \
    zstackdocmenustore.h \
    zstackdocmenufactory.h \
    dialogs/penwidthdialog.h \
    dialogs/dvidobjectdialog.h \
    zpainter.h \
    dialogs/resolutiondialog.h \
    dvid/zdvidrequest.h \
    zmatlabprocess.h \
    zneuronseed.h \
    dialogs/dvidimagedialog.h \
    ztiledstackframe.h \
    ztilemanager.h \
    ztilemanagerview.h \
    ztilegraphicsitem.h \
    ztileinfo.h \
    dialogs/flyemneuronthumbnaildialog.h \
    zmultitaskmanager.h \
    zinteractionevent.h \
    dialogs/flyemhotspotdialog.h \
    zworkspacefactory.h \
    dvid/zdvidreader.h \
    dialogs/flyembodyiddialog.h \
    dialogs/zdviddialog.h \
    dialogs/zdvidtargetproviderdialog.h \
    dialogs/flyembodyfilterdialog.h \
    zflyemdvidreader.h \
    zstroke2darray.h \
    tilemanager.h \
    zactionfactory.h \
    zstackreadfactory.h \
    zstackdoclabelstackfactory.h \
    zsparseobject.h \
    zlabelcolortable.h \
    zdocplayer.h \
    zlinesegmentobject.h \
    openvdb_header.h \
    zopenvdbobject.h \
    dialogs/flyembodysplitprojectdialog.h \
    dialogs/zflyemnewbodysplitprojectdialog.h \
    zstroke2dobjsmodel.h \
    zdocplayerobjsmodel.h \
    zabstractmodelpresenter.h \
    zstackobjectarray.h \
    zwindowfactory.h \
    dvid/zdvidwriter.h \
    dialogs/dvidskeletonizedialog.h \
    zdialogfactory.h \
    zwidgetfactory.h \
    zlabelededitwidget.h \
    zlabeledcombowidget.h \
    dialogs/zspinboxdialog.h \
    zbuttonbox.h \
    zkeyeventswcmapper.h \
    dialogs/zflyemroidialog.h \
    newprojectmainwindow.h \
    zmouseeventmapper.h \
    dialogs/shapepaperdialog.h \
    zframefactory.h \
    zactionbutton.h \
    dvid/zdvidbufferreader.h \
    zmouseevent.h \
    zmouseeventrecorder.h \
    zmouseeventprocessor.h \
    zstackoperator.h \
    zsleeper.h \
    dvid/libdvidheader.h \
    dialogs/dvidoperatedialog.h \
    zthreadfuturemap.h \
    zstackball.h \
    zstackdochittest.h \
    zkeyeventmapper.h \
    zuserinputevent.h \
    zstackobjectmanager.h \
    zstackobjectgroup.h \
    zcolorscheme.h \
    zpunctumcolorscheme.h \
    zstackpatch.h \
    zrect2d.h \
    zobjectcolorscheme.h \
    dialogs/synapseimportdialog.h \
    dialogs/flyembodymergeprojectdialog.h \
    zstackdvidgrayscalefactory.h \
    zstackdocreader.h \
    dialogs/flyemprojectdialog.h \
    dialogs/zsegmentationprojectdialog.h \
    zsegmentationproject.h \
    zsegmentationprojectmodel.h \
    dialogs/zsubtractswcsdialog.h \
    dialogs/zmarkswcsomadialog.h \
    zlabeledspinboxwidget.h \
    dialogs/zspinboxgroupdialog.h \
    dialogs/zautotracedialog.h \
    zstackviewmanager.h \
    zstackviewparam.h \
    zflyemprojectmanager.h \
    zflyemdataloader.h \
    dialogs/swcexportdialog.h \
    zprogressmanager.h \
    dvid/zdvidtile.h \
    dvid/zdvidresolution.h \
    dvid/zdvidtileinfo.h \
    dvid/zdvidversionmodel.h \
    zmessagemanager.h \
    zmessageprocessor.h \
    zmessage.h \
    zmainwindowmessageprocessor.h \
    dialogs/ztestdialog.h \
    dialogs/ztestdialog2.h \
    zstackdocloader.h \
    zstackwidget.h \
    dvid/zdvidversiondag.h \
    dvid/zdvidversion.h \
    dvid/zdvidversionnode.h \
    zbodysplitbutton.h \
    zmessagefactory.h \
    zmessagemanagermodel.h \
    zflyemcontrolform.h \
    dvid/zdvidtileensemble.h \
    dvid/zdvidlabelslice.h \
    zsttransform.h \
    zpixmap.h \
    dvid/zdvidgrayslice.h \
    dvid/zdvidsparsestack.h \
    zwidgetmessage.h \
    zprogresssignal.h \
    zkeyeventstrokemapper.h \
    zkeyoperation.h \
    zkeyoperationmap.h \
    zkeyoperationconfig.h \
    dvid/zdvidsparsevolslice.h \
    ztextmessage.h \
    ztextmessagefactory.h \
    z3dgraphfactory.h \
    zkeyeventmapperfactory.h \
    zkeyoperationmapsequence.h \
    zpuncta.h \
    dialogs/flyembodyinfodialog.h \
    biocytin/zbiocytinprojectiondoc.h \
    dialogs/zflyemsplitcommitdialog.h \
    zstackdocfactory.h \
    zintcuboidobj.h \
    ztask.h \
    zstackobjectpainter.h \
    zslicedpuncta.h \
    zmultiscalepixmap.h \
    biocytin/zbiocytinprojmaskfactory.h \
    zpunctumselector.h \
    zgraphobjsmodel.h \
    zsurfaceobjsmodel.h \
    dvid/zdvidsynapse.h \
    dvid/zdvidsynapseensenmble.h \
    zcubearray.h \
    dvid/zdvidannotationcommand.h \
    dvid/zflyembookmarkcommand.h \
    dvid/zdvidannotation.h \
    dialogs/stringlistdialog.h \
    zroiwidget.h \
    dialogs/flyemtododialog.h \
    zstackdocselector.h \
    dialogs/flyemsettingdialog.h \
    protocols/protocolswitcher.h \
    protocols/protocolchooser.h \
    protocols/protocolmetadata.h \
    protocols/protocoldialog.h \
    protocols/doNthingsprotocol.h \
    protocols/synapsepredictionprotocol.h \
    protocols/synapsepredictioninputdialog.h \
    protocols/synapsepredictionbodyinputdialog.h \
    protocols/synapsereviewprotocol.h \
    protocols/synapsereviewinputdialog.h \
    zactionlibrary.h \
    zmenufactory.h \
    zcrosshair.h \
    zapplication.h \
    dialogs/flyemsynapsefilterdialog.h \
    dialogs/zflyemsynapseannotationdialog.h \
    zdvidutil.h \
    zstackreader.h \
    dvid/zdvidpath.h \
    dialogs/zcontrastprotocaldialog.h \
    dvid/zdvidsynapsecommand.h \
    dvid/zdvidannotation.hpp \
    dialogs/zflyemroitooldialog.h \
    dvid/zdvidpatchdatafetcher.h \
    dvid/zdvidpatchdataupdater.h \
    dvid/zdviddatafetcher.h \
    dvid/zdviddataupdater.h \
    zcubearraymovieactor.h \
    zstackdocdatabuffer.h \
    dvid/zdvidroi.h \
    z3dwindow.h \
    z3dmainwindow.h \
    dvid/zdvidgrayscale.h \
    zscrollslicestrategy.h \
    dvid/zdvidgrayslicescrollstrategy.h \
    zviewproj.h \
    dialogs/zflyemgrayscaledialog.h \
    dvid/zdvidneurontracer.h \
    zorthoviewhelper.h \
    dialogs/zcomboeditdialog.h \
    dialogs/zflyembodycomparisondialog.h \
    dvid/zdvidstore.h \
    zglobal.h \
    dvid/zdvidresultservice.h \
    zstackgarbagecollector.h \
    dialogs/zflyembodysplitdialog.h \
    neu3window.h \
    zstackdockeyprocessor.h \
    zutils.h \
    zflags.h \
    zbbox.h \
    zspinboxwithscrollbar.h \
    zsysteminfo.h \
    zvertexbufferobject.h \
    zvertexarrayobject.h \
    zmesh.h \
    zmeshio.h \
    zmeshutils.h \
    zstringutils.h \
    ztakescreenshotwidget.h \
    zioutils.h \
    zmeshobjsmodel.h \
    protocols/taskprotocoltask.h \
    protocols/taskbodyreview.h \
    dialogs/dvidbranchdialog.h \
    protocols/tasktesttask.h \
    protocols/tasksplitseeds.h \
    protocols/bodyprefetchqueue.h \
    zopenglwidget.h \
    misc/zvtkutil.h \
    zstackdocaccessor.h \
    zcontrastprotocol.h \
    dialogs/zflyemmergeuploaddialog.h \
    zmeshfactory.h \
    protocols/taskbodyhistory.h \
    protocols/taskbodycleave.h \
    dialogs/zflyemproofsettingdialog.h \
    zstackdocptr.h \
    zstackdoc3dhelper.h \
    zstackobjectinfo.h \
    zstackobjectptr.h \
    zstackdocproxy.h \
    zroiobjsmodel.h \
    zstackobjectaccessor.h \
    zgraphptr.h \
    misc/zmarchingcube.h \
    ilastik/marching_cubes.h \
    ilastik/laplacian_smoothing.h \
    zarbsliceviewparam.h \
    dialogs/zneu3sliceviewdialog.h \
    znetbufferreader.h \
    zstackviewhelper.h \
    dvid/zdviddataslicehelper.h \
    zstackdocnullmenufactory.h \
    mvc/zpositionmapper.h \
    data3d/zstackobjecthelper.h \
    data3d/utilities.h \
    zmenuconfig.h \
    protocols/taskbodymerge.h \
    zobjsmodelmanager.h \
    zobjsmodelfactory.h \
    concurrent/zworkthread.h \
    concurrent/zworker.h \
    concurrent/ztaskqueue.h \
    dvid/zdvidbodyhelper.h \
    z3dwindowcontroller.h \
    zstackblockfactory.h \
    dvid/zdvidstackblockfactory.h \
    zstackblocksource.h \
    protocols/protocoltaskfactory.h \
    protocols/protocoltaskconfig.h \
    data3d/zstackobjectconfig.h \
    protocols/taskfalsesplitreview.h \
    protocols/taskprotocoltaskfactory.h \
    dvid/zdvidblockstream.h \
    imgproc/zstackmultiscalewatershed.h \
    protocols/taskmergereview.h \
    dialogs/zflyemtodoannotationdialog.h \
    main.h \
    dialogs/zflyemsplituploadoptiondialog.h \
    dialogs/zflyembodychopdialog.h \
    dialogs/ztestoptiondialog.h \
    dialogs/zinfodialog.h \
    dialogs/zswcisolationdialog.h \
    dialogs/zstresstestoptiondialog.h \
    dialogs/zflyembodyscreenshotdialog.h\
    dialogs/zflyemskeletonupdatedialog.h \
    dialogs/zdvidadvanceddialog.h \
    dialogs/zdvidbodypositiondialog.h \
    dialogs/neuprintquerydialog.h \
    dialogs/zflyemtodofilterdialog.h \
    dialogs/zstackframesettingdialog.h \
    dialogs/neuprintsetupdialog.h

FORMS += dialogs/settingdialog.ui \
    dialogs/frameinfodialog.ui \
    mainwindow.ui \
    dialogs/traceoutputdialog.ui \
    dialogs/bcadjustdialog.ui \
    dialogs/channeldialog.ui \
    dialogs/cannyedgedialog.ui \
    dialogs/medianfilterdialog.ui \
    dialogs/diffusiondialog.ui \
    dialogs/connectedthresholddialog.ui \
    dialogs/zmergeimagedialog.ui \
    dialogs/zrescaleswcdialog.ui \
    dialogs/distancemapdialog.ui \
    dialogs/regionexpanddialog.ui \
    dialogs/neuroniddialog.ui \
    dialogs/flyemskeletonizationdialog.ui \
    dialogs/parameterdialog.ui \
    dialogs/pixelquerydialog.ui \
    dialogs/swctypedialog.ui \
    dialogs/swcsizedialog.ui \
    dialogs/swcskeletontransformdialog.ui \
    dialogs/mexicanhatdialog.ui \
    dialogs/informationdialog.ui \
    flyemdataform.ui \
    dialogs/zalphadialog.ui \
    dialogs/helpdialog.ui \
    dialogs/projectiondialog.ui \
    dialogs/startsettingdialog.ui \
    dialogs/moviedialog.ui \
    dialogs/flyemdataquerydialog.ui \
    dialogs/flyemdataprocessdialog.ui \
    dialogs/autosaveswclistdialog.ui \
    dialogs/diagnosisdialog.ui \
    dialogs/flyemdataexportdialog.ui \
    dialogs/flyemgeosearchdialog.ui \
    dialogs/flyemgeofilterdialog.ui \
    dialogs/penwidthdialog.ui \
    dialogs/dvidobjectdialog.ui \
    dialogs/resolutiondialog.ui \
    dialogs/dvidimagedialog.ui \
    dialogs/flyemneuronthumbnaildialog.ui \
    dialogs/flyemhotspotdialog.ui \
    dialogs/flyembodyiddialog.ui \
    dialogs/zdviddialog.ui \
    dialogs/flyembodyfilterdialog.ui \
    tilemanager.ui \
    dialogs/flyembodysplitprojectdialog.ui \
    dialogs/zflyemnewbodysplitprojectdialog.ui \
    dialogs/dvidskeletonizedialog.ui \
    dialogs/zflyemroidialog.ui \
    newprojectmainwindow.ui \
    dialogs/shapepaperdialog.ui \
    dialogs/dvidoperatedialog.ui \
    dialogs/synapseimportdialog.ui \
    dialogs/flyembodymergeprojectdialog.ui \
    dialogs/zsegmentationprojectdialog.ui \
    dialogs/zmarkswcsomadialog.ui \
    dialogs/swcexportdialog.ui \
    dialogs/ztestdialog.ui \
    dialogs/ztestdialog2.ui \
    zflyemcontrolform.ui \
    dialogs/flyembodyinfodialog.ui \
    protocols/protocolchooser.ui \
    dialogs/zflyemsplitcommitdialog.ui \
    dialogs/stringlistdialog.ui \
    dialogs/flyemtododialog.ui \
    protocols/doNthingsprotocol.ui \
    protocols/synapsepredictionprotocol.ui \
    protocols/synapsepredictioninputdialog.ui \
    protocols/synapsepredictionbodyinputdialog.ui \
    protocols/synapsereviewprotocol.ui \
    protocols/synapsereviewinputdialog.ui \
    protocols/protocoldialog.ui \
    dialogs/flyemsettingdialog.ui \
    dialogs/flyemsynapsefilterdialog.ui \
    dialogs/zflyemsynapseannotationdialog.ui \
    dialogs/zcontrastprotocaldialog.ui \
    dialogs/zflyemroitooldialog.ui \
    dialogs/zdvidbodypositiondialog.ui \
    dialogs/zflyemsplituploadoptiondialog.ui \
    neu3window.ui \
    dialogs/ztestoptiondialog.ui \
    dialogs/zinfodialog.ui \
    dialogs/zswcisolationdialog.ui \
    dialogs/zstresstestoptiondialog.ui \
    dialogs/zflyembodyscreenshotdialog.ui \
    dialogs/zdvidadvanceddialog.ui \
    dialogs/zflyemskeletonupdatedialog.ui \
    dialogs/zflyemgrayscaledialog.ui \
    dialogs/zcomboeditdialog.ui \
    dialogs/zflyembodycomparisondialog.ui \
    dialogs/zflyembodysplitdialog.ui \
    dialogs/dvidbranchdialog.ui \
    dialogs/zflyemmergeuploaddialog.ui \
    dialogs/zflyemproofsettingdialog.ui \
    dialogs/zneu3sliceviewdialog.ui \
    dialogs/zflyemtodoannotationdialog.ui \
    dialogs/neuprintquerydialog.ui \
    dialogs/zflyemtodofilterdialog.ui \
    dialogs/zautotracedialog.ui \
    dialogs/zstackframesettingdialog.ui \
    dialogs/neuprintsetupdialog.ui

SOURCES += main.cpp \
    mainwindow.cpp \
    zimage.cpp \
    zslider.cpp \
    dialogs/settingdialog.cpp \
    dialogs/frameinfodialog.cpp \
    plotsettings.cpp \
    plotter.cpp \
    zinteractivecontext.cpp \
    dialogs/traceoutputdialog.cpp \
    dialogs/bcadjustdialog.cpp \
    ztraceproject.cpp \
    dialogs/channeldialog.cpp \
    zpunctum.cpp \
    dialogs/zeditswcdialog.cpp \
    dialogs/cannyedgedialog.cpp \
    zdirectionaltemplatechain.cpp \
    zrandom.cpp \
    dialogs/distancemapdialog.cpp \
    dialogs/regionexpanddialog.cpp \
    dialogs/neuroniddialog.cpp \
    dialogs/flyemskeletonizationdialog.cpp \
    dialogs/parameterdialog.cpp \
    dialogs/pixelquerydialog.cpp \
    dialogs/medianfilterdialog.cpp \
    dialogs/diffusiondialog.cpp \
    dialogs/connectedthresholddialog.cpp \
    dialogs/zrescaleswcdialog.cpp \
    z3dwindow.cpp \
    zobjsitem.cpp \
    zobjsmodel.cpp \
    zcolormap.cpp \
    zclickablelabel.cpp \
    zcolormapeditor.cpp \
    zlocsegchainconn.cpp \
    zlocsegchain.cpp \
    zcurve.cpp \
    zselectfilewidget.cpp \
    zcolormapwidgetwitheditorwindow.cpp \
    zspinbox.cpp \
    zcombobox.cpp \
    zspinboxwithslider.cpp \
    zeventlistenerparameter.cpp \
    zspanslider.cpp \
    ztest.cpp \
    znormcolormap.cpp \
    dialogs/swctypedialog.cpp \
    z3dgraph.cpp \
    dialogs/swcsizedialog.cpp \
    dialogs/swcskeletontransformdialog.cpp \
    zprocessprogressbase.cpp \
    dialogs/mexicanhatdialog.cpp \
    dialogs/informationdialog.cpp \
    zmoviescene.cpp \
    zmovieactor.cpp \
    zswcmovieactor.cpp \
    zmoviemaker.cpp \
    zmoviephotographer.cpp \
    zmoviescript.cpp \
    zpunctamovieactor.cpp \
    zstackmovieactor.cpp \
    zmoviesceneclipper.cpp \
    zmoviecamera.cpp \
    flyemdataform.cpp \
    zswcobjsmodel.cpp \
    zpunctaobjsmodel.cpp \
    zobjsmanagerwidget.cpp \
    zmoviescriptgenerator.cpp \
    zmoviestage.cpp \
    dialogs/zalphadialog.cpp \
    zqtbarprogressreporter.cpp \
    zstackdoccommand.cpp \
    zcursorstore.cpp \
    zqtmessagereporter.cpp \
    zstroke2d.cpp \
    dialogs/helpdialog.cpp \
    zswcnodeobjsmodel.cpp \
    biocytin/biocytin.cpp \
    dialogs/projectiondialog.cpp \
    zstackviewlocator.cpp \
    dialogs/startsettingdialog.cpp \
    zstackreadthread.cpp \
    zswccolorscheme.cpp \
    dialogs/moviedialog.cpp \
    zpunctumio.cpp \
    dialogs/flyemdataquerydialog.cpp \
    dialogs/flyemdataprocessdialog.cpp \
    dialogs/autosaveswclistdialog.cpp \
    zswcfilelistmodel.cpp \
    zcommandline.cpp \
    neutube.cpp \
    dialogs/diagnosisdialog.cpp \
    zpaintbundle.cpp \
    dialogs/flyemdataexportdialog.cpp \
    zflyemqueryview.cpp \
    dialogs/flyemgeosearchdialog.cpp \
    zqslogmessagereporter.cpp \
    dialogs/flyemgeofilterdialog.cpp \
    zactionactivator.cpp \
    zswccurvaturefeatureanalyzer.cpp \
    zstackdocmenustore.cpp \
    zstackdocmenufactory.cpp \
    dialogs/penwidthdialog.cpp \
    dialogs/dvidobjectdialog.cpp \
    zpainter.cpp \
    dialogs/resolutiondialog.cpp \
    dvid/zdvidrequest.cpp \
    zmatlabprocess.cpp \
    zneuronseed.cpp \
    dialogs/dvidimagedialog.cpp \
    ztiledstackframe.cpp \
    ztilemanager.cpp \
    ztilemanagerview.cpp \
    ztilegraphicsitem.cpp \
    ztileinfo.cpp \
    dialogs/flyemneuronthumbnaildialog.cpp \
    zmultitaskmanager.cpp \
    zinteractionevent.cpp \
    dialogs/flyemhotspotdialog.cpp \
    zworkspacefactory.cpp \
    dvid/zdvidreader.cpp \
    dialogs/flyembodyiddialog.cpp \
    dialogs/zdviddialog.cpp \
    dialogs/zdvidtargetproviderdialog.cpp \
    dialogs/flyembodyfilterdialog.cpp \
    zflyemdvidreader.cpp \
    zstroke2darray.cpp \
    tilemanager.cpp \
    zactionfactory.cpp \
    zstackreadfactory.cpp \
    zstackdoclabelstackfactory.cpp \
    zsparseobject.cpp \
    zlabelcolortable.cpp \
    zdocplayer.cpp \
    zlinesegmentobject.cpp \
    zopenvdbobject.cpp \
    dialogs/flyembodysplitprojectdialog.cpp \
    dialogs/zflyemnewbodysplitprojectdialog.cpp \
    zstroke2dobjsmodel.cpp \
    zdocplayerobjsmodel.cpp \
    zabstractmodelpresenter.cpp \
    zstackobjectarray.cpp \
    zwindowfactory.cpp \
    dvid/zdvidwriter.cpp \
    dialogs/dvidskeletonizedialog.cpp \
    zdialogfactory.cpp \
    zwidgetfactory.cpp \
    zlabelededitwidget.cpp \
    zlabeledcombowidget.cpp \
    dialogs/zspinboxdialog.cpp \
    zbuttonbox.cpp \
    zkeyeventswcmapper.cpp \
    dialogs/zflyemroidialog.cpp \
    newprojectmainwindow.cpp \
    zmouseeventmapper.cpp \
    dialogs/shapepaperdialog.cpp \
    zframefactory.cpp \
    zactionbutton.cpp \
    dvid/zdvidbufferreader.cpp \
    zmouseevent.cpp \
    zmouseeventrecorder.cpp \
    zmouseeventprocessor.cpp \
    zstackoperator.cpp \
    zsleeper.cpp \
    dialogs/dvidoperatedialog.cpp \
    z3dwindowfactory.cpp \
    zthreadfuturemap.cpp \
    zstackball.cpp \
    zstackdochittest.cpp \
    zkeyeventmapper.cpp \
    zuserinputevent.cpp \
    zstackobjectmanager.cpp \
    zstackobjectgroup.cpp \
    zcolorscheme.cpp \
    zpunctumcolorscheme.cpp \
    zstackpatch.cpp \
    zrect2d.cpp \
    zobjectcolorscheme.cpp \
    dialogs/synapseimportdialog.cpp \
    dialogs/flyembodymergeprojectdialog.cpp \
    zstackdvidgrayscalefactory.cpp \
    zstackdocreader.cpp \
    dialogs/flyemprojectdialog.cpp \
    dialogs/zsegmentationprojectdialog.cpp \
    zsegmentationproject.cpp \
    zsegmentationprojectmodel.cpp \
    dialogs/zsubtractswcsdialog.cpp \
    dialogs/zmarkswcsomadialog.cpp \
    zlabeledspinboxwidget.cpp \
    dialogs/zspinboxgroupdialog.cpp \
    dialogs/zautotracedialog.cpp \
    zstackviewmanager.cpp \
    zstackviewparam.cpp \
    zflyemprojectmanager.cpp \
    zflyemdataloader.cpp \
    dialogs/swcexportdialog.cpp \
    zprogressmanager.cpp \
    dvid/zdvidtile.cpp \
    dvid/zdvidresolution.cpp \
    dvid/zdvidtileinfo.cpp \
    dvid/zdvidversionmodel.cpp \
    zmessagemanager.cpp \
    zmessageprocessor.cpp \
    zmessage.cpp \
    zmainwindowmessageprocessor.cpp \
    dialogs/ztestdialog.cpp \
    dialogs/ztestdialog2.cpp \
    zstackdocloader.cpp \
    zstackwidget.cpp \
    dvid/zdvidversiondag.cpp \
    dvid/zdvidversion.cpp \
    dvid/zdvidversionnode.cpp \
    zbodysplitbutton.cpp \
    zmessagefactory.cpp \
    zmessagemanagermodel.cpp \
    zflyemcontrolform.cpp \
    dvid/zdvidtileensemble.cpp \
    dvid/zdvidlabelslice.cpp \
    zsttransform.cpp \
    zpixmap.cpp \
    dvid/zdvidgrayslice.cpp \
    dvid/zdvidsparsestack.cpp \
    zwidgetmessage.cpp \
    zprogresssignal.cpp \
    zkeyeventstrokemapper.cpp \
    zkeyoperation.cpp \
    zkeyoperationmap.cpp \
    zkeyoperationconfig.cpp \
    dvid/zdvidsparsevolslice.cpp \
    ztextmessage.cpp \
    ztextmessagefactory.cpp \
    z3dgraphfactory.cpp \
    zkeyeventmapperfactory.cpp \
    zkeyoperationmapsequence.cpp \
    zpuncta.cpp \
    dialogs/flyembodyinfodialog.cpp \
    biocytin/zbiocytinprojectiondoc.cpp \
    dialogs/zflyemsplitcommitdialog.cpp \
    zstackdocfactory.cpp \
    zintcuboidobj.cpp \
    ztask.cpp \
    zstackobjectpainter.cpp \
    zslicedpuncta.cpp \
    zmultiscalepixmap.cpp \
    biocytin/zbiocytinprojmaskfactory.cpp \
    zpunctumselector.cpp \
    zgraphobjsmodel.cpp \
    zsurfaceobjsmodel.cpp \
    dvid/zdvidsynapse.cpp \
    dvid/zdvidsynapseensenmble.cpp \
    zcubearray.cpp \
    dvid/zdvidsynapsecommand.cpp \
    dvid/zdvidannotationcommand.cpp \
    dvid/zflyembookmarkcommand.cpp \
    dvid/zdvidannotation.cpp \
    dialogs/stringlistdialog.cpp \
    zroiwidget.cpp \
    dialogs/flyemtododialog.cpp \
    zstackdocselector.cpp \
    dialogs/flyemsettingdialog.cpp \
    protocols/protocolswitcher.cpp \
    protocols/protocolchooser.cpp \
    protocols/protocolmetadata.cpp \
    protocols/protocoldialog.cpp \
    protocols/doNthingsprotocol.cpp \
    protocols/synapsepredictionprotocol.cpp \
    protocols/synapsepredictioninputdialog.cpp \
    protocols/synapsepredictionbodyinputdialog.cpp \
    protocols/synapsereviewprotocol.cpp \
    protocols/synapsereviewinputdialog.cpp \
    zactionlibrary.cpp \
    zmenufactory.cpp \
    zcrosshair.cpp \
    zapplication.cpp \
    dialogs/flyemsynapsefilterdialog.cpp \
    dialogs/zflyemsynapseannotationdialog.cpp \
    zdvidutil.cpp \
    dialogs/zcontrastprotocaldialog.cpp \
    dialogs/zflyemroitooldialog.cpp \
    dvid/zdvidpatchdatafetcher.cpp \
    dvid/zdvidpatchdataupdater.cpp \
    dvid/zdviddatafetcher.cpp \
    dvid/zdviddataupdater.cpp \
    dialogs/zdvidbodypositiondialog.cpp \
    zcubearraymovieactor.cpp \
    dialogs/zflyemsplituploadoptiondialog.cpp \
    dialogs/zflyembodychopdialog.cpp \
    zstackdocdatabuffer.cpp \
    dialogs/ztestoptiondialog.cpp \
    dialogs/zinfodialog.cpp \
    dialogs/zswcisolationdialog.cpp \
    dialogs/zstresstestoptiondialog.cpp \
    dialogs/zflyembodyscreenshotdialog.cpp \
    dialogs/zflyemskeletonupdatedialog.cpp \
    dialogs/zdvidadvanceddialog.cpp \
    dvid/zdvidroi.cpp \
    z3dmainwindow.cpp \
    dvid/zdvidgrayscale.cpp \
    zscrollslicestrategy.cpp \
    dvid/zdvidgrayslicescrollstrategy.cpp \
    zviewproj.cpp \
    dialogs/zflyemgrayscaledialog.cpp \
    dvid/zdvidneurontracer.cpp \
    zorthoviewhelper.cpp \
    dialogs/zcomboeditdialog.cpp \
    dialogs/zflyembodycomparisondialog.cpp \
    dvid/zdvidstore.cpp \
    zglobal.cpp \
    dvid/zdvidresultservice.cpp \
    zstackgarbagecollector.cpp \
    dialogs/zflyembodysplitdialog.cpp \
    zxmldoc.cpp \
    neu3window.cpp \
    zstackdockeyprocessor.cpp \
    zspinboxwithscrollbar.cpp \
    zsysteminfo.cpp \
    zvertexbufferobject.cpp \
    zvertexarrayobject.cpp \
    zmesh.cpp \
    zmeshio.cpp \
    zmeshutils.cpp \
    zstringutils.cpp \
    ztakescreenshotwidget.cpp \
    zioutils.cpp \
    zmeshobjsmodel.cpp \
    protocols/taskprotocoltask.cpp \
    protocols/taskbodyreview.cpp \
    dialogs/dvidbranchdialog.cpp \
    protocols/tasktesttask.cpp \
    protocols/tasksplitseeds.cpp \
    protocols/bodyprefetchqueue.cpp \
    zopenglwidget.cpp \
    zstackreader.cpp \
    dvid/zdvidpath.cpp \
    misc/zvtkutil.cpp \
    zstackdocaccessor.cpp \
    zcontrastprotocol.cpp \
    dialogs/zflyemmergeuploaddialog.cpp \
    zmeshfactory.cpp \
    protocols/taskbodyhistory.cpp \
    protocols/taskbodycleave.cpp \
    dialogs/zflyemproofsettingdialog.cpp \
    zstackdoc3dhelper.cpp \
    zstackobjectinfo.cpp \
    zstackdocproxy.cpp \
    zroiobjsmodel.cpp \
    zstackobjectaccessor.cpp \
    zgraphptr.cpp \
    misc/zmarchingcube.cpp \
    ilastik/marching_cubes.cpp \
    ilastik/laplacian_smoothing.cpp \
    zarbsliceviewparam.cpp \
    dialogs/zneu3sliceviewdialog.cpp \
    znetbufferreader.cpp \
    zstackviewhelper.cpp \
    dvid/zdviddataslicehelper.cpp \
    zstackdocnullmenufactory.cpp \
    data3d/zstackobjecthelper.cpp \
    data3d/utilities.cpp \
    zmenuconfig.cpp \
    protocols/taskbodymerge.cpp \
    zobjsmodelmanager.cpp \
    zobjsmodelfactory.cpp \
    concurrent/zworkthread.cpp \
    concurrent/zworker.cpp \
    concurrent/ztaskqueue.cpp \
    dvid/zdvidbodyhelper.cpp \
    z3dwindowcontroller.cpp \
    zstackblockfactory.cpp \
    dvid/zdvidstackblockfactory.cpp \
    zstackblocksource.cpp \
    protocols/protocoltaskfactory.cpp \
    protocols/protocoltaskconfig.cpp \
    protocols/taskfalsesplitreview.cpp \
    protocols/taskprotocoltaskfactory.cpp \
    data3d/zstackobjectconfig.cpp \
    dvid/zdvidblockstream.cpp \
    imgproc/zstackmultiscalewatershed.cpp \
    protocols/taskmergereview.cpp \
    dialogs/zflyemtodoannotationdialog.cpp \
    dialogs/neuprintquerydialog.cpp \
    dialogs/zflyemtodofilterdialog.cpp \
    dialogs/zstackframesettingdialog.cpp \
    dialogs/neuprintsetupdialog.cpp

DISTFILES += \
    Resources/shader/wblended_final.frag \
    Resources/shader/wblended_init.frag

#debugging
message("[[ DEFINE ]]: $${DEFINES}")
message("[[ QMAKE_CXXFLAGS ]]: $${QMAKE_CXXFLAGS}")
message("[[ CONDA_ENV ]]: $${CONDA_ENV}")
message("[[ LIBS ]]: $${LIBS}")
message("[[ TARGET ]]: $$app_name")
message("[[ OUT_PWD ]]: $${OUT_PWD}")
macx {
  message("[[ Mac Target ]]: $$QMAKE_MACOSX_DEPLOYMENT_TARGET")
}
