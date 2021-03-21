#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define PRLN printf("\n");

/**
 * Ingredient Struct
 *  name: string
 *  percentage: int
 */
typedef struct IngredientsStruct {
  char name[1024];
  int percentage;
} Ingredient;

/**
 * Recipes Struct 
 *  name: string
 *  ingredients: Ingredient[]
 *  ingredientsSise: number
 *  completeIngredients: Ingredient[]
 *  completeIngredientsSize: number
 */
typedef struct RecipesStruct {
  char name[1024];
  Ingredient* ingredients[100];
  Ingredient* completeIngredients[100];
  int ingredientsSize;
  int completeIngredientsSize;
} Recipe;

/**
 * FileInfo Struct
 *  recipes: Recipe[]
 *  ingredients: string[]
 *  ingredientsSize: number
 */
typedef struct FileInfoStruct {
  Recipe* recipes[100];
  int recipesSize;
  char **ingredients;
  int ingredientsSize;
} FileInfo;

/**
 * Append a new string value to arr if it is not already in
 * the arr.
 * 
 * @param oldMatrix {string[]}
 * @param sise {int*}
 * @param str {string}
 * 
 * @returns string[] with new value
 */
char **append(char **oldMatrix, int *size, const char str[1024]) {
  for(int i = 0; i < *size; i++){
    if(strcmp(oldMatrix[i], str) == 0){
      return oldMatrix;
    }
  }

  char **newMatrix = (char **)realloc(oldMatrix, (*size + 1)*sizeof(char *));
  newMatrix[*size] = (char *)malloc(1024*sizeof(char));
  strcpy(newMatrix[*size], str);
  (*size)++;

  return newMatrix;
}

/**
 * String comparison function used with qsort
 * 
 * @param a {string}
 * @param b {string}
 * 
 * @returns strcmp result
 */
static int stringCompare(const void* a, const void* b) { 
  return strcmp(*(const char**)a, *(const char**)b); 
} 

/**
 * Ingredient comparison function used with qsort
 * 
 * @param a {Ingredient*}
 * @param b {Ingredient*}
 * 
 * @returns strcmp result
 */
static int ingredientCompare(const void *a, const void *b) {
  const Ingredient *i1 = (Ingredient *)a;
  const Ingredient *i2 = (Ingredient *)b;

  return strcmp(*(const char**)i1->name, *(const char**)i2->name);
}

/**
 * Read text file and fill file info struct. This function will
 * exit the program if there is an error with the file or if any
 * of the recipes exceeds 100% of ingredients.
 * 
 * @param file {string}
 * 
 * @returns FileInfo struct with the info.
 */
FileInfo readFileInfo(char* file) {
  int percentage;
  int MAX_LINE = 1024;
  char line[MAX_LINE], delim[2] = ":", *token;

  FileInfo fileInfo = {
    .ingredients = (char **)malloc(sizeof(char *)),
    .ingredientsSize = 0,
    .recipesSize = 0
  };

  FILE *ptrFile = fopen(file, "r");

  if(ptrFile != NULL) {
    while(!feof(ptrFile)) {
      fgets(line, MAX_LINE, ptrFile);

      line[strcspn(line, "\n")] = 0;
      // separate strings using ":" as delimiter
      token = strtok(line, delim);

      // if the string "Recipe" is found before ":"...
      if(strcmp(token, "Recipe") == 0) {
        token = strtok(NULL, "");
        // Create new recipe
        Recipe* newRecipe = (Recipe*)malloc(sizeof(Recipe));
        strcpy(newRecipe->name, token);
        newRecipe->ingredientsSize = 0;
        newRecipe->completeIngredientsSize = 0;
        fileInfo.recipes[fileInfo.recipesSize] = newRecipe;
      }

      // if the string "Ingredients" is found before ":"...
      if(strcmp(token, "Ingredients") == 0) {
        percentage = 0;

        Recipe* currRecipe = fileInfo.recipes[fileInfo.recipesSize];

        // Loop over all ingredients
        while(percentage < 100) {
          fgets(line, MAX_LINE, ptrFile);
          line[strcspn(line, "\n")] = 0;

          char *ing, *perc;
          // separate strings using ":" as delimiter
          ing = strtok(line, delim);
          perc = strtok(NULL, "");

          fileInfo.ingredients = append(fileInfo.ingredients, &fileInfo.ingredientsSize, ing);

          // Add new ingredient to current recipe
          Ingredient* newIngredient = (Ingredient*)malloc(sizeof(Ingredient));
          strcpy(newIngredient->name, ing);
          newIngredient->percentage = atoi(perc);
          currRecipe->ingredients[currRecipe->ingredientsSize++] = newIngredient;

          percentage += atoi(perc);
        }

        // Check that the percentage is correct
        if(percentage == 100) {
          qsort(currRecipe->ingredients, currRecipe->ingredientsSize, sizeof(Ingredient*), ingredientCompare);
          fileInfo.recipesSize++;
        } else {
          printf("%s exceeds the ingredients percentage\n", currRecipe->name);
          exit(1);
        }
      }
    }
  } else {
    printf("Error while opening file\n");
    exit(1);
  }

  fclose(ptrFile);

  qsort(fileInfo.ingredients, fileInfo.ingredientsSize, sizeof(const char*), stringCompare);

  return fileInfo;
}

