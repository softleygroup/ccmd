/** 
 * @file positionlistener.h
 * @brief Class that writes all ion positions to a file on each timestep
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_POSITIONLISTENER_H_
#define INCLUDE_POSITIONLISTENER_H_

#include "ccmdsim.h"
#include "integratorlistener.h"
#include "ioncloud.h"
#include "logger.h"

#include <string>

class PositionListener : public IntegratorListener {
 public:
  PositionListener(const IntegrationParams& int_params,
                   const TrapParams& trap_params,
                   std::string path);

  void update(const int i);
  void finished();
  void save(const std::string basePath) const;

  PositionListener(const PositionListener&) = delete;
  const PositionListener& operator=(const PositionListener&) = delete;
 private:
  const IntegrationParams& int_params_;
  const TrapParams& trap_params_;
  int write_every_;
  int frameNumber_;
  std::string path_;
  Logger& log_;
};


#endif  // INCLUDE_POSITIONLISTENER_H_

