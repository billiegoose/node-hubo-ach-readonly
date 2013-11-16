#include <node.h>

/* Required Hubo Headers */
#include <hubo.h>

/* For Ach IPC */
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <inttypes.h>
#include "ach.h"

using namespace v8;

/* Ach Channel IDs */
ach_channel_t chan_hubo_state;    // Feed-Back (State)

/* Create initial structures to read and write from */
struct hubo_state H_state;

// This function opens the ACH channel. Returns 1 if successful, 0 if unsuccessful.
Handle<Value> Init(const Arguments& args) {
    HandleScope scope; // Ensures that any v8 handles created in this function are properly cleaned up.

    /* Open Ach Channel */
    int r = ach_open(&chan_hubo_state, HUBO_CHAN_STATE_NAME , NULL);
    if ( ACH_OK != r ) {
        return scope.Close(Integer::New(0));
    };
    //memset( &H_state, 0, sizeof(H_state)); // I don't think we need this. -Will

    // When returning a value from a function, make sure to wrap it in
    // scope.Close(). This ensures that the handle stays valid after the current
    // scope (declared with the previous statement) is cleaned up.
    return scope.Close(
        // Creating a new JavaScript integer is as simple as passing a C int
        // (technically a int32_t) to this function.
        Integer::New(1)
    );  
}

// This function returns the current robot state.
Handle<Value> getState(const Arguments& args) {
    HandleScope scope; // Ensures that any v8 handles created in this function are properly cleaned up.
    /* for size check */
    size_t fs;

    /* Get the current feed-back (state) */
    int r = ach_get( &chan_hubo_state, &H_state, sizeof(H_state), &fs, NULL, ACH_O_LAST );
    if(ACH_OK != r) {
        // TODO: Throw exception?
        // assert( sizeof(H_state) == fs );
    }

    // NOTE: These are all the things in H_state:
    // typedef struct hubo_state {
    //     hubo_imu_t imu[HUBO_IMU_COUNT]; ///< IMU
    //     hubo_ft_t ft[4];   ///< ft sensors
    //     struct hubo_joint_state joint[HUBO_JOINT_COUNT]; ///> Joint pos, velos, and current
    //         hubo_joint_status_t status[HUBO_JOINT_COUNT];
    //     struct hubo_jmc_state driver[HUBO_JMC_COUNT];
    //     hubo_power_t power; // back power board
    //         double time;
    //     int16_t refWait;
    // }__attribute__((packed)) hubo_state_t;

    // Create the object we are going to return.
    Handle<Object> state = Object::New();

    // Create FT array
    Handle<Array> fts = Array::New();
    state->Set(String::New("ft"),fts);

    // Set FT array props
    for (int i = 0; i < 4; i++) {        
        // Create FT
        Handle<Object> ft = Object::New();
        fts->Set(Number::New(i),ft);
        // Set FT props
        ft->Set(String::New("m_x"),Number::New(H_state.ft[i].m_x));
        ft->Set(String::New("m_y"),Number::New(H_state.ft[i].m_y));
        ft->Set(String::New("f_z"),Number::New(H_state.ft[i].f_z));
    }

    // Create joint array
    Handle<Array> joints = Array::New();
    state->Set(String::New("joint"),joints);

    // Set joint array props
    for (int i = 0; i < HUBO_JOINT_COUNT; i++) {
        // Create joint
        Handle<Object> joint = Object::New();
        joints->Set(Number::New(i),joint);
        // Set joint props
        joint->Set(String::New("ref"),Number::New(H_state.joint[i].ref)); //double
        joint->Set(String::New("comply"),Integer::New((int)H_state.joint[i].comply)); //uint8_t
        joint->Set(String::New("pos"),Number::New(H_state.joint[i].pos)); //double
        joint->Set(String::New("cur"),Number::New(H_state.joint[i].cur)); //double
        joint->Set(String::New("vel"),Number::New(H_state.joint[i].vel)); //double 
        joint->Set(String::New("duty"),Number::New(H_state.joint[i].duty)); //double
        joint->Set(String::New("heat"),Number::New(H_state.joint[i].heat)); //double
        joint->Set(String::New("tmp"),Number::New(H_state.joint[i].tmp)); //double
        joint->Set(String::New("active"),Integer::New((int)H_state.joint[i].active)); //uint8_t
        joint->Set(String::New("zeroed"),Integer::New((int)H_state.joint[i].zeroed)); //uint8_t
    }
    return scope.Close(state);
}

