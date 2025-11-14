#include "hw7.h"

// Helper function to allocate and initialize a matrix
static matrix_sf* LetsFixMatrix(unsigned int num_rows, unsigned int num_cols) {
    matrix_sf *new_matrix = malloc(sizeof(matrix_sf) + num_rows * num_cols * sizeof(int));
    if (new_matrix == NULL) {
        return NULL;
    }
    new_matrix->name = '?';
    new_matrix->num_rows = num_rows;
    new_matrix->num_cols = num_cols;
    return new_matrix;
}

// Helper function to safely free memory (checks for NULL)
static void FreeFunc(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
}

// Helper function to skip whitespace 214
static const char* SkipSpaces(const char *cursor) {
    while (*cursor == ' ') {
        cursor++;
    }
    return cursor;
}

// Helper function to parse an unsigned integer from string
static const char* ParseInteger(const char *cursor, unsigned int *value) {
    *value = 0;
    while (*cursor >= '0' && *cursor <= '9') {
        *value = *value * 10 + (*cursor - '0');
        cursor++;
    }
    return cursor;
}

// Helper function to parse a signed integer from string (handles negative!!!)
static const char* ParseSignedInteger(const char *cursor, int *value) {
    int sign_multiplier = 1;
    if (*cursor == '-') {
        sign_multiplier = -1;
        cursor++;
    }
    unsigned int abs_value = 0;
    cursor = ParseInteger(cursor, &abs_value);
    *value = sign_multiplier * (int)abs_value;
    return cursor;
}

// Helper function to perform matrix addition 
static void AddMatrix(matrix_sf *result, const matrix_sf *mat1, const matrix_sf *mat2) {
    unsigned int total_elements = result->num_rows * result->num_cols;
    for (unsigned int element_pos = 0; element_pos < total_elements; element_pos++) {
        result->values[element_pos] = mat1->values[element_pos] + mat2->values[element_pos];
    }
}

// Helper function to perform matrix multiplication 
static void MultMatrix(matrix_sf *result, const matrix_sf *mat1, const matrix_sf *mat2) {
    unsigned int output_rows = result->num_rows;
    unsigned int output_cols = result->num_cols;
    unsigned int shared_dimension = mat1->num_cols;
    
    // Initialize all result values to zero first
    unsigned int matrix_size = output_rows * output_cols;
    for (unsigned int pos = 0; pos < matrix_size; pos++) {
        result->values[pos] = 0;
    }
    
    // Then accumulate products
    for (unsigned int row = 0; row < output_rows; row++) {
        for (unsigned int shared_idx = 0; shared_idx < shared_dimension; shared_idx++) {
            int left_value = mat1->values[row * mat1->num_cols + shared_idx];
            for (unsigned int col = 0; col < output_cols; col++) {
                int right_value = mat2->values[shared_idx * mat2->num_cols + col];
                result->values[row * output_cols + col] += left_value * right_value;
            }
        }
    }
}

// Helper function to perform matrix transpose computation
static void TransposeMatrix(matrix_sf *result, const matrix_sf *mat) {
    // Iterate through result matrix positions instead of original
    for (unsigned int result_row = 0; result_row < result->num_rows; result_row++) {
        for (unsigned int result_col = 0; result_col < result->num_cols; result_col++) {
            // Map result position back to original: [result_row][result_col] = [result_col][result_row]
            unsigned int orig_row = result_col;
            unsigned int orig_col = result_row;
            unsigned int result_idx = result_row * result->num_cols + result_col;
            unsigned int orig_idx = orig_row * mat->num_cols + orig_col;
            result->values[result_idx] = mat->values[orig_idx];
        }
    }
}

// Matrix addition: mat1 + mat2
matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    if (mat1 == NULL || mat2 == NULL) {
        return NULL;
    }
    
    unsigned int rows = mat1->num_rows;
    unsigned int cols = mat1->num_cols;
    
    matrix_sf *sum_matrix = LetsFixMatrix(rows, cols);
    if (sum_matrix == NULL) {
        return NULL;
    }
    
    AddMatrix(sum_matrix, mat1, mat2);
    
    return sum_matrix;
}

// Matrix multiplication: mat1 * mat2
matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    if (mat1 == NULL || mat2 == NULL) {
        return NULL;
    }
    
    unsigned int rows = mat1->num_rows;
    unsigned int cols = mat2->num_cols;
    
    matrix_sf *product_matrix = LetsFixMatrix(rows, cols);
    if (product_matrix == NULL) {
        return NULL;
    }
    
    MultMatrix(product_matrix, mat1, mat2);
    
    return product_matrix;
}

