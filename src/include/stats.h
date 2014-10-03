/**
 * @file stats.h
 * @brief Definition and declaration of a statistics class.
 *
 * @author Chris Rennick
 * @copyright Copyright 2014 University of Oxford.
 */

#ifndef INCLUDE_STATS_H_
#define INCLUDE_STATS_H_

/**
 *  @class Stats
 *  @brief Accumulate average and variance for double values.
 *
 *  This class uses the Knuth algorithm to calculate the average and variance
 *  cumulatively, avoiding the need to know the total number of items.
 */

template <class T>
class Stats {
 public:
    Stats() {
        count_ = 0;
    }

    /**
     *  @brief Append a new value and update the statistics.
     *  @param newVal   Value to append.
     */
    void append(const T& newVal) {
        count_++;

        T m_prev, s_prev;
        if (count_ > 1) {
            m_prev = mean_;
            s_prev = n_variance_;
            mean_ = m_prev + (newVal-m_prev)/count_;
            n_variance_ = s_prev + (newVal-m_prev)*(newVal-mean_);
            } else {
                mean_ = newVal;
                n_variance_ = T();
        }
    }

    /** @brief Reset the statistics.
     */
    void reset() {
        count_ = 0;
        mean_ = T();
        n_variance_ = T();
    }

    /**
     *  @brief Calculate and return the variance.
     *  @return The variance.
     */
    T variance() const {
        return n_variance_/(count_-1);
    }

    /**
     *  @brief Return the average.
     *  @return The average.
     */
    T average() const {
        return mean_;
    }
  private:
    /** @brief A running count of the number of elements added to this object.
     */
    int count_;
    /** @brief Working variables from which the statistics are finally
     * calculated.
     */
    T mean_, n_variance_;
};

#endif  // INCLUDE_STATS_H_
