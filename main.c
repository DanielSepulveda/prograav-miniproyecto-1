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

void readRecipes(FILE *ptrFile, RecipesMatrix *recipesMatrix) {
  int percentage;
  char word[1024], delim[2] = ":", *token;
  (*recipesMatrix).recipes = (char **)malloc(sizeof(char *));
  (*recipesMatrix).ingredients = (char **)malloc(sizeof(char *));
  (*recipesMatrix).recipesSize = 0;
  (*recipesMatrix).ingredientsSize = 0;

  if(ptrFile != NULL) {
    printf("File succesfully opened\n");

    while(!feof(ptrFile)) {
      fscanf(ptrFile, "%1023s", word);
      // separate strings using ":" as delimiter
      token = strtok(word, delim);
      // if the string "Recipe" is found before ":"...
      if(strcmp(token, "Recipe") == 0) {
        (*recipesMatrix).recipesSize++;
      }
      // if the string "Ingredients" is found before ":"...
      if(strcmp(word, "Ingredients") == 0) {
        percentage = 0;
        while(percentage < 100) {
          fscanf(ptrFile, "%1023s", word);
          // separate strings using ":" as delimiter
          token = strtok(word, delim);
          (*recipesMatrix).ingredients = append((*recipesMatrix).ingredients, &((*recipesMatrix).ingredientsSize), token);
          // obtain the second token from "ingredient name:percentage"
          token = strtok(NULL, delim);
          // cast token from str to int
          percentage += atoi(token);
        }
      } 
    }
  }
  else {
    printf("Error while opening file\n");
  }

}

int main (void) {

  FILE *ptrFile = fopen("Recipes.txt", "r");
  RecipesMatrix matrix;
  readRecipes(ptrFile, &matrix);
  
  for(int i = 0; i < matrix.ingredientsSize; i++) printf("%d. %s\n", i + 1, matrix.ingredients[i]);
  printf("\n%d %d\n", matrix.recipesSize, matrix.ingredientsSize);
  
}
