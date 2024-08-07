#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matxlib.h"

#define __MAX_TOKEN_LEN__ 512
#define STR_END 6

int row1, col1;
double **resultss;

typedef enum {
    normal,
    determinant,
    transpose,
    inverse,
    add_subtr,
    multiplication,
}operation;

typedef enum {
    NOT_FOUND = 404,
    BAD_DIM = -3,
    ERR = -1,
    OK = 10,
}error_check;

/**
 * syntaxerror: Prints a syntax error message
 * 
 * message- The error message printed
 * 
 * Return: Always returns ERR
*/
int syntaxerror(char *erro_message)
{
    fprintf(stderr," *** syntax error ***\n");
    fprintf(stderr, "%s\n", erro_message);

    return ERR;
}

/**
 * check_matxcol: Checks the number of coulumns of a matrix 
 *                string against the columns of a refrence matrix string.
 * 
 * @s- The matrix string
 * @column- The refrence number of columns
 * 
 * return: Returns OK if the number of colums are equal else returns BAD_DIM
*/
int check_matxcol(char *s, const int *column) 
{
    int test_col = 0;

    while(*s == ' ')    /* skip any trailing white spae at the begining of string */
        s++;

    if(*s == '\0' || *s == '\n')
        return OK;
    
    while(*s != ';') {
        if(*s == ',')
            test_col++;
        s++;
    }
    test_col++;

    
    if(test_col != *column)
        return BAD_DIM;

    return check_matxcol(s + 1, column);
}

/**
 * check_matxrow: Checks the number of rows of a matrix 
 *                string against the rows of a refrence matrix string.
 * 
 * @s- The matrix string
 * @column- The refrence number of rows
 * 
 * return: Returns OK if the number of rows are equal else returns BAD_DIM
*/
int check_matxrow(char *s, const int *row) 
{
    int test_row = 0;;

    while(*s != '\0') {
        if(*s == ';')
            test_row += 1;
        s++;
    }

    if(*row != test_row)
        return BAD_DIM;
    
    return OK;
}

/**
 * find_matxdim: Finds the dimension of a matrix string.
 * 
 * s- The matrix string
 * col- A pointer to the variable to hold the number of columns
 * row- A pointer to the variable to hold the number of rows.
 * 
 * return: Returns OK if the following columns match the first one
 *         else return BAD_DIM
*/
int find_matxdim(char *s, int *col, int *row)
{
    char *t = s; 
    int ret = 0;

    while(*s == ' ')
        s++;

    while(*s != ';') {
        if(*s == ',')
            *col += 1;
        s++;  
    }
    *col += 1;

    ret = check_matxcol(s + 1, col);

    if(ret == BAD_DIM) 
        return ret;

    while(*t != '\0') {
        if(*t == ';')
            *row += 1;
        t++;
    }

    return OK;
}

/**
 * check_matxdim: Checks the matrix dimension, with respect
 *                to a refrence matrix if it is compatible for 
 *                a particular operation.
 * 
 * *str- The string holding the matrix.
 * row- The row of the refrence matrix.
 * col- The col of the refrence matrix.
 * op- The operation to be performed whith the matrix.
 * 
 * Return: returns BAD_DIM if the dimension isn't appropriate
 *          for operation, ERR if the operation isn't valid or
 *          OK if it all goes well.
*/
error_check check_matxdim(char *str, int row, int col, char op)
{
     char *s = str;

     int tcol = 0, trow = 0;

    switch(op) {
        case '+':
        case '-':
            if(check_matxcol(str, &col) != OK || check_matxrow(str, &row) != OK)
                return BAD_DIM;

            break;
        
        case '*':
            if(check_matxcol(str, &row) != OK || check_matxrow(str, &col) != OK)
                return BAD_DIM;

            break;

        default:
            return ERR;
            break;
    }

    return OK;
}

/*This is where it starts */
/**
 * mnatxinit: Initializes al elements of amatrix 
 *            to zero.
 * 
 * matx- The matrix.
 * row- The number of rows of the matrix
 * col- The number of columns of the matrix.
*/
void matxinit(int row, int col, double matx[row][col])
{
    int i, j;

    for(i = 0; i < row; i++) {
        for(j = 0; j < col; j++)
            matx[i][j] = 0;
    }

}