// Matrix transpose: mat'
matrix_sf* transpose_mat_sf(const matrix_sf *mat) {
    if (mat == NULL) {
        return NULL;
    }
    
    unsigned int transposed_rows = mat->num_cols;
    unsigned int transposed_cols = mat->num_rows;
    
    matrix_sf *transposed_matrix = LetsFixMatrix(transposed_rows, transposed_cols);
    if (transposed_matrix == NULL) {
        return NULL;
    }
    
    TransposeMatrix(transposed_matrix, mat);
    
    return transposed_matrix;
}

// Parse matrix definition from string
matrix_sf* create_matrix_sf(char name, const char *expr) {
    if (expr == NULL) {
        return NULL;
    }
    
    const char *input_cursor = expr;
    
    // Skip leading spaces and parse num_rows
    input_cursor = SkipSpaces(input_cursor);
    unsigned int rows = 0;
    input_cursor = ParseInteger(input_cursor, &rows);
    
    // Skip spaces and parse num_cols
    input_cursor = SkipSpaces(input_cursor);
    unsigned int cols = 0;
    input_cursor = ParseInteger(input_cursor, &cols);
    
    // Skip spaces until '['
    input_cursor = SkipSpaces(input_cursor);
    
    if (*input_cursor != '[') {
        return NULL;
    }
    input_cursor++;
    
    // Allocate matrix
    matrix_sf *new_matrix = LetsFixMatrix(rows, cols);
    if (new_matrix == NULL) {
        return NULL;
    }
    new_matrix->name = name;
    
    // Parse values
    unsigned int element_position = 0;
    for (unsigned int row_index = 0; row_index < rows; row_index++) {
        // Skip leading spaces
        input_cursor = SkipSpaces(input_cursor);
        
        // Parse row values
        for (unsigned int col_index = 0; col_index < cols; col_index++) {
            // Skip spaces and parse integer
            input_cursor = SkipSpaces(input_cursor);
            int element_value = 0;
            input_cursor = ParseSignedInteger(input_cursor, &element_value);
            new_matrix->values[element_position++] = element_value;
            
            // Skip spaces
            input_cursor = SkipSpaces(input_cursor);
        }
        
        // Skip semicolon
        input_cursor = SkipSpaces(input_cursor);
        if (*input_cursor == ';') {
            input_cursor++;
        }
        input_cursor = SkipSpaces(input_cursor);
    }
    
    // Skip trailing spaces and ']'
    input_cursor = SkipSpaces(input_cursor);
    if (*input_cursor == ']') {
        input_cursor++;
    }
    
    return new_matrix;
}

// Insert matrix into BST
bst_sf* insert_bst_sf(matrix_sf *matrix, bst_sf *tree_root) {
    if (matrix == NULL) {
        return tree_root;
    }
    
    // Base case: empty tree, create new node
    if (tree_root == NULL) {
        bst_sf *new_tree_node = malloc(sizeof(bst_sf));
        if (new_tree_node == NULL) {
            return NULL;
        }
        new_tree_node->mat = matrix;
        new_tree_node->left_child = NULL;
        new_tree_node->right_child = NULL;
        return new_tree_node;
    }
    
    // Recursive case: compare names and insert in appropriate subtree
    char matrix_name = matrix->name;
    char root_matrix_name = tree_root->mat->name;
    
    if (matrix_name < root_matrix_name) {
        tree_root->left_child = insert_bst_sf(matrix, tree_root->left_child);
    } else if (matrix_name > root_matrix_name) {
        tree_root->right_child = insert_bst_sf(matrix, tree_root->right_child);
    }

    
    return tree_root;
}

// Find matrix in BST by name
matrix_sf* find_bst_sf(char target_name, bst_sf *current_node) {
    // Base case: not found
    if (current_node == NULL) {
        return NULL;
    }
    
    char node_matrix_name = current_node->mat->name;
    
    // Found the matrix
    if (target_name == node_matrix_name) {
        return current_node->mat;
    }
    
    // Search left subtree if name is smaller
    if (target_name < node_matrix_name) {
        return find_bst_sf(target_name, current_node->left_child);
    }
    
    // Otherwise search right subtree
    return find_bst_sf(target_name, current_node->right_child);
}

