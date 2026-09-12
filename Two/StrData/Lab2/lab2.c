#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_SIZE 1000
#define MAX_VAR_SIZE 50
#define MAX_FUNC_SIZE 10
#define PI 3.14159265358979323846
#define E 2.71828182845904523536

// Stack node structure definition
typedef struct StackNode {
    char operatorr;
    double value;
    int type; // 0 - operator, 1 - number, 2 - function
    char function[MAX_FUNC_SIZE];
    struct StackNode* next;
} StackNode;

// Queue node structure definition
typedef struct QueueNode {
    char operatorr;
    double value;
    int type; // 0 - operator, 1 - number, 2 - function
    char function[MAX_FUNC_SIZE];
    char variable[MAX_VAR_SIZE];
    int isVariable; // 0 - not variable, 1 - variable
    struct QueueNode* next;
} QueueNode;

// Variable dictionary node structure definition
typedef struct VarNode {
    char name[MAX_VAR_SIZE];
    double value;
    struct VarNode* next;
} VarNode;

// Stack
typedef struct {
    StackNode* top;
} Stack;

// Queue
typedef struct {
    QueueNode* front;
    QueueNode* rear;
} Queue;

// Variable dictionary
typedef struct {
    VarNode* head;
} VarDict;

// Initialize stack
void initStack(Stack* stack) {
    stack->top = NULL;
}

// Initialize queue
void initQueue(Queue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

// Initialize variable dictionary
void initVarDict(VarDict* dict) {
    dict->head = NULL;
}

// Check if stack is empty
int isStackEmpty(Stack* stack) {
    return stack->top == NULL;
}

// Check if queue is empty
int isQueueEmpty(Queue* queue) {
    return queue->front == NULL;
}

// Push operator to stack
void pushOperator(Stack* stack, char op) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    newNode->operatorr = op;
    newNode->type = 0;
    newNode->next = stack->top;
    stack->top = newNode;
}

// Push value to stack
void pushValue(Stack* stack, double value) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    newNode->value = value;
    newNode->type = 1;
    newNode->next = stack->top;
    stack->top = newNode;
}

// Push function to stack
void pushFunction(Stack* stack, const char* func) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    strcpy(newNode->function, func);
    newNode->type = 2;
    newNode->next = stack->top;
    stack->top = newNode;
    newNode->operatorr = 0;
}

// Pop from stack
StackNode* pop(Stack* stack) {
    if (isStackEmpty(stack)) {
        printf("Error: stack is empty\n");
        exit(1);
    }
    StackNode* temp = stack->top;
    stack->top = stack->top->next;
    return temp;
}

// Peek at top stack element
StackNode* peek(Stack* stack) {
    if (isStackEmpty(stack)) {
        return NULL;
    }
    return stack->top;
}

// Enqueue operator
void enqueueOperator(Queue* queue, char op) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    newNode->operatorr = op;
    newNode->type = 0;
    newNode->isVariable = 0;
    newNode->next = NULL;
    
    if (isQueueEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Enqueue value
void enqueueValue(Queue* queue, double value) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    newNode->value = value;
    newNode->type = 1;
    newNode->isVariable = 0;
    newNode->next = NULL;
    
    if (isQueueEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Enqueue variable
void enqueueVariable(Queue* queue, const char* varName) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    strcpy(newNode->variable, varName);
    newNode->type = 1; // Variable will be represented as a number after substitution
    newNode->isVariable = 1;
    newNode->next = NULL;
    
    if (isQueueEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Enqueue function
void enqueueFunction(Queue* queue, const char* func) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    strcpy(newNode->function, func);
    newNode->type = 2;
    newNode->isVariable = 0;
    newNode->next = NULL;
    
    if (isQueueEmpty(queue)) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Dequeue
QueueNode* dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        printf("Error: queue is empty\n");
        exit(1);
    }
    QueueNode* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    return temp;  // Fixed: removed memory freeing as we're returning a pointer
}

// Add variable to dictionary
void addVariable(VarDict* dict, const char* name, double value) {
    VarNode* newNode = (VarNode*)malloc(sizeof(VarNode));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        exit(1);
    }
    strcpy(newNode->name, name);
    newNode->value = value;
    newNode->next = dict->head;
    dict->head = newNode;
}

