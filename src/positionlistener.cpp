#include "include/positionlistener.h"
#include "include/datawriter.h"
#include "include/logger.h"

#include <string>

PositionListener::PositionListener(const IntegrationParams& int_params,
                                   const TrapParams& trap_params,
                                   std::string path)
    : int_params_(int_params), trap_params_(trap_params), path_(path), 
    log_(Logger::getInstance()) {
        write_every_ = int_params_.steps_per_period;
        frameNumber_ = 0;
        log_.debug("Started PositionListener.");
    }

void PositionListener::update(const int i) {
    if (i%write_every_==0) {
        char buffer[50];
        std::sprintf(buffer, "%.4i", frameNumber_++);
        std::string framepath = path_ + buffer;
        save(framepath);
    }
}

void PositionListener::finished() {
    log_.debug("Finished PositionListener.");
}

void PositionListener::save(const std::string basePath) const {
    std::string fileEnding = ".csv";

    double vel_scale = trap_params_.length_scale/trap_params_.time_scale;
    double x, y, z, rotated_x, rotated_y;
    double sqrt2 = 1.414213562;
    DataWriter writer(",");
    typedef Ion_ptr_vector::const_iterator ion_itr;
    std::list<double> rowdata;
    std::string name;

    for (auto ion : ions_->get_ions()) {
        // Write the final position and velocity for each ion.
        name = basePath + ion->name() + fileEnding;
        rowdata.clear();
        // Scale reduced units to real=world units and rotate to align to
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
    }
}
