#include "shapepaperdialog.h"
#include <QProcess>
#include <fstream>
#include "ui_shapepaperdialog.h"
#include "zdialogfactory.h"
#include "neutubeconfig.h"
#include "zparameterarray.h"
#include "zfilelist.h"
#include "zstack.hxx"
#include "zdoublevector.h"
#include "tz_math.h"
#include "zstring.h"
#include "flyem/zflyemdataframe.h"
#include "zframefactory.h"
#include "mainwindow.h"
#include "zsvggenerator.h"
#include "zdendrogram.h"
#include "zswcglobalfeatureanalyzer.h"
#include "zmatrix.h"

ShapePaperDialog::ShapePaperDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ShapePaperDialog), m_frame(NULL)
{
  ui->setupUi(this);
  m_objectStackDir = new ZStringParameter(
        "Object stack directory",
        (GET_DATA_DIR + "/flyem/TEM/skeletonization/session3").c_str(), this);
  m_sparseObjectDir = new ZStringParameter(
        "Sparse object directory",
        (GET_DATA_DIR + "/flyem/TEM/skeletonization/session3").c_str(), this);
  m_bundleDir = new ZStringParameter(
        "Bundle directory",
        (GET_DATA_DIR + "/flyem/TEM/data_release/bundle1").c_str(), this);
  m_resultDir = new ZStringParameter(
        "Result directory",
        (GET_DATA_DIR + "/flyem/shape_paper").c_str(), this);

  ui->sparseObjectPushButton->hide();
}

ShapePaperDialog::~ShapePaperDialog()
{
  delete ui;
}

void ShapePaperDialog::on_configPushButton_clicked()
{
  ZParameterArray parameterArray;
  parameterArray.append(m_objectStackDir);
  parameterArray.append(m_sparseObjectDir);
  parameterArray.append(m_bundleDir);
  parameterArray.append(m_resultDir);
  QDialog *dlg = ZDialogFactory::makeParameterDialog(parameterArray, this);
  dlg->exec();

  delete dlg;
}

QString ShapePaperDialog::getSparseObjectDir() const
{
  return m_sparseObjectDir->get();
}

QString ShapePaperDialog::getObjectStackDir() const
{
  return m_objectStackDir->get();
}

QString ShapePaperDialog::getDataBundlePath() const
{
  return m_bundleDir->get() + "/data_bundle.json";
}

MainWindow* ShapePaperDialog::getMainWindow()
{
  return dynamic_cast<MainWindow*>(parent());
}

void ShapePaperDialog::on_sparseObjectPushButton_clicked()
{
  QString objDir = getSparseObjectDir();
  QString stackDir = getObjectStackDir();
  ZFileList fileList;
  fileList.load(stackDir.toStdString(), "tif");
  for (int i = 0; i < fileList.size(); ++i) {
    std::string filePath = fileList.getFilePath(i);
    std::string objFilePath =
        objDir.toStdString() + "/" +
        ZString::removeFileExt(ZString::getBaseName(filePath)) + ".sobj";

    if (!fexist(objFilePath.c_str())) {
      ZStack stack;
      stack.load(filePath);
      std::string offsetFilePath = filePath + ".offset.txt";
      if (fexist(offsetFilePath.c_str())) {
        ZDoubleVector vec;
        vec.importTextFile(offsetFilePath);
        if (vec.size() == 3) {
          stack.setOffset(iround(vec[0]), iround(vec[1]), iround(vec[2]));
        }

        ZObject3dScan obj;
        obj.loadStack(stack);


        obj.save(objFilePath);
      }
    }
  }
}

void ShapePaperDialog::detachFrame()
{
  m_frame = NULL;
}

void ShapePaperDialog::on_dataBundlePushButton_clicked()
{
  if (m_frame == NULL) {
    m_frame = ZFrameFactory::MakeFlyEmDataFrame(getDataBundlePath());
    if (m_frame != NULL) {
      connect(m_frame, SIGNAL(destroyed()), this, SLOT(detachFrame()));
      getMainWindow()->addFlyEmDataFrame(m_frame);
    }
  }

  if (m_frame != NULL) {
    m_frame->show();
  }
}

QString ShapePaperDialog::getSimmatPath() const
{
  return m_resultDir->get() + "/simmat.txt";
}

QString ShapePaperDialog::getFeaturePath() const
{
  return m_resultDir->get() + "/feature.csv";
}

QString ShapePaperDialog::getDendrogramPath() const
{
  return m_resultDir->get() + "/dendrogram.svg";
}

void ShapePaperDialog::on_simmatPushButton_clicked()
{
  if (m_frame != NULL) {
    m_frame->exportSimilarityMatrix(getSimmatPath());
  }
}

void ShapePaperDialog::dump(const QString &str, bool appending)
{
  if (appending) {
    ui->messageTextEdit->append(str);
  } else {
    ui->messageTextEdit->setText(str);
  }
}

