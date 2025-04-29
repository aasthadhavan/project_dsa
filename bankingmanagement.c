#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<ctype.h>

struct transaction{  //structure to store each transaction
    char type[30];
    int amt;
    time_t time;
    struct transaction* next;

};

struct account{ //structure to represent an account
    char name[50];
    int acc_no;
    int balance;
    int transaction_count;
    struct transaction* history;
};

struct transaction* create_transaction(char type[],int amt){
    struct transaction* new_transac=(struct transaction*)malloc(sizeof(struct transaction)); //allocate mempory to a new ptr- new transac
    strcpy(new_transac->type,type); 
    new_transac->amt=amt; 
    new_transac->time=time(NULL); 
    new_transac->next=NULL; 
    return new_transac; 
}

void add_transaction(struct account* acc,char type[],int amt){
    struct transaction* new_transac=create_transaction(type,amt);
    new_transac->next=acc->history; 
    acc->history=new_transac;
    acc->transaction_count++;
}

int find_account_in_file(struct account* acc, int acc_no); //forward declared 
int update_account_in_file(struct account* acc);
void save_account_data(struct account* acc);

void deposit(struct account* acc,int amt){
    acc->balance+=amt;
    add_transaction(acc,"Deposit",amt);
    printf("\n Deposited Rs. %d\n New Balance: Rs. %d\n",amt,acc->balance);
    update_account_in_file(acc);
}

void withdraw(struct account* acc,int amt){
    if(acc->balance<amt){
        printf("\n Not enough balance!\n");
        return;
    }
    acc->balance-=amt;
    add_transaction(acc,"Withdraw",amt);
    printf("\n Withdrawn Rs.%d\n New Balance: Rs. %d\n",amt,acc->balance);
    update_account_in_file(acc);
}


void transfer(struct account* sender, int amt, int to_accno) {
    if (sender->balance < amt) {
        printf("\nNot enough balance to transfer.\n");
        return;
    }

    struct account receiver;
    if (!find_account_in_file(&receiver, to_accno)) {
        printf("\nReceiver account not found.\n");
        return;
    }

    sender->balance -= amt;
    receiver.balance += amt;

    char msg_sender[50];
    sprintf(msg_sender, "Transfer");
    add_transaction(sender, msg_sender, amt);

    char msg_receiver[50];
    sprintf(msg_receiver, "Transfer");
    add_transaction(&receiver, msg_receiver, amt);

    printf("\nTransferred Rs. %d to account %d\n", amt, to_accno);
    printf("Sender's New Balance: Rs. %d\n", sender->balance);
    printf("Receiver's New Balance: Rs. %d\n", receiver.balance);

    update_account_in_file(sender); // Update sender's account
    update_account_in_file(&receiver); // Update receiver's account
}

void show_account(struct account* acc){
    printf("\n ACCOUNT DETAILS\n--------------------\n");
    printf("Name: %s\n",acc->name);
    printf("Acc No.: %d\n",acc->acc_no);
    printf("Balance: %d\n",acc->balance);
    printf("Transactions: %d\n",acc->transaction_count);
}

void show_history(struct account * acc){
    printf("\n TRANSACTION HISTORY\n--------------------\n");
    if(acc->history==NULL){
        printf("No transactions yet.\n");
        return;
    }
    struct transaction* temp=acc->history;
    while(temp!=NULL){
        printf("%s of Rs. %d on %s",temp->type,temp->amt,ctime(&temp->time));// ctime is a fn to convert time to a human readable str 
        temp=temp->next; 
    }
}

int load_account_data(struct account* acc) {
    FILE* file = fopen("account.txt", "r");
    if (file == NULL) {
        printf("Account file not found, creating a new account.\n");
        return 0; 
    }

    fscanf(file, "%d %s %d %d", &acc->acc_no, acc->name, &acc->balance, &acc->transaction_count);
    acc->history = NULL;  // to read formatted input from a file

    fclose(file);
    return 1; 
}

