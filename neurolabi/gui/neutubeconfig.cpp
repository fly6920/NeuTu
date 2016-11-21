#include "neutubeconfig.h"

#include <iostream>

#ifdef _QT_GUI_USED_
#include <QDir>
#include <QsLog.h>
#include <QDebug>
#include <QFileInfo>
#include <QString>
#endif

#include "tz_cdefs.h"
#include "zxmldoc.h"
#include "zstring.h"
#include "zlogmessagereporter.h"
#include "neutube.h"
#include "zjsonobject.h"
#include "zjsonparser.h"

using namespace std;

NeutubeConfig::NeutubeConfig() :
  #ifdef _QT_GUI_USED_
    m_settings(QSettings::UserScope, "NeuTu-be")
  #endif
{
  init();
}
/*
NeutubeConfig::NeutubeConfig(const NeutubeConfig& config) :
  m_segmentationClassifThreshold(0.5), m_isSettingOn(true)
{
  UNUSED_PARAMETER(&config);
  m_messageReporter = new ZLogMessageReporter;
}
*/

NeutubeConfig::~NeutubeConfig()
{
  delete m_messageReporter;
#ifdef _QT_GUI_USED_
//  delete m_traceStream;
#endif
}

void NeutubeConfig::init()
{
  m_segmentationClassifThreshold = 0.5;
  m_isSettingOn = true;
  m_isStereoOn = true;
  m_autoSaveInterval = 600000;
  m_autoSaveEnabled =true;
  m_usingNativeDialog = true;

  m_messageReporter = new ZLogMessageReporter;
  setDefaultSoftwareName();
#ifdef _QT_GUI_USED_
  m_workDir = m_settings.value("workDir").toString().toStdString();
#if 0
  QString traceFilePath(getPath(NeutubeConfig::LOG_TRACE).c_str());
  QFileInfo fileInfo(traceFilePath);
  if (fileInfo.exists()) {
    QFile::rename(traceFilePath, traceFilePath + ".bk");
  }
  QFile *file = new QFile(traceFilePath);
  if (file->open(QIODevice::WriteOnly)) {
    m_traceStream = new QDebug(file);
  } else {
    m_traceStream = new QDebug(QtDebugMsg);
  }
#endif
  //  std::cout << m_settings.fileName().toStdString() << std::endl;
#endif

  m_loggingProfile = false;
  m_verboseLevel = 1;

  updateLogDir();

}

void NeutubeConfig::setDefaultSoftwareName()
{
  m_softwareName = "NeuTu";
}

void NeutubeConfig::setTestSoftwareName()
{
  setDefaultSoftwareName();
  m_softwareName += "_test";
}

void NeutubeConfig::SetDefaultSoftwareName()
{
  getInstance().setDefaultSoftwareName();
}

void NeutubeConfig::SetTestSoftwareName()
{
  getInstance().setTestSoftwareName();
}

void NeutubeConfig::setWorkDir(const string str)
{
  if (m_workDir == m_logDir) { //Reset log dir
    m_logDir = "";
    m_logDestDir = "";
  }

  m_workDir = str;
#ifdef _QT_GUI_USED_
  getSettings().setValue("workDir", m_workDir.c_str());
#endif

  updateLogDir();
}

void NeutubeConfig::SetApplicationDir(const string &str)
{
  getInstance().setApplicationDir(str);
}

void NeutubeConfig::updateLogDir()
{
  if (m_logDir.empty()) {
    ZString dir = getPath(WORKING_DIR);
#if defined(_QT_GUI_USED_) && defined(_FLYEM_)
    if (dir.startsWith("/groups/flyem/")) {
      m_logDir = "/opt/neutu_log/" + NeuTube::GetCurrentUserName();
      m_logDestDir = "/groups/flyem/data/neutu_log/" + NeuTube::GetCurrentUserName();
      if (!QDir(m_logDir.c_str()).exists()) {
        m_logDir = m_logDestDir;
        m_logDestDir = "";
      }
    }
#endif
    if (m_logDir.empty()) {
      m_logDir = dir;
      m_logDestDir = "";
    }
  }
}

void NeutubeConfig::operator=(const NeutubeConfig& config)
{
  UNUSED_PARAMETER(&config);
}

