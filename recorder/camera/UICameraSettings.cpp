//
// Created by farindk on 12.01.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_UICameraSettings.h" resolved

#include "UICameraSettings.h"
#include "ui_UICameraSettings.h"
#include <sstream>

#include <iostream>
#include <QSettings>
#include "CameraCapture.h"


UICameraSettings::UICameraSettings(QWidget* parent) :
        QWidget(parent), ui(new Ui::UICameraSettings)
{
  ui->setupUi(this);

  connect(ui->comboBox_camera, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int idx) {
    QVariant cam = ui->comboBox_camera->itemData(idx);
    if (!cam.isNull()) {
      mCurrentCamera = cam.toString();
    }
    else {
      mCurrentCamera.reset();
    }

    camera_selected(mCurrentCamera);
  });

  connect(ui->comboBox_resolution, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int idx) {
    QVariant res = ui->comboBox_resolution->itemData(idx);
    if (!res.isNull()) {
      mCurrentResolution = res.toSize();
    }
    else {
      mCurrentResolution.reset();
    }

    resolution_selected(mCurrentResolution);
  });

  connect(ui->comboBox_framerate, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int idx) {
    QVariant fr = ui->comboBox_framerate->itemData(idx);
    if (!fr.isNull()) {
      mCurrentFrameRate = fr.toReal();
    }
    else {
      mCurrentFrameRate.reset();
    }

    framerate_selected(mCurrentFrameRate);
  });
}


UICameraSettings::~UICameraSettings()
{
  delete ui;
}


void UICameraSettings::set_cameras(const QList<QString>& cameras)
{
//  mCameras = cameras;

  ui->comboBox_camera->blockSignals(true);
  ui->comboBox_camera->clear();

  int idx = 0;
  if (cameras.empty()) {
    ui->comboBox_camera->addItem("none");
    idx++;
  }

  int selection = 0;
  for (const auto& cam : cameras) {
    ui->comboBox_camera->addItem(cam, cam);
    if (mCurrentCamera && *mCurrentCamera == cam) {
      selection = idx;
    }
    idx++;
  }

  ui->comboBox_camera->setCurrentIndex(selection);
  ui->comboBox_camera->blockSignals(false);

  // set the default camera
  ui->comboBox_camera->currentIndexChanged(selection);
}


void UICameraSettings::set_resolutions(const QList<QSize>& resolutions)
{
//  mResolutions = resolutions;

  ui->comboBox_resolution->blockSignals(true);
  ui->comboBox_resolution->clear();
  ui->comboBox_resolution->addItem("undefined");

  int idx = 1, selection = 0;
  for (auto res : resolutions) {
    std::stringstream sstr;
    sstr << res.width() << "x" << res.height();
    ui->comboBox_resolution->addItem(QString::fromStdString(sstr.str()), res);
    if (mCurrentResolution && mCurrentResolution->width() == res.width()) {
      selection = idx;
    }
    idx++;
  }

  ui->comboBox_resolution->setCurrentIndex(selection);
  ui->comboBox_resolution->blockSignals(false);
}


void UICameraSettings::set_framerates(const QList<QCamera::FrameRateRange>& framerates)
{
//  mFrameRates = framerates;

  ui->comboBox_framerate->blockSignals(true);
  ui->comboBox_framerate->clear();
  ui->comboBox_framerate->addItem("undefined");

  int idx = 1, selection = 0;
  for (auto fr : framerates) {
    std::stringstream sstr;
    sstr << fr.minimumFrameRate;
    ui->comboBox_framerate->addItem(QString::fromStdString(sstr.str()), QVariant(fr.minimumFrameRate));
    if (mCurrentFrameRate && *mCurrentFrameRate == fr.minimumFrameRate) {
      selection = idx;
    }
    idx++;
  }

  ui->comboBox_framerate->setCurrentIndex(selection);
  ui->comboBox_framerate->blockSignals(false);
}


void UICameraSettings::set_camera_widget_to_current()
{
  int idx = ui->comboBox_camera->findData(mCurrentCamera ? QVariant(*mCurrentCamera) : QVariant());
  if (idx != -1) {
    ui->comboBox_camera->setCurrentIndex(idx);
  }
}

void UICameraSettings::set_resolution_widget_to_current()
{
  int idx = ui->comboBox_resolution->findData(mCurrentResolution ? QVariant(*mCurrentResolution) : QVariant());
  if (idx != -1) {
    ui->comboBox_resolution->setCurrentIndex(idx);
  }
}

void UICameraSettings::set_frame_rate_widget_to_current()
{
  int idx = ui->comboBox_framerate->findData(mCurrentFrameRate ? QVariant(*mCurrentFrameRate) : QVariant());
  if (idx != -1) {
    ui->comboBox_framerate->setCurrentIndex(idx);
  }
}

void UICameraSettings::set_enable(bool enable)
{
  ui->comboBox_camera->setEnabled(enable);
  ui->comboBox_resolution->setEnabled(enable);
  ui->comboBox_framerate->setEnabled(enable);
}


void UICameraSettings::connect_to_CameraCapture(CameraCapture& capture)
{
  connect(&capture, &CameraCapture::resolution_selection_changed, [&]() {
    auto resolutions = capture.get_resolutions();
    set_resolutions(resolutions);
  });

  connect(&capture, &CameraCapture::framerate_selection_changed, [&]() {
    auto framerates = capture.get_framerates();
    set_framerates(framerates);
  });

  connect(&capture, &CameraCapture::current_camera_changed, [&]() {
    mCurrentCamera = capture.get_current_camera();
    set_camera_widget_to_current();
  });

  connect(&capture, &CameraCapture::current_resolution_changed, [&]() {
    mCurrentResolution = capture.get_current_resolution();
    set_resolution_widget_to_current();
  });

  connect(&capture, &CameraCapture::current_frame_rate_changed, [&]() {
    mCurrentFrameRate = capture.get_current_frame_rate();
    set_frame_rate_widget_to_current();
  });

  connect(this, &UICameraSettings::camera_selected, [&](const std::optional<QString>& cam) {
    capture.set_user_preferred_camera(cam);
  });

  connect(this, &UICameraSettings::resolution_selected, [&](const std::optional<QSize>& res) {
    capture.set_user_preferred_resolution(res);
  });

  connect(this, &UICameraSettings::framerate_selected, [&](const std::optional<qreal>& fr) {
    capture.set_user_preferred_frame_rate(fr);
  });

  connect(&capture, &CameraCapture::status_changed, [&](QCamera::Status status) {
    set_enable(status == QCamera::Status::LoadedStatus);
  });

  mCurrentCamera = capture.get_current_camera();
  mCurrentResolution = capture.get_current_resolution();
  mCurrentFrameRate = capture.get_current_frame_rate();
}