// Get variable value from dictionary
double getVariableValue(VarDict* dict, const char* name) {
    VarNode* current = dict->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    // If variable not found, ask user for value
    double value;
    printf("Enter value for variable %s: ", name);
    scanf("%lf", &value);
    addVariable(dict, name, value);
    return value;
}

// Check if string is a function
int isFunction(const char* token) {
    return strcmp(token, "sin") == 0 || 
            strcmp(token, "cos") == 0 || 
            strcmp(token, "tan") == 0 || 
            strcmp(token, "tg") == 0 || 
            strcmp(token, "ctg") == 0 || 
            strcmp(token, "arcsin") == 0 || 
            strcmp(token, "arccos") == 0 || 
            strcmp(token, "arctan") == 0 || 
            strcmp(token, "arctg") == 0 || 
            strcmp(token, "sqrt") == 0;
}

// Get operator precedence
int getPrecedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
            return 3;
        case '!':  // Factorial - unary operator, highest precedence
            return 4;
        default:
            return 0;  // Fixed: return 0 for parentheses and other characters
    }
}

// Calculate factorial
double factorial(double n) {
    if (n < 0) {
        printf("Error: unable to calculate factorial of a negative number\n");
        exit(1);
    }
    
    int intN = (int)n;
    if (intN != n) {
        printf("Warning: factorial is calculated only for integers\n");
    }
    
    double result = 1;
    for (int i = 2; i <= intN; i++) {
        result *= i;
    }
    return result;
}

// Apply function to value
double applyFunction(const char* func, double value) {
    double result;
    double epsilon = 1e-10; // Threshold for approximating to zero or one
    
    // Normalize value for trigonometric functions at multiples of PI
    double normalizedValue = value;
    if (fabs(fmod(value, PI)) < epsilon || fabs(fmod(value, PI) - PI) < epsilon) {
        normalizedValue = round(value / PI) * PI;
    }
    
    if (strcmp(func, "sin") == 0) {
        result = sin(value);
        // Handle sine values at multiples of PI
        if (fabs(fmod(normalizedValue, PI)) < epsilon) {
            return 0.0; // sin(n*pi) = 0
        }
        // Handle sin(pi/2 + n*pi)
        if (fabs(fmod(normalizedValue - PI/2, PI)) < epsilon) {
            return (fmod(round(normalizedValue / (PI/2)), 4) == 1) ? 1.0 : -1.0;
        }
        if (fabs(result) < epsilon) {
            return 0.0;
        }
        return result;
    } else if (strcmp(func, "cos") == 0) {
        result = cos(value);
        // cos(pi/2 + n*pi) = 0
        if (fabs(fmod(normalizedValue - PI/2, PI)) < epsilon) {
            return 0.0;
        }
        // cos(n*pi) = 1 or -1
        if (fabs(fmod(normalizedValue, PI)) < epsilon) {
            return (fmod(round(normalizedValue / PI), 2) == 0) ? 1.0 : -1.0;
        }
        if (fabs(result) < epsilon) {
            return 0.0;
        }
        if (fabs(result - 1.0) < epsilon) {
            return 1.0;
        }
        if (fabs(result + 1.0) < epsilon) {
            return -1.0;
        }
        return result;
    } else if (strcmp(func, "tan") == 0 || strcmp(func, "tg") == 0) {
        // Check for undefined values (multiples of pi/2)
        if (fabs(fmod(normalizedValue - PI/2, PI)) < epsilon) {
            printf("Error: tangent is not defined at x = pi/2 + n*pi\n");
            exit(1);
        }
        result = tan(value);
        // tan(n*pi) = 0
        if (fabs(fmod(normalizedValue, PI)) < epsilon) {
            return 0.0;
        }
        if (fabs(result) < epsilon) {
            return 0.0;
        }
        return result;
    } else if (strcmp(func, "ctg") == 0) {
        // Check for undefined values (multiples of pi)
        if (fabs(fmod(normalizedValue, PI)) < epsilon) {
            printf("Error: cotangent is not defined at x = n*pi\n");
            exit(1);
        }
        result = 1.0 / tan(value);
        // ctg(pi/2 + n*pi) = 0
        if (fabs(fmod(normalizedValue - PI/2, PI)) < epsilon) {
            return 0.0;
        }
        if (fabs(result) < epsilon) {
            return 0.0;
        }
        return result;
    } else if (strcmp(func, "arcsin") == 0) {
        if (value < -1.0 || value > 1.0) {
            printf("Error: arcsin argument must be in range [-1, 1]\n");
            exit(1);
        }
        result = asin(value);
        // Special cases
        if (fabs(value) < epsilon) {
            return 0.0; // arcsin(0) = 0
        }
        if (fabs(value - 1.0) < epsilon) {
            return PI/2; // arcsin(1) = pi/2
        }
        if (fabs(value + 1.0) < epsilon) {
            return -PI/2; // arcsin(-1) = -pi/2
        }
        return result;
    } else if (strcmp(func, "arccos") == 0) {
        if (value < -1.0 || value > 1.0) {
            printf("Error: arccos argument must be in range [-1, 1]\n");
            exit(1);
        }
        result = acos(value);
        // Special cases
        if (fabs(value - 1.0) < epsilon) {
            return 0.0; // arccos(1) = 0
        }
        if (fabs(value + 1.0) < epsilon) {
            return PI; // arccos(-1) = pi
        }
        if (fabs(value) < epsilon) {
            return PI/2; // arccos(0) = pi/2
        }
        return result;
    } else if (strcmp(func, "arctan") == 0 || strcmp(func, "arctg") == 0) {
        result = atan(value);
        // Special cases
        if (fabs(value) < epsilon) {
            return 0.0; // arctan(0) = 0
        }
        return result;
    } else if (strcmp(func, "sqrt") == 0) {
        if (value < 0) {
            printf("Error: cannot calculate square root of a negative number\n");
            exit(1);
        }
        return sqrt(value);
    } else {
        printf("Error: unknown function %s\n", func);
        exit(1);
    }
}
// Apply operator to two values
double applyOperator(double a, double b, char op) {
switch (op) {
case '+':
    return a + b;
case '-':
    return a - b;
case '*':
    return a * b;
case '/':
    if (b == 0) {
        printf("Error: division by zero\n");
        exit(1);
    }
    return a / b;
case '^':
    return pow(a, b);
default:
    printf("Error: unknown operator %c\n", op);
    exit(1);
}
}

