#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "spell.h"

/*  Suggestions
- When you work with 2D arrays, be careful. Either manage the memory yourself, or
work with local 2D arrays. Note C99 allows parameters as array sizes as long as 
they are declared before the array in the parameter list. See: 
https://www.geeksforgeeks.org/pass-2d-array-parameter-c/

Worst case time complexity to compute the edit distance from T test words
 to D dictionary words where all words have length MAX_LEN:
Student answer:  Theta( T*D*(MAX_LEN^2) )

*/


/* Write helper functions here */
int minVal(int a, int b, int c) {
  if((a <= b) && (a <= c)) {
    return a;
  }
  else if((b <= a) && (b <= c)) {
    return b;
  }
  else {
    return c;
  }
}
/*
Parameters:
  - first_string - pointer to the first string (displayed vertical in the table)
  - second_string - pointer to the second string (displayed horizontal in the table)
  - print_table - If 1, the table with the calculations for the edit distance will be printed.
                  If 0 (or any value other than 1)it will not print the table
Return:  the value of the edit distance
*/
int edit_distance(char * first_string, char * second_string, int print_table){
  int w1 = strlen(first_string);
  int w2 = strlen(second_string);

  // + 1 for /0 chr
  int a[w1 + 1][w2 + 1];

  for(int i = 0; i <= w1; i++) {
    for(int j = 0; j <= w2; j++) {
      if(i == 0 ) {
        a[i][j] = j;
      }
      else if (j == 0) {
        a[i][j] = i;
      }
      else if (first_string[i - 1] == second_string[j - 1]) {
        a[i][j] = a[i - 1][j - 1];
      }
      else {
        a[i][j] = 1 + minVal(a[i][j - 1], a[i - 1][j], a[i - 1][j - 1]);
      }
    }
  }
  if(print_table) {
    int count = 1;
    printf("\n");
    printf("  |");
    printf("    |");

    //second word thats on top
    for (int j = 0; j < w2; j++) {
      printf(" %3c|", second_string[j]);
    }
    printf("\n");
    printf("---");
    for (int j = 0; j < w2 + count; j++) {
      printf("-----");
    }
    printf("\n");
    for (int i = 0; i <= w1; i++) {
      if (i == 0) {
        printf("  |");
      }
      else {
        printf(" %c|", first_string[i - 1]);
      }
      for(int j = 0; j <= w2; j++) {
        printf(" %3d|", a[i][j]);
      }
      printf("\n");
      printf("---");

      for(int j = 0; j < w2 + count; j++) {
        printf("-----");
      }
      printf("\n");
    }
    printf("\n");
  }
  return a[w1][w2];
}
/*
Parameters:
  - testname - string containing the name of the file with the paragraph to spell check, includes file extenssion e.g. "text1.txt" 
  - dictname - name of file with dictionary words. Includes file extenssion, e.g. "dsmall.txt"
Behavior: If any of the files cannot be open displays a message and returns. (Does not exit the program.)
*/
void spell_check(char *testname, char *dictname) {
  FILE *testFile = fopen(testname, "r");
  FILE *dictFile = fopen(dictname, "r");

  if(testFile == NULL || dictFile == NULL) {
    if(testFile != NULL) {
      fclose(testFile);
    }
    else if(dictFile != NULL) {
      fclose(dictFile);
    }
    return;
  }
   //Size determination of file: 
  int d_size;

  fscanf(dictFile, "%d", &d_size);
  char **d_str = malloc(d_size * sizeof(char *));
  if(d_str == NULL) {
    printf("Memory allocation failed for dictionary strings.\n");
    fclose(testFile);
    fclose(dictFile);
    return;
  }
  for(int i = 0; i < d_size; i++) {
    d_str[i] = malloc(1005 * sizeof(char));
    if(d_str[i] == NULL) {
      for(int j = 0; j < i; j++) {
        free(d_str[j]);
      }
      free(d_str);
      fclose(testFile);
      fclose(dictFile);
      return;
    }
    fscanf(dictFile, "%s", d_str[i]);
    }

  fclose(dictFile);
  printf("\nLoaded dictionary %s has size: %d\n\n", dictname, d_size);

  //minimum distance calculation + prompt:
  int t_size;
  char t_str[1005 + 1];
  fscanf(testFile, "%d", &t_size);

  while((fscanf(testFile, "%s", t_str) != EOF)) {
    printf("---> |%s|\n", t_str);

    int *dist = malloc(d_size * sizeof(int));
    int min_distance = INT_MAX;

    for(int i = 0; i < d_size; i++) {
      dist[i] = edit_distance(t_str, d_str[i], 0);
      if(dist[i] < min_distance) {
        min_distance = dist[i];
      }
    }
    printf("-1 - type correction\n");
    printf(" 0 - leave word as it is (do not fix spelling)\n");
    printf("     Minimum distance: %d\n", min_distance);
    printf("     Words that give minimum distance:\n");

    int count = 1;
    //index to hold the min distance words

    int min_distance_index[d_size];
    int num_min_distance = 0;
    for(int i = 0; i < d_size; i++) {
      if(dist[i] == min_distance) {
        min_distance_index[num_min_distance++] = i;
        printf("%d - %s\n", count, d_str[i]);
        count++;
      }
    }
    free(dist);

    //Choosing the different options:
    int input;
    printf("Enter your choice (from the above options): ");
    if(scanf("%d", &input) != 1) {
      int c;
      while ((c = getchar()) != '\n' && c != EOF);
      printf("Invalid input. Please enter a valid integer.\n");
    }

    if(input == -1) {
      char correction[1005 + 1];
      printf("Enter the correct word: ");
      fgets(correction, sizeof(correction), stdin);
      correction[strcspn(correction, "\n")] = 0;
      strcpy(t_str, correction);
    }
    else if (input >= 1 && input <= d_size) {
      int index = min_distance_index[input -1];
      strcpy(t_str, d_str[index]);
    }
    else if (input == 0) {
      printf("The original word will be kept.\n");
    }
    else {
      printf("Invalid choice. Original word will be kept.\n");
    }
    printf("The corrected word is: %s\n", t_str);
    printf("\n");
  }
  fclose(testFile);

  for(int i = 0; i< d_size; i++) {
    free(d_str[i]);
  }
  free(d_str);
}