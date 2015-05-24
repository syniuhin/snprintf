#include "snprintf.h"

#include <string.h>
#include <stdio.h>
#include <float.h>

void test_parse();
void test_snprintf();

int main(void){
    test_snprintf();
    return 0;
}

//void test_parse(){
//    int n = 5;
//    char** test = calloc(n, sizeof(char*));
//    for (int i = 0; i < n; ++i)
//        test[i] = calloc(n, sizeof(char*));
//    strcpy(test[0], "%d");
//    strcpy(test[1], "%+10.10d");
//    strcpy(test[2], "%.3f");
//    strcpy(test[3], "%.3q");
//    strcpy(test[4], "%+1J.10d");
//
//    char flag = 0;
//    int width = 0;
//    int precision = 0;
//    char type = 0;
//    printf("\nparse() tests started...\n");
//    for (int i = 0; i < n; ++i){
//        printf("parsing \"%s\" code: %d ", test[i], parse(test[i],
//                   &flag, &width, &precision, &type));
//        printf("flag: %c width: %d precision: %d type: %c\n",
//                (flag)
//                    ? flag
//                    : '_', width, precision,
//                (type > 0 && type < 127)
//                    ? type
//                    : ((type == 0) ? '_' : '@'));
//    }
//
//    for (int i = 0; i < n; ++i)
//        free(test[i]);
//    free(test);
//}


void test_snprintf(){
    printf("my_snprintf() test started...\n");
    const size_t str_size = 1000;

    char str[str_size];
    char std_str[str_size];
    int test_res = 0;

    const size_t test_count = 10;
    char formats[][str_size] = {
        "%d + %d == %d",
        "%.5d + %.3d == %.10d",
        "%.10f + %10.3f == %.3f",
        "\"%c%c%c\" should be equals \"%s\"",
        "%.3f + %.3f == %.3f",
        "%.32f + %.32f == %.32f"
    };

    my_snprintf(str, str_size, formats[0], 1, 2, 3);
    snprintf(std_str, str_size, formats[0], 1, 2, 3);
    printf("%s\n", (test_res = strcmp(std_str, str) == 0)
            ? "1: PASSED" : "1: FAILED");
    if (!test_res)
        printf("%s\n%s\n", std_str, str);
//    printf("%s\n", str);

    my_snprintf(str, str_size, formats[1], 7, 8, 3);
    snprintf(std_str, str_size, formats[1], 7, 8, 3);
    printf("%s\n", (test_res = strcmp(std_str, str) == 0)
            ? "2: PASSED" : "2: FAILED");
    if (!test_res)
        printf("%s\n%s\n", std_str, str);

    my_snprintf(str, str_size, formats[2], 1.001000126, 3.6765, 10.775768);
    snprintf(std_str, str_size, formats[2], 1.001000126, 3.6765, 10.775768);
    printf("%s\n", (test_res = strcmp(std_str, str) == 0)
            ? "3: PASSED" : "3: FAILED");
    if (!test_res)
        printf("%s\n%s\n", std_str, str);

    my_snprintf(str, str_size, formats[3], 'a', 'b', 'c', "abc");
    snprintf(std_str, str_size, formats[3], 'a', 'b', 'c', "abc");
    printf("%s\n", (test_res = strcmp(std_str, str) == 0)
            ? "4: PASSED" : "4: FAILED");
    if (!test_res)
        printf("%s\n%s\n", std_str, str);

    my_snprintf(str, str_size, formats[4], 999.9999, 1.0, 2.0);
    snprintf(std_str, str_size, formats[4], 999.9999, 1.0, 2.0);
    printf("%s\n", (test_res = strcmp(std_str, str) == 0)
            ? "5: PASSED" : "5: FAILED");
    if (!test_res)
        printf("%s\n%s\n", std_str, str);

    my_snprintf(str, str_size, formats[5], 909.9996, 1.0, 2.0);
    snprintf(std_str, str_size, formats[5], 909.9996, 1.0, 2.0);
    printf("%s\n", (test_res = strcmp(std_str, str) == 0)
            ? "6: PASSED" : "6: FAILED");
    if (!test_res)
        printf("%s\n%s\n", std_str, str);


    /*
    int byteLimit = 200;
    my_snprintf(str, byteLimit, "Trying to append only"
            " %d bytes: %s", byteLimit, "blah blah blah abc defg h");
    printf("%s\n", str);

    my_snprintf(str, str_size, "%s", "lold");
    printf("%s\n", str);

    int ubiq_num = 123123123;
    my_snprintf(str, str_size, "DEC: %d", ubiq_num);
    printf("%s\n", str);
    my_snprintf(str, str_size, "OCT: %O", ubiq_num);
    printf("%s\n", str);
    my_snprintf(str, str_size, "HEX: %X", ubiq_num);
    printf("%s\n", str);
    my_snprintf(str, str_size, "hex: %x", ubiq_num);
    printf("%s\n", str);
    */
}
