#!/usr/bin/env python
from numpy import arange
from string import Template, split

import os
import subprocess
import sys
import shutil
import threading
import time

# Loop through a range of two variables, creating an input file for each and
# execute CCMD. The script limits the number of concurrently executing CCMD
# simulations to the value in max_threads.

# Ranges for the two scan parameters
#i_start = 0
#i_end   = 510
#i_incr  = 10
#j_start = 0.23
#j_end   = 0.25
#j_incr  = 0.01

# Generate ranges of values for the for loops. Can also be replaced with an
# explicitly defined list as:
# i_vals = ["waveform","digital"]
i_vals = [50] 
j_vals = [5]
#j_vals = [120, 125.5, 130, 137, 141.5, 147, 151.5, 155, 156]


#i_vals = arange(i_start, i_end, i_incr)
#j_vals = arange(j_start, j_end, j_incr)


# Base path is the directory in which each sub-directory is created. Run path
# is a template for the name of each working directory. Each value of i and j
# are used to create the directory name through substitution of ${i} and ${j}
# placeholders respectively. The template file is the source for the input file
# that will be copied to each working directory. The markers ${i} and ${j} are
# replaced with each value from the loop.
base_path = r'Ca200_ND3_Xe'
run_path  = r'Ca200_ND3${i}_Xe${j}'
template_file = r'trap_template.info'
source_waveform = r'waveform.dat'

# CCMD execution. Specify the path to the ccmd executable, and specify the
# maximum number of copies to run simultaneously.
ccmd_command = r'..\bin\linux\ccmd.exe'
# ccmd_command = os.path.expanduser(ccmd_command)
max_threads = 4

#======================================================================

# Read base input file and convert to a Python template.
try:
    basefile = open(template_file)
    params_template = Template(basefile.read())
    path_template = Template(run_path)
    basefile.close()
except IOError as e:
    print "Error opening template file %s\nerror(%s) : %s" \
            % (template_file, e[0], e[1])
    sys.exit()


def do_loop():
    """
    Perform the loop over parameters. During each iteration, the name of the
    working directory is built from the current values of i and j and the
    template stored in run_path. This directory is created if it doesn't
    already exist. Placeholders marked ${i} and ${j} CCMD input file
    template_file are replaced with the current values and the file is written
    to the new working drectory, overwriting one already there.

    Each working directory is passed to a ThreadCounter object that handles
    calling the ccmd executable and limiting the number of concurrently running
    copies.
    """

    for i_val in i_vals:
        for j_val in j_vals:
            # Build a dictionary of replacements for the placeholders
            replacements = dict(i=i_val, j=j_val)

            # Start a thread to execute CCMD when ready.
            exe = ThreadCounter(replacements)
            exe.start()

#===============================================================================

# Create a semaphore to keep count of the number of running copies.
thread_limit = threading.BoundedSemaphore(max_threads)

class ThreadCounter(threading.Thread):
    """
    Class to handle executing a new copy of CCMD when ready. The execution is
    blocked until the semaphore can be acquired, then the command-line for
    CCMD is passed to a subprocess for execution.
    """
    def __init__(self, replacements):
        threading.Thread.__init__(self)
        self.replacements = replacements
    def run(self):
        thread_limit.acquire()
        try:
            # Substitute placeholders in file path and input file templates.
            # Fail and quit the script if we are left with anything
            # un-substituted.
            new_params = params_template.substitute(self.replacements)
            new_path   = path_template.substitute(self.replacements)


            # Concatentate the base path with the new path name and create this
            # directory if it doesn't exist. Fail and quit the script if this
            # causes problems.
            target_path = os.path.join(base_path, new_path)
            if not os.path.exists(target_path):
                os.makedirs(target_path)

            # Write the new, substituted, input file to the working directory
            newfile = open(os.path.join(target_path, "trap.info"), 'w')
            newfile.write(new_params)
            newfile.close()

            # If the waveform file is in the current directory, copy it to the
            # new working directory. Otherwise, skip this.
            if os.path.exists(source_waveform):
                target_waveform = os.path.join(target_path, "waveform.dat")
                shutil.copy(source_waveform, target_waveform)

            print "\n********************"
            print "Thread starting in %s" % new_path
            print "********************\n"
            cmd = split(ccmd_command) + [target_path]
            proc = subprocess.Popen(cmd)
            proc.communicate()

        except KeyError as ke:
            print "Unexpected key in file"
            print ke.strerror
            sys.exit(1)
        except os.error as e:
            print "Can't create new working directory. Exiting."
            print e.strerror
            sys.exit(1)
        except IOError as e:
            print "Error writing input file %s\nerror(%s) : %s" \
                    % (target_path, e.errno, e.errstr)
        finally:
            thread_limit.release()

def main():
    """
    Call the primary function when the script is run from the command line.
    """
    do_loop()

if __name__ == "__main__":
    main()