bool NeutubeConfig::load(const std::string &filePath)
{
#ifdef _DEBUG_
  cout << "Loading configuration ..." << endl;
#endif

  if (fexist(filePath.c_str())) {
    ZXmlDoc doc;
    doc.parseFile(filePath);

    doc.printInfo();

    ZXmlNode node =
        doc.getRootElement().queryNode("FlyEmQASegmentationClassifier");
    if (!node.empty()) {
      m_segmentationClassifierPath =
          ZString::absolutePath(m_applicationDir, node.stringValue());
    }

    node = doc.getRootElement().queryNode("dataPath");
    m_dataPath = node.stringValue();

    node = doc.getRootElement().queryNode("docUrl");
    m_docUrl = node.stringValue();

    node = doc.getRootElement().queryNode("developPath");
    m_developPath = node.stringValue();

    if (m_dataPath.empty() && !m_developPath.empty()) {
      m_dataPath = m_developPath + "/neurolabi/data";
    }

    node = doc.getRootElement().queryNode("FlyEmQASegmentationTraining");
    if (!node.empty()) {
      ZXmlNode childNode = node.queryNode("test");
      m_segmentationTrainingTestPath = childNode.stringValue();
      childNode = node.queryNode("truth");
      m_segmentationTrainingTruthPath = childNode.stringValue();
      childNode = node.queryNode("feature");
      ZString str = childNode.stringValue();
      m_bodyConnectionFeature = str.toWordArray();
    }

    node = doc.getRootElement().queryNode("FlyEmQASegmentationEvaluation");
    if (!node.empty()) {
      ZXmlNode childNode = node.queryNode("test");
      m_segmentationEvaluationTestPath = childNode.stringValue();
      childNode = node.queryNode("truth");
      m_segmentationEvaluationTruthPath = childNode.stringValue();
      childNode = node.queryNode("threshold");
      m_segmentationClassifThreshold = childNode.doubleValue();
    }

    node = doc.getRootElement().queryNode("SwcRepository");
    if (!node.empty()) {
      m_swcRepository = node.stringValue();
    }

    node = doc.getRootElement().queryNode("Settings");
    if (!node.empty()) {
      if (node.getAttribute("status") == "off") {
        m_isSettingOn = false;
      }
    }

    node = doc.getRootElement().queryNode("Stereo");
    if (!node.empty()) {
      if (node.getAttribute("status") == "off") {
        m_isStereoOn = false;
      }
    }

    node = doc.getRootElement().queryNode("NativeFileDialog");
    if (!node.empty()) {
      if (node.getAttribute("status") == "off") {
        m_usingNativeDialog = false;
      }
    }

    node = doc.getRootElement().queryNode("MainWindow");
    if (!node.empty()) {
      m_mainWindowConfig.loadXmlNode(&node);
    }

    node = doc.getRootElement().queryNode("Z3DWindow");
    if (!node.empty()) {
      m_z3dWindowConfig.loadXmlNode(&node);
    }

    node = doc.getRootElement().queryNode("Application");
    if (!node.empty()) {
      m_application = node.stringValue();
      if (m_application == "Biocytin") {
        m_mainWindowConfig.setExpandSwcWith3DWindow(true);
      }
    }

    node = doc.getRootElement().queryNode("Object_Manager");
    if (!node.empty()) {
      m_objManagerConfig.loadXmlNode(&node);
    }

    node = doc.getRootElement().queryNode("Autosave");
    if (!node.empty()) {
      ZXmlNode childNode = node.queryNode("Enabled");
      if (!childNode.empty()) {
        m_autoSaveEnabled = childNode.intValue();
      }

      childNode = node.queryNode("Interval");
      if (!childNode.empty()) {
        m_autoSaveInterval = childNode.intValue();
      }
    }

    ZLogMessageReporter *reporter =
        dynamic_cast<ZLogMessageReporter*>(m_messageReporter);
    if (reporter != NULL) {
      reporter->setInfoFile(getPath(LOG_APPOUT));
      reporter->setErrorFile(getPath(LOG_WARN));
      reporter->setErrorFile(getPath(LOG_ERROR));
    }

    if (GET_APPLICATION_NAME == "General") {
      m_softwareName = "neuTube";
    }

    return true;
  }

  return false;
}

