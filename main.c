#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    int numbOfModules, ND[100], NU[100], NT[100], rellocationConstant = 0;
    int ND_R[100], NU_R[100], word, R_NU, NT_word[100], stR[100], result[100];
    int countD = 0, countT = 0, countU = 0, c, index, index1, counter = 0, numb, index2,defined = 0, used = 0, countNew = 0;
    char ND_S[100][5], NU_S[100][5], NT_type[100][5], stS[100][5];

    //read in values
    printf("Enter input set\n");
    scanf("%d", &numbOfModules);

    for (int m = 0; m < numbOfModules; m++) {

        //read in definition list
        //symbols are at most 5 letters long
        //at most 100 symbols can be defined
        scanf("%d", &ND[m]);
        if (ND[m] != 0){
            for(int i = 0; i< ND[m]; i++){
                scanf("%5s", ND_S[countD]);
                scanf("%d", &ND_R[countD]);
                countD += 1;
            }
        }
        else{
            strcpy(ND_S[countD], "0");
            ND_R[countD] = -1;
            countD += 1;
        }



        //read in use list
        //symbols are at most 5 letters long
        //at most 100 symbols can be used
        scanf("%d", &NU[m]);
        if (NU[m] != 0) {
            for (int i = 0; i < NU[m]; i++) {
                scanf("%5s", &NU_S[countU]);
                scanf("%d", &R_NU);
                NU_R[countU] = R_NU;
                countU += 1;
            }
        }
        else{
            strcpy(NU_S[countU], "0");
            NU_R[countU] = -1;
            countU += 1;
        }

        //read in program text
        //type at most 5 letters
        //at most 100 words can be read in
        scanf("%d", &NT[m]);
        if (NT[m] != 0) {
            for (int i = 0; i < NT[m]; i++) {
                scanf(" %5s", &NT_type[countT]);
                scanf("%d", &word);
                NT_word[countT] = word;
                countT += 1;
            }

        }

    }



    // first pass
    // objective: generate symbol table
    countD = 0;
    c = 0;
    for (int m= 0; m < numbOfModules; m++){
        if (ND[m] == 0){
            counter += 1;
            countD += 1;
        }
        else {
            counter += ND[m];
            while(countD < counter){
                strcpy(stS[c], ND_S[countD]);
                stR[c] = ND_R[countD] + rellocationConstant;
                c += 1;
                countD += 1;
            }
        }
        rellocationConstant += NT[m];

    }

    //Warning for symbols defined but never used
    for (int i = 0; i < c; i++){
        for (int j = 0; j < countU; j++){
            if (strcmp(stS[i], NU_S[j]) == 0){
                used = 1;
                break;
            }
        }
        if (used == 0){
            printf("Warning: %s was defined but never used.\n", stS[i]);
        }
        used = 0;

    }

    //Error check for multiply defined symbols
    for (int i=0; i< c; i++){
        for(int j=0; j<c; j++){
            if((i != j) && (strcmp(stS[i], stS[j]) == 0)){
                printf("Error: %s is multiply defined\n", stS[j]);
                for(int k=i+1; k<c;k++){
                    countNew += 1;
                    strcpy(stS[k-1], stS[k]);
                    stR[k-1] = stR[k];
                }
            }
        }
    }



    //second pass
    //objective: generate memory map
    rellocationConstant = 0;
    counter = 0;
    countT = 0;
    countU = 0;
    index = 0;

    for (int m = 0; m < numbOfModules; m++){
        if (NU[m] == 0){
            counter += 1;
            countU += 1;
        }
        else{
            counter += NU[m];


            while(countU < counter){
                index1 = NU_R[countU];


                //Error check for symbols used but not defined
                for (int i = 0; i< c; i++){
                    if(strcmp(stS[i], NU_S[countU]) == 0){
                        defined += 1;
                        numb = stR[i];

                    }
                }
                if (defined == 0){
                    printf("Error: %s is not defined; 111 used\n", NU_S[countU]);
                    numb = 111;
                }


                if ((NT_word[index1+rellocationConstant] % 1000) == 777){
                    result[index1+rellocationConstant] = ((NT_word[index1+rellocationConstant] / 1000) * 1000) + numb;
                }

                else{
                    index2 = index1;
                    while(NT_word[index2+rellocationConstant] % 1000 != 777){
                        result[index2+rellocationConstant] = ((NT_word[index1+rellocationConstant] / 1000) * 1000) + numb;
                        index2 = NT_word[index2+rellocationConstant] % 1000;
                    }
                   result[index2+rellocationConstant] = ((NT_word[index2+rellocationConstant] / 1000) * 1000) + numb;
                }


                countU += 1;
            }

        }

        index += NT[m];
        while(countT < index){
            if(strcmp(NT_type[countT], "R") == 0){
                //Error check for type R address exceeding module size
                if ((NT_word[countT] % 1000) > NT[m]){
                    printf("Error: Type R address exceeds module size; 0 (relative) used\n");
                    result[countT] = ((NT_word[countT] / 1000) * 1000) + rellocationConstant;
                }
                else{
                    result[countT] = NT_word[countT] + rellocationConstant;
                }

            }
            //Error check for absolute address exceeding size of machine
            else if(strcmp(NT_type[countT], "A") == 0){
                if ((NT_word[countT] % 1000) >= 300){
                    printf("Error: A type address exceeds machine size; max legal value used\n");
                    result[countT] = ((NT_word[countT] / 1000) * 1000) + 299;
                }
                else{
                    result[countT] = NT_word[countT];
                }
            }
            else if(strcmp(NT_type[countT], "I") == 0){
                result[countT] = NT_word[countT];
            }
            countT += 1;
        }
        rellocationConstant += NT[m];

    }

    //print symbol table
    printf("\nSymbol Table\n");
    for(int i = 0; i < c-countNew; i++){
        printf("%s = %d\n", stS[i], stR[i]);
    }


    //print memory map
    printf("\nMemory Map\n");
    for (int i = 0; i< countT; i++){
        printf("%d: %d\n", i, result[i]);
    }





    return 0;
}





