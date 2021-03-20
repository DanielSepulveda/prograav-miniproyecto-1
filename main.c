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



int findIndex(char **strArray, int size, char *str) {
  for(int i = 0; i < size; i++){
    if(!strcmp(strArray[i], str)) return i;
  }
  // if string not found in array
  return -1;
}

char **append(char **oldMatrix, int *size, const char str[1024]) {
  int strFound = 0;
  for(int i = 0; i < *size; i++){
    if(!strcmp(oldMatrix[i], str)){
      strFound = 1;
      break;
    }
  }
  if(!strFound){
    char **newMatrix = (char **)realloc(oldMatrix, (*size + 1) * sizeof(char *));
    newMatrix[*size] = (char *)malloc(1024 * sizeof(char));
    strcpy(newMatrix[*size], str);
    (*size)++;
    return newMatrix;
  }
  return oldMatrix;
}

void readRecipes(FILE *ptrFile, RecipesMatrix *recipesMatrix) {
  // for file reading 
  int percentage, currPercentage, currRecipe, index;
  char word[1024], delim[2] = ":", *token, c;

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
      if(!strcmp(token, "Recipe")) {
        // allocate recipes names with spaces
        c = getc(ptrFile);
        while(c != '\n') {
          strncat(token, &c, 1);
          c = getc(ptrFile);
        }
        (*recipesMatrix).recipes = append((*recipesMatrix).recipes, &(*recipesMatrix).recipesSize, token);
      }
      // if the string "Ingredients" is found before ":"...
      if(!strcmp(word, "Ingredients")) {
        percentage = 0;
        // TODO: add a condition for percentages exceding 100%
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

    // jump to BOF to fill percentages matrix
    
    rewind(ptrFile);

    // allocate percentages matrix
    (*recipesMatrix).percentages = (int **)malloc((*recipesMatrix).recipesSize * sizeof(int *));
    for(int i = 0; i < (*recipesMatrix).recipesSize; i++){
      (*recipesMatrix).percentages[i] = (int *)malloc((*recipesMatrix).ingredientsSize * sizeof(int));
      for(int j = 0; j < (*recipesMatrix).ingredientsSize; j++) (*recipesMatrix).percentages[i][j] = 0;
    }
    
    currRecipe = 0;
    while(!feof(ptrFile)) {
      fscanf(ptrFile, "%1023s", word);
      token = strtok(word, delim);
      if(!strcmp(word, "Ingredients")) {
        percentage = 0;
        //currPercentage = 0;
        // TODO: add a condition for percentages exceding 100%
        while(percentage < 100) {
          fscanf(ptrFile, "%1023s", word);
          // separate strings using ":" as delimiter
          token = strtok(word, delim);
          index = findIndex((*recipesMatrix).ingredients, (*recipesMatrix).ingredientsSize, token);
          // obtain the second token from "ingredient name:percentage"
          token = strtok(NULL, delim);
          // insert percentage in percentages matrix
          (*recipesMatrix).percentages[currRecipe][index] = atoi(token);
          printf("%d ", (*recipesMatrix).percentages[currRecipe][index]);
          // cast token from str to int
          percentage += atoi(token);
          //currPercentage++;
        }
        currRecipe++;
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

  printf("Recipes:\n");
  for(int i = 0; i < matrix.recipesSize; i++) printf("%d. %s\n", i + 1, matrix.recipes[i]);
  
  printf("Ingredients:\n");
  for(int i = 0; i < matrix.ingredientsSize; i++) printf("%d. %s\n", i + 1, matrix.ingredients[i]);
  
  printf("Matrix dims: %d %d\n", matrix.recipesSize, matrix.ingredientsSize);
  for(int i = 0; i < matrix.recipesSize; i++){
    for (int j = 0; j < matrix.ingredientsSize; j++) printf("%d ", matrix.percentages[i][j]);
    printf("\n");
  }

}