void NeutubeConfig::print()
{
  cout << m_dataPath << endl;
#if 0
  cout << "SWC repository: " << m_swcRepository << endl;
  cout << "Body connection classifier: " << m_segmentationClassifierPath << endl;
  cout << "Body connection training: " << endl;
  cout << "  data: " << m_segmentationTrainingTestPath << endl;
  cout << "  ground truth: " << m_segmentationTrainingTruthPath << endl;
  cout << "Body connection evaluation: " << endl;
  cout << "  data: " << m_segmentationEvaluationTestPath << endl;
  cout << "  ground truth: " << m_segmentationEvaluationTruthPath << endl;

  cout << "Bcf: ";
  for (vector<string>::const_iterator iter = m_bodyConnectionFeature.begin();
       iter != m_bodyConnectionFeature.end(); ++iter) {
    cout << *iter << " ";
  }
  cout << endl;
#endif
  cout << "Application dir: " << getApplicatinDir() << endl;
  cout << "Autosave dir: " << getPath(AUTO_SAVE) << endl;
  cout << "Autosave interval: " << m_autoSaveInterval << endl;
  cout << "Log dir: " << getPath(LOG_DIR) << endl;
  cout << "Log dest dir: " << getPath(LOG_DEST_DIR) << endl;
  cout << endl;
}

std::string NeutubeConfig::getPath(Config_Item item) const
{
  switch (item) {
  case DATA:
  {
    std::string dataPath;
#ifdef _QT_GUI_USED_
    if (m_settings.contains("data_dir")) {
      dataPath = m_settings.value("data_dir").toString().toStdString();
    }
#endif
    if (dataPath.empty()) {
      return m_dataPath;
    }

    return dataPath;
  }
    break;
  case FLYEM_BODY_CONN_CLASSIFIER:
    return m_segmentationClassifierPath;
  case FLYEM_BODY_CONN_TRAIN_DATA:
    return m_segmentationTrainingTestPath;
  case FLYEM_BODY_CONN_TRAIN_TRUTH:
    return m_segmentationTrainingTruthPath;
  case FLYEM_BODY_CONN_EVAL_DATA:
    return m_segmentationEvaluationTestPath;
  case FLYEM_BODY_CONN_EVAL_TRUTH:
    return m_segmentationEvaluationTruthPath;
  case CONFIGURE_FILE:
    return getConfigPath();
  case SWC_REPOSOTARY:
    return m_swcRepository;
  case AUTO_SAVE:
#ifdef _QT_GUI_USED_
      return
          QDir(getPath(WORKING_DIR).c_str()).filePath("autosave").toStdString();
#else
      return ZString::fullPath(getPath(WORKING_DIR), "autosave");
#endif
  case SKELETONIZATION_CONFIG:
    return getApplicatinDir() + ZString::FileSeparator + "json" +
        ZString::FileSeparator + "skeletonize_fib25_len40.json";
  case DOCUMENT:
    return m_docUrl;
  case TMP_DATA:
  {
    std::string tmpDir;
#if defined(_QT_GUI_USED_)
    std::string user = NeuTube::GetCurrentUserName();
    tmpDir = QDir::tempPath().toStdString() + "/.neutube.z." + user;
    QDir tmpDirObj(tmpDir.c_str());
    if (!tmpDirObj.exists()) {
      if (!tmpDirObj.mkpath(tmpDir.c_str())) {
        LERROR() << "Failed to make tmp directory: " << tmpDir;
        tmpDir = "";
      }
    }
#else
    tmpDir = "/tmp";
#endif
    return tmpDir;
  }
  case WORKING_DIR:
    if (m_workDir.empty()) {
#ifdef _QT_GUI_USED_
      return QDir::home().filePath(".neutube.z").toStdString();
#else
      return getApplicatinDir();
#endif
    }
    return m_workDir;
  case LOG_DIR:
    return m_logDir;
  case LOG_DEST_DIR:
    return m_logDestDir;
  case LOG_FILE:
#ifdef _QT_GUI_USED_
    return QDir(getPath(LOG_DIR).c_str()).filePath("log.txt").toStdString();
#else
    return ZString::fullPath(getPath(WORKING_DIR), "log.txt");
#endif
  case LOG_APPOUT:
#ifdef _QT_GUI_USED_
    return QDir(getPath(WORKING_DIR).c_str()).filePath("log_appout.txt").toStdString();
#else
    return ZString::fullPath(getPath(WORKING_DIR), "log_appout.txt");
#endif
  case LOG_TRACE:
#ifdef _QT_GUI_USED_
    return QDir(getPath(LOG_DIR).c_str()).filePath("log_trace.txt").toStdString();
#else
    return ZString::fullPath(getPath(WORKING_DIR), "log_trace.txt");
#endif
  case LOG_WARN:
#ifdef _QT_GUI_USED_
    return QDir(getPath(WORKING_DIR).c_str()).filePath("log_warn.txt").toStdString();
#else
    return ZString::fullPath(getPath(WORKING_DIR), "log_warn.txt");
#endif
  case LOG_ERROR:
#ifdef _QT_GUI_USED_
    return QDir(getPath(WORKING_DIR).c_str()).filePath("log_error.txt").toStdString();
#else
    return ZString::fullPath(getPath(WORKING_DIR), "log_error.txt");
#endif
  default:
    break;
  }

  return "";
}