void RegisterModule(Handle<Object> target) {

    target->Set(String::NewSymbol("init"),
        FunctionTemplate::New(Init)->GetFunction());
    target->Set(String::NewSymbol("getState"),
        FunctionTemplate::New(getState)->GetFunction());

    // Declare useful constants
    target->Set(String::NewSymbol("WST"),Integer::New(0));  //  Trunk Yaw
    target->Set(String::NewSymbol("NKY"),Integer::New(1));  //  Neck Yaw
    target->Set(String::NewSymbol("NK1"),Integer::New(2));  //  Neck 1
    target->Set(String::NewSymbol("NK2"),Integer::New(3));  //  Neck 2
    target->Set(String::NewSymbol("LSP"),Integer::New(4));  //  Left Shoulder Pitch
    target->Set(String::NewSymbol("LSR"),Integer::New(5));  //  Left Shoulder Yaw
    target->Set(String::NewSymbol("LSY"),Integer::New(6));  //  Left Shoulder Roll
    target->Set(String::NewSymbol("LEB"),Integer::New(7));  //  Left Elbow Pitch
    target->Set(String::NewSymbol("LWY"),Integer::New(8));  //  Left Wrist Yaw
    target->Set(String::NewSymbol("LWR"),Integer::New(9));  //  Left Wrist Roll
    target->Set(String::NewSymbol("LWP"),Integer::New(10)); //  Left Wrist Pitch
    target->Set(String::NewSymbol("RSP"),Integer::New(11)); //  Right Shoulder Pitch
    target->Set(String::NewSymbol("RSR"),Integer::New(12)); //  Right Shoulder Roll
    target->Set(String::NewSymbol("RSY"),Integer::New(13)); //  Right Shoulder Yaw
    target->Set(String::NewSymbol("REB"),Integer::New(14)); //  Right Elbow Pitch
    target->Set(String::NewSymbol("RWY"),Integer::New(15)); //  Right Wrist Yaw
    target->Set(String::NewSymbol("RWR"),Integer::New(16)); //  Right Wrist Roll
    target->Set(String::NewSymbol("RWP"),Integer::New(17)); //  Right Wrist Pitch

    target->Set(String::NewSymbol("LHY"),Integer::New(19)); //  Left Hip Yaw
    target->Set(String::NewSymbol("LHR"),Integer::New(20)); //  Left Hip Roll
    target->Set(String::NewSymbol("LHP"),Integer::New(21)); //  Left Hip Pitch
    target->Set(String::NewSymbol("LKN"),Integer::New(22)); //  Left Knee Pitch
    target->Set(String::NewSymbol("LAP"),Integer::New(23)); //  Left Ankle Pitch
    target->Set(String::NewSymbol("LAR"),Integer::New(24)); //  Left Ankle Roll

    target->Set(String::NewSymbol("RHY"),Integer::New(26)); //  Right Hip Yaw
    target->Set(String::NewSymbol("RHR"),Integer::New(27)); //  Right Hip Roll
    target->Set(String::NewSymbol("RHP"),Integer::New(28)); //  Right Hip Pitch
    target->Set(String::NewSymbol("RKN"),Integer::New(29)); //  Right Knee Pitch
    target->Set(String::NewSymbol("RAP"),Integer::New(30)); //  Right Ankle Pitch
    target->Set(String::NewSymbol("RAR"),Integer::New(31)); //  Right Ankle Roll
    target->Set(String::NewSymbol("RF1"),Integer::New(32)); //  Right Finger
    target->Set(String::NewSymbol("RF2"),Integer::New(33)); //  Right Finger
    target->Set(String::NewSymbol("RF3"),Integer::New(34)); //  Right Finger
    target->Set(String::NewSymbol("RF4"),Integer::New(35)); //  Right Finger
    target->Set(String::NewSymbol("RF5"),Integer::New(36)); //  Right Finger
    target->Set(String::NewSymbol("LF1"),Integer::New(37)); //  Left Finger
    target->Set(String::NewSymbol("LF2"),Integer::New(38)); //  Left Finger
    target->Set(String::NewSymbol("LF3"),Integer::New(39)); //  Left Finger
    target->Set(String::NewSymbol("LF4"),Integer::New(40)); //  Left Finger
    target->Set(String::NewSymbol("LF5"),Integer::New(41)); //  Left Finger

    target->Set(String::NewSymbol("HUBO_FT_R_HAND"),Integer::New(0));
    target->Set(String::NewSymbol("HUBO_FT_L_HAND"),Integer::New(1));
    target->Set(String::NewSymbol("HUBO_FT_R_FOOT"),Integer::New(2));
    target->Set(String::NewSymbol("HUBO_FT_L_FOOT"),Integer::New(3));
    target->Set(String::NewSymbol("HUBO_IMU0"),Integer::New(4));
    target->Set(String::NewSymbol("HUBO_IMU1"),Integer::New(5));
    target->Set(String::NewSymbol("HUBO_IMU2"),Integer::New(6));
}

NODE_MODULE(hubo_ach_readonly, RegisterModule);
