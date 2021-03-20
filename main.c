#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG0
#define DEBUG1

typedef struct recipesMatrix RecipesMatrix; 

struct recipesMatrix {
  char **recipes;
  int recipesSize;
  char **ingredients;
  int ingredientsSize;
  int **percentages; 
};

char **append(char **oldMatrix, int *size, const char str[1024]) {
  int strFound = 0;
  for(int i = 0; i < *size; i++){
    if(strcmp(oldMatrix[i], str) == 0){
      strFound = 1;
      break;
    }
  }
  if(!strFound){
    char **newMatrix = (char **)realloc(oldMatrix, (*size + 1)*sizeof(char *));
    newMatrix[*size] = (char *)malloc(1024*sizeof(char));
    strcpy(newMatrix[*size], str);
    (*size)++;
    return newMatrix;
  }
  return oldMatrix;
}

int main (void) {

  // for file reading
  int percentage;
  char word[1024], delim[2] = ":", *token;

  char **ingredients = (char **)malloc(sizeof(char *));
  int ingredientsSize = 0;
  char **recipes = (char **)malloc(sizeof(char *));
  int recipesSize = 0;

  int matrixSize = 0;

  FILE *ptrFile = fopen("Recipes.txt", "r");
  
  if(ptrFile != NULL) {
    printf("File succesfully opened\n");

    while(!feof(ptrFile)) {
      fscanf(ptrFile, "%1023s", word);
      // separate strings using ":" as delimiter
      token = strtok(word, delim);
      // if the string "Recipe" is found before ":"...
      if(strcmp(token, "Recipe") == 0) {
        recipes++;
      }
      // if the string "Ingredients" is found before ":"...
      if(strcmp(word, "Ingredients") == 0) {
        percentage = 0;
        while(percentage < 100) {
          fscanf(ptrFile, "%1023s", word);
#ifdef DEBUG1
          printf("%s\n", word);          
#endif // DEBUG
          // separate strings using ":" as delimiter
          token = strtok(word, delim);
          ingredients = append(ingredients, &ingredientsSize, token);
          // obtain the second token from "ingredient name:percentage"
          token = strtok(NULL, delim);
          // cast token from str to int
          percentage += atoi(token);
        }
#ifdef DEBUG0
        if(percentage == 100) printf("--------\n");
#endif // DEBUG
      } 
    }
  }
  else {
    printf("Error while opening file\n");
  }

  for(int i = 0; i < ingredientsSize; i++) printf("%d. %s\n", i + 1, ingredients[i]);
  printf("\n%d %d\n", recipesSize, ingredientsSize);
  
}
