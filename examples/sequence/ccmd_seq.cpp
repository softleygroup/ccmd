//
//  ccmd_sequence
//
//  Program to generate a sequence of images for a reacting ion cloud 
//  
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

double stopWatchTimer();

void printProgBar( int percent );

double stopWatchTimer() {
    static clock_t start = clock();
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
        RESPA_integrator integrator(trap, cloud, integrator_params);

        // Construct integrator
        //CUDA_integrator integrator(trap, cloud, integrator_params);
        
        // 3D histogram for image creation
        Hist3D hist;
        hist.set_bin_size(.1);

        // Cool down ion cloud
        cout << flush << "Running cool down" << endl;
        int nt_cool = 5000;
        for (int t=0; t<nt_cool; ++t) {    
            integrator.evolve();
            
            // Track progress
            int percent = static_cast<int>( (t*100)/nt_cool );
            if ( (t*100/5)%nt_cool == 0 ) {
                printProgBar(percent);
            } 
        }
        printProgBar(100);
        cout << '\n';
        
        // Evolution        
        int nt = 10000;
        
        cout << flush << "Accuiring histogram data" << endl;

	    // Start timer
        stopWatchTimer();

        for (int t=0; t<nt; ++t) {    
            integrator.evolve();
            cloud.update_position_histogram(hist);
            
            // Track progress
            int percent = static_cast<int>( (t*100)/nt );
            if ( (t*100/5)%nt == 0) {
                printProgBar(percent);
                cout << setw(4) << stopWatchTimer() << "s";
            }
        }

        printProgBar(100);
        std::cout << endl;
        
        cout << "Generating image \n"; 
        Microscope_image ccd_image(640,640,hist);
        while ( !ccd_image.is_finished() ) {
            ccd_image.draw();
            printProgBar( static_cast<int>(ccd_image.get_progress()) );
        }
        
        string image_file_name = "../../output/image.txt";
        ccd_image.ouput_to_file(image_file_name);

    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

