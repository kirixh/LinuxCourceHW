#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/syscall.h>
#include <unistd.h>

#include "phoneBook.h"

#define DEVICE_NAME "/dev/phonebook"

enum commands {
    ADD = 1,
    DEL = 2,
    GET = 3
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    char surname[NAME_SIZE];
    commands command;
    struct user_data user = {
        .first_name = "Kirill",
        .surname = "piatk",
        .age = 20,
        .number = "23424",
        .email = "kir@gmail.com"
    };

    command = atoi(argv[1]);

    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the phonebook\n");
        return errno;
    }

    switch (command) {
        case ADD: {
            ret = syscall(451, &user);
            if (ret < 0) {
                perror("Failed to add user\n");
            }
            printf("User added successfully\n\n");
            break;
        }
            
        case DEL: {
            ret = syscall(452, surname, 5);
            if (ret == 0) {
                printf("Removed successfully\n\n");
            }
            else {
                perror("Failed to remove user\n");
            }
            break;
        }
            
        case GET: {
            ret = syscall(453, surname, 5, &user);
            if (ret < 0) {
                perror("Error while getting user\n");
            }
            else {
                if (user && user.age > 0) {
                    printf("Name: %s\nSurname: %s\nAge: %zu\nPhone number: %s\nEmail: %s\n\n",
                     user.name, user.surname, user.age, user.number, user.email);
                }
                else {
                    printf("No such user in phonebook\n\n");
                }
            }
            break;
        }
            
        default:
            printf("Invalid command...\n");
            break;
    }

    close(fd);

    return 0;
}
