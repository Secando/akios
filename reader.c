#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define MSG_SIZE 1024
#define MSG_TYPE_1 1
#define MSG_TYPE_2 2

struct message {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct message msg;
    time_t current_time;

    // Генерация ключа и подключение к очереди
    key = ftok("progfile", 65);
    msgid = msgget(key, 0666);

    if (msgid == -1) {
        perror("Failed to access message queue");
        exit(1);
    }

    // Чтение 1-го сообщения
    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), MSG_TYPE_1, 0) != -1) {
        printf("Received Message 1:\n");
        char *token = strtok(msg.msg_text, ",");
        while (token != NULL) {
            printf("%s\n", token);
            token = strtok(NULL, ",");
        }
    } else {
        perror("Failed to receive message 1");
    }

    // Чтение 2-го сообщения и вывод времени
    if (msgrcv(msgid, &msg, sizeof(msg.msg_text), MSG_TYPE_2, 0) != -1) {
        printf("\nReceived Message 2:\n%s\n", msg.msg_text);

        // Получение и вывод текущего времени
        time(&current_time);
        printf("Time of last message received: %s\n", ctime(&current_time));
    } else {
        perror("Failed to receive message 2");
    }

    // Удаление очереди
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Failed to delete message queue");
    } else {
        printf("Message queue deleted successfully.\n");
    }

    return 0;
}
