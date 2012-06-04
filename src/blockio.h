#ifndef BLOCKIO_H
#define BLOCKIO_H

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/queue.h>
#include <native/pipe.h>

#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>     //exit(1))
#include <sys/types.h> //open()
#include <fcntl.h>	//open() related
#include <argp.h>


//Useful Macros
//Commenting this define disables debug messages
#define DEBUGON

#define ERROR(...) fprintf(stderr, "E:"__VA_ARGS__),exit(1)

#ifdef DEBUGON
        #define DEBUG(...) printf(__VA_ARGS__)
#else
        #define	DEBUG(...)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct ioelements
{
        //For actual access
        int *input_pipes;	//this hold file descriptors
        RT_PIPE *output_pipes;
        RT_QUEUE *input_queues,*output_queues;

        //Ensure mutual exclusion on
        //a resource subject to contention.
        //This is normaly the I\O device being used
        //in this case the LPT port.
        RT_MUTEX mutex_lpt;

        //Names and count
        char** input_strings,**output_strings,
                **inputp_strings,**outputp_strings;
        short input_num,output_num,inputp_num,outputp_num;

        //These hold the inputs
        double input_result[25];
        double inputp_result[25];

        //configuration file
        char *config_file;
        //block name
        char *block_name;
};



extern struct ioelements io;

//Every loop is while(stopflag). On ctrl-c
//and other signals, stopflag turns false
//and the loops exit
extern int stopflag;

void* safe_malloc(int bytes);

void parse_args(int argc, char* argv[]);
void free_args(void);

void create_io(void);
void assert_io_min(int min_input, int min_output);
void free_io(void);

void write_outputs(double sample);
void read_inputs(RTIME mode);

void register_mutex(void);
void free_mutex(void);

void stop(int signum);

#ifdef __cplusplus
}
#endif


#endif // BLOCKIO_H