// Free BST and all matrices
void free_bst_sf(bst_sf *root) {
    if (root == NULL) {
        return;
    }
    
    free_bst_sf(root->left_child);
    free_bst_sf(root->right_child);
    
    FreeFunc(root->mat);
    FreeFunc(root);
}

// Convert infix to postfix
char* infix2postfix_sf(char *infix) {
    if (infix == NULL) {
        return NULL;
    }
    
    int input_length = strlen(infix);
    char *postfix_expr = malloc((input_length * 2 + 1) * sizeof(char));
    char *operator_stack = malloc(input_length * sizeof(char));
    int output_position = 0;
    int stack_top_index = -1;
    
    int char_position = 0;
    while (infix[char_position] != '\0') {
        char current_char = infix[char_position];
        
        if (current_char == ' ') {
            char_position++;
            continue;
        }
        
        if (current_char >= 'A' && current_char <= 'Z') {
            postfix_expr[output_position++] = current_char;
            char_position++;
            continue;
        }
        
        if (current_char == '(') {
            stack_top_index++;
            operator_stack[stack_top_index] = current_char;
            char_position++;
            continue;
        }
        
        if (current_char == ')') {
            while (stack_top_index >= 0 && operator_stack[stack_top_index] != '(') {
                postfix_expr[output_position++] = operator_stack[stack_top_index];
                stack_top_index--;
            }
            if (stack_top_index >= 0) {
                stack_top_index--;
            }
            char_position++;
            continue;
        }
        
        if (current_char == '\'') {
            // Transpose is right-associative with highest precedence
            // Just push it to stack without popping
            stack_top_index++;
            operator_stack[stack_top_index] = current_char;
            char_position++;
            continue;
        }
        
        if (current_char == '*') {
            while (stack_top_index >= 0 && (operator_stack[stack_top_index] == '\'' || operator_stack[stack_top_index] == '*')) {
                postfix_expr[output_position++] = operator_stack[stack_top_index];
                stack_top_index--;
            }
            stack_top_index++;
            operator_stack[stack_top_index] = current_char;
            char_position++;
            continue;
        }
        
        if (current_char == '+') {
            while (stack_top_index >= 0 && operator_stack[stack_top_index] != '(' && 
                   (operator_stack[stack_top_index] == '\'' || operator_stack[stack_top_index] == '*' || operator_stack[stack_top_index] == '+')) {
                postfix_expr[output_position++] = operator_stack[stack_top_index];
                stack_top_index--;
            }
            stack_top_index++;
            operator_stack[stack_top_index] = current_char;
            char_position++;
            continue;
        }
        
        char_position++;
    }
    
    while (stack_top_index >= 0) {
        postfix_expr[output_position++] = operator_stack[stack_top_index];
        stack_top_index--;
    }
    
    postfix_expr[output_position] = '\0';
    
    FreeFunc(operator_stack);
    
    return postfix_expr;
}