// Display help about available operations and functions
void printHelp() {
printf("\n=== CALCULATOR HELP ===\n");
printf("Available arithmetic operations:\n");
printf("  + : addition (example: 2+3)\n");
printf("  - : subtraction (example: 5-2)\n");
printf("  * : multiplication (example: 4*5)\n");
printf("  / : division (example: 10/2)\n");
printf("  ^ : exponentiation (example: 2^3)\n");
printf("  ! : factorial (example: 5!)\n");

printf("\nAvailable constants:\n");
printf("  PI, pi : Pi number (3.14159...)\n");
printf("  E, e   : Euler's number (2.71828...)\n");

printf("\nAvailable trigonometric functions:\n");
printf("  sin  : sine (example: sin(0.5))\n");
printf("  cos  : cosine (example: cos(0.5))\n");
printf("  tg   : tangent (example: tg(0.5))\n");
printf("  tan  : tangent (example: tan(0.5))\n");
printf("  ctg  : cotangent (example: ctg(0.5))\n");
printf("  arcsin : arcsine (example: arcsin(0.5))\n");
printf("  arccos : arccosine (example: arccos(0.5))\n");
printf("  arctg  : arctangent (example: arctg(0.5))\n");
printf("  arctan : arctangent (example: arctan(0.5))\n");
printf("\nOther functions:\n");
printf("  sqrt : square root (example: sqrt(16))\n");
printf("\nSupport for parentheses and variables:\n");
printf("  Parentheses: (2+3)*(4-1)\n");
printf("  Variables: x+y, 2*a+b/c\n");
printf("  The program will ask for variable values during execution.\n");

printf("\nExpression examples:\n");
printf("  2+3*4\n");
printf("  (2+3)*(4-1)\n");
printf("  sin(0.5) + cos(0.3)\n");
printf("  sqrt(16) + 5!\n");
printf("  2^3 + x*y\n\n");
}


