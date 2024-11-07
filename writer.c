#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <dirent.h>
#include <time.h>

#define MSG_SIZE 1024
#define MSG_TYPE_1 1
#define MSG_TYPE_2 2

// Структура сообщения
struct message {
    long msg_type;
    char msg_text[MSG_SIZE];
};

// Функция для создания списка файлов *.conf в /etc
void list_conf_files(char *buffer) {
    struct dirent *entry;
    DIR *dir = opendir("/etc");

    if (dir == NULL) {
        perror("Could not open /etc directory");
        exit(1);
    }

    buffer[0] = '\0';
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".conf") != NULL) {
            strcat(buffer, entry->d_name);
            strcat(buffer, ",");
        }
    }
    closedir(dir);
    buffer[strlen(buffer) - 1] = '\0'; // Удаляем последнюю запятую
}

// Функция для создания списка каталогов в /dev
void list_dev_directories(char *buffer) {
    struct dirent *entry;
    DIR *dir = opendir("/dev");

    if (dir == NULL) {
        perror("Could not open /dev directory");
        exit(1);
    }

    buffer[0] = '\0';
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            strcat(buffer, entry->d_name);
            strcat(buffer, ",");
        }
    }
    closedir(dir);
    buffer[strlen(buffer) - 1] = '\0'; // Удаляем последнюю запятую
}

int main() {
    key_t key;
    int msgid;
    struct message msg;

    // Генерация ключа и создание очереди
    key = ftok("progfile", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1) {
        perror("Failed to create message queue");
        exit(1);
    }

    // Создание 1-го сообщения
    msg.msg_type = MSG_TYPE_1;
    snprintf(msg.msg_text, MSG_SIZE, "writer %s, ", "%STUDENT_ID%"); // Замените %STUDENT_ID% на свой идентификатор
    list_conf_files(msg.msg_text + strlen(msg.msg_text));
    msgsnd(msgid, &msg, sizeof(msg.msg_text), 0);

    // Создание 2-го сообщения
    msg.msg_type = MSG_TYPE_2;
    list_dev_directories(msg.msg_text);
    msgsnd(msgid, &msg, sizeof(msg.msg_text), 0);

    printf("Messages sent to queue.\n");

    return 0;
}