void chatbot() {
    char query[100];
    printf("\n--- Welcome to BankBot! Ask me anything (type 'exit' to go back) ---\n");

    while (1) {
        printf("\nYou: ");
        
        if (fgets(query, sizeof(query), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }

        
        query[strcspn(query, "\n")] = '\0'; //to remove newline character and set it to null terminator

        
        for (int i = 0; query[i]; i++) {
            query[i] = tolower(query[i]); // converts to lowercase 
        }

        
        if (strstr(query, "deposit")) {
            printf("Bot: To deposit money, go to option 1 from the main menu.\n");
        } else if (strstr(query, "withdraw")) {
            printf("Bot: To withdraw money, use option 2 in the main menu.\n");
        } else if (strstr(query, "transfer")) {
            printf("Bot: You can transfer using option 3 in the menu. You'll need another account number.\n");
        } else if (strstr(query, "balance") || strstr(query, "account")) {
            printf("Bot: Use option 4 to view your account balance and details.\n");
        } else if (strstr(query, "history") || strstr(query, "transaction")) {
            printf("Bot: Option 5 shows your transaction history.\n");
        } else if (strstr(query, "exit")) {
            printf("Bot: Exiting chatbot. Going back to menu.\n");
            break;
        } else {
            printf("Bot: I'm not sure how to help with that. Try asking about deposit, withdraw, transfer, balance or history\n");
        }
    }
}

void save_account_data(struct account* acc) {
    if (update_account_in_file(acc)) {
        return;
    }

    
    FILE* file = fopen("account.txt", "a");
    if (!file) {
        printf("Unable to open account file to save.\n");
        return;
    }

    fprintf(file, "%d %s %d %d\n", acc->acc_no, acc->name, acc->balance, acc->transaction_count);
    struct transaction* t = acc->history;
    while (t != NULL) {
        fprintf(file, "%s %d %ld\n", t->type, t->amt, t->time);
        t = t->next;
    }

    fclose(file);
}


int find_account_in_file(struct account* acc, int acc_no) {
    FILE* file = fopen("account.txt", "r");
    if (!file) {
        printf("Unable to open account file.\n");
        return 0;
    }

    int temp_acc_no;
    char name[50];
    int balance, t_count;

    while (fscanf(file, "%d %49s %d %d", &temp_acc_no, name, &balance, &t_count) == 4) {
        if (temp_acc_no == acc_no) {
            acc->acc_no = temp_acc_no;
            strcpy(acc->name, name);
            acc->balance = balance;
            acc->transaction_count = t_count;
            acc->history = NULL;

            fgetc(file);  

            for (int i = 0; i < t_count; i++) {
                char type[30];
                int amt;
                time_t time_val;
                if (fscanf(file, "%29s %d %ld", type, &amt, &time_val) == 3) {
                    struct transaction* t = (struct transaction*)malloc(sizeof(struct transaction));
                    strcpy(t->type, type);
                    t->amt = amt;
                    t->time = time_val;
                    t->next = acc->history;
                    acc->history = t;
                }
            }
            fclose(file);
            return 1; 
        }

        for (int i = 0; i < t_count; i++) {
            char type[30];
            int amt;
            long time_val;
            fscanf(file, "%s %d %ld", type, &amt, &time_val);
        }
    }

    fclose(file);
    return 0;  
}

int update_account_in_file(struct account* acc) {
    FILE* original_file = fopen("account.txt", "r");
    FILE* temp_file = fopen("temp.txt", "w");
    int found = 0;

    if (!original_file || !temp_file) {
        perror("Error opening files");
        if (original_file) fclose(original_file);
        if (temp_file) fclose(temp_file);
        return 0;
    }

    int temp_acc_no, balance, t_count, read_result;
    char name[50];

    while ((read_result = fscanf(original_file, "%d %49s %d %d", &temp_acc_no, name, &balance, &t_count)) == 4) {
        if (temp_acc_no == acc->acc_no) {
            
            fprintf(temp_file, "%d %s %d %d\n", acc->acc_no, acc->name, acc->balance, acc->transaction_count);
            struct transaction* t = acc->history;
            while (t != NULL) {
                fprintf(temp_file, "%s %d %ld\n", t->type, t->amt, t->time);
                t = t->next;
            }
            
            for (int i = 0; i < t_count; i++) {
                char dummy_type[30];
                int dummy_amt;
                long dummy_time;
                fscanf(original_file, "%s %d %ld", dummy_type, &dummy_amt, &dummy_time);
            }
            found = 1;
        } else {
            
            fprintf(temp_file, "%d %s %d %d\n", temp_acc_no, name, balance, t_count);
          
            for (int i = 0; i < t_count; i++) {
                char type[30];
                int amt;
                long time_val;
                if (fscanf(original_file, "%29s %d %ld", type, &amt, &time_val) == 3) {
                    fprintf(temp_file, "%s %d %ld\n", type, amt, time_val);
                }
            }
        }
    }

    

    fclose(original_file);
    fclose(temp_file);

    if (remove("account.txt") == 0) {
        if (rename("temp.txt", "account.txt") != 0) {
            perror("Error renaming temp file");
            return found;
        }
    } else {
        perror("Error removing original file");
    }

    return found;
}

int main(){
    struct account user;
    user.transaction_count = 0;
    user.history = NULL;
    int option;
    printf("Welcome to the Bank System\n");
    printf("1. Login to Existing Account\n");
    printf("2. Create New Account\n");
    printf("Enter your choice: ");
    scanf("%d", &option);

    if (option == 1) {
        printf("Enter your account number: ");
        scanf("%d", &user.acc_no);
        
        if (!find_account_in_file(&user, user.acc_no)) {
            printf("Account not found. Please make sure the account exists.\n");
            return 1;
        }
        printf("You are successfully logged in to your account, %s!\n",user.name);
    } else if (option == 2) {
        printf("Enter your name: ");
        scanf(" %[^\n]", user.name); 
        printf("Choose a unique account number: ");
        scanf("%d", &user.acc_no);

        if (find_account_in_file(&user, user.acc_no)) {
            printf("Account number already exists. Please try again.\n");
            return 1;
        }

        user.balance = 0;
        user.transaction_count = 0;
        user.history = NULL;

        save_account_data(&user);
        printf("Account created successfully!\n");
    } else {
        printf("Invalid option. Exiting.\n");
        return 1;
    }

    int choice, amount, to_acc;

    while (1) {
        printf("\n========== MENU ==========\n");
        printf("1. Deposit Money\n");
        printf("2. Withdraw Money\n");
        printf("3. Transfer Money\n");
        printf("4. View Account Info\n");
        printf("5. View Transaction History\n");
        printf("6. Ask BankBot (Chatbot)\n");  // NEW
        printf("7. Exit\n");
        printf("==========================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
    
        switch (choice) {
            case 1:
                printf("Enter amount to deposit: ");
                scanf("%d", &amount);
                deposit(&user, amount);
                update_account_in_file(&user); 
                break;
    
            case 2:
                printf("Enter amount to withdraw: ");
                scanf("%d", &amount);
                withdraw(&user, amount);
                update_account_in_file(&user); 
                break;
    
            case 3:
                printf("Enter account number to transfer to: ");
                scanf("%d", &to_acc);
                printf("Enter amount to transfer: ");
                scanf("%d", &amount);
                transfer(&user, amount, to_acc);
                break;
    
            case 4:
                show_account(&user);
                break;
    
            case 5:
                show_history(&user);
                break;
    
            case 6: 
                while ((getchar()) != '\n'); 
                chatbot(); 
                break;
    
            case 7:
                update_account_in_file(&user);
                printf("Exiting. Thank you for using the bank!\n");
                return 0;
    
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    

}