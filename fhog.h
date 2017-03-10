#ifndef FHOG_H
#define FHOG_H

#include <annotator/plugins/plugin.h>
#include "widget.h"

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtGui/QIcon>
#include <memory>
#include <opencv2/core/mat.hpp>

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

 protected:
  cv::Rect findObject();
  cv::Mat frameImg;
  shared_ptr<Annotation> lastAnnotation = nullptr;
  shared_ptr<Object> object = nullptr;

  Widget widget;

  shared_ptr<Frame> frame = nullptr;
  shared_ptr<Frame> lastFrame = nullptr;

};
}
}

#endif  // FHOG_H