/**
 * addmatx: Adds up two matrixes and stores the result
 *          in one of them.
 * 
 * matx- the first matrix
 * res- The other matrix, which will also hold the result of 
 *      computation.
 * row- The row of the matrices.
 * col- The column of the matrices.
*/
void addmatx(int row, int col, double matx[row][col], double res[row][col])
{
    int i, j;

    for(i = 0; i < row; i++) {
        for(j = 0; j < col; j++)
            res[i][j] += matx[i][j];
    }    
}

/**
 * submatx: Subtracts two matrixes and stores the result
 *          in one of them.
 * 
 * matx- the first matrix
 * res- The other matrix, which will also hold the result of 
 *      computation.
 * row- The row of the matrices.
 * col- The column of the matrices.
*/
void submatx(int row, int col, double matx[row][col], double res[row][col])
{
    int i, j;

    for(i = 0; i < row; i++) {
        for(j = 0; j < col; j++)
            res[i][j] -= matx[i][j];
    }    
}

/**
 * multmatx: Multiplies two matrices.
 * 
 * row1- number of rows of matrix 1
 * col1- The number of columns of matrix 1
 * matx1- One of the matrices to be multiplied.
 * row2- number of rows of matrix 2
 * col2- The number of columns of matrix 2
 * matx2- The other matrix to be modified.
 * 
*/
void multmatx(int row1, int col1, int row2, int col2, double matx1[row1][col1], double matx2[row2][col2])
{
    int i, j, k;
    
    resultss = malloc(row1 * sizeof(double *));
    for(i = 0; i < row1; i++) {
        resultss[i] = malloc(col2 * sizeof(double));
    }

    for (i = 0; i < row1; i++) {
        for (j = 0; j < col2; j++) {
            resultss[i][j] = 0;
            for (k = 0; k < col1; k++) {
                resultss[i][j] += matx1[i][k] * matx2[k][j];
            }
        }
    }

}

/**
 * matx_transpose: Find the transpose of a matrx.
 * 
 * matx- The matrix.
 * row- The number of rows of the matrix.
 * col- The number of columns of the matrix.
*/
void matx_transpose(int row, int col, double matx[row][col])
{
    int i, j;

    resultss = malloc(col * sizeof(double *));
    for(i = 0; i < col; i++) {
        resultss[i] = malloc(row * sizeof(double));
    }

    for(i = 0; i < row; i++) {
        for(j = 0; j < col; j++) {
            resultss[j][i] = 0;
            resultss[j][i] = matx[i][j];
        }
    }
}


/**
 * formuppertriangularmatx: Reduces a matrix to it's upper trangular form.
 * 
 * row- The matrix row.
 * col- The column of the matrix.
 * matx- The matrix to be reduced to upper triangular form.
 * 
 * return: Returns 1 if the at some point, the elements on the leading
 *          diagonal is zero.
*/
int formuppertriangularmatx(int row, int col, double matx[row][col]) {
    int i, j, k;
    double factor;

    for (i = 0; i < row - 1; i++) {
        for (j = i + 1; j < row; j++) {
            if(matx[i][i] == 0)
                return 1;
            factor = matx[j][i] / matx[i][i];
            for (k = i; k < row; k++) {
                matx[j][k] -= factor * matx[i][k];
            }
        }
    }
}

/**
 * matxdeterminant: Computes the determinant of a matrix, by
 *                  multiplying the elements of the leading diagonal of the 
 *                  upper trangular form of that matrix.
 * 
 * row- The number of row of the matrix.
 * col- The number of columns of the matrix.
 * matx- The matrix to be computed.
 * 
 * return: Returns the determinant of the matrix.
*/
int matxdeterminant(int row, int col, double matx[row][col])
{
    int i, j;
    double det = 1.0;

    if(formuppertriangularmatx(row, col, matx) == 1)
        return 0;   /* if at some point elements on the leading daigonal is zero when forming upper
                        triangular matrix is zero, then the determinant is obviously zero*/

    for (i = 0; i < row; i++) {
        for (j = 0; j < row; j++) {
            printf("%.2lf\t", matx[i][j]);
        }
        printf("\n");
    }

    for(i = 0; i < row; i++) {
        det *= matx[i][i];
    }

    return det;
}

