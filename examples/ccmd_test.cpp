//
//  ccmd_test
//
//  Program to illustrate the use of the CCMD library for performing
//  molecular dynamics simulations of Coulomb crystals and producing
//  simulated microscope images.
//  
//

#include <iostream>
#include <fstream>

#include "ccmd_sim.h"
#include "ion_trap.h"
#include "ion_cloud.h"

#include "integrator.h"
// #include "cuda_integrator.h"

#include "ImageCollection.h"
#include "IonHistogram.h"
#include "ccmd_image.h"

#include <sstream>
#include <ctime>
#include <iomanip>

double stopWatchTimer();
double KE;
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
    if (argc != 2) {
	std::cout << "usage: " << argv[0] << " [working directory]" << std::endl;
	std::exit(1);
    }


    std::string path = std::string(argv[1]);
    if (path[path.length()-1] != '/') 
	    path += "/";

    // Parameter file paths
    string info_file = path + "trap.info";
    string trap_param_file = path + "trap_config.txt";
    string ion_types_file = path + "ion_types.txt";
    string cloud_param_file = path + "ion_numbers.txt";
    string integrator_param_file = path + "integrator.txt";
    
    try {
        // Get simulation parameters from files
        Trap_params trap_params(info_file);
        Cloud_params cloud_params(info_file);
        Integration_params integrator_params(info_file);
        
        // Construct trap
        //Cosine_trap trap(trap_params);
        Pulsed_trap trap(trap_params);
        
        // Construct ion cloud
        Ion_cloud cloud(trap, cloud_params);
        
        // Construct integrator
        RESPA_integrator integrator(trap, cloud, integrator_params);

        // Construct integrator
        //CUDA_integrator integrator(trap, cloud, integrator_params);
        
        // 3D histogram for image creation
        ImageCollection ionImages(0.1);
        
        // Cool down ion cloud
        cout << flush << "Running cool down" << endl;
        int nt_cool = 20000;
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
        int nt = 20000;
        
        cout << flush << "Accuiring histogram data" << endl;

	    // Start timer
        stopWatchTimer();
        KE = 0;
        for (int t=0; t<nt; ++t) {    
            integrator.evolve();
            cloud.update_position_histogram(ionImages);
            cloud.updateStats();
            
            // Track progress
            int percent = static_cast<int>( (t*100)/nt );
            if ( (t*100/5)%nt == 0) {
                printProgBar(percent);
                cout << setw(4) << stopWatchTimer() << "s";
            }
            KE += cloud.kinetic_energy();
        }
        KE /= nt;
        printProgBar(100);
        std::cout << endl;
        
        cout << "total kinetic energy = " << KE << endl;
        //ionImages.writeFiles(path);
        cloud.saveStats(path);

    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

