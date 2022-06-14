//
// Created by farindk on 14.01.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Settings.h"

#include <iostream>
#include <QErrorMessage>


MainWindow::MainWindow(QWidget* parent) :
        QWidget(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  Settings::load_camera_settings(capture, 0);

  // connect the change-signals after loading so that the values will not be written back immediately
  // any maybe even overwrite the Settings partially with default values

  QObject::connect(&capture, &CameraCapture::any_settings_changed, [&]() {
    Settings::save_camera_settings(capture, 0);
  });

  connect_to_Preview();
  connect_to_Recorder();
}


MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::connect_to_Preview()
{
  ui->cameraSettings->connect_to_CameraCapture(capture);
  ui->cameraSettings->set_cameras(capture.get_cameras());

  connect(ui->button_togglePreview, &QPushButton::clicked, [&](bool) {
    if (mPreviewStarted) {
      capture.stop_capture();
      mPreviewStarted = false;
      ui->button_togglePreview->setText("Start preview");
    }
    else {
      capture.start_capture();
      mPreviewStarted = true;
      ui->button_togglePreview->setText("Stop preview");
    }
  });

  connect(&capture, &CameraCapture::status_changed, [&](QCamera::Status status) {
    set_preview_enabled(status);
  });

  connect(&capture, &CameraCapture::frame_captured, [&](const QVideoFrame& frame) {
    if (mPreviewStarted) {
      ui->videoPreview->push_image(frame);
    }
  });

  set_preview_enabled(QCamera::Status::UnavailableStatus);
}


void MainWindow::set_preview_enabled(QCamera::Status status)
{
  bool enabled = (status == QCamera::Status::LoadedStatus ||
                  status == QCamera::Status::ActiveStatus);

  if (capture.get_cameras().empty()) {
    enabled = false;
  }

  ui->button_togglePreview->setEnabled(enabled);
}

void MainWindow::connect_to_Recorder()
{
}
