//
// Created by farindk on 14.01.21.
//

#ifndef CAPTURE_MAINWINDOW_H
#define CAPTURE_MAINWINDOW_H

#include <QWidget>
#include "camera/CameraCapture.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QWidget
{
Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);

  ~MainWindow() override;

private:
  Ui::MainWindow* ui;

  CameraCapture capture;

  bool mPreviewStarted = false;

  void set_preview_enabled(QCamera::Status status);

  void connect_to_Preview();
  void connect_to_Recorder();
};

#endif //CAPTURE_MAINWINDOW_H
