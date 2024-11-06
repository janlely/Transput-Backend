#include "transput.h"
#include "stdlib.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    char *settled_text;
    size_t settled_text_length;
    size_t break_pos;
} TransputComposition;

typedef struct {
    RimeApi *rime_api;
    TransputSchemaList *schema_list;
    RimeContext *context;
    TransputCadidates *cadidates;
    TransputComposition *composition;
    Bool command_mode;
    char* command_buffer;
    Bool ai_mode;
} InternalTransputState;

const char** commands = {
    "paste",
    "schemas",
    "translate",
    ""
};


TransputApi* create_transput_api() {
    TransputApi *api = malloc(sizeof(TransputApi));
    memset(api, 0, sizeof(TransputApi));

    api->state = malloc(sizeof(InternalTransputState));
    
    api->init = init_transput;
    api->start = start_transput;
    api->deinit = deinit_transput;
    api->new_session = transput_new_session;
    api->process_key = transput_process_key;
    api->get_cadidates = transput_get_cadidates;
    api->deploy = transput_deploy;
    init_transput(api);
    return api;
}

void init_transput(TransputApi *api) {
    InternalTransputState *state = api->state;
    
    state->rime_api = rime_get_api();

    state->schema_list = malloc(sizeof(TransputSchemaList));
    memset(state->schema_list, 0, sizeof(TransputSchemaList));

    state->context = malloc(sizeof(RimeContext));
    memset(state->context, 0, sizeof(RimeContext));
    state->context->data_size = sizeof(RimeContext);

    state->cadidates = malloc(sizeof(TransputCadidates));
    memset(state->cadidates, 0, sizeof(TransputCadidates));
    state->cadidates->size = 100;
    state->cadidates->cadidates = malloc(sizeof(char*) * 100);

    state->command_mode = 0;
    state->command_buffer = malloc(sizeof(char) * 32);
    memset(state->command_buffer, 0, sizeof(char) * 32);

    state->ai_mode = 0;

    state->composition = malloc(sizeof(TransputComposition));
    memset(state->composition, 0, sizeof(TransputComposition));
    state->composition->settled_text = malloc(sizeof(char) * 300);
    memset(state->composition->settled_text, 0, sizeof(char) * 300);
    state->composition->break_pos = 0;
    state->composition->settled_text_length = 299;
}



void start_transput(TransputApi *api, TransputProperties *props) {
    RimeTraits *traits = malloc(sizeof(RimeTraits));
    memset(traits, 0, sizeof(RimeTraits));
    traits->log_dir = props->log_dir;
    traits->shared_data_dir = props->shared_dir;
    traits->user_data_dir = props->user_dir;
    traits->app_name = "rime.transput";
    traits->distribution_code_name = "Transput";
    traits->distribution_name = "Transput";
    traits->distribution_version = "1.0.0";
    traits->data_size = sizeof(RimeTraits);

    InternalTransputState *state = api->state;
    state->rime_api->initialize(traits);
    state->rime_api->start_maintenance(1);
    free(traits);
}
void deinit_transput(TransputApi *api) {
    InternalTransputState *state = api->state;
    state->rime_api->finalize();
}
TransputSessionId transput_new_session(TransputApi *api) {
    InternalTransputState *state = api->state;
    return state->rime_api->create_session();
}
enum InputResultType transput_process_key(TransputApi *api, TransputSessionId sid, int key) {
    InternalTransputState *state = api->state;

    //命令模式中
    if (state->command_mode) {

    }
    if (state->rime_api->process_key(sid, key, 0) == 0) {
        return IR_NOT_READY;
    }
    return IR_TYPING;
}
void transput_deploy(TransputApi *api) {
}

TransputCadidates* transput_get_cadidates(TransputApi *api, TransputSessionId sid) {
    InternalTransputState *state = api->state;
    if (state->rime_api->get_context(sid, state->context) == 0) {
        return NULL;
    }
    state->cadidates->size = state->context->menu.num_candidates;
    // printf("cadidates size: %d\n", api->cadidates->size);
    for (int i = 0; i < state->context->menu.num_candidates; i++) {
        state->cadidates->cadidates[i] = state->context->menu.candidates[i].text;
    }
    return state->cadidates;
}
