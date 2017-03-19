// this C++ code is transpiled from Pinecone
// Pinecone v0.3.0 was used

#include <stdio.h>
int getIntInput();

int getIntInput()
{
    int tmp;
    scanf("%d", &tmp);
    return tmp;
}

int main()
{
    // list
    {
        int a;
        a = (getIntInput());
        printf("%d\n", ((a) + (3)));
    }
}

