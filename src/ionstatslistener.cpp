#include "include/ionstatslistener.h"
#include "include/datawriter.h"
#include "include/logger.h"

/**
 * @class IonStatsListener
 * @brief Calls the updateStats method on the ion cloud to update statistics
 * stored for each ion.
 *
 * All statistics are saved to a file when finished.
 *
 */

IonStatsListener::IonStatsListener(const IntegrationParams& int_params,
                                   const TrapParams& trap_params,
                                   const CloudParams& cloud_params,
                                   std::string base_path)
    : int_params_(int_params), trap_params_(trap_params),
    base_path_(base_path), cloud_params_(cloud_params),
    log_(Logger::getInstance()) {
        has_finished_ = false;
        log_.debug("Started IonStatsListener");
}

void IonStatsListener::update(const int i) {
    ions_->updateStats();
}

IonStatsListener::~IonStatsListener() {
    log_.debug("Trying to deconstruct IonStatsListener.");
    finished();
}

void IonStatsListener::finished() {
    if (has_finished_)
        return;

    std::string statsFileEnding = "_stats.csv";
    std::string posFileEnding = "_pos.csv";

    double vel_scale = trap_params_.length_scale/trap_params_.time_scale;
    double x, y, z, rotated_x, rotated_y;
    double sqrt2 = 1.414213562;
    DataWriter writer(",");
    typedef Ion_ptr_vector::const_iterator ion_itr;
    std::list<double> rowdata;
    std::string name;

    // Write the header for each file
      std::string statsHeader="avg(r), var(r), avg(z), var(z), avg(KE), var(KE)";
      std::string posHeader="x, y, z, vx, vy, vz";
    for (auto& type : cloud_params_.ion_type_list) {
      name = base_path_ + type.name + statsFileEnding;
      writer.writeComment(name, statsHeader);
      name = base_path_ + type.name + posFileEnding;
      writer.writeComment(name, posHeader);
    }

    for (auto ion : ions_->get_ions()) {
        // Write the final position and velocity for each ion.
        name = base_path_ + ion->name() + posFileEnding;
        rowdata.clear();
        // Scale reduced units to real-world units and rotate to align to
        // axes between rods (calculation has axes crossing rods.)
        x = (ion->get_pos())[0] * trap_params_.length_scale;
        y = (ion->get_pos())[1] * trap_params_.length_scale;
        z = (ion->get_pos())[2] * trap_params_.length_scale;
        rotated_x = (x+y)/sqrt2;
        rotated_y = (x-y)/sqrt2;
        rowdata.push_back(rotated_x);
        rowdata.push_back(rotated_y);
        rowdata.push_back(z);

        x = (ion->get_vel())[0] * vel_scale;
        y = (ion->get_vel())[1] * vel_scale;
        z = (ion->get_vel())[2] * vel_scale;
        rotated_x = (x+y)/sqrt2;
        rotated_y = (x-y)/sqrt2;
        rowdata.push_back(rotated_x);
        rowdata.push_back(rotated_y);
        rowdata.push_back(z);

        writer.writeRow(name, rowdata);

        // Write the average data for each ion.
        name = base_path_ + ion->name() + statsFileEnding;
        rowdata.clear();

        Stats<double> vel = (ion->get_velStats());
        Stats<Vector3D> pos = (ion->get_posStats());
        double mon2 = (ion->get_mass())/2;
        double avg_energy = (vel.average() * vel.average()) * (mon2 * trap_params_.energy_scale);
        double var_energy = (vel.variance()/vel.average()) * (avg_energy * 1.41);
        Vector3D avg_pos = pos.average() * trap_params_.length_scale;
        Vector3D var_pos = pos.variance() * trap_params_.length_scale;

        rowdata.push_back(avg_pos[0]);
        rowdata.push_back(var_pos[0]);
        rowdata.push_back(avg_pos[1]);
        rowdata.push_back(var_pos[1]);
        rowdata.push_back(avg_energy);
        rowdata.push_back(var_energy);

        writer.writeRow(name, rowdata);
    }
    has_finished_ = true;
    log_.debug("Finished IonStatsListener.");
}