NeutubeConfig::MainWindowConfig::MainWindowConfig() : m_tracingOn(true),
  m_isMarkPunctaOn(true), m_isSwcEditOn(true), m_isExpandSwcWith3DWindow(false),
  m_isProcessBinarizeOn(true), m_isMaskToSwcOn(true), m_isBinaryToSwcOn(true),
  m_isThresholdControlOn(true)
{
}

void NeutubeConfig::MainWindowConfig::loadXmlNode(const ZXmlNode *node)
{
  ZXmlNode childNode = node->queryNode("tracing");
  if (!childNode.empty()) {
    enableTracing(childNode.getAttribute("status") != "off");
  } else {
    enableTracing(true);
  }

  childNode = node->queryNode("markPuncta");
  if (!childNode.empty()) {
    enableMarkPuncta(childNode.getAttribute("status") != "off");
  } else {
    enableMarkPuncta(true);
  }

  childNode = node->queryNode("swcEdit");
  if (!childNode.empty()) {
    enableSwcEdit(childNode.getAttribute("status") != "off");
  } else {
    enableSwcEdit(true);
  }

  childNode = node->queryNode("mergeImage");
  if (!childNode.empty()) {
    enableMergeImage(childNode.getAttribute("status") != "off");
  } else {
    enableMergeImage(true);
  }

  childNode = node->queryNode("Expand");
  if (!childNode.empty()) {
    ZXmlNode expandNode = childNode.queryNode("Neuron_Network");
    if (!expandNode.empty()) {
      m_isExpandNeuronNetworkOn = (expandNode.getAttribute("status") != "off");
    }

    expandNode = childNode.queryNode("Tracing_Result");
    if (!expandNode.empty()) {
      m_isExpandTracingResultOn = (expandNode.getAttribute("status") != "off");
    }

    expandNode = childNode.queryNode("V3D_APO");
    if (!expandNode.empty()) {
      m_isExpandV3dApoOn = (expandNode.getAttribute("status") != "off");
    }

    expandNode = childNode.queryNode("V3D_Marker");
    if (!expandNode.empty()) {
      m_isExpandV3dMarkerOn = (expandNode.getAttribute("status") != "off");
    }
  }

  childNode = node->queryNode("Process");
  if (!childNode.empty()) {
    ZXmlNode processNode = childNode.queryNode("Binarize");
    if (!processNode.empty()) {
      m_isProcessBinarizeOn = (processNode.getAttribute("status") != "off");
    }
  }

  childNode = node->queryNode("Trace");
  if (!childNode.empty()) {
    ZXmlNode traceNode = childNode.queryNode("Binary_To_Swc");
    if (!traceNode.empty()) {
      m_isBinaryToSwcOn = (traceNode.getAttribute("status") != "off");
    }

    traceNode = childNode.queryNode("Mask_To_Swc");
    if (!traceNode.empty()) {
      m_isMaskToSwcOn = (traceNode.getAttribute("status") != "off");
    }
  }

  childNode = node->queryNode("Threshold_Control");
  if (!childNode.empty()) {
    setThresholdControl(childNode.getAttribute("status") != "off");
  }
}

NeutubeConfig::Z3DWindowConfig::Z3DWindowConfig() : m_isUtilsOn(true),
  m_isVolumeOn(true), m_isGraphOn(true), m_isSwcsOn(true), m_isTubesOn(true),
  m_isPunctaOn(true), m_isMeshOn(true), m_isTensorOn(true), m_isAxisOn(true),
  m_isBackgroundOn(true)
{
}

