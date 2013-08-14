//
//  Hist1D.h
//  HistTest
//
//  Created by Chris Rennick on 08/03/2013.
//  Copyright (c) 2013 Chris Rennick. All rights reserved.
//

#ifndef __HistTest__Hist1D__
#define __HistTest__Hist1D__

#include <vector>
#include <map>
#include <iostream>

class Hist1D {
public:
    Hist1D(const double width);
    void update(const double& p);

    size_t get_number_bins () const { return max_bin_num; }
    double get_bin_width () const { return bin_width; }
    
private:
    double bin_width;
    int max_bin_num;
    std::map<int, int> hist;
    friend std::ostream& operator<<(std::ostream& os, const Hist1D& h);
};


#endif /* defined(__HistTest__Hist1D__) */
