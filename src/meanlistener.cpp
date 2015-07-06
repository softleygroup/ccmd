#include "include/meanlistener.h"
#include "include/logger.h"

#include <list>

/**
 * @class MeanListener
 * @brief Stores mean of all ions' kinetic energy over an RF cycle. Writes each
 * cycle mean to text file.
 *
 */

MeanListener::MeanListener(const IntegrationParams& int_params,
                           const TrapParams& trap_params,
                           std::string stats_file)
    : int_params_(int_params), trap_params_(trap_params), writer_(","),
    stats_file_(stats_file), log_(Logger::getInstance()) {
        write_every_ = int_params_.steps_per_period;
        energy_row_ = 0;
        log_.debug("Started MeanListener");
}

void MeanListener::update(const int i) {
    mean_energy_.append(ions_->kinetic_energy());
    if (i%write_every_==0) {
        std::list<double> rowdata;
        rowdata.push_back(energy_row_++);
        rowdata.push_back(mean_energy_.average() * trap_params_.energy_scale);
        rowdata.push_back(mean_energy_.variance() * trap_params_.energy_scale);
        writer_.writeRow(stats_file_, rowdata);
        mean_energy_.reset();
    }
}

void MeanListener::finished() {
    log_.debug("Finished MeanListener.");
}
