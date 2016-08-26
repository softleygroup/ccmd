/** * @file ionstatslistener.h
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_IONSTATSLISTENER_H_
#define INCLUDE_IONSTATSLISTENER_H_

#include "ccmdsim.h"
#include "datawriter.h"
#include "integratorlistener.h"
#include "ioncloud.h"
#include "logger.h"
#include "stats.h"

#include <string>

class IonStatsListener : public IntegratorListener {
 public:
  ~IonStatsListener();
  IonStatsListener(const IntegrationParams& int_params,
               const TrapParams& trap_params,
               const CloudParams& cloud_params,
               std::string stats_file);

  void update(const int i);
  void finished();

  IonStatsListener(const IonStatsListener&) = delete;
  const IonStatsListener& operator=(const IonStatsListener&) = delete;
 private:
  std::string base_path_;
  const IntegrationParams& int_params_;
  const TrapParams& trap_params_;
  const CloudParams& cloud_params_;
  bool has_finished_;
  Logger& log_;
};

#endif  // INCLUDE_IONSTATSLISTENER_H_
