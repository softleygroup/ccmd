//
//  Stats.h
//  templateTest
//
//  Created by Chris Rennick on 22/05/2013.
//  Copyright (c) 2013 Chris Rennick. All rights reserved.
//

#ifndef __templateTest__Stats__
#define __templateTest__Stats__

template <class T>
class Stats
{
public:
    Stats(){
        count=0;
    }
    
    
    
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

    T variance() const{
        return s/(count-1);
    }
    
    T average() const{
        return m;
    }
private:
    int count;
    T m, s;
};

#endif /* defined(__templateTest__Stats__) */
