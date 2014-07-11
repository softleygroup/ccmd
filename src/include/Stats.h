//
//  stats.h
//  templateTest
//
//  Created by Chris Rennick on 22/05/2013.
//  Copyright (c) 2013 Chris Rennick. All rights reserved.
//

#ifndef __Stats__
#define __Stats__

/**
 *  @class Stats
 *  @brief Accumulate average and variance for double values.
 *
 *  This class uses the Knuth algorithm to calculate the average and variance
 *  cumulatively, avoiding the need to know the total number of items. 
 */
 
template <class T>
class Stats
{
public:
    Stats(){
        count=0;
    }

    /**
     *  @brief Append a new value and update the statistics.
     *  @param newVal   Value to append.
     */
    void append(const T& newVal){
        count++;
        
        T m_prev, s_prev;
        if (count>1){
            m_prev = m;
            s_prev = s;
            m = m_prev + (newVal-m_prev)/count;
            s = s_prev + (newVal-m_prev)*(newVal-m);
            } else {
                m = newVal;
        }
    }

    /** @brief Reset the statistics.
     */
    void reset () {
        count=0;
        m = T();
        s = T();
    }

    /**  
     *  @brief Calculate and return the variance.
     *  @return The variance.
     */
    T variance() const{
        return s/(count-1);
    }
    
    /**
     *  @brief Return the average.
     *  @return The average.
     */
    T average() const{
        return m;
    }
private:
    /** @brief A running count of the number of elements added to this object. */
    int count;
    /** @brief Working variables from which the statistics are finally calculated. */
    T m, s;
};

#endif /* defined(__Stats__) */