/**
 * eval: Checks the operation to be performed, and calls the 
 *       appropriate function to do it.
 * 
 * matx- The first matrix.
 * res- The other matrix, which will also store the result.
 * row- The row of the matrices.
 * col- The col of the matrices.
*/
void eval(int row, int col, double matx[row][col], double res[row][col], char op)
{
    switch(op) {
        case '+':
            addmatx(row, col, matx, res);
            break;
        case '-':
            submatx(row, col, matx, res);
            break;
        case '*':
            /*multiply*/
            break;
        default:
            return;
    }
}

/**
 * parsematx: Extracts the matrix from a string, and 
 *              converts it into integer storing it into the matrix.
 * 
 * str- String containing the matrix.
 * col- The column of the matrix.
 * row- The row of the matrix.
 * matrix- The matrix that would hold the converted matrix.
 * 
 * Return: returns a pointer to the next string in the input string.
*/
char *parsematx(char* str, int col, int row, double matrix[row][col])
{
    char *s = str;
       
    while(*s != '\0')   /* move the the end of the string so the function can return
                             a pointer to the next matrix string representation, since they are seperated by '\0'*/
        s++;

    char *token = strtok(str, ",;");
    int i = 0, j = 0;

    while(token != NULL) {
        matrix[i][j] = atof(token);
        j++;

        if (j == col) {
            j = 0;
            i++;
        }

        token = strtok(NULL, ",;");
    }
    return s + 1;
}

/**
 * checkoperation: Determines the operation to be 
 *                 performed on a matrix, by reading it form the
 *                 input string.
 * 
 * in: Pointer to the input string.
*/
operation checkoperation(char *in)
{
    char str[__MAX_TOKEN_LEN__ / 4];
    int i = 0;

    if(*in == '|')     /* the functionality for putting |matxstr| is still to be implemented */
        return determinant;
    
    if(*in == 'q') {
        strncpy(str, in, 4);
        str[strlen(str)] = '\0';

        if(strncmp(str, "q!", 2) == 0)
            return immediatequit;

        else if(strncmp(str, "q", 1) == 0)
            return l_quit;
    }
    
    if(isalpha(*in)) {
        while(*in != '(' && *in != '\0') {
            str[i++] = *in++;
        }
        str[i] = '\0';

        if(*in != '(')
            return ERR;
        
        if(strchr(in, ')') == NULL) {
            printf("missen closing parenthesis\n\n");
            return ERR;
        }
    }
    else
        return normal;

    if(strncmp(str, "trps", 4) == 0)
        return transpose;
    
    if(strncmp(str, "invs", 4) == 0)
        return inverse;
    
    if(strncmp(str, "det", 3) == 0)
        return determinant;
    
    return NOT_FOUND;


}

/**
 * printresult: Prints the result after addition and subtraction
 *              doesn't yet work for other operations.
 * 
 * row- The number of rows of the matrix
 * col- The number of columns of the matrix
 * matx- The matx that holds the result.
 * 
 * 
*/
void printresult(int row, int col, double matx[row][col])
{
    int i, j;

    printf("\n");
    for(i = 0; i < row; i++) {
        for(j = 0; j < col; j++)
            printf("%.2lf ", matx[i][j]);
        printf("\n");
    }
}

