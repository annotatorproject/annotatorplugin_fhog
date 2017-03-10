#ifndef FHOG_H
#define FHOG_H

#include <annotator/plugins/plugin.h>
#include "widget.h"

#include <dlib/data_io.h>
#include <dlib/image_processing.h>
#include <dlib/svm_threaded.h>
#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtGui/QIcon>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <vector>

using std::shared_ptr;
using namespace AnnotatorLib;

namespace AnnotatorLib {
class Session;
}

namespace Annotator {
namespace Plugins {

class FHOG : public Plugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "annotator.fhog" FILE "fhog.json")
  Q_INTERFACES(Annotator::Plugin)

 public:
  FHOG();
  ~FHOG();
  QString getName() override;
  QWidget *getWidget() override;
  bool setFrame(shared_ptr<Frame> frame, cv::Mat image) override;
  void setObject(shared_ptr<Object> object) override;
  shared_ptr<Object> getObject() const override;
  void setLastAnnotation(shared_ptr<Annotation>) override;
  std::vector<shared_ptr<Commands::Command>> getCommands() override;

  void train();
  void getImagesTrain();
  void upsampleImages();

 protected:
  cv::Rect findObject();
  QPixmap getImgCrop(shared_ptr<AnnotatorLib::Annotation> annotation,
                     int size) const;
  cv::Mat getImg(shared_ptr<AnnotatorLib::Annotation> annotation) const;
  void initScanner();

  cv::Mat frameImg;
  shared_ptr<Annotation> lastAnnotation = nullptr;
  shared_ptr<Object> object = nullptr;

  Widget widget;

  shared_ptr<Frame> frame = nullptr;
  shared_ptr<Frame> lastFrame = nullptr;

  dlib::array<dlib::array2d<unsigned char>> images_train;
  std::vector<std::vector<dlib::rectangle>> boxes_train;

  typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6>> image_scanner_type;
  image_scanner_type scanner;
  dlib::object_detector<image_scanner_type> detector;
};
}
}

#endif  // FHOG_H
