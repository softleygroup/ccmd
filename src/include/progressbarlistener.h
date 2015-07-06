/** 
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_PROGRESSLISTENER_H_
#define INCLUDE_PROGRESSLISTENER_H_

#include "integratorlistener.h"

class ProgressBarListener : public IntegratorListener {
 public:
  ProgressBarListener(const int tick_max);

  void update(const int i);
  void finished();
 private:
  int tick_max_;
};

#endif  // INCLUDE_PROGRESSLISTENER_H_
