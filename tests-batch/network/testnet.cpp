//Network testing Dev
// So I can get print lines and control flow easier (screw you ctest)
#include <stdio.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//dev dependencies
#include <errno.h>
#include <string>
#include <cstdlib>

#include "net/VRNetClient.h"
#include "net/VRNetServer.h"
#include "net/VRNetInterface.h"

extern "C" {
    #include <pthread.h>
}

#define PORT "3490"
#define NUMCLIENTS 1

int tasks[NUMCLIENTS];

void *ls(void *blank){
        //printf("Launch Server\n"); 
        //launch server on port 
        MinVR::VRNetServer server = MinVR::VRNetServer(PORT,NUMCLIENTS);
        printf("Server filled connections: Status OK\n"); 

        //MinVR::VRDataQueue::serialData d = "<VRDataQueue num = \"1\" >eggs</ VRDataQueue>";

        //MinVR::VRDataQueue::serialData d = "why tho";

        MinVR::VRDataQueue::serialData r = server.syncEventDataAcrossAllNodes("abcd");
        printf("Finished Server event data request \n");
     /*    server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes();
        server.syncSwapBuffersAcrossAllNodes(); */

        //std::cout << "SERVER DATA: " << d << std::endl; 

        pthread_exit(NULL);
    
}

void *lc(void *blank){
    srand(time(NULL));
    long r = random();
    MinVR::VRNetClient client = MinVR::VRNetClient("127.0.0.1", PORT);

    // //printf("%ld\n",r); 
    // if (!(r % 3)){
    //     sleep(2); 
    // }
    //MinVR::VRDataQueue::serialData d = "<VRDataQueue num = \"2\" >doubleback</ VRDataQueue>";
    MinVR::VRDataQueue::serialData c = client.syncEventDataAcrossAllNodes("abcd");
    printf("Finished Client event data req\n");
    
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();
    // client.syncSwapBuffersAcrossAllNodes();

    // std::cout << "CLIENT DATA: "<< c << std::endl;
 
	pthread_exit(NULL); 
}

//this function tests the server and the client in tandem 
int main(int argc,char* argv[]){
    pthread_t stID, cid1, cid2; 
    pthread_t cids[NUMCLIENTS];
    pthread_attr_t ct_attr, st_attr;

    int blank = 1;

    #ifdef USE_PTHREAD 
    printf("OH WHOA\n"); 
    #endif
    
    pthread_attr_init(&st_attr);
    pthread_attr_setdetachstate(&st_attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&st_attr, SCHED_FIFO); 

    // create server thread
    int st_status = pthread_create(&stID,&st_attr,ls,(void *) blank);

    if (st_status) { 
        fprintf(stderr,"pthread_create falied with Error code %d\n",st_status); 
    }

    pthread_attr_destroy(&st_attr); 
     
    //put a small break before starting the clients for clarity 
    // and to avoid clobbering (if that's even a thing)
    sleep(2);

    pthread_attr_init(&ct_attr);
    pthread_attr_setdetachstate(&ct_attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&ct_attr, SCHED_FIFO);

    int ct_status; //check the client threads

    for (int i = 1; i <= NUMCLIENTS; i++){
        //printf("client thread %d started\n",i);
        ct_status = pthread_create(&cids[i - 1],&ct_attr,lc,(void *) i); 
        
        if(ct_status){
            printf("Failed to create client thread %d\n",i);
        }

    }

    pthread_attr_destroy(&ct_attr); 
    // pthread_join(); 
       
    //MinVR::VRNetServer server = MinVR::VRNetServer(PORT,NUMCLIENTS);

    void *rs;
    //pthread_join(cids[0],&rs);
    pthread_exit(NULL); 

    printf("did we exit?"); 

    return 12; 
}

