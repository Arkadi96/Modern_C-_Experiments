/*
 *  This module was created just to experiment with the pointers
 *  to understand underneath logic.
 */

#include <cstdlib>
#include <stdio.h>

// We all know the real memcpy interface
// here the things are bit different
size_t memcpy(const void* const s, const void* d, const size_t l)
{
    if (s == d) {
        return 0;
    }
    for (size_t i = 0; i < l; ++i) {
        ((char *)d)[i] = ((char *)s)[i];
    }
    return 1;
}

int main()
{
    const size_t l = 5;
    const char ss[] = "hello world!";
    const char* const s = ss;
    const char* d = (char*)malloc(l * sizeof(char));
    if (!memcpy(s, d, l)) {
        printf("\nCould not copy to the same address");
    }
    printf("\nMemcpy worked Successfully");
    printf("\nCopied value is: ");
    printf("%s\n", d);
    return 0;
}
