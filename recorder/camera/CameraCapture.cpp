
#include "CameraCapture.h"
#include <QCameraInfo>
#include <QAbstractVideoSurface>
#include <QCameraViewfinder>


#include <iostream>
#include <QSettings>
#include <cassert>



class MyVideoSurface : public QAbstractVideoSurface
{
public:
  explicit MyVideoSurface(CameraCapture* capture)
  {
    mCameraCapture = capture;
  }

  void set_ignore_counter(int n) {
    mNFramesToIgnore = n;
  }

  [[nodiscard]] QList<QVideoFrame::PixelFormat>
  supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override
  {
    QList<QVideoFrame::PixelFormat> formats;

    if (handleType == QAbstractVideoBuffer::NoHandle)
    {
      formats.append(QVideoFrame::Format_YUYV);
    }

    return formats;
  }

  bool present(const QVideoFrame& frame) override
  {
    if (mNFramesToIgnore>0) {
      mNFramesToIgnore--;
      return true;
    }

    mCameraCapture->frame_captured(frame);
    return true;
  }

private:
  CameraCapture* mCameraCapture;
  int mNFramesToIgnore = 0;
};


CameraCapture::CameraCapture()
{
  mCameras = QCameraInfo::availableCameras();
  mVideoSurface = new MyVideoSurface(this);
}


CameraCapture::~CameraCapture() noexcept
{
  delete mVideoSurface;
  delete mCamera;
}


void CameraCapture::set_user_preferred_settings(const CameraSettings& settings)
{
  mUserPreferredSettings = settings;
  set_user_preferred_camera(settings.cameraName);
}


void CameraCapture::set_user_preferred_camera(const std::optional<QString>& camera)
{
  if (camera == mActiveSettings.cameraName) {
    return;
  }

  delete mCamera;
  mCamera = nullptr;

  mUserPreferredSettings.cameraName = camera;
  mActiveSettings.cameraName.reset();

  for (const QCameraInfo& cameraInfo : mCameras) {
    if (camera && cameraInfo.description() == *camera) {
      mCamera = new QCamera(cameraInfo);
      //mCamera->setViewfinder(viewfinder);
      mCamera->setViewfinder(mVideoSurface);
    }
  }

  emit current_camera_changed();
  emit any_settings_changed();

  if (mCamera) {
    mActiveSettings.cameraName = *camera;

    QObject::connect(mCamera, &QCamera::statusChanged, [&](QCamera::Status status) {
#if DEBUG
      std::cout << "status changed: " << status << "\n";
#endif

      if (status == QCamera::LoadedStatus) {
        mActiveSettings.resolution.reset();
        mActiveSettings.frameRate.reset();

        reload_possible_resolutions();
        select_current_resolution_from_user_preferred(true);

        reload_possible_framerates();
        select_current_framerate_from_user_preferred(true);

        if (mActiveSettings.frameRate) {
          reload_possible_resolutions();
        }
      }

      emit status_changed(status);
    });

    mCamera->load();
  }
}


QCamera::Status CameraCapture::get_status() const
{
  if (!mCamera) {
    return QCamera::Status::UnloadedStatus;
  }

  return mCamera->status();
}


bool CameraCapture::camera_loaded() const
{
  return mCamera && mCamera->status() == QCamera::LoadedStatus;
}


void CameraCapture::set_user_preferred_resolution(const std::optional<QSize>& s)
{
  mUserPreferredSettings.resolution = s;
  select_current_resolution_from_user_preferred(false);

  if (camera_loaded()) {
    reload_possible_framerates();

    // modify active frame-rate if needed

    select_current_framerate_from_user_preferred(true);
  }
}


void CameraCapture::set_user_preferred_frame_rate(const std::optional<qreal>& fr)
{
  mUserPreferredSettings.frameRate = fr;
  select_current_framerate_from_user_preferred(false);

  if (camera_loaded()) {
    reload_possible_resolutions();

    // modify active resolution if needed

    select_current_resolution_from_user_preferred(true);
  }
}


QList<QString> CameraCapture::get_cameras() const
{
  QList<QString> list;
  for (const auto& camera : mCameras) {
    list.append(camera.description());
  }

  return list;
}


void CameraCapture::reload_possible_resolutions()
{
  QCameraViewfinderSettings res_settings;

  if (mActiveSettings.frameRate) {
    res_settings.setMinimumFrameRate(*mActiveSettings.frameRate);
    res_settings.setMaximumFrameRate(*mActiveSettings.frameRate);
#if DEBUG
    std::cout << "for frame-rate: " << *mActiveSettings.frameRate << "\n";
#endif
  }

  auto resolutions = mCamera->supportedViewfinderResolutions(res_settings);
#if DEBUG
  for (auto s : resolutions) {
    std::cout << s.width() << " x " << s.height() << "\n";
  }
#endif

  if (mPossibleResolutions != resolutions) {
    mPossibleResolutions = resolutions;
    resolution_selection_changed();
  }
}