// Convert infix expression to postfix
void infixToPostfix(const char* infix, Queue* output) {
Stack operators;
initStack(&operators);

char token[MAX_FUNC_SIZE];
int tokenIndex = 0;
int isUnaryMinus = 1; // Flag for unary minus

for (int i = 0; infix[i] != '\0'; i++) {
char c = infix[i];

// Skip spaces
if (c == ' ') {
    continue;
}

// Process numbers
if (isdigit(c) || c == '.') {
    token[tokenIndex++] = c;
    while (isdigit(infix[i + 1]) || infix[i + 1] == '.') {
        token[tokenIndex++] = infix[++i];
    }
    token[tokenIndex] = '\0';
    tokenIndex = 0;
    
    enqueueValue(output, atof(token));
    isUnaryMinus = 0;
}
// Process variables and functions
else if (isalpha(c)) {
    token[tokenIndex++] = c;
    while (isalnum(infix[i + 1])) {
        token[tokenIndex++] = infix[++i];
    }
    token[tokenIndex] = '\0';
    tokenIndex = 0;
    
    // Check for constants
    if (strcmp(token, "PI") == 0 || strcmp(token, "pi") == 0) {
        enqueueValue(output, PI);
    }
    else if (strcmp(token, "E") == 0 || strcmp(token, "e") == 0) {
        enqueueValue(output, E);
    }
    // Check if this is a function
    else if (isFunction(token)) {
        pushFunction(&operators, token);
    } else {
        // This is a variable
        enqueueVariable(output, token);
    }
    isUnaryMinus = 0;
}
// Process left parenthesis
else if (c == '(') {
    pushOperator(&operators, c);
    isUnaryMinus = 1;
}
// Process right parenthesis
else if (c == ')') {
    while (!isStackEmpty(&operators) && peek(&operators)->operatorr != '(') {
        StackNode* op = pop(&operators);
        if (op->type == 0) {
            enqueueOperator(output, op->operatorr);
        } else if (op->type == 2) {
            enqueueFunction(output, op->function);
        }
        free(op);
    }
    
    if (isStackEmpty(&operators) || peek(&operators)->operatorr != '(') {
        printf("Error: unbalanced parentheses\n");
        exit(1);
    }
    
    // Remove left parenthesis
    free(pop(&operators));
    
    // If there was a function before the parenthesis, add it to the output queue
    if (!isStackEmpty(&operators) && peek(&operators)->type == 2) {
        StackNode* func = pop(&operators);
        enqueueFunction(output, func->function);
        free(func);
    }
    isUnaryMinus = 0;
}
// Process factorial
else if (c == '!') {
    enqueueOperator(output, c);
    isUnaryMinus = 0;
}
// Process operators
else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') {
    // Process unary minus
    if (c == '-' && isUnaryMinus) {
        enqueueValue(output, 0); // Convert unary minus to binary
    }
    
    while (!isStackEmpty(&operators) && 
            peek(&operators)->type == 0 && 
            peek(&operators)->operatorr != '(' && 
           (getPrecedence(peek(&operators)->operatorr) > getPrecedence(c) || 
            (getPrecedence(peek(&operators)->operatorr) == getPrecedence(c) && c != '^'))) {
        StackNode* op = pop(&operators);
        enqueueOperator(output, op->operatorr);
        free(op);
    }
    
    pushOperator(&operators, c);
    isUnaryMinus = 1; // Next minus after an operator might be unary
}
else {
    printf("Error: unknown character %c\n", c);
    exit(1);
}
}

// Add remaining operators to the queue
while (!isStackEmpty(&operators)) {
StackNode* op = pop(&operators);
if (op->type == 0) {
    if (op->operatorr == '(') {
        printf("Error: unbalanced parentheses\n");
        exit(1);
    }
    enqueueOperator(output, op->operatorr);
} else if (op->type == 2) {
    enqueueFunction(output, op->function);
}
free(op);
}
}


