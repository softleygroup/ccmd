//
//  ccmd_time.cpp
//
//  Compare execution times for the CUDA and RESPA integrators
//

#include <iostream>

#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"
//#include "integrator.h"
#include "cuda_integrator.h"

#include "hist3D.h"
#include "ccmd_image.h"

#include <sstream>
#include <ctime>
#include <iomanip>

double stopWatchTimer(bool reset=false);

void printProgBar( int percent );

double stopWatchTimer(bool reset) {
    static clock_t start = std::clock();
    if (reset) {
       start = std::clock();
    } 
    double time_elapsed;
    time_elapsed = ( std::clock() - start )/static_cast<double>(CLOCKS_PER_SEC); 
    return time_elapsed;
}

void printProgBar( int percent ){
    std::string bar;
    for(int i = 0; i < 50; i++){
        if( i < (percent/2)){
            bar.replace(i,1,"=");
        }else if( i == (percent/2)){
            bar.replace(i,1,">");
        }else{
            bar.replace(i,1," ");
        }
    }
    std::cout << '\r' << "[" << bar << "] ";
    std::cout.width( 3 );
    std::cout<< percent << "%     " << std::flush;
}

using namespace std;


int main (int argc, char * const argv[]) {

    // Parameter file paths
    string trap_param_file = "../../config/trap_config.txt";
    string ion_types_file = "../../config/ion_types.txt";
    string cloud_param_file = "../../config/ion_numbers.txt";
    string integrator_param_file = "../../config/integrator.txt";
    
    try {
        // Get simulation parameters from files
        Trap_params trap_params(trap_param_file);
        Cloud_params cloud_params(cloud_param_file,ion_types_file);
        Integration_params integrator_params(integrator_param_file);

        // Construct trap
        Cosine_trap trap(trap_params);
        
        // Construct ion cloud
        Ion_cloud cloud(trap, cloud_params);

        // Construct integrator
        RESPA_integrator respa_integrator(trap, cloud, integrator_params);

        // Construct integrator
        CUDA_integrator cuda_integrator(trap, cloud, integrator_params);
        
        // Evolution        
        int nt = 1000;
        
        // Start timer for CUDA
        stopWatchTimer();

        for (int t=0; t<nt; ++t) {    
            cuda_integrator.evolve();
            
            // Track progress
            int percent = static_cast<int>( (t*100)/nt );
            if ( (t*100/5)%nt == 0) {
                printProgBar(percent);
                cout << setw(4) << stopWatchTimer() << "s";
            }
        }
        double cuda_time = stopWatchTimer();
        printProgBar(100);
        std::cout << endl;

        
        // Start timer for RESPA
        stopWatchTimer(true);

        for (int t=0; t<nt; ++t) {    
            respa_integrator.evolve();
            
            // Track progress
            int percent = static_cast<int>( (t*100)/nt );
            if ( (t*100/5)%nt == 0) {
                printProgBar(percent);
                cout << setw(4) << stopWatchTimer() << "s";
            }
        }
        double respa_time = stopWatchTimer();
        printProgBar(100);
        std::cout << endl;

        cout << "CUDA: " << cuda_time << ' ' << "RESPA: " << respa_time 
             << " " << "ratio: " << respa_time/cuda_time << endl;
        
    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