void ShapePaperDialog::on_dendrogramPushButton_clicked()
{
  if (m_frame != NULL) {
    QString simmatFile = getSimmatPath();
    if (!fexist(simmatFile.toStdString().c_str())) {
      QString targetFilePath = (GET_DATA_DIR + "/tmp/simmat.txt").c_str();
      QFile targetFile(targetFilePath);
      if (targetFile.exists()) {
        targetFile.remove();
      }

      if (QFile::copy(simmatFile, targetFilePath)) {
        QString output = getDendrogramPath();
        QProcess::execute("/Applications/MATLAB.app/bin/matlab < "
                          "/Users/zhaot/Work/SLAT/matlab/SLAT/run/flyem/tz_run_flyem_dendrogram_command.m "
                          "-nodesktop -nosplash");

        //Create name file
        std::string neuronNameFilePath = GET_DATA_DIR + "/tmp/neuron_name.txt";
        ZFlyEmDataBundle *bundle = m_frame->getDataBundle();

        std::vector<ZFlyEmNeuron> neuronArray = bundle->getNeuronArray();

        std::ofstream stream(neuronNameFilePath.c_str());
        for (std::vector<ZFlyEmNeuron>::const_iterator iter = neuronArray.begin();
             iter != neuronArray.end(); ++iter) {
          const ZFlyEmNeuron &neuron = *iter;
          stream << neuron.getName() << std::endl;
        }
        stream.close();

        ZDendrogram dendrogram;

        ZMatrix Z;
        Z.importTextFile(GET_DATA_DIR + "/tmp/Z.txt");
        for (int i = 0; i < Z.getRowNumber(); ++i) {
          dendrogram.addLink(Z.at(i, 0), Z.at(i, 1), Z.at(i, 2) - 0.5);
        }
        dendrogram.loadLeafName(neuronNameFilePath);
        std::string svgString = dendrogram.toSvgString(15.0);

        ZSvgGenerator svgGenerator(0, 0, 1000, 6000);
        svgGenerator.write(output.toStdString().c_str(), svgString);

        dump(output + " saved.");
      }
    }
  }
}

void ShapePaperDialog::on_predictPushButton_clicked()
{
  ZMatrix simmat;
  simmat.importTextFile(getSimmatPath().toStdString());
  int nrows = simmat.getRowNumber();
  int ncols = simmat.getColumnNumber();

  //Normalize
  /*
  for (int i = 1; i < nrows; ++i) {
    int neuronIndex = i - 1;
    for (int j = 0; j < ncols; ++j) {
      if (neuronIndex != j) {
        simmat.set(neuronIndex, j, simmat.at(neuronIndex, j) /
                   std::max(simmat.at(neuronIndex, neuronIndex), simmat.at(j, j)));
      }
    }
  }
  */

  //Find maximum score
  int correctNumber = 0;
  for (int i = 1; i < nrows; ++i) {
    int neuronIndex = i - 1;
    double maxScore = 0.0;
    int matchedIndex = neuronIndex;
    for (int j = 0; j < ncols; ++j) {
      if (neuronIndex != j) {
        if (maxScore < simmat.at(i, j)) {
          maxScore = simmat.at(i, j);
          matchedIndex = j;
        }
      }
    }

    const ZFlyEmNeuron *neuron1 = m_frame->getNeuronFromIndex(neuronIndex);
    const ZFlyEmNeuron *neuron2 = m_frame->getNeuronFromIndex(matchedIndex);
    if (neuron1->getClass() == neuron2->getClass()) {
      ++correctNumber;
    } else {
      /*
      dump(QString("Wrong predction: %1 -> %2; (s = %3)").
           arg(neuron1->getName().c_str()).
           arg(neuron2->getClass().c_str()).arg(maxScore), true);
           */
    }
  }

  dump(QString("Accurate count: %1").arg(correctNumber), true);

  predictFromOrtAdjustment();
}

