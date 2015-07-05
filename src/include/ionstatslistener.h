/** * @file ionstatslistener.h
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_IONSTATSLISTENER_H_
#define INCLUDE_IONSTATSLISTENER_H_

#include "ccmdsim.h"
#include "integratorlistener.h"
#include "ioncloud.h"
#include "stats.h"
#include "datawriter.h"

#include <string>

class IonStatsListener : public IntegratorListener {
 public:
  IonStatsListener(const IntegrationParams& int_params,
               const TrapParams& trap_params,
               std::string stats_file);

  void update(const int i);
  void finished();

  IonStatsListener(const IonStatsListener&) = delete;
  const IonStatsListener& operator=(const IonStatsListener&) = delete;
 private:
  std::string base_path_;
  IntegrationParams& int_params_;
  TrapParams& trap_params_;
};

#endif  // include_IONSTATSLISTENER_H_