// Evaluate expression using postfix notation
matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) {
    if (expr == NULL || root == NULL) {
        return NULL;
    }
    
    char *postfix_expr = infix2postfix_sf(expr);
    if (postfix_expr == NULL) {
        return NULL;
    }
    
    // Stack for matrix pointers
    matrix_sf **matrix_stack = malloc(1000 * sizeof(matrix_sf*));
    int stack_top_position = -1;
    
    int expr_position = 0;
    char temporary_name = '!';
    
    while (postfix_expr[expr_position] != '\0') {
        char current_operator = postfix_expr[expr_position];
        
        if (current_operator >= 'A' && current_operator <= 'Z') {
            matrix_sf *found_matrix = find_bst_sf(current_operator, root);
            if (found_matrix == NULL) {
                FreeFunc(postfix_expr);
                FreeFunc(matrix_stack);
                return NULL;
            }
            stack_top_position++;
            matrix_stack[stack_top_position] = found_matrix;
            expr_position++;
            continue;
        }
        
        if (current_operator == '\'') {
            if (stack_top_position < 0) {
                FreeFunc(postfix_expr);
                FreeFunc(matrix_stack);
                return NULL;
            }
            
            // Pop operand for unary transpose operation
            matrix_sf *operand_matrix = matrix_stack[stack_top_position];
            stack_top_position--;
            
            matrix_sf *transposed_result = transpose_mat_sf(operand_matrix);
            
            // Free operand if it was a temporary matrix
            if (!isalpha(operand_matrix->name)) {
                FreeFunc(operand_matrix);
            }
            
            // Assign temporary name to result
            transposed_result->name = temporary_name;
            temporary_name++;
            
            // Push result back onto stack
            stack_top_position++;
            matrix_stack[stack_top_position] = transposed_result;
            expr_position++;
            continue;
        }
        
        if (current_operator == '*') {
            if (stack_top_position < 1) {
                FreeFunc(postfix_expr);
                FreeFunc(matrix_stack);
                return NULL;
            }
            
            // Pop operands in reverse order (right operand first)
            matrix_sf *right_matrix = matrix_stack[stack_top_position];
            stack_top_position--;
            matrix_sf *left_matrix = matrix_stack[stack_top_position];
            stack_top_position--;
            
            matrix_sf *product_result = mult_mats_sf(left_matrix, right_matrix);
            product_result->name = temporary_name;
            temporary_name++;
            
            // Free temporary matrices (non-alphabetic names indicate temporary)
            if (!isalpha(left_matrix->name)) {
                FreeFunc(left_matrix);
            }
            if (!isalpha(right_matrix->name)) {
                FreeFunc(right_matrix);
            }
            
            stack_top_position++;
            matrix_stack[stack_top_position] = product_result;
            expr_position++;
            continue;
        }
        
        if (current_operator == '+') {
            if (stack_top_position < 1) {
                FreeFunc(postfix_expr);
                FreeFunc(matrix_stack);
                return NULL;
            }
            
            // Pop operands in reverse order (right operand first)
            matrix_sf *right_matrix = matrix_stack[stack_top_position];
            stack_top_position--;
            matrix_sf *left_matrix = matrix_stack[stack_top_position];
            stack_top_position--;
            
            matrix_sf *sum_result = add_mats_sf(left_matrix, right_matrix);
            sum_result->name = temporary_name;
            temporary_name++;
            
            // Free temporary matrices (non-alphabetic names indicate temporary)
            if (!isalpha(left_matrix->name)) {
                FreeFunc(left_matrix);
            }
            if (!isalpha(right_matrix->name)) {
                FreeFunc(right_matrix);
            }
            
            stack_top_position++;
            matrix_stack[stack_top_position] = sum_result;
            expr_position++;
            continue;
        }
        
        expr_position++;
    }
    
    if (stack_top_position < 0) {
        FreeFunc(postfix_expr);
        FreeFunc(matrix_stack);
        return NULL;
    }
    
    matrix_sf *final_result = matrix_stack[stack_top_position];
    final_result->name = name;
    
    FreeFunc(postfix_expr);
    FreeFunc(matrix_stack);
    
    return final_result;
}

// Helper function to free BST nodes except the one with the given name
static void free_bst_except(bst_sf *root, char except_name) {
    if (root == NULL) {
        return;
    }
    
    free_bst_except(root->left_child, except_name);
    free_bst_except(root->right_child, except_name);
    
    if (root->mat != NULL && root->mat->name != except_name) {
        FreeFunc(root->mat);
    }
    
    FreeFunc(root);
}

// Execute script file
matrix_sf *execute_script_sf(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }
    
    bst_sf *root = NULL;
    matrix_sf *last_matrix = NULL;
    
    char *line = NULL;
    size_t max_line_size = MAX_LINE_LEN;
    
    while (getline(&line, &max_line_size, file) != -1) {
        // Remove newline if present
        int len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Skip empty lines
        int i = 0;
        while (line[i] == ' ') {
            i++;
        }
        if (line[i] == '\0') {
            continue;
        }
        
        // Parse matrix name
        char name = line[i];
        i++;
        
        // Skip spaces and '='
        while (line[i] == ' ') {
            i++;
        }
        if (line[i] == '=') {
            i++;
        }
        while (line[i] == ' ') {
            i++;
        }
        
        // Check if it's a matrix definition or expression
        int has_bracket = 0;
        int j = i;
        while (line[j] != '\0') {
            if (line[j] == '[') {
                has_bracket = 1;
                break;
            }
            j++;
        }
        
        matrix_sf *new_mat = NULL;
        
        if (has_bracket) {
            // Matrix definition
            new_mat = create_matrix_sf(name, line + i);
        } else {
            // Expression
            new_mat = evaluate_expr_sf(name, line + i, root);
        }
        
        if (new_mat != NULL) {
            root = insert_bst_sf(new_mat, root);
            last_matrix = new_mat;
        }
    }
    
    FreeFunc(line);
    
    fclose(file);
    
    // Free all matrices except the last one
    if (root != NULL && last_matrix != NULL) {
        free_bst_except(root, last_matrix->name);
    }
    
    return last_matrix;
}

// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don't touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
