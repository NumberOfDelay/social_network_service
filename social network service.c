#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_USERS 100
#define MAX_POSTS 10
#define MAX_MESSAGES 10
#define MAX_LENGTH 100

typedef struct Post {
    char title[MAX_LENGTH];
    char content[MAX_LENGTH];
    int likes;
    struct Post* next;
} Post;

typedef struct Message {
    char sender[MAX_LENGTH];
    char message[MAX_LENGTH];
    struct Message* next;
} Message;

typedef struct FriendNode {
    int friendId;
    struct FriendNode* next;
} FriendNode;

typedef struct User {
    char id[MAX_LENGTH];
    char password[MAX_LENGTH];
    char name[MAX_LENGTH];
    char birthdate[MAX_LENGTH];
    char gender[MAX_LENGTH];
    char city[MAX_LENGTH];
    Post* posts;
    FriendNode* friends;
    Message* messages;
    int postCount;
    int friendCount;
    int messageCount;
} User;

User users[MAX_USERS];
int userCount = 0;
int loggedInUser = -1;

void clearScreen();
void setColor(int color);
void gotoXY(int x, int y);
void drawBox(int x, int y, int width, int height);
void loadUsers();
void saveUsers();
void signUp();
void viewPosts(int userId);
void writePost();
void signIn();
void signOut();
void viewProfile();
void addFriend();
void viewFriendsPosts();
void manageFriends();
void mainMenu();

enum ConsoleColors {
    CC_RED = 12,
    CC_GREEN = 10,
    CC_YELLOW = 14,
    CC_CYAN = 11,
    CC_MAGENTA = 13,
    CC_WHITE = 15
};

void clearScreen() {
    system("cls");
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void drawBox(int x, int y, int width, int height) {
    gotoXY(x, y);
    printf("┏");

    for (int i = 1; i < width - 1; i++) {
        printf("━");
    }
    printf("┓");
    gotoXY(x, y + height - 1);
    printf("┗");

    for (int i = 1; i < width - 1; i++) {
        printf("━");
    }
    printf("┛");

    for (int i = y + 1; i < y + height - 1; i++) {
        gotoXY(x, i);
        printf("┃");
        gotoXY(x + width - 1, i);
        printf("┃");
    }
}

void loadUsers() {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) return;

    while (fread(&users[userCount], sizeof(User), 1, file)) {
        users[userCount].posts = NULL;
        for (int i = 0; i < users[userCount].postCount; i++) {
            Post* newPost = (Post*)malloc(sizeof(Post));
            fread(newPost, sizeof(Post), 1, file);
            newPost->next = users[userCount].posts;
            users[userCount].posts = newPost;
        }

        users[userCount].friends = NULL;
        for (int i = 0; i < users[userCount].friendCount; i++) {
            FriendNode* newFriend = (FriendNode*)malloc(sizeof(FriendNode));
            fread(&(newFriend->friendId), sizeof(int), 1, file);
            newFriend->next = users[userCount].friends;
            users[userCount].friends = newFriend;
        }

        users[userCount].messages = NULL;
        for (int i = 0; i < users[userCount].messageCount; i++) {
            Message* newMessage = (Message*)malloc(sizeof(Message));
            fread(newMessage, sizeof(Message), 1, file);
            newMessage->next = users[userCount].messages;
            users[userCount].messages = newMessage;
        }

        userCount++;
    }
    fclose(file);
}

void saveUsers() {
    FILE* file = fopen("users.txt", "w");
    if (file == NULL) return;

    for (int i = 0; i < userCount; i++) {
        fwrite(&users[i], sizeof(User), 1, file);

        Post* currentPost = users[i].posts;
        while (currentPost != NULL) {
            fwrite(currentPost, sizeof(Post), 1, file);
            currentPost = currentPost->next;
        }

        FriendNode* currentFriend = users[i].friends;
        while (currentFriend != NULL) {
            fwrite(&(currentFriend->friendId), sizeof(int), 1, file);
            currentFriend = currentFriend->next;
        }

        Message* currentMessage = users[i].messages;
        while (currentMessage != NULL) {
            fwrite(currentMessage, sizeof(Message), 1, file);
            currentMessage = currentMessage->next;
        }
    }
    fclose(file);
}

