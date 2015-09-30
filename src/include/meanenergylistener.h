/** * @file meanlistener.h
 * @brief Store mean energy over one RF cycle and append to output at the end
 * of each cycle.
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_MEANENERGYLISTENER_H_
#define INCLUDE_MEANENERGYLISTENER_H_

#include "ccmdsim.h"
#include "integratorlistener.h"
#include "ioncloud.h"
#include "stats.h"
#include "datawriter.h"
#include "logger.h"

#include <string>

class MeanEnergyListener : public IntegratorListener {
 public:
  MeanEnergyListener(const IntegrationParams& int_params,
               const TrapParams& trap_params,
               std::string stats_file);

  void update(const int i);
  void finished();

  MeanEnergyListener(const MeanEnergyListener&) = delete;
  const MeanEnergyListener& operator=(const MeanEnergyListener&) = delete;
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

#endif  // include_MEANENERGYLISTENER_H_
