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
    char command_key;
    Bool ai_mode;
} InternalTransputState;

const char commands[] = "vsrt";
const int command_size = 4;
const char** commands_list = {
    "v(从系统剪切板粘贴)",
    "s(切换输入方案)",
    "r(开/关AI翻译)",
    "t(执行AI翻译)"
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
    memset(state->cadidates->cadidates, 0, sizeof(char*) * 100);

    state->command_mode = 0;
    state->command_key = 0;

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

    free(state->schema_list);
    free(state->context);
    free(state->cadidates);
    free(state->cadidates->cadidates);
    free(state->composition);
    free(state->composition->settled_text);

}
TransputSessionId transput_new_session(TransputApi *api) {
    InternalTransputState *state = api->state;
    return state->rime_api->create_session();
}

void insert_composition(TransputComposition *composition, int key) {
    //在break_pos处插入key
}


void trigger_command_mode(InternalTransputState *state) {
    state->command_mode = (state->command_mode + 1) % 2; // trigger command_mode
}

Bool no_cadidates(TransputApi *api) {
    return 0;
}

void commit_rime_preedit(TransputApi *api) {
    //TODO: 把rime中的commitText和preedit都插入到composition中
}

Bool no_matching_command(char key) {
    for(int i = 0; i < command_size; i++) {
        if (commands[i] == key) {
            return 0; 
        }
    }
    return 1;
}


enum InputResultType transput_process_key(TransputApi *api, TransputSessionId sid, int key) {
    InternalTransputState *state = api->state;

    //命令模式中
    if (state->command_mode) {
        insert_composition(state->composition, key);
        
        state->command_key = key;

        if (no_matching_command(state->command_key)) {
            trigger_command_mode(state);
            return IR_TYPING;
        }
        return IR_CMD_TYPING;
    }

    //输入'/'进入命令模式
    if (key == '/') {
        //如果rime中有preedit，则提交到composition中
        commit_rime_preedit(api);
        trigger_command_mode(state);
        insert_composition(state->composition, key);
        return IR_CMD_TYPING;
    }

    //正常输入
    if (state->rime_api->process_key(sid, key, 0) == 0) {
        return IR_NOT_READY;
    }
    //如果没有候选词，则直接提交preedit，如果是非AI模型则直接提交到系统
    if (no_cadidates(api)) {
        commit_rime_preedit(api);
        return state->ai_mode ? IR_TYPING : IR_COMMIT;
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


TransputSchemaList transput_get_schema_list (TransputApi *api) {
    //TODO: 从rime查询方案列表
    TransputSchemaList result;
    return result; 
}