void signUp() {
    if (userCount >= MAX_USERS) {
        clearScreen();
        drawBox(10, 5, 50, 3);
        gotoXY(12, 6); printf("더 이상 사용자를 추가할 수 없습니다.\n");
        Sleep(2000);
        return;
    }

    User newUser;
    clearScreen();
    drawBox(10, 5, 50, 12);

    gotoXY(12, 6); printf("ID: ");
    gotoXY(12, 7); printf("Password: ");
    gotoXY(12, 8); printf("이름: ");
    gotoXY(12, 9); printf("생년월일(YYYYMMDD): ");
    gotoXY(12, 10); printf("성별: ");
    gotoXY(12, 11); printf("도시: ");

    gotoXY(16, 6); scanf("%s", newUser.id);
    gotoXY(22, 7); scanf("%s", newUser.password);
    gotoXY(18, 8); scanf("%s", newUser.name);
    gotoXY(32, 9); scanf("%s", newUser.birthdate);
    gotoXY(18, 10); scanf("%s", newUser.gender);
    gotoXY(18, 11); scanf("%s", newUser.city);

    newUser.posts = NULL;
    newUser.friends = NULL;
    newUser.messages = NULL;
    newUser.postCount = 0;
    newUser.friendCount = 0;
    newUser.messageCount = 0;

    users[userCount++] = newUser;
    saveUsers();

    clearScreen();
    drawBox(10, 5, 50, 3);
    gotoXY(12, 6); printf("회원가입이 완료되었습니다!\n");
    Sleep(2000);
}

void writePost() {
    clearScreen();
    drawBox(10, 5, 50, 7);

    gotoXY(12, 6); printf("제목: ");
    gotoXY(12, 7); printf("내용: ");

    char title[MAX_LENGTH], content[MAX_LENGTH];
    gotoXY(19, 6); scanf("%s", title);
    gotoXY(19, 7); scanf("%s", content);

    Post* newPost = (Post*)malloc(sizeof(Post));
    strcpy(newPost->title, title);
    strcpy(newPost->content, content);
    newPost->likes = 0;
    newPost->next = NULL;

    newPost->next = users[loggedInUser].posts;
    users[loggedInUser].posts = newPost;
    users[loggedInUser].postCount++;

    saveUsers();

    clearScreen();
    drawBox(10, 5, 50, 3);
    gotoXY(12, 6); printf("게시글이 작성되었습니다!\n");
    Sleep(2000);
}

void viewPosts(int userId) {
    clearScreen();
    drawBox(10, 5, 50, 20);

    int count = 0;
    Post* currentPost = users[userId].posts;
    while (currentPost != NULL && count < MAX_POSTS) {
        gotoXY(12, 6 + count * 4); printf("제목: %s", currentPost->title);
        gotoXY(12, 7 + count * 4); printf("내용: %s", currentPost->content);
        gotoXY(12, 8 + count * 4); printf("♥: %d", currentPost->likes);
        count++;
        currentPost = currentPost->next;
    }

    gotoXY(12, 25); printf("[Enter] 돌아가기");

    while (getchar() != '\n');
    getchar();

    clearScreen();
}

void signIn() {
    clearScreen();
    drawBox(10, 5, 50, 5);

    char id[MAX_LENGTH], password[MAX_LENGTH];
    gotoXY(12, 6); printf("ID: ");
    gotoXY(12, 7); printf("Password: ");

    gotoXY(16, 6); scanf("%s", id);
    gotoXY(22, 7); scanf("%s", password);

    int found = 0;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, id) == 0 && strcmp(users[i].password, password) == 0) {
            found = 1;
            loggedInUser = i;
            break;
        }
    }

    if (found) {
        clearScreen();
        drawBox(10, 5, 50, 3);
        gotoXY(12, 6); printf("%s님 환영합니다!\n", users[loggedInUser].name);
        Sleep(2000);
    }
    else {
        clearScreen();
        drawBox(10, 5, 50, 3);
        gotoXY(12, 6); printf("ID 또는 비밀번호가 올바르지 않습니다.\n");
        Sleep(2000);
    }
}

