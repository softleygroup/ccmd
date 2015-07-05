/** * @file meanlistener.h
 * @brief Store mean energy over one RF cycle and append to output at the end
 * of each cycle.
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_MEANLISTENER_H_
#define INCLUDE_MEANLISTENER_H_

#include "ccmdsim.h"
#include "integratorlistener.h"
#include "ioncloud.h"
#include "stats.h"
#include "datawriter.h"
#include "logger.h"

#include <string>

class MeanListener : public IntegratorListener {
 public:
  MeanListener(const IntegrationParams& int_params,
               const TrapParams& trap_params,
               std::string stats_file);

  void update(const int i);
  void finished();

  MeanListener(const MeanListener&) = delete;
  const MeanListener& operator=(const MeanListener&) = delete;
 private:
  const IntegrationParams& int_params_;
  const TrapParams& trap_params_;
  Stats<double> mean_energy_;
  std::string stats_file_;
  int write_every_;
  int energy_row_;
  DataWriter writer_;
  Logger& log_;
};

#endif  // include_MEANLISTENER_H_
