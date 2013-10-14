//
//  Hist1D.cpp
//  HistTest
//
//  Created by Chris Rennick on 08/03/2013.
//  Copyright (c) 2013 Chris Rennick. All rights reserved.
//

#include "Hist1D.h"
#include <cmath>
#include <algorithm>

Hist1D::Hist1D(double width) : hist()
{
    this->bin_width = width;
    max_bin_num = 0;
}

void Hist1D::update(const double &p)
{
    int bin_num = int(std::floor(p/bin_width));
    max_bin_num = std::max(max_bin_num, bin_num);
//    max_bin_num = bin_num > max_bin_num ? bin_num : max_bin_num;
    ++ hist[bin_num];
}

std::ostream& operator<<(std::ostream& os, const Hist1D& h)
{
    typedef std::map<int, int>::const_iterator mapit;
    int bin_count;
    
    for (int i=0; i<h.get_number_bins(); i++)
    {
        if (h.hist.count(i))
            bin_count = h.hist.at(i);
        else
            bin_count = 0;
        os << h.get_bin_width()*i << "\t" << bin_count << "\n";
    }
    return os;
}