void NeutubeConfig::Z3DWindowConfig::loadXmlNode(const ZXmlNode *node)
{
  ZXmlNode childNode = node->queryNode("Utils");
  if (!childNode.empty()) {
    enableUtils(childNode.getAttribute("status") != "off");
  } else {
    enableUtils(true);
  }

  childNode = node->queryNode("Volume");
  if (!childNode.empty()) {
    enableVolume(childNode.getAttribute("status") != "off");
  } else {
    enableVolume(true);
  }

  childNode = node->queryNode("Graph");
  if (!childNode.empty()) {
    enableGraph(childNode.getAttribute("status") != "off");
    m_graphTabConfig.loadXmlNode(&childNode);
  } else {
    enableGraph(true);
  }

  childNode = node->queryNode("Swcs");
  if (!childNode.empty()) {
    enableSwcs(childNode.getAttribute("status") != "off");
    m_swcTabConfig.loadXmlNode(&childNode);
  } else {
    enableSwcs(true);
  }

  childNode = node->queryNode("Tubes");
  if (!childNode.empty()) {
    enableTubes(childNode.getAttribute("status") != "off");
  } else {
    enableTubes(true);
  }

  childNode = node->queryNode("Puncta");
  if (!childNode.empty()) {
    enablePuncta(childNode.getAttribute("status") != "off");
  } else {
    enablePuncta(true);
  }

  childNode = node->queryNode("Tensor");
  if (!childNode.empty()) {
    enableTensor(childNode.getAttribute("status") != "off");
  } else {
    enableTensor(true);
  }

  childNode = node->queryNode("Mesh");
  if (!childNode.empty()) {
    enableMesh(childNode.getAttribute("status") != "off");
  } else {
    enableMesh(true);
  }

  childNode = node->queryNode("Background");
  if (!childNode.empty()) {
    enableBackground(childNode.getAttribute("status") != "off");
  } else {
    enableBackground(true);
  }

  childNode = node->queryNode("Axis");
  if (!childNode.empty()) {
    enableAxis(childNode.getAttribute("status") != "off");
  } else {
    enableAxis(true);
  }
}

NeutubeConfig::Z3DWindowConfig::GraphTabConfig::GraphTabConfig() :
  m_isVisible(true), m_opacity(1.0)
{
}

void NeutubeConfig::Z3DWindowConfig::GraphTabConfig::loadXmlNode(const ZXmlNode *node)
{
  ZXmlNode childNode = node->queryNode("Visible");
  if (!childNode.empty()) {
    m_isVisible = childNode.intValue() > 0;
  }

  childNode = node->queryNode("Opacity");
  if (!childNode.empty()) {
    m_opacity = childNode.doubleValue();
  }
}

NeutubeConfig::Z3DWindowConfig::SwcTabConfig::SwcTabConfig() : m_primitive("Normal"),
  m_colorMode("Branch Type"), m_zscale(1.0)
{
}


void NeutubeConfig::Z3DWindowConfig::SwcTabConfig::loadXmlNode(const ZXmlNode *node)
{
  ZXmlNode childNode = node->queryNode("Primitive");
  if (!childNode.empty()) {
    m_primitive = childNode.stringValue();
  }

  childNode = node->queryNode("Color_Mode");
  if (!childNode.empty()) {
    m_colorMode = childNode.stringValue();
  }

  childNode = node->queryNode("ZScale");
  if (!childNode.empty()) {
    m_zscale = childNode.doubleValue();
  }
}

NeutubeConfig::ObjManagerConfig::ObjManagerConfig() :
  m_isSwcOn(true), m_isSwcNodeOn(true), m_isPunctaOn(true)
{

}

void NeutubeConfig::ObjManagerConfig::loadXmlNode(
    const ZXmlNode *node)
{
  ZXmlNode childNode = node->queryNode("Swc");
  if (!childNode.empty()) {
    m_isSwcOn = (childNode.getAttribute("status") != "off");
  } else {
    m_isSwcOn = true;
  }

  childNode = node->queryNode("Swc_Node");
  if (!childNode.empty()) {
    m_isSwcNodeOn = (childNode.getAttribute("status") != "off");
  } else {
    m_isSwcNodeOn = true;
  }

  childNode = node->queryNode("Puncta");
  if (!childNode.empty()) {
    m_isPunctaOn = (childNode.getAttribute("status") != "off");
  } else {
    m_isPunctaOn = true;
  }
}

