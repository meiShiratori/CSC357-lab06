#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pipe_parent_child1[2], pipe_child1_child2[2], pipe_child2_parent[2];

    if (pipe(pipe_parent_child1) == -1 || pipe(pipe_child1_child2) == -1 || pipe(pipe_child2_parent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        close(pipe_parent_child1[1]);
        close(pipe_child1_child2[0]);
        close(pipe_child2_parent[0]);
        close(pipe_child2_parent[1]);


        int num;
        while (read(pipe_parent_child1[0], &num, sizeof(num)) > 0) {
            num = num * num;
            write(pipe_child1_child2[1], &num, sizeof(num));
        }

        close(pipe_parent_child1[0]);
        close(pipe_child1_child2[1]);
        exit(EXIT_SUCCESS);
    } else {
        pid_t pid2 = fork();
        if (pid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            close(pipe_parent_child1[0]);
            close(pipe_parent_child1[1]);
            close(pipe_child1_child2[1]);
            close(pipe_child2_parent[0]);


            int num;
            while (read(pipe_child1_child2[0], &num, sizeof(num)) > 0) {
                num = num + 1;
                write(pipe_child2_parent[1], &num, sizeof(num));
            }

            close(pipe_child1_child2[0]);
            close(pipe_child2_parent[1]);
            exit(EXIT_SUCCESS);
        } else {
            close(pipe_parent_child1[0]);
            close(pipe_child1_child2[0]);
            close(pipe_child1_child2[1]);
            close(pipe_child2_parent[1]);


            int num;
            while (scanf("%d", &num) != EOF) {
                write(pipe_parent_child1[1], &num, sizeof(num));
                read(pipe_child2_parent[0], &num, sizeof(num));
                printf("Result: %d\n", num);
            }


            close(pipe_parent_child1[1]);
            close(pipe_child2_parent[0]);
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
