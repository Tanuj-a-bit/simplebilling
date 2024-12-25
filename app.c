#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_QUEUE_SIZE 5

// Product structure for tree node
typedef struct Product {
    int id;
    char name[MAX_NAME_LENGTH];
    float price;
    struct Product *left, *right;
} Product;

// Bill item structure for linked list
typedef struct BillItem {
    int id;
    char name[MAX_NAME_LENGTH];
    int quantity;
    float price;
    struct BillItem *next;
} BillItem;

// Stack node for undo operation
typedef struct StackNode {
    BillItem *billItem;
    struct StackNode *next;
} StackNode;

// Queue for recent transactions
typedef struct QueueNode {
    char transaction[MAX_NAME_LENGTH];
    struct QueueNode *next;
} QueueNode;

// Globals for queue
QueueNode *front = NULL, *rear = NULL;
int queueSize = 0;

// Globals for bill and stack
BillItem *billHead = NULL;
StackNode *undoStack = NULL;

// Function prototypes
Product* insertProduct(Product *root, int id, char *name, float price);
void displayProducts(Product *root);
BillItem* addItemToBill(int id, char *name, int quantity, float price);
void displayBill();
void pushUndo(BillItem *item);
BillItem* popUndo();
void addTransaction(char *transaction);
void displayRecentTransactions();

// Helper function to create a new product
Product* createProduct(int id, char *name, float price) {
    Product *newProduct = (Product *)malloc(sizeof(Product));
    newProduct->id = id;
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->left = newProduct->right = NULL;
    return newProduct;
}

// Insert product into the product tree
Product* insertProduct(Product *root, int id, char *name, float price) {
    if (!root) return createProduct(id, name, price);
    if (id < root->id) {
        root->left = insertProduct(root->left, id, name, price);
    } else if (id > root->id) {
        root->right = insertProduct(root->right, id, name, price);
    }
    return root;
}

// Display all products in the catalog
void displayProducts(Product *root) {
    if (!root) return;
    displayProducts(root->left);
    printf("ID: %d, Name: %s, Price: %.2f\n", root->id, root->name, root->price);
    displayProducts(root->right);
}

// Add item to the bill
BillItem* addItemToBill(int id, char *name, int quantity, float price) {
    BillItem *newItem = (BillItem *)malloc(sizeof(BillItem));
    newItem->id = id;
    strcpy(newItem->name, name);
    newItem->quantity = quantity;
    newItem->price = price * quantity;
    newItem->next = billHead;
    billHead = newItem;

    // Add to undo stack
    pushUndo(newItem);

    // Record transaction
    char transaction[MAX_NAME_LENGTH];
    snprintf(transaction, MAX_NAME_LENGTH, "Added: %s x%d", name, quantity);
    addTransaction(transaction);

    return newItem;
}

// Display the bill
void displayBill() {
    if (!billHead) {
        printf("No items in the bill.\n");
        return;
    }
    BillItem *current = billHead;
    float total = 0;
    printf("\nBill:\n");
    printf("ID\tName\t\tQty\tPrice\n");
    while (current) {
        printf("%d\t%-10s\t%d\t%.2f\n", current->id, current->name, current->quantity, current->price);
        total += current->price;
        current = current->next;
    }
    printf("Total: %.2f\n", total);
}

// Push an item to the undo stack
void pushUndo(BillItem *item) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->billItem = item;
    newNode->next = undoStack;
    undoStack = newNode;
}

// Pop an item from the undo stack
BillItem* popUndo() {
    if (!undoStack) return NULL;
    StackNode *temp = undoStack;
    undoStack = undoStack->next;
    BillItem *item = temp->billItem;
    free(temp);
    return item;
}

// Add a transaction to the queue
void addTransaction(char *transaction) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    strcpy(newNode->transaction, transaction);
    newNode->next = NULL;

    if (queueSize == MAX_QUEUE_SIZE) {
        QueueNode *temp = front;
        front = front->next;
        free(temp);
        queueSize--;
    }

    if (!rear) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    queueSize++;
}

// Display recent transactions
void displayRecentTransactions() {
    if (!front) {
        printf("No recent transactions.\n");
        return;
    }
    QueueNode *current = front;
    printf("Recent Transactions:\n");
    while (current) {
        printf("%s\n", current->transaction);
        current = current->next;
    }
}

// Main function
int main() {
    Product *catalog = NULL;

    // Add realistic products to the catalog
    catalog = insertProduct(catalog, 1, "Apple", 100);
    catalog = insertProduct(catalog, 2, "Banana", 50);
    catalog = insertProduct(catalog, 3, "Orange", 75);
    catalog = insertProduct(catalog, 4, "Milk (1L)", 50);
    catalog = insertProduct(catalog, 5, "Bread", 20);
    catalog = insertProduct(catalog, 6, "Rice (1kg)", 75.0);
    catalog = insertProduct(catalog, 7, "Eggs (12)", 25);
    catalog = insertProduct(catalog, 8, "Chicken (1kg)", 500.0);
    catalog = insertProduct(catalog, 9, "Tomato (1kg)", 30);
    catalog = insertProduct(catalog, 10, "Potato (1kg)", 60.0);
    catalog = insertProduct(catalog, 11, "Cheese (200g)", 140);
    catalog = insertProduct(catalog, 12, "Butter (200g)", 120);

    int choice, id, quantity;

    while (1) {
        printf("\n--- Billing System ---\n");
        printf("1. View Catalog\n");
        printf("2. Add Item to Bill\n");
        printf("3. View Bill\n");
        printf("4. Undo Last Item\n");
        printf("5. View Recent Transactions\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("\n--- Product Catalog ---\n");
                displayProducts(catalog);
                break;
            case 2:
                printf("Enter Product ID, Quantity: ");
                scanf("%d %d", &id, &quantity);
                Product *current = catalog;
                while (current && current->id != id) {
                    current = (id < current->id) ? current->left : current->right;
                }
                if (current) {
                    addItemToBill(current->id, current->name, quantity, current->price);
                } else {
                    printf("Product not found.\n");
                }
                break;
            case 3:
                displayBill();
                break;
            case 4:
                if (popUndo()) {
                    printf("Last item undone.\n");
                } else {
                    printf("Nothing to undo.\n");
                }
                break;
            case 5:
                displayRecentTransactions();
                break;
            case 6:
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
