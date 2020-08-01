#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../common/constants.cpp"

using namespace std;

struct args {
    const char *command;
    char *key;
    char *value;
    const char *result_value;
    const char *response;
};

void *convert_xml_to_raw(char *xml, struct args *reqArgs) {
    char *token = strtok(xml, "\n");
    token = strtok(NULL, "\n");
    char *type = (token + 11);
    if (strcmp(type, "type=\"getreq\">") == 0) {
        reqArgs->command = "get";
        char *key = strtok(NULL, "\n");
        key += 5;
        int i = 0;
        while (*(key + i) != '<') {
            i++;
        }
        reqArgs->key = (char *) malloc(i + 1);
        strncpy(reqArgs->key, key, i);
        reqArgs->key[i] = '\0';
    } else if (strcmp(type, "type=\"putreq\">") == 0) {
        reqArgs->command = "put";
        char *key = strtok(NULL, "\n");
        key += 5;
        int i = 0;
        while (*(key + i) != '<') {
            i++;
        }
        reqArgs->key = (char *) malloc(i + 1);
        strncpy(reqArgs->key, key, i);
        reqArgs->key[i] = '\0';
        //puts("end of sxml");
        //Value
        char *value = strtok(NULL, "\n");
        value += 7;
        i = 0;
        while (*(value + i) != '<') {
            i++;
        }
        reqArgs->value = (char *) malloc(i + 1);
        strncpy(reqArgs->value, value, i);
        reqArgs->value[i] = '\0';

    } else if (strcmp(type, "type=\"delreq\">") == 0) {
        reqArgs->command = "del";
        char *key = strtok(NULL, "\n");
        key += 5;
        int i = 0;
        while (*(key + i) != '<') {
            i++;
        }
        reqArgs->key = (char *) malloc(i + 1);
        strncpy(reqArgs->key, key, i);
        reqArgs->key[i] = '\0';
    }

    return reqArgs;
}


char *GET_RES(const char *key, const char *value) {
    char p1[MAX_BUFFER_SIZE+GEN_BUFFER_SIZE] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                               "<KVMessage type=\"resp\">\n"
                                                               "<Key>";
    strcat(p1, key);

    char p2[] = "</Key>\n"
                "<Value>";
    strcat(p1, p2);
    strcat(p1, value);
    char p3[GEN_BUFFER_SIZE] = "</Value>\n"
                "</KVMessage>\0";
    strcat(p1, p3);
    return strdup(p1);
}

char *PUT_RES() {
    char p1[GEN_BUFFER_SIZE] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                   "<KVMessage type=\"resp\">\n"
                                   "<Message>success</Message>\n"
                                   "</KVMessage>\0";
    return strdup(p1);
}

char *DEL_RES() {
    char p1[GEN_BUFFER_SIZE] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                               "<KVMessage type=\"resp\">\n"
                               "<Message>success</Message>\n"
                               "</KVMessage>\0";
    //strcat(p1, "success");
    //char p2[GEN_BUFFER_SIZE] = "</Message>\n"
    //            "</KVMessage>\0";
    //strcat(p1, p2);
    return strdup(p1);
}

char *ERROR_RES(const char *error) {
    char p1[GEN_BUFFER_SIZE + strlen(error)] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                               "<KVMessage type=\"resp\">\n"
                                               "<Message>error,";
    strcat(p1, error);
    char p2[] = "</Message>\n"
                "</KVMessage>\0";
    strcat(p1, p2);
    return strdup(p1);
}