/**
 * Fills the recipes ingredients arr with any missing ingredients.
 * 
 * @param fileInfo {FileInfo*}
 * 
 * @returns updated fileInfo 
 */
FileInfo fillDims (FileInfo* fileInfo) {
  FileInfo newFileInfo = *fileInfo;

  for (int i = 0; i<newFileInfo.recipesSize; i++) {
    Recipe* currRecipe = newFileInfo.recipes[i];

    int currRecipeIngredientIndex = 0;
    for (int ii = 0; ii < newFileInfo.ingredientsSize; ii++) {
      Ingredient* currRecipeIngredient = currRecipe->ingredients[currRecipeIngredientIndex];

      if (strcmp(newFileInfo.ingredients[ii], currRecipeIngredient->name) == 0) {
        currRecipe->completeIngredients[currRecipe->completeIngredientsSize++] = currRecipeIngredient;
        currRecipeIngredientIndex++;
      } else {
        Ingredient* newIngredient = (Ingredient*)malloc(sizeof(Ingredient));
        strcpy(newIngredient->name, newFileInfo.ingredients[ii]);
        newIngredient->percentage = 0;
        currRecipe->completeIngredients[currRecipe->completeIngredientsSize++] = newIngredient;
      }
    }
  }

  return newFileInfo;
}

/**
 * Calculates the euclidean distance between the vector ingredients
 * of 2 recipes.
 * 
 * @param r1 {Recipe*}
 * @param r2 {Recipe*}
 * 
 * @return euclidean distance
 */
double calculateDistance (Recipe* r1, Recipe* r2) {
  double distance = 0;
  for (int i = 0; i < r1->completeIngredientsSize; i++) {
    double dd = r1->completeIngredients[i]->percentage - r2->completeIngredients[i]->percentage;
    distance += (dd*dd);
  }
  return sqrt(distance);
}

/**
 * Calculates the euclidean distance between all the recipes.
 * 
 * @param fileInfo {FileInfo*}
 * 
 * @return matrix with the euclidean distances
 */
double** pairwiseComparison (FileInfo* fileInfo) {
  double** matrix;
  matrix = malloc(fileInfo->recipesSize * sizeof *matrix);
  for (int i = 0; i < fileInfo->recipesSize; i++) {
    matrix[i] = malloc(fileInfo->recipesSize * sizeof *matrix[i]);
  }

  for (int i = 0; i < fileInfo->recipesSize; i++) {
    matrix[i][i] = 0;
  }

  for (int i = 0; i < fileInfo->recipesSize; i++) {
    for (int j = i + 1; j < fileInfo->recipesSize; j++) {
      matrix[i][j] = calculateDistance(fileInfo->recipes[i], fileInfo->recipes[j]);
    }
  }

  for (int i = 1; i < fileInfo->recipesSize; i++) {
    for (int j = 0; j < i; j++) {
      matrix[i][j] = matrix[j][i];
    }
  }

  return (double**)matrix;
}
  
int main (void) {
  FileInfo fileInfo = readFileInfo("Recipes.txt");
  fileInfo = fillDims(&fileInfo);

  printf("Recipes: \n");
  PRLN
  for (int i = 0; i<fileInfo.recipesSize; i++) {
    Recipe* currRecipe = fileInfo.recipes[i];
    printf("%s\n", currRecipe->name);
    for (int j = 0; j < currRecipe->completeIngredientsSize; j++) {
      printf("%s %d\n", currRecipe->completeIngredients[j]->name, currRecipe->completeIngredients[j]->percentage);
    }
    PRLN
  }

  printf("Ingredients: \n");
  PRLN

  for (int i = 0; i<fileInfo.ingredientsSize; i++) {
    printf("%s\n", fileInfo.ingredients[i]);
  }
  PRLN

  double **matrix;
  matrix = pairwiseComparison(&fileInfo);

  printf("Pairwise comparison: \n");
  for (int i = 0; i < fileInfo.recipesSize; i++) {
    for (int j = 0; j < fileInfo.recipesSize; j++) {
      printf("%f ", matrix[i][j]);
    }
    PRLN
  }

  return 0;
}
