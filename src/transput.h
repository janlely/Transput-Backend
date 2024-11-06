#ifndef TRANSPUT_H_ 
#define TRANSPUT_H_ 

#include "rime_api.h"


typedef struct transput_api_t TransputApi;
typedef struct transput_properties_t TransputProperties; 
typedef struct transput_cadidates_t TransputCadidates;
typedef struct transput_schema_t TransputSchema;
typedef struct transput_schema_list_t TransputSchemaList;
typedef struct TransputState TransputState;

typedef RimeSessionId TransputSessionId;
typedef void (*TransputInit) (TransputApi *api);
typedef void (*TransputStart) (TransputApi *api, TransputProperties *props);
typedef void (*TransputDeinit) (TransputApi *api);
typedef TransputSessionId (*TransputNewSession) (TransputApi *api);
typedef enum InputResultType (*TransputProcessKey) (TransputApi *api, TransputSessionId sid, int key);
typedef TransputCadidates* (*TransputGetCadidates) (TransputApi *api, TransputSessionId sid);
typedef void (*TransputDeploy) (TransputApi *api);

struct transput_api_t {
    TransputState *state;

    TransputInit init;
    TransputStart start;
    TransputDeinit deinit;
    TransputNewSession new_session;
    TransputProcessKey process_key;
    TransputGetCadidates get_cadidates;
    TransputDeploy deploy;
};


struct transput_properties_t {
    char *log_dir;
    char *shared_dir;
    char *user_dir;
};


struct transput_cadidates_t {
    int size;
    char **cadidates;
};


enum InputResultType {
    IR_NOT_READY = 0, //未就绪
    IR_TYPING = 1, //正常输入中
};

struct transput_schema_t {
    char * schema_id;
    char * schema_name;
};

struct transput_schema_list_t {
    TransputSchema *schemas;
    int size;
};

TransputApi* create_transput_api();

void init_transput(TransputApi *api);
void start_transput (TransputApi *api, TransputProperties *props);
void deinit_transput(TransputApi *api);
TransputSessionId transput_new_session(TransputApi *api);
enum InputResultType transput_process_key(TransputApi *api, TransputSessionId sid, int key);
void transput_deploy(TransputApi *api);
TransputCadidates* transput_get_cadidates (TransputApi *api, TransputSessionId sid);

#endif