void NeutubeConfig::configure(const ZJsonObject &obj)
{
  if (obj.hasKey("profiling")) {
    m_loggingProfile = ZJsonParser::booleanValue(obj["profiling"]);
  }

  if (obj.hasKey("verbose")) {
    m_verboseLevel = ZJsonParser::integerValue(obj["verbose"]);
  }
}

void NeutubeConfig::enableProfileLogging(bool on)
{
#ifdef _QT_GUI_USED_
  m_settings.setValue("profiling", on);
#else
  m_loggingProfile = on;
#endif
}

void NeutubeConfig::setVerboseLevel(int level)
{
#ifdef _QT_GUI_USED_
  m_settings.setValue("verbose", level);
#else
  m_verboseLevel = on;
#endif
}

int NeutubeConfig::getVerboseLevel() const
{
#ifdef _QT_GUI_USED_
  if (m_settings.contains("verbose")) {
    return m_settings.value("verbose").toInt();
  }
#endif

  return m_verboseLevel;
}

bool NeutubeConfig::parallelTileFetching() const
{
#ifdef _QT_GUI_USED_
  if (m_settings.contains("parallel_tile")) {
    return m_settings.value("parallel_tile").toBool();
  }
#endif

  return true;
}

void NeutubeConfig::setParallelTileFetching(bool on)
{
#ifdef _QT_GUI_USED_
  m_settings.setValue("parallel_tile", on);
#endif
}

void NeutubeConfig::SetParallelTileFetching(bool on)
{
  getInstance().setParallelTileFetching(on);
}

bool NeutubeConfig::loggingProfile() const
{
#ifdef _QT_GUI_USED_
  if (m_settings.contains("profiling")) {
    return m_settings.value("profiling").toBool();
  }
#endif

  return m_loggingProfile;
}

void NeutubeConfig::enableAutoStatusCheck(bool on)
{
#ifdef _QT_GUI_USED_
  m_settings.setValue("auto_status", on);
#endif
}

bool NeutubeConfig::autoStatusCheck() const
{
#if _QT_GUI_USED_
  if (m_settings.contains("auto_status")) {
    return m_settings.value("auto_status").toBool();
  }
#endif

  return true;
}

void NeutubeConfig::EnableAutoStatusCheck(bool on)
{
  getInstance().enableAutoStatusCheck(on);
}

bool NeutubeConfig::AutoStatusCheck()
{
  return getInstance().autoStatusCheck();
}

void NeutubeConfig::EnableProfileLogging(bool on)
{
  getInstance().enableProfileLogging(on);
}

bool NeutubeConfig::LoggingProfile()
{
  return getInstance().loggingProfile();
}

int NeutubeConfig::GetVerboseLevel()
{
  return getInstance().getVerboseLevel();
}

void NeutubeConfig::SetVerboseLevel(int level)
{
  getInstance().setVerboseLevel(level);
}

bool NeutubeConfig::ParallelTileFetching()
{
  return getInstance().parallelTileFetching();
}

void NeutubeConfig::Configure(const ZJsonObject &obj)
{
  getInstance().configure(obj);
}

std::string NeutubeConfig::GetSoftwareName()
{
  return getInstance().getSoftwareName();
}


#ifdef _QT_GUI_USED_
QString NeutubeConfig::GetFlyEmConfigPath()
{
  QString path = GetSettings().value("flyem_config").toString();

  return path;
}

void NeutubeConfig::SetFlyEmConfigPath(const QString &path)
{
  GetSettings().setValue("flyem_config", path);
}

void NeutubeConfig::UseDefaultFlyEmConfig(bool on)
{
  GetSettings().setValue("default_flyem_config", on);
}

QString NeutubeConfig::GetNeuTuServer()
{
  return GetSettings().value("neutu_server").toString();
}

bool NeutubeConfig::UsingDefaultFlyemConfig()
{
  if (GetSettings().contains("default_flyem_config")) {
    return GetSettings().value("default_flyem_config").toBool();
  }

  return true;
}

void NeutubeConfig::SetNeuTuServer(const QString &path)
{
  if (path.isEmpty()) {
    GetSettings().setValue("neutu_server", ":");
  } else {
    GetSettings().setValue("neutu_server", path);
  }
}

void NeutubeConfig::SetDataDir(const QString &dataDir)
{
  GetSettings().setValue("data_dir", dataDir);
}

#endif



