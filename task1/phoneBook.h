#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define NAME_SIZE 32
#define NUMBER_SIZE 16
#define MAIL_SIZE 32

#define REPETATIVE_NUMBER 100 // how many users with same surname can be stored
#define MAX_USERS 1000
#define DEVICE_NAME "phonebook"

#define BUFFER_SIZE 512

struct user_data {
    char name[NAME_SIZE];
    char surname[NAME_SIZE];
    size_t age;
    char number[NUMBER_SIZE];
    char email[MAIL_SIZE];
};

struct found_users_data {
    struct user_data* users_data;
    size_t found_num;
};

struct found_users_data* get_user_by_surname(const char *surname);
long add_user(struct user_data *user);
long remove_user(const char *surname);

#endif