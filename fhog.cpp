
// source: http://dlib.net/fhog_object_detector_ex.cpp.html
#include "fhog.h"
#include "widget.h"

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>

#include <dlib/data_io.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/svm_threaded.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ctype.h>
#include <chrono>
#include <iostream>
#include <thread>

using namespace Annotator::Plugins;

Annotator::Plugins::FHOG::FHOG() {
  widget.setFHOG(this);
  initScanner();
}

FHOG::~FHOG() {}

QString FHOG::getName() { return "FHOG"; }

QWidget *FHOG::getWidget() { return &widget; }

bool FHOG::setFrame(shared_ptr<Frame> frame, cv::Mat image) {
  this->lastFrame = this->frame;
  this->frame = frame;
  this->frameImg = image;
  return lastFrame != frame;
}

// first call
void FHOG::setObject(shared_ptr<Object> object) {
  if (object != this->object) {
    this->object = object;
    widget.setObjectPixmap(getImgCrop(object->getFirstAnnotation(), 96));
  }
}

shared_ptr<Object> FHOG::getObject() const { return object; }

void FHOG::setLastAnnotation(shared_ptr<Annotation> /*annotation*/) {}

std::vector<shared_ptr<Commands::Command>> FHOG::getCommands() {
  std::vector<shared_ptr<Commands::Command>> commands;
  if (object == nullptr || frame == nullptr || lastFrame == nullptr ||
      lastFrame == frame)
    return commands;

  try {
    cv::Rect res = findObject();

    if (res.width > 0 && res.height > 0) {
      int x = res.x;
      int y = res.y;
      int w = res.width;
      int h = res.height;

      shared_ptr<Commands::NewAnnotation> nA =
          std::make_shared<Commands::NewAnnotation>(project->getSession(),
                                                    this->object, this->frame,
                                                    x, y, w, h, 0.9f);
      commands.push_back(nA);
    }
  } catch (std::exception &e) {
  }

  return commands;
}

void FHOG::train() {
  if (!object) return;
  widget.setProgress(10);
  getImagesTrain();
  widget.setProgress(20);
  // upsampleImages();
  widget.setProgress(30);

  dlib::structural_object_detection_trainer<image_scanner_type> trainer(
      scanner);
  trainer.set_num_threads(4);
  // The trainer is a kind of support vector machine and therefore has the usual
  // SVM
  // C parameter.  In general, a bigger C encourages it to fit the training data
  // better but might lead to overfitting.  You must find the best C value
  // empirically by checking how well the trained detector works on a test set
  // of
  // images you haven't trained on.  Don't just leave the value set at 1.  Try a
  // few
  // different C values and see what works best for your data.
  trainer.set_c(1);
  trainer.be_verbose();
  trainer.set_epsilon(0.01);

  dlib::image_window win;
  for (unsigned long i = 0; i < images_train.size(); ++i) {
    // Run the detector and get the face detections.
    std::vector<dlib::rectangle> dets = boxes_train[i];
    win.clear_overlay();
    win.set_image(images_train[i]);
    win.add_overlay(dets, dlib::rgb_pixel(255, 0, 0));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  widget.setProgress(40);
  detector = trainer.train(images_train, boxes_train);
  widget.setProgress(50);
  widget.setProgress(0);
}

void FHOG::getImagesTrain() {
  assert(object);
  this->images_train.clear();
  this->boxes_train.clear();
  this->object->getAnnotations();

  for (auto annotation : this->object->getAnnotations()) {
    std::shared_ptr<AnnotatorLib::Annotation> a = annotation.second.lock();
    if (a->isTemporary()) continue;
    cv::Mat image =
        project->getImageSet()->getImage(a->getFrame()->getFrameNumber());
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, CV_BGR2GRAY);
    dlib::array2d<unsigned char> dlibImageGray;
    dlib::assign_image(dlibImageGray, dlib::cv_image<unsigned char>(imageGray));
    this->images_train.push_back(dlibImageGray);
    std::vector<dlib::rectangle> rects;
    long x1 = std::max(0L, (long)a->getX());
    long y1 = std::max(0L, (long)a->getY());
    long x2 =
        std::min((long)image.cols - 1L, (long)a->getX() + (long)a->getWidth());
    long y2 =
        std::min((long)image.rows - 1L, (long)a->getY() + long(a->getHeight()));
    dlib::rectangle rect(x1, y1, x2, y2);
    rects.push_back(rect);
    this->boxes_train.push_back(rects);
  }
}

void FHOG::upsampleImages() {
  dlib::upsample_image_dataset<dlib::pyramid_down<2>>(images_train,
                                                      boxes_train);
}

void FHOG::loadSVM(std::string file) {
  try {
    dlib::deserialize(file) >> detector;
  } catch (...) {
  }
}

void FHOG::saveSVM(std::string file) {
  try {
    dlib::serialize(file) << detector;
  } catch (...) {
  }
}

cv::Rect FHOG::findObject() {
  cv::Mat imageGray;
  cv::cvtColor(this->frameImg, imageGray, CV_BGR2GRAY);
  dlib::array2d<unsigned char> dlibImageGray;
  dlib::assign_image(dlibImageGray, dlib::cv_image<unsigned char>(imageGray));

  std::vector<dlib::rectangle> dets = detector(dlibImageGray);
  if (dets.size() < 1) return cv::Rect();

  dlib::rectangle found = dets[0];
  return cv::Rect(found.left(), found.top(), found.width(), found.height());
}

QPixmap FHOG::getImgCrop(shared_ptr<AnnotatorLib::Annotation> annotation,
                         int size) const {
  if (annotation == nullptr) return QPixmap();

  cv::Mat cropped = getImg(annotation);

  cropped.convertTo(cropped, CV_8U);
  cv::cvtColor(cropped, cropped, CV_BGR2RGB);

  QImage img((const unsigned char *)(cropped.data), cropped.cols, cropped.rows,
             cropped.step, QImage::Format_RGB888);

  QPixmap pim = QPixmap::fromImage(img);
  pim = pim.scaledToHeight(size);
  return pim;
}

cv::Mat FHOG::getImg(shared_ptr<Annotation> annotation) const {
  cv::Mat tmp = project->getImageSet()->getImage(
      annotation->getFrame()->getFrameNumber());

  float x = std::max(annotation->getX(), 0.f);
  float y = std::max(annotation->getY(), 0.f);
  float w = std::min(annotation->getWidth(), tmp.cols - x);
  float h = std::min(annotation->getHeight(), tmp.rows - y);

  cv::Rect rect(x, y, w, h);
  cv::Mat cropped;
  try {
    tmp(rect).copyTo(cropped);
  } catch (cv::Exception &e) {
    std::cout << e.what();
  }
  return cropped;
}

void FHOG::initScanner() { scanner.set_detection_window_size(80, 80); }
