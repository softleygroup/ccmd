#ifndef INCLUDE_IMAGEHISTOGRAMLISTENER_H_
#define INCLUDE_IMAGEHISTOGRAMLISTENER_H_

#include "ccmdsim.h"
#include "integratorlistener.h"
#include "imagecollection.h"
#include "logger.h"

class ImageHistogramListener : public IntegratorListener {
 public:
  ImageHistogramListener(const IntegrationParams& int_params,
                         const TrapParams& trap_params,
                         const MicroscopeParams& scope_params,
                         std::string path);
  ~ImageHistogramListener();

  void update(const int i);
  void finished();

  ImageHistogramListener(const ImageHistogramListener&) = delete;
  const ImageHistogramListener& operator=(const ImageHistogramListener&) = delete;
 private:
  std::string base_path_;
  const IntegrationParams& int_params_;
  const TrapParams& trap_params_;
  const MicroscopeParams& scope_params_;
  bool has_finished_;
  Logger& log_;
  ImageCollection images_;
};

#endif  // INCLUDE_IMAGEHISTOGRAMLISTENER_H_
