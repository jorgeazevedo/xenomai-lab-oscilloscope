#include "blockio.h"
/*
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <native/queue.h>
#include <native/pipe.h>

#include <errno.h>
*/
#define QUEUE_SIZE 256
#define MAX_MESSAGE_LENGTH 5


int stopflag=1;

struct ioelements io;

const char *argp_program_version =
"XenomaiLab blocks 1.0";
const char *argp_program_bug_address =
"<jorge.azevedo@ua.pt>";
static char doc[] =
"XenomaiLab Blocks -- Self-contained Xenomai tasks used by Xenomailab\vExamples: gain -i sensor -o controler -p observer1,observer2 gain4\n          display -u error,setpoint pid_project/display1";
static char args_doc[] = "CONFIG_FILE";

/* The options we understand. */
static struct argp_option options[] = {
        {"input",   'i', "INPUTS", 0,
        "Comma separated list of input queues", 0 },
        {"output",   'o', "OUTPUTS", 0,
        "Comma separated list of output queues", 0 },
        {"input-pipes",   'u', "INPUTPIPES", 0,
        "Comma separated list of non real-time input pipes", 0 },
        {"output-pipes",   'p', "OUTPUTPIPES", 0,
        "Comma separated list of non real-time output pipes", 0 },
        { 0,0,0,0,0,0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
        char *config_file;
        char *input_queues;
        char *output_queues;
        char *output_pipes;
        char *input_pipes;
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
/* Get the input argument from argp_parse, which we
  know is a pointer to our arguments structure. */
struct arguments *arguments = (struct arguments *)state->input;

        switch (key)
         {
         case 'i':
           arguments->input_queues = arg;
           break;
         case 'o':
           arguments->output_queues = arg;
           break;
         case 'u':
           arguments->input_pipes = arg;
           break;
         case 'p':
           arguments->output_pipes = arg;
           break;

         case ARGP_KEY_ARG:
           if (state->arg_num >= 1)
             /* Too many arguments. */
             argp_usage (state);

           /* arguments->config_file[state->arg_num] = arg; */
           arguments->config_file = arg;

           break;

         case ARGP_KEY_END:
           if (state->arg_num <1 )
             /* Not enough arguments. */
             argp_usage (state);
           break;

         default:
           return ARGP_ERR_UNKNOWN;
         }
        return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void* safe_malloc(int bytes){
        void* mem_add;

        if(bytes)
                mem_add=malloc(bytes);
        else
                return NULL;

        if(mem_add==NULL)
                ERROR("Malloc failed, out of memory!\n");
        else
                return mem_add;

}

void parse_args(int argc, char* argv[]){
        struct arguments arguments;
        int i,strlen=0;

        /* Default values. */
        arguments.input_queues = "";
        arguments.output_queues = "";
        arguments.input_pipes = "";
        arguments.output_pipes = "";

        io.input_num=0;
        io.output_num=0;
        io.inputp_num=0;
        io.outputp_num=0;

        // Parse our arguments
        argp_parse (&argp, argc, argv, 0, 0, &arguments);

        //Block name
        //ghetto strlen
        for(strlen=0;argv[0][strlen]!='\0';strlen++);

        //from end to start, search for /
        for(i=strlen;argv[0][i]!='/';i--);

        //this will give us block_name plus a '\0'
        io.block_name=(char*)safe_malloc((strlen-i)*sizeof(char));

        sprintf(io.block_name,"%s", argv[0]+i+1);

        //Configuration file
        for(strlen=0;arguments.config_file[strlen]!='\0';strlen++);

        DEBUG("strlen of config is %d\n",strlen);

        io.config_file=(char*)safe_malloc((strlen+1)*sizeof(char));

        sprintf(io.config_file,"%s", arguments.config_file);

        //Process

        //Input
        if(arguments.input_queues[0] == '\0'){
                io.input_num=0;
        }
        else{
                //numbers of strings==number of commas+1
                for(i=0,io.input_num=1;arguments.input_queues[i]!='\0';i++)
                        io.input_num+=(arguments.input_queues[i]==',')?1:0;

                DEBUG("%d input strings found\n",io.input_num);

                //get space for pointer to those strings
                io.input_strings=(char**)safe_malloc(io.input_num*sizeof(char*));

                for(i=0;i<io.input_num;i++){

                        //ghetto strlen
                        for(strlen=0;(arguments.input_queues[strlen]!=',')&&(arguments.input_queues[strlen]!='\0');strlen++);

                        //terminate string
                        arguments.input_queues[strlen]='\0';

                        //name
                        io.input_strings[i]=(char*)safe_malloc((strlen+1)*sizeof(char));

                        sprintf(io.input_strings[i],"%s", arguments.input_queues);

                        //discard copied string
                        arguments.input_queues+=strlen+1;
                }
        }
        //Output
        if(arguments.output_queues[0] == '\0'){
                io.output_num=0;
        }
        else{
                //numbers of strings==number of commas+1
                for(i=0,io.output_num=1;arguments.output_queues[i]!='\0';i++)
                        io.output_num+=(arguments.output_queues[i]==',')?1:0;

                DEBUG("%d output strings found\n",io.output_num);

                //get space for pointer to those strings
                io.output_strings=(char**)safe_malloc(io.output_num*sizeof(char*));

                for(i=0;i<io.output_num;i++){

                        //ghetto strlen
                        for(strlen=0;(arguments.output_queues[strlen]!=',')&&(arguments.output_queues[strlen]!='\0');strlen++);

                        //terminate string
                        arguments.output_queues[strlen]='\0';

                        //name
                        io.output_strings[i]=(char*)safe_malloc((strlen+1)*sizeof(char));

                        sprintf(io.output_strings[i],"%s", arguments.output_queues);

                        //discard copied string, avance pointer to next string
                        arguments.output_queues+=strlen+1;
                }
        }

        //Inputp
        if(arguments.input_pipes[0] == '\0'){
                io.inputp_num=0;
        }
        else{
                //numbers of strings==number of commas+1
                for(i=0,io.inputp_num=1;arguments.input_pipes[i]!='\0';i++)
                        io.inputp_num+=(arguments.input_pipes[i]==',')?1:0;

                DEBUG("%d inputp strings found\n",io.inputp_num);

                //get space for pointer to those strings
                io.inputp_strings=(char**)safe_malloc(io.inputp_num*sizeof(char*));

                for(i=0;i<io.inputp_num;i++){

                        //ghetto strlen
                        for(strlen=0;(arguments.input_pipes[strlen]!=',')&&(arguments.input_pipes[strlen]!='\0');strlen++);

                        //terminate string
                        arguments.input_pipes[strlen]='\0';

                        //name
                        io.inputp_strings[i]=(char*)safe_malloc(36+((strlen+1)*sizeof(char)));

                        sprintf(io.inputp_strings[i],"/proc/xenomai/registry/native/pipes/%s", arguments.input_pipes);

                        //discard copied string
                        arguments.input_pipes+=strlen+1;
                }
        }

        //Outputp
        if(arguments.output_pipes[0] == '\0'){
                io.outputp_num=0;
        }
        else{
                //numbers of strings==number of commas+1
                for(i=0,io.outputp_num=1;arguments.output_pipes[i]!='\0';i++)
                        io.outputp_num+=(arguments.output_pipes[i]==',')?1:0;

                DEBUG("%d outputp strings found\n",io.outputp_num);

                //get space for pointer to those strings
                io.outputp_strings=(char**)safe_malloc(io.outputp_num*sizeof(char*));

                for(i=0;i<io.outputp_num;i++){

                        //ghetto strlen
                        for(strlen=0;(arguments.output_pipes[strlen]!=',')&&(arguments.output_pipes[strlen]!='\0');strlen++);

                        //terminate string
                        arguments.output_pipes[strlen]='\0';

                        //name
                        io.outputp_strings[i]=(char*)safe_malloc((strlen+1)*sizeof(char));

                        sprintf(io.outputp_strings[i],"%s", arguments.output_pipes);

                        //discard copied string
                        arguments.output_pipes+=strlen+1;
                }
        }

        DEBUG("Block name is %s\n",io.block_name);

        DEBUG("Config file is %s\n",io.config_file);

        for(i=0;i<io.input_num;i++)
                DEBUG("%d - %s\n",i,io.input_strings[i]);

        for(i=0;i<io.output_num;i++)
                DEBUG("%d - %s\n",i,io.output_strings[i]);

        for(i=0;i<io.inputp_num;i++)
                DEBUG("%d - %s\n",i,io.inputp_strings[i]);

        for(i=0;i<io.outputp_num;i++)
                DEBUG("%d - %s\n",i,io.outputp_strings[i]);
}


void free_args(void){
        short i;

        free(io.block_name);

        free(io.config_file);

        //free strings first, string vector second
        for(i=0;i<io.input_num;i++)
                free(io.input_strings[i]);
        free(io.input_strings);

        for(i=0;i<io.output_num;i++)
                free(io.output_strings[i]);
        free(io.output_strings);

        for(i=0;i<io.inputp_num;i++)
                free(io.inputp_strings[i]);
        free(io.inputp_strings);

        for(i=0;i<io.outputp_num;i++)
                free(io.outputp_strings[i]);
        free(io.outputp_strings);

}

void assert_io_min(int min_input, int min_output)
{
        if((io.input_num+io.inputp_num)<min_input)
                ERROR("This block needs at least %d input(s), not %d!\n",min_input,io.input_num);
        if((io.output_num+io.outputp_num)<min_output)
                ERROR("This block needs at least %d output(s)!\n",min_output);
        /* if(io.inputp_num<min_inputp) */
        /* 	ERROR("This block needs at least %d non real-time input(s), not %d!\n", */
        /* 			min_inputp,io.inputp_num); */
        /* if(io.outputp_num<min_outputp) */
        /* 	ERROR("This block needs at least %d non real-time output(s), not %d!\n", */
        /* 			min_outputp,io.outputp_num); */
}

void create_io(void){
        int i;
        double init_value=0.0;

        io.input_queues=(RT_QUEUE*)safe_malloc(io.input_num*sizeof(RT_QUEUE));
        for(i=0;i<io.input_num;i++){

                if(rt_queue_create(io.input_queues+i,io.input_strings[i],
                QUEUE_SIZE,MAX_MESSAGE_LENGTH,Q_FIFO|Q_SHARED)){
                        DEBUG("%s already exists, binding\n",io.input_strings[i]);
                        if(rt_queue_bind(io.input_queues+i,io.input_strings[i],TM_INFINITE))
                                ERROR("Failed to create bind to queue %s!\n",io.input_strings[i]);
                }
                rt_queue_write(io.input_queues+i,&init_value,sizeof(init_value),Q_URGENT);
        }

        io.output_queues=(RT_QUEUE*)safe_malloc(io.output_num*sizeof(RT_QUEUE));
        for(i=0;i<io.output_num;i++){

                if(rt_queue_create(io.output_queues+i,io.output_strings[i],
                QUEUE_SIZE,MAX_MESSAGE_LENGTH,Q_FIFO|Q_SHARED)){
                        DEBUG("%s already exists, binding\n",io.output_strings[i]);
                        if(rt_queue_bind(io.output_queues+i,io.output_strings[i],TM_INFINITE))
                                ERROR("Failed to create and bind to queue %s!\n",io.output_strings[i]);
                }
                rt_queue_write(io.output_queues+i,&init_value,sizeof(init_value),Q_URGENT);
        }


        io.input_pipes=(int*)safe_malloc(io.inputp_num*sizeof(int));
        for(i=0;i<io.inputp_num;i++){
                if((io.input_pipes[i]=open(io.inputp_strings[i],O_RDWR))<0)
                        ERROR("Failed to open pipe at %s!\n",io.inputp_strings[i]);
        }

        io.output_pipes=(RT_PIPE*)safe_malloc(io.outputp_num*sizeof(RT_PIPE));
        for(i=0;i<io.outputp_num;i++){

                if(rt_pipe_create(io.output_pipes+i,io.outputp_strings[i],P_MINOR_AUTO,0))
                        ERROR("Failed to create pipe %s\n",io.outputp_strings[i]);

        }

}

void free_io(void){
        int i;

        free(io.input_queues);

        free(io.output_queues);

        for(i=0;i<io.inputp_num;i++)
                close(io.input_pipes[i]);
        free(io.input_pipes);

        free(io.output_pipes);
}

void write_outputs(double sample)
{
        short i;
        //printf("%4.2f ->",sample);
        for(i=0;i<io.output_num;i++){

                rt_queue_write(io.output_queues+i,&sample,sizeof(sample),Q_URGENT);

        //	printf(" %s",io.output_strings[i]);
        }
        //printf("\n");
        for(i=0;i<io.outputp_num;i++){
                //rt_pipe_flush(io.output_pipes+i,XNPIPE_OFLUSH);
                rt_pipe_write(io.output_pipes+i, &sample, sizeof(sample), P_URGENT);
        }
}

void read_inputs(RTIME mode)
{
        short i;
        double sample;

        //if(mode==TM_NONBLOCK)
        //	mode=1000;

        //These two types of input should be always
        //mutualy exclusive.

        for(i=0;i<io.input_num;i++){
                switch(rt_queue_read(io.input_queues+i,&sample,sizeof(sample),mode)){
                case -EWOULDBLOCK:
                        //DEBUG("%d-%s: EWOULDBLOCK\n",i,io.input_strings[i]);
                        break;
                default:
                        //DEBUG("%d-%s--> %4.2f\t",i,io.input_strings[i],sample);
                        io.input_result[i]=sample;

                }

        }

        for(i=0;i<io.inputp_num;i++){
                read(io.input_pipes[i],&sample,sizeof(sample));
                io.inputp_result[i]=sample;
                //DEBUG("%s-->%4.2f\t", io.inputp_strings[i]+36,io.inputp_result[i]);
        }
        //DEBUG("\n");
}

void register_mutex(void)
{


        switch (rt_mutex_create(&io.mutex_lpt,"LPT")){
                case 0://sucess!
                        DEBUG("Mutex creation done!\n");
                        break;
                case -EEXIST:
                        DEBUG("Failed to create mutex, ");
                        if(rt_mutex_bind(&io.mutex_lpt,"LPT",TM_INFINITE))
                                ERROR("binding failed\n");
                        else
                                DEBUG("bind succeed\n");

                        break;
                case -EPERM:
                        ERROR("Invalid context for calling mutex\n");
        }
}


void free_mutex(void)
{
        rt_mutex_unbind(&io.mutex_lpt);
        rt_mutex_delete(&io.mutex_lpt);

}

void stop(int signum){

        DEBUG("\nCaught signal %d, exiting\n", signum);
        stopflag=0;
}
