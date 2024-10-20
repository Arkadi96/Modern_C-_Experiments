#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/* Globals */
#define LINE_LENGTH 16 // hexdump default line length in bytes
#define LINE_END 10    // ASCII line feed

void hexdumpPrint(char fb, char sb) {
        printf("%x%x", ((sb & 0xF0) >> 4), sb & 0x0F);
        printf("%x%x", ((fb & 0xF0) >> 4), fb & 0x0F);
}
int main() {
    char s[] = "123456asdsadasdasdooiqwxckj788921*&665w%A&^S7  asd9[p";
    size_t arr_size = sizeof(s) - 1; // Without considering the end of the line '\0'

    char fb, sb;
    for (int i = 0; i < arr_size - 1; i += 2) {
        fb = s[i];
        sb = s[i+1];
        hexdumpPrint(fb, sb);
        ((i > 0) && ((i + 2) % LINE_LENGTH) == 0) ? printf("\n") : printf(" ");
    }

    // When we have odd array
    if (arr_size % 2) {
        fb = s[arr_size - 1];
        sb = LINE_END;
    } else {
        fb = LINE_END;
        sb = 0;
    }
    hexdumpPrint(fb, sb);
    printf("\n");

    return 0;
}
