#include <stdio.h>
#include "transput.h"
#include <unistd.h>

int main(int argc, char** argv) {
    TransputApi *api = create_transput_api();
    TransputProperties props;
    props.log_dir = "/Users/jinjunjie/github/Transput-Backend/log";
    props.user_dir= "/Users/jinjunjie/github/Transput-Backend/user_data";
    props.shared_dir = "/Users/jinjunjie/github/Transput-Backend/shared";

    //切换工作目录
    chdir(props.shared_dir);
    api->start(api, &props);
    TransputSessionId sid = api->new_session(api);
    while (sid == 0) {
        sid = api->new_session(api);
    }
    printf("session id: %d\n", sid);
    enum InputResultType result = api->process_key(api, sid, 'a');
    printf("input result: %d\n", result);
    TransputCadidates* cadidates = api->get_cadidates(api, sid);
    printf("cadidates size: %d\n", cadidates->size);
    for(int i = 0; i < cadidates->size; i++) {
        printf("hello world");
        printf("%s\n", cadidates->cadidates[i]);
    }
    printf("session id: %d\n", sid);
    return 0;
}