void ShapePaperDialog::predictFromOrtAdjustment()
{
  ZMatrix featmat;
  featmat.importTextFile(getFeaturePath().toStdString() + ".txt");

  ZDoubleVector ratioArray(featmat.getRowNumber());
  for (size_t i = 0; i < ratioArray.size(); ++i) {
    ratioArray[i] = log(featmat.at(i, 9) / featmat.at(i, 10));
//    ratioArray[i] = log(featmat.at(i, 7));
  }

//  const double mu1 = -1.3302;
//  const double mu2 = 0.3322;
//  const double var1 = 0.2944;
//  const double var2 = 1.0380;

//  const double mu1 = -1.4074;
//  const double mu2 = 0.11518;
//  const double var1 = 0.11213;
//  const double var2 = 1.0489;

//  const double mu1 = -1.4144;
//  const double mu2 = 0.13755;
//  const double var1 = 0.11133;
//  const double var2 = 1.0146;

//  const double mu1 = -1.4083;
//  const double mu2 = 0.11153;
//  const double var1 = 0.11168;
//  const double var2 = 1.0514;

//  const double mu1 = -1.4009;
//  const double mu2 = 0.14225;
//  const double var1 = 0.11591;
//  const double var2 = 1.031;

  const double mu1 = -1.3287;
  const double mu2 = 0.23399;
  const double var1 = 0.2902;
  const double var2 = 1.1109;

//    const double mu1 = -1.4041;
//    const double mu2 = 0.1346;
//    const double var1 = 0.1140;
//    const double var2 = 1.0343;
//    const double mu1 = -1.111;
//    const double mu2 = 0.077144;
//    const double var1 = 0.057516;
//    const double var2 = 0.55438;

//  const double mu1 = -1.4061;
//  const double mu2 = 0.0867;
//  const double var1 = 0.1135;
//  const double var2 = 1.0798;


//  const double mu1 = -1.3813;
//  const double mu2 = 0.2738;
//  const double var1 = 0.1277;
//  const double var2 = 0.9292;

//  const double mu1 = -0.9830;
//  const double mu2 = 0.2135;
//  const double var2 = 1.0312;
//  const double var1 = 0.2932;

//  const double mu1 = -1.1103;
//  const double mu2 = 0.2637;
//  const double var1 = 0.1003;
//  const double var2 = 0.7663;


  ZMatrix simmat;
  simmat.importTextFile(getSimmatPath().toStdString());
  int nrows = simmat.getRowNumber();
  int ncols = simmat.getColumnNumber();

  //Find maximum score
  int correctNumber = 0;
  for (int i = 1; i < nrows; ++i) {
    int neuronIndex = i - 1;
    double maxScore = 0.0;
    int matchedIndex = neuronIndex;
    for (int j = 0; j < ncols; ++j) {
      if (neuronIndex != j) {
        double k = computeRatioDiff(ratioArray[neuronIndex],
                                            ratioArray[j], mu1, var1,
                                            mu2, var2);
        double score = simmat.at(i, j) * sqrt(k); /// (1 + exp((0.5 - k) * 5));
        if (maxScore < score) {
          maxScore = score;
          matchedIndex = j;
        }
      }
    }

    const ZFlyEmNeuron *neuron1 = m_frame->getNeuronFromIndex(neuronIndex);
    const ZFlyEmNeuron *neuron2 = m_frame->getNeuronFromIndex(matchedIndex);
    if (neuron1->getClass() == neuron2->getClass()) {
      ++correctNumber;
    } else {
      /*
      dump(QString("Wrong predction: %1 -> %2; (s = %3)").
           arg(neuron1->getName().c_str()).
           arg(neuron2->getClass().c_str()).arg(maxScore), true);
           */
    }
  }

  dump(QString("Accurate count (ratio adjusted): %1").arg(correctNumber), true);
}

void ShapePaperDialog::on_featurePushButton_clicked()
{
  //Compute features
  if (m_frame != NULL) {
    ZSwcGlobalFeatureAnalyzer::EFeatureSet setName =
        ZSwcGlobalFeatureAnalyzer::NGF3;
    ZFlyEmDataBundle *bundle = m_frame->getDataBundle();
    ZFlyEmNeuronArray &neuronArray = bundle->getNeuronArray();
    ZMatrix featmat(neuronArray.size(),
                    ZSwcGlobalFeatureAnalyzer::getFeatureNumber(setName));
    int row = 0;
    std::vector<std::string> neuronName;
    for (ZFlyEmNeuronArray::iterator iter = neuronArray.begin();
         iter != neuronArray.end(); ++iter, ++row) {
      ZFlyEmNeuron &neuron = *iter;
      std::vector<double> featureSet =
          ZSwcGlobalFeatureAnalyzer::computeFeatureSet(
            *(neuron.getModel()), setName);
      neuronName.push_back(neuron.getName());
      featmat.setRowValue(row, featureSet);
    }

    featmat.exportCsv(getFeaturePath().toStdString() + ".txt");
    featmat.exportCsv(getFeaturePath().toStdString(), neuronName,
                      ZSwcGlobalFeatureAnalyzer::getFeatureNameArray(setName));
  }
}

double ShapePaperDialog::computeRatioDiff(
    double x, double y, double mu1, double var1, double mu2, double var2)
{
  double zx1 = (x - mu1) * (x - mu1) / var1 / 2.0;
  double zx2 = (x - mu2) * (x - mu2) / var2 / 2.0;
  double zy1 = (y - mu1) * (y - mu1) / var1 / 2.0;
  double zy2 = (y - mu2) * (y - mu2) / var2 / 2.0;
  double rx = exp(zx2 - zx1) * sqrt(var2 / var1);
  double ry = exp(zy2 - zy1) * sqrt(var2 / var1);

  double k = 1.0;
  if (rx > 0.0 || ry > 0.0) {
    double t = (rx * ry + 1) / (rx + ry);
    k = t / (1 + t);
  }

  return k;

      /*
  return std::fabs((x - y) * ((x + y - mu1 - mu1) /var1 -
                   (x + y - mu2 - mu2)/var2));
                       */
}
