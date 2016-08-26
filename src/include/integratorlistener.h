/** 
 * @file integratorlistener.h
 * @brief Base class for notification of a complete integration step.
 *
 * @author Chris Rennick
 * @copyright Copyright 2015 University of Oxford.
 */

#ifndef INCLUDE_INTEGRATORLISTENER_H_
#define INCLUDE_INTEGRATORLISTENER_H_

#include "ioncloud.h"

#include <memory>

class IntegratorListener {
 public:

  void setCloud (IonCloud_ptr i);

  /// Called on each iteration of the integrator, with the current iteration
  //number.
  virtual void update(const int i) = 0;
  /// Called when the intergration is complete.
  virtual void finished() = 0;
 protected:
  IonCloud_ptr ions_;
};

typedef std::shared_ptr<IntegratorListener> IntegratorListener_ptr;

#endif  // INCLUDE_INTEGRATORLISTENER_H_
