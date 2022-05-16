/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "stdio.h"
#include <unistd.h>

void app_main() {
    while(1) {
        printf("Hello, Host!\n");

        for (int i = 10; i >= 0; i--) {
            printf("Restarting in %d seconds...\n", i);
            sleep(1);
        }
    }
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    app_main();

    return 0;
}
