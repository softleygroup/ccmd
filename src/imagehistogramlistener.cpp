
#include "include/imagehistogramlistener.h"
#include "include/vector3D.h"

ImageHistogramListener::ImageHistogramListener(const IntegrationParams& int_params,
                                   const TrapParams& trap_params,
                                   const MicroscopeParams& scope_params,
                                   std::string path)
    : int_params_(int_params),
    trap_params_(trap_params),
    base_path_(path),
    scope_params_(scope_params),
    log_(Logger::getInstance()),
    images_((1.0)/(1e6 * scope_params.pixels_to_distance *
                 trap_params.length_scale)) {
        has_finished_ = false;
        log_.debug("Started ImageHistogramListener");
    }

void ImageHistogramListener::update(const int i) {
    Vector3D posn;
    Vector3D rotated_pos;
    for (auto ion : ions_->get_ions()) {
        posn = ion->get_pos();
        rotated_pos.x = (posn.x+posn.y)/sqrt(2.0);
        rotated_pos.y = (posn.x-posn.y)/sqrt(2.0);
        rotated_pos.z = posn.z;
        images_.addIon(ion->name(), rotated_pos);
    }
}

void ImageHistogramListener::finished() {
    if (has_finished_)
        return;

    log_.debug("Trying to finish ImageHistogramListener");
    images_.writeFiles(base_path_, scope_params_);
    has_finished_ = true;
    log_.debug("Finished ImageHistogramListener");
}

ImageHistogramListener::~ImageHistogramListener() {
    log_.debug("Trying to deconstruct ImageHistogramListener");
    finished();
}