int matxmain(void)
{   

    printf("\n*** MATRIX CALCULATOR ***\n\n");

    char input[__MAX_TOKEN_LEN__];
    char operator[__MAX_TOKEN_LEN__ / 2];
    int ReturnState = 0;

    char *p = '\0', *st = '\0';

    int i = 0, j = 0;
    int col, row, ind = 0;
    int op = NOT_FOUND;

    while(1) {
        printf("Enter matrix expression: ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';

        op = checkoperation(input);
        
        if(op == NOT_FOUND)
            printf("operation '%s' not recognised as valid operation.Try again\n\n", input); 

        else if(op == ERR)
            printf("Error: '(' or ')' not correctly embedded in expression or expression not recognised\n\n");
        
        else
            break;
    }

    if(op == l_quit)
        return l_quit;
    else if(op == immediatequit)
        exit(EXIT_FAILURE);
    
    if(op == normal) {
        if((st = strchr(input, '=')) == NULL) {
            syntaxerror("Required '=' at end of string");
            return Continue;
        }
    }

    p = strpbrk(input, "+-*=)");
    while(1){
        if(p != NULL) {
            if(*p == '-' && *(p + 1) == ' ') {
                operator[i++] = *p;
                *p = '\0';
                break;
            }
            else if(*p != '-') {
                operator[i++] = *p;
                *p = '\0';
                break;
            }
        }
        p = strpbrk(p + 1, "+-*=)");
    }  

    row = col = 0;
    if(find_matxdim(input, &col, &row) != OK) {
        syntaxerror("Bad matrix dimension");
        return Continue;
    }
        
    printf("\nrowxcol: %dx%d\n\n", row, col);

    double matx[row][col];
    double result[row][col];

    matxinit(row, col, result);


    /**
     *  since operators seperate differnt matrixes
     * replace it with '\0' so they can be as seperate strings
     */
    double de;
    int ret_status;

   switch(op) {
        case normal:

            while(1) {
                p = strpbrk(p + 1, "+-*="); 
                if( p == NULL)
                    break;
                if(*p != '=') {
                    if(*p == '-' && *(p + 1) == ' ') {
                        operator[i++] = *p;
                        *p = '\0';
                    }
                    else if(*p != '-') {
                        operator[i++] = *p;
                        *p = '\0';
                    }
                }
                else if( *p == '=') {
                    *p = '\0';
                    break;
                }
            }
            operator[i] = '\0';
            j = 0;

            p = parsematx(input, col, row, matx);
            addmatx(row, col, matx, result);

            switch(operator[j]) {
                case '+':
                case '-':
                    while(*p != '\n') {
                        if(p + 1 != NULL) {
                            if(ind < i && (ret_status = check_matxdim(p, row, col, operator[ind])) != OK) {
                                syntaxerror("Matrix not embedded correctly");
                                break;
                            }
                        p = parsematx(p, col, row, matx);
                        eval(row, col, matx, result, operator[ind++]);
                    }
                }
                if(ret_status != OK)
                    break;
                printresult(row, col, result);
                break;

                case '*':
                    col1 = row1 = 0;

                    if(find_matxdim(p, &col1, &row1) != OK) {
                        syntaxerror("Matrix not embedded correctly for multiplucation");
                        break;
                    }

                    double mmatx[row1][col1];

                    matxinit(row1, col1, mmatx);

                    if(ind < i && (ret_status = check_matxdim(p, row, col, operator[ind])) != OK) {
                        syntaxerror("Matrix not embedded properly");
                        break;
                    }

                    p = parsematx(p, col1, row1, mmatx);
                    multmatx(row, col, row1, col1, result, mmatx);

                    /* print result */
                    printf("\n");
                    for(i = 0; i < row; i++) {
                        for(j = 0; j < col1; j++)
                            printf("%.2lf ", resultss[i][j]);
                        printf("\n");
                    }
                    break;


            }
            break;

        case determinant:
            p = input;

            while(*p != '(') /* move pointer to begining of the matrix in string */
                p++;

            p = parsematx(++p, col, row, matx);
            de = matxdeterminant(row, col, matx);
            printf("determinant: %.2lf\n", de);
            break;

        case transpose:
            printf("transpose: \n");
            p = input;
            while(*p != '(')
                p++;
            p = parsematx(++p, col, row, matx);
            matx_transpose(row, col, matx);

            /* print result */
            for(i = 0; i < col; i++) {
                for(j = 0; j < row; j++)
                    printf("%.2lf ", resultss[i][j]);
                printf("\n");
            }

            break;

        default:
            printf("Sorry Opertion is not yet implemented or isn't recognised\n");
            break;

   }   

    return Continue;
} 
