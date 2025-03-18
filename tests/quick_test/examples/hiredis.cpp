#include <hiredis/hiredis.h>
#include <iostream>

struct Data{
    double field_1 = 1.2;
    char field_2[5] = "hiy";
};

int main() {
    redisContext *c = redisConnect("127.0.0.1", 6380);
    if (c == nullptr || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
            // handle error
        } else {
            printf("Can't allocate redis context\n");
        }
    }

    Data d{3.5};
    char* data_bytes = reinterpret_cast<char*>(&d);
    redisReply *reply;
//    reply = static_cast<redisReply*>(redisCommand(c, "SET test %s", "some_string"));
//    reply = static_cast<redisReply*>(redisCommand(c, "SET test %b", data_bytes, sizeof(Data)));
    reply = static_cast<redisReply *>(redisCommand(c, "GET test_2"));
    auto d2 = reinterpret_cast<Data*>(reply->str);
    std::cout << d2->field_1 << std::endl;
    std::cout << d2->field_2 << std::endl;
    freeReplyObject(reply);
    reply = static_cast<redisReply*>(redisCommand(c, "SUBSCRIBE channel_1"));
//    channel_1
    freeReplyObject(reply);
    std::cout << "init" << std::endl;
    int count{0};
    while(redisGetReply(c, reinterpret_cast<void**>(&reply)) == REDIS_OK) {
        // consume message
        std::cout << "here" << std::endl;
        std::cout << reply->element[2]->str << std::endl;

        freeReplyObject(reply);
        count++;
        if (count == 10) break;
    }
    redisFree(c);

    return 0;
}


//#include <hiredis/hiredis.h>
//#include <stdio.h>
//#include <stdlib.h>
//
//int main(void) {
//    redisContext *context = redisConnect("127.0.0.1", 6380);
//
//    redisReply *reply = static_cast<redisReply*>(redisCommand(context,"SUBSCRIBE channel_1"));
//    if (!reply || context->err) {
//        fprintf(stderr, "Error:  Cannot send SUBSCRIBE!\n");
//        exit(-1);
//    }
//
//    printf("Subscribed to channel 'foo', ready to get messages!\n");
//    freeReplyObject(reply);
//
//    while(redisGetReply(context,(void**)&reply) == REDIS_OK) {
//        if (reply->type != REDIS_REPLY_ARRAY || reply->elements != 3) {
//            fprintf(stderr, "Error:  Malformed subscribe response!\n");
//            exit(-1);
//        }
//
//        /* Print our message */
//        printf("Received message: %s\n", reply->element[2]->str);
//
//        freeReplyObject(reply);
//    }
//}