void signOut() {
    loggedInUser = -1;
    clearScreen();
    drawBox(10, 5, 50, 3);
    gotoXY(12, 6); printf("로그아웃되었습니다.\n");
    Sleep(2000);
}

void viewProfile() {
    clearScreen();
    drawBox(10, 5, 50, 12);

    gotoXY(12, 6); printf("ID: %s\n", users[loggedInUser].id);
    gotoXY(12, 7); printf("이름: %s\n", users[loggedInUser].name);
    gotoXY(12, 8); printf("생년월일: %s\n", users[loggedInUser].birthdate);
    gotoXY(12, 9); printf("성별: %s\n", users[loggedInUser].gender);
    gotoXY(12, 10); printf("도시: %s\n", users[loggedInUser].city);

    gotoXY(12, 17); printf("[Enter] 돌아가기");

    while (getchar() != '\n');
    getchar();

    clearScreen();
}

void addFriend() {
    clearScreen();
    drawBox(10, 5, 50, 5);

    char friendId[MAX_LENGTH];
    gotoXY(12, 6); printf("친구 ID: ");
    gotoXY(21, 6); scanf("%s", friendId);

    int found = 0;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, friendId) == 0) {
            found = 1;

            FriendNode* currentFriend = users[loggedInUser].friends;
            while (currentFriend != NULL) {
                if (currentFriend->friendId == i) {
                    clearScreen();
                    drawBox(10, 5, 50, 3);
                    gotoXY(12, 6); printf("이미 친구입니다.\n");
                    Sleep(2000);
                    return;
                }
                currentFriend = currentFriend->next;
            }

            FriendNode* newFriend = (FriendNode*)malloc(sizeof(FriendNode));
            newFriend->friendId = i;
            newFriend->next = users[loggedInUser].friends;
            users[loggedInUser].friends = newFriend;
            users[loggedInUser].friendCount++;

            clearScreen();
            drawBox(10, 5, 50, 3);
            gotoXY(12, 6); printf("%s님이 친구로 추가되었습니다.\n", users[i].name);
            Sleep(2000);
            break;
        }
    }

    if (!found) {
        clearScreen();
        drawBox(10, 5, 50, 3);
        gotoXY(12, 6); printf("해당 ID의 사용자를 찾을 수 없습니다.\n");
        Sleep(2000);
    }
}

void viewFriendsPosts() {
    clearScreen();
    drawBox(10, 5, 50, 20);

    int postNumber = 1;
    int count = 0;
    FriendNode* currentFriend = users[loggedInUser].friends;

    while (currentFriend != NULL && count < MAX_POSTS) {
        Post* currentPost = users[currentFriend->friendId].posts;

        while (currentPost != NULL && count < MAX_POSTS) {
            gotoXY(12, 6 + count * 4); printf("%d.", postNumber);
            gotoXY(16, 6 + count * 4); printf("작성자: %s", users[currentFriend->friendId].name);
            gotoXY(12, 7 + count * 4); printf("제목: %s", currentPost->title);
            gotoXY(12, 8 + count * 4); printf("내용: %s", currentPost->content);
            gotoXY(12, 9 + count * 4); printf("♥: %d", currentPost->likes);
            count++;
            postNumber++;
            currentPost = currentPost->next;
        }

        currentFriend = currentFriend->next;
    }

    gotoXY(12, 25); printf("좋아요를 누를 게시글 번호 입력 (0: 돌아가기): ");

    int selectedNumber;
    scanf("%d", &selectedNumber);

    if (selectedNumber > 0 && selectedNumber < postNumber) {

        postNumber = 1;
        count = 0;
        currentFriend = users[loggedInUser].friends;

        while (currentFriend != NULL) {
            Post* currentPost = users[currentFriend->friendId].posts;

            while (currentPost != NULL) {
                count++;
                if (count == selectedNumber) {
                    currentPost->likes++;
                    clearScreen();
                    drawBox(10, 5, 50, 3);
                    gotoXY(12, 6); printf("게시글에 좋아요를 눌렀습니다!\n");
                    Sleep(2000);
                    saveUsers();
                    return;
                }
                currentPost = currentPost->next;
            }
            currentFriend = currentFriend->next;
        }
    }

    clearScreen();
}