// Evaluate postfix expression
double evaluatePostfix(Queue* postfix, VarDict* variables) {
Stack evalStack;
initStack(&evalStack);
Queue tempQueue;  // Create temporary queue to preserve source data
initQueue(&tempQueue);

// Copy all nodes from postfix to tempQueue for preliminary traversal
QueueNode* current = postfix->front;
while (current != NULL) {
if (current->type == 1) {
    if (current->isVariable) {
        enqueueVariable(&tempQueue, current->variable);
    } else {
        enqueueValue(&tempQueue, current->value);
    }
} else if (current->type == 0) {
    enqueueOperator(&tempQueue, current->operatorr);
} else if (current->type == 2) {
    enqueueFunction(&tempQueue, current->function);
}
current = current->next;
}

// Process nodes from the temporary queue
while (!isQueueEmpty(&tempQueue)) {
QueueNode* token = dequeue(&tempQueue);

if (token == NULL) {
    printf("Error: empty token in queue\n");
    exit(1);
}

if (token->type == 1) {
    // Numeric value or variable
    if (token->isVariable) {
        double value = getVariableValue(variables, token->variable);
        pushValue(&evalStack, value);
    } else {
        pushValue(&evalStack, token->value);
    }
} else if (token->type == 0) {
    // Operator
    if (token->operatorr == '!') {
        StackNode* a = pop(&evalStack);
        if (!a) {
            printf("Error: not enough operands for '!'\n");
            exit(1);
        }
        pushValue(&evalStack, factorial(a->value));
        free(a);
    } else {
        StackNode* b = pop(&evalStack);
        StackNode* a = pop(&evalStack);

        if (!a || !b) {
            printf("Error: not enough operands for '%c'\n", token->operatorr);
            exit(1);
        }

        pushValue(&evalStack, applyOperator(a->value, b->value, token->operatorr));
        free(a);
        free(b);
    }
} else if (token->type == 2) {
    // Function
    StackNode* a = pop(&evalStack);
    if (!a) {
        printf("Error: not enough operands for function '%s'\n", token->function);
        exit(1);
    }
    pushValue(&evalStack, applyFunction(token->function, a->value));
    free(a);
}

free(token); // Free token memory
}

// Check for errors in the expression
if (isStackEmpty(&evalStack)) {
printf("Error: empty stack after evaluation\n");
exit(1);
}

if (evalStack.top->next != NULL) {
printf("Error: more than one element left in stack\n");
exit(1);
}

StackNode* result = pop(&evalStack);
double value = result->value;
free(result);

return value;
}


// Free queue memory
void freeQueue(Queue* queue) {
while (!isQueueEmpty(queue)) {
QueueNode* node = dequeue(queue);
free(node);
}
}

// Free stack memory
void freeStack(Stack* stack) {
while (!isStackEmpty(stack)) {
StackNode* node = pop(stack);
free(node);
}
}

// Free variable dictionary memory
void freeVarDict(VarDict* dict) {
VarNode* current = dict->head;
while (current != NULL) {
VarNode* next = current->next;
free(current);
current = next;
}
}

int main() {
char infix[MAX_EXPR_SIZE];
Queue postfix;
VarDict variables;

initVarDict(&variables);

int choice = 0;

do {
printf("\n===== CALCULATOR =====\n");
printf("1. Calculate expression\n");
printf("2. Show operations help\n");
printf("3. Exit\n");
printf("Select action (1-3): ");
scanf("%d", &choice);
getchar(); // Clear buffer after selection

switch (choice) {
    
    case 1: {
        // Use existing infix variable
        initQueue(&postfix);

        printf("\nEnter expression: ");
        fgets(infix, MAX_EXPR_SIZE, stdin);
        infix[strcspn(infix, "\n")] = 0; // Remove newline character

        printf("\nPerforming calculation...\n");
        infixToPostfix(infix, &postfix);
        double result = evaluatePostfix(&postfix, &variables);

        printf("\nResult: %g\n", result);

        // No need to free postfix queue memory here,
        // as evaluatePostfix uses a temporary queue
        break;
    }
    
    case 2:
        printHelp();
        break;
        
    case 3:
        printf("\nCalculator shutting down. Goodbye!\n");
        break;
        
    default:
        printf("\nError! Please choose a number between 1 and 3.\n");
}
} while (choice != 3);

freeVarDict(&variables);

return 0;
}