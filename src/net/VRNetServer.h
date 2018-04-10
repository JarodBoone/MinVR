#ifndef VRNETSERVER_H
#define VRNETSERVER_H

#include "VRNetInterface.h"

#ifndef WIN32
  #include <netinet/tcp.h>
  #include <netdb.h>
  #include <arpa/inet.h>
  #include <sys/wait.h>
  #include <signal.h>
#endif

#include <math/VRMath.h>
#include <config/VRDataIndex.h>
#include <pthread.h> // pthreads
#include <sys/select.h> // select()
#include <poll.h> // poll(); 

namespace MinVR {

  // these are the various actions a client thread is capable of handling
  // setting the action flag in the client_control struct will make all client 
  // threads attempt to handle that type of request for the client on their fd
  // 
  // Sleep does not actually block the client thread but rather keeps it spinning 
  // this is because we are flagging new requests more often than not and don't want 
  // the overhead of blocking and waking threads. Because of this client threads will 
  // keep processor space!
  enum clientAction {
    EVENTDATAREQ,
    SWAPBUFFERREQ,
    SLEEP,
    TERMINATE
  };

  // struct to contain variables to sychronize client threads 
  typedef struct client_control {
    // the number of clients this should not change 
    int nc; 
    // instruction for client threads
    clientAction ca; 

    // **************************** TASK CONTROL *********************************
    // client threads will hang on this condition variable after they have recieved
    // data from their clients and are waiting for the rest of the client threads to 
    // finish recieving and operating on their own data to respond, i.e. this syncronizes
    // the threads so response to all clients after handling all requests will more or less 
    // happen at the same time
    pthread_cond_t respond; 

    // this mutex will lock the condition variable as well as synchronize access to the 
    // client control struct. We can make accessing the client control singular because the 
    // updates should be fast and any updates will happen after the data has been waited on
    pthread_mutex_t mtx; 

    // counter variable to tell how many client threads have finished their computation and
    // are ready to respond
    int ready;

    // centrally accessible queues to push and serialize data when responding to an event message
    VRDataQueue dq;
    VRDataQueue::serialData* sq;

    // ***************** TASK DELEGATION AND COMPLETION CONTROL **************************
    // the function that issues a command will wait on this condition variable. This condition
    // is signaled when all of the client threads have fully completed their respective task 
    pthread_cond_t complete; 

    // condition variable that will make sure that client threads do not reenter the task manager
    // without the function that delegated an action returning the action to sleep and making sure 
    // each thread has completed it's task 
    pthread_cond_t threadSync;

    // mutex for thread sync condition variables 
    pthread_mutex_t threadSyncMtx;

    int finished; 

  } client_control_t; 

  enum msgType {
    EVENTDATA, 
    SWAPBUFFER
  };

class VRNetServer : public VRNetInterface {
 public:

  VRNetServer(const std::string &listenPort, int numExpectedClients);
  ~VRNetServer();

  VRDataQueue::serialData
    syncEventDataAcrossAllNodes(VRDataQueue::serialData eventData);
  VRDataQueue::serialData syncEventDataAcrossAllNodes();

  void syncSwapBuffersAcrossAllNodes();

  private:
    // initialize client control variables, must be static in order to access from
    // thread scoped functions
    static pthread_t* ctList; // array of client thread ids
    static client_control_t* ctr; // client control struct
    static int numClients; // number of clients

    static void handleED(int fd);
    static void handleSB(int fd);
    static void* clientThread(int);
    void JarodFunction(msgType);

    // variable to store client file descriptors
    std::vector<SOCKET> _clientSocketFDs;


};

}

#endif