void manageFriends() {
    while (1) {
        clearScreen();
        drawBox(10, 5, 50, 12);

        gotoXY(12, 6); printf("1. 친구 추가\n");
        gotoXY(12, 7); printf("2. 친구 목록\n");
        gotoXY(12, 8); printf("3. 친구의 게시글 보기\n");
        gotoXY(12, 9); printf("4. 메인 메뉴로 돌아가기\n");
        gotoXY(12, 11); printf("선택: ");

        char choice;
        gotoXY(20, 11); scanf(" %c", &choice);

        switch (choice) {
        case '1':
            addFriend();
            break;
        case '2':
            clearScreen();
            drawBox(10, 5, 50, 20);

            gotoXY(12, 6); printf("친구 목록:\n");

            FriendNode* currentFriend = users[loggedInUser].friends;
            int count = 1;
            while (currentFriend != NULL) {
                gotoXY(12, 6 + count); printf("%d. %s\n", count, users[currentFriend->friendId].name);
                currentFriend = currentFriend->next;
                count++;
            }

            gotoXY(12, 25); printf("[Enter] 돌아가기");

            while (getchar() != '\n');
            getchar();

            break;
        case '3':
            viewFriendsPosts();
            break;
        case '4':
            return;
        default:
            clearScreen();
            drawBox(10, 5, 50, 3);
            gotoXY(12, 6); printf("잘못된 선택입니다.\n");
            Sleep(2000);
            break;
        }
    }
}

void mainMenu() {
    int choice;

    while (1) {
        clearScreen();
        drawBox(10, 5, 50, 12);

        if (loggedInUser == -1) {
            gotoXY(12, 6); printf("1. 회원가입\n");
            gotoXY(12, 7); printf("2. 로그인\n");
            gotoXY(12, 8); printf("3. 종료\n");
            gotoXY(12, 10); printf("선택: ");
            gotoXY(20, 10); scanf("%d", &choice);

            switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                signIn();
                break;
            case 3:
                printf("\n\n\n\n\n");
                exit(0);
            default:
                clearScreen();
                drawBox(10, 5, 50, 3);
                gotoXY(12, 6); printf("잘못된 선택입니다.\n");
                Sleep(2000);
            }
        }
        else {
            gotoXY(12, 6); printf("1. 게시글 작성\n");
            gotoXY(12, 7); printf("2. 내 게시글 보기\n");
            gotoXY(12, 8); printf("3. 친구 관리\n");
            gotoXY(12, 9); printf("4. 내 프로필 보기\n");
            gotoXY(12, 10); printf("5. 로그아웃\n");
            gotoXY(12, 12); printf("선택: ");
            gotoXY(20, 12); scanf("%d", &choice);

            switch (choice) {
            case 1:
                writePost();
                break;
            case 2:
                viewPosts(loggedInUser);
                break;
            case 3:
                manageFriends();
                break;
            case 4:
                viewProfile();
                break;
            case 5:
                signOut();
                break;
            default:
                clearScreen();
                drawBox(10, 5, 50, 3);
                gotoXY(12, 6); printf("잘못된 선택입니다.\n");
                Sleep(2000);
            }
        }
    }
}

int main() {
    loadUsers();
    mainMenu();
    saveUsers();
    return 0;
}