void CameraCapture::reload_possible_framerates()
{
  QCameraViewfinderSettings fr_settings;

  if (mActiveSettings.resolution) {
    fr_settings.setResolution(*mActiveSettings.resolution);
#if DEBUG
    std::cout << "for resolution: " << mActiveSettings.resolution->width() << " x " << mActiveSettings.resolution->height() << "\n";
#endif
  }

  auto frame_rates = mCamera->supportedViewfinderFrameRateRanges(fr_settings);
#if DEBUG
  for (auto fr : frame_rates) {
    std::cout << fr.minimumFrameRate << " - " << fr.maximumFrameRate << "\n";
  }
#endif

  if (mPossibleFrameRates != frame_rates) {
    mPossibleFrameRates = frame_rates;
    framerate_selection_changed();
  }
}


bool CameraCapture::supports(const std::optional<QSize>& resolution, const std::optional<qreal>& framerate) const
{
  QCameraViewfinderSettings settings;

  if (resolution) {
    settings.setResolution(*resolution);
  }

  if (framerate) {
    settings.setMinimumFrameRate(*framerate);
    settings.setMaximumFrameRate(*framerate);
  }

  return !mCamera->supportedViewfinderSettings(settings).empty();
}


void CameraCapture::select_current_resolution_from_user_preferred(bool select_unique_possibility)
{
  if (!mUserPreferredSettings.resolution || mPossibleResolutions.empty()) {
    if (select_unique_possibility && mPossibleResolutions.size() == 1) {
      mActiveSettings.resolution = mPossibleResolutions[0];
      emit current_resolution_changed();
      emit any_settings_changed();
    }
    else if (mActiveSettings.resolution) {
      mActiveSettings.resolution.reset();
      emit current_resolution_changed();
      emit any_settings_changed();
    }
  }
  else {
    QSize bestMatch;
    int minError = std::numeric_limits<int>::max();

    // find best matching resolution in list

    for (const auto& r : mPossibleResolutions) {
      int err = (std::abs(r.width() - mUserPreferredSettings.resolution->width()) +
                 std::abs(r.height() - mUserPreferredSettings.resolution->height()));

      if (err < minError) {
        minError = err;
        bestMatch = r;
      }
    }

    if (!mActiveSettings.resolution || bestMatch != *mActiveSettings.resolution) {
      mActiveSettings.resolution = bestMatch;
      emit current_resolution_changed();
      emit any_settings_changed();
    }
  }
}


void CameraCapture::select_current_framerate_from_user_preferred(bool select_unique_possibility)
{
  if (!mUserPreferredSettings.frameRate || mPossibleFrameRates.empty()) {
    if (select_unique_possibility && mPossibleFrameRates.size() == 1) {
      mActiveSettings.frameRate = mPossibleFrameRates[0].minimumFrameRate;
      emit current_frame_rate_changed();
      emit any_settings_changed();
    }
    else if (mActiveSettings.frameRate) {
      mActiveSettings.frameRate.reset();
      emit current_frame_rate_changed();
      emit any_settings_changed();
    }
  }
  else {
    qreal bestMatch;
    qreal minError = std::numeric_limits<qreal>::max();

    // find best matching frame-rate in list

    for (const auto& fr : mPossibleFrameRates) {
      qreal err = (std::abs(fr.minimumFrameRate - *mUserPreferredSettings.frameRate));

      if (err < minError) {
        minError = err;
        bestMatch = fr.minimumFrameRate;
      }
    }

    if (!mActiveSettings.frameRate || bestMatch != *mActiveSettings.frameRate) {
      mActiveSettings.frameRate = bestMatch;
      emit current_frame_rate_changed();
      emit any_settings_changed();
    }
  }
}


void CameraCapture::start_capture()
{
  mNumVideoCaptures++;

  if (mNumVideoCaptures != 1) {
    // when we are not the first capture, we don't have to do anything here
    return;
  }

  // get compatible video formats

  QCameraViewfinderSettings settings;

  if (mActiveSettings.frameRate) {
    settings.setMinimumFrameRate(*mActiveSettings.frameRate);
    settings.setMaximumFrameRate(*mActiveSettings.frameRate);
  }

  if (mActiveSettings.resolution) {
    settings.setResolution(*mActiveSettings.resolution);
  }

#if 0
  auto formats = mCamera->supportedViewfinderPixelFormats(settings);
  for (auto f : formats) {
    std::cout << "FORMAT " << f << "\n";
  }


  auto sets = mCamera->supportedViewfinderSettings();
  for (auto& s : sets) {
    std::cout << "format " << s.pixelFormat() << " : " << s.resolution().width() << "x" << s.resolution().height()
              << " " << s.minimumFrameRate() << "\n";
  }

  //settings = sets[0];
#endif

  mVideoSurface->set_ignore_counter(3); // Note: this is fixing the bug that the first frames in the video stream have a wrong PTS

  mCamera->setViewfinderSettings(settings);
  mCamera->start();
}


void CameraCapture::stop_capture()
{
  mNumVideoCaptures--;
  assert(mNumVideoCaptures>=0);

  if (mNumVideoCaptures == 0) {
    mCamera->stop();
  }
}
