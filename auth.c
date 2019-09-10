#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct account
{
    char username[30];
    char password[30];
    int isLogin;
    int status;
};

struct node
{
    struct account data;
    struct node * next;
};

struct node * head = NULL;
struct node * tail = NULL;


int menu() {
    printf("USER MANAGEMENT PROGRAM\n---------------------------------------------\n1.Register\n2.Sign in\n3.Search\n4.Sign Out\nYour choice (1-4, other to quit):\n");
    char s[10];
    scanf("%[^\n]%*c", s);
    int c = (int)atoi(s);
    return c;
}

struct node * makeNode(struct account data) {
    struct node * new = (struct node *)malloc(sizeof(struct node));
    new->data = data;
    new->next = NULL;
    return new;
}

void add(struct node * new) {
    if (head == NULL) {
        head = new;
        tail = head;
    } else {
        tail->next = new;
        tail = tail->next;
    }
    
}

struct node * existedUsername(char * username) {
    struct node * i = head;
    while (i!=NULL) {
        if (!strcmp(username, i->data.username)) {
            return i;
        }

        i = i->next;
    }

    return NULL;
}

struct node * auth(char * username, char * password) {
    struct node * i = head;
    while (i!=NULL) {
        if (!strcmp(username, i->data.username) && !(strcmp(password, i->data.password))) {
            return i;
        }

        i = i->next;
    }

    return NULL;
}

void read(char * filename) {
    FILE * f;
    f = fopen(filename, "r+");
    printf("...reading...\n");
    if (f == NULL) perror("ERROR");
    else {
        do {
            struct account temp;
            temp.isLogin = 0;
            if (fscanf(f, "%s %s %d", temp.username, temp.password, &temp.status) == EOF) break;
            else {
                struct node * t = makeNode(temp);
                add(t);
            }
        } while (fgetc(f)!=EOF);
    }

    printf("...done...\n");
    fclose(f);
}

void put() {
    struct node * i = head;
    printf("--------ACCOUNT-LIST---------\n");
    while (i!=NULL) {
        printf("%s\n", i->data.username);

        i = i->next;
    }
}

void appendTo(char * filename, struct account temp) {
    FILE * f;
    f = fopen(filename, "a");
    if (f == NULL) perror("ERROR");
    else {
        fprintf(f, "%s %s %d\n", temp.username, temp.password, temp.status);
    }
    fclose(f);
}

void updateTo(char * filename) {
    FILE * f;
    f = fopen(filename, "w+");
    if (f == NULL) perror("ERROR");
    else {
        struct node * i = head;
        while (i!=NULL) {
            fprintf(f, "%s %s %d\n", i->data.username, i->data.password, i->data.status);

            i = i->next;
        }
    }
    fclose(f);
}

void Register() {
    printf("-----------REGISTER-------\n"); 
    struct account temp;
    temp.status = 1;
    printf("username:");
    scanf("%s%*c", temp.username);
    if (!existedUsername(temp.username)) {
        printf("password:");
        scanf("%s%*c", temp.password);
        struct node * t = makeNode(temp);
        add(t);
        appendTo("account.txt", temp);

        printf("Successful registration\n\n");
    } else {
        printf("Account existed\n\n");
    }
    
}

void SignIn() {
    printf("-----------SIGN-IN-------\n"); 
    struct account temp;
    printf("username:");
    scanf("%s%*c", temp.username);

    struct node * u = existedUsername(temp.username);

    if (!u) {
        printf("Cannot find account\n\n");
        return;
    } else if (u->data.status == 0) {
        printf("Account is blocked\n\n");
        return;
    }

    int error_times = 0;

    do {
        printf("password:");
        scanf("%s%*c", temp.password);
        struct node * t = auth(temp.username, temp.password);
        if (!t) {
            printf("Password is incorrect\n\n");
            error_times ++;
        } else {
            t->data.isLogin = 1;
            updateTo("account.txt");
            printf("Hello %s\n\n", temp.username);
            return;
        }
    } while (error_times<3);

    u->data.status = 0;
    updateTo("account.txt");
    printf("Password is incorrect. Account is blocked\n\n");
    
}

void Search() {
    printf("-----------SEARCH-------\n"); 
    struct account temp;
    temp.status = 0;
    printf("username:");
    scanf("%s%*c", temp.username);
    struct node * t = existedUsername(temp.username);
    if (t) {
        if (t->data.status) {
            printf("Account is active\n\n");
        } else {
            printf("Account is blocked\n\n");
        }
    } else {
        printf("Cannot find account\n\n");
    }
}

void SignOut() {
    printf("-----------SIGN-OUT-------\n"); 
    struct account temp;
    temp.status = 0;
    printf("username:");
    scanf("%s%*c", temp.username);
    struct node * t = existedUsername(temp.username);
    if (t) {
        if (t->data.isLogin == 0) printf("Account is not sign in\n\n");
        else {
            t->data.isLogin = 0;
            printf("Goodbye %s\n\n", t->data.username);
        }
    }
}

int main(int argc, char const *argv[])
{
    read("account.txt");

    int c;
    while (c!=6)
    {
        c = menu();
        switch (c)
        {
            case 1: Register();
            break;
            case 2: SignIn();
            break;
            case 3: Search();
            break;
            case 4: SignOut();
            break;
            default: exit(0);
        }
    }
    return 0;
}
