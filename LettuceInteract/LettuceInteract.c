// gcc -g RemoteInteraction.c -lcurl -o RemoteInteraction

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <curl/curl.h>

#define JS_EVENT_BUTTON 0x01  /* button pressed/released */
#define JS_EVENT_AXIS   0x02  /* joystick moved */
#define JS_EVENT_INIT   0x80  /* initial state of device */

struct js_event {
        uint32_t time;     /* event timestamp in milliseconds */
        int16_t value;     /* value */
        uint8_t type;      /* event type */
        uint8_t number;    /* axis/button number */
};

struct status_t {
        char x, y, z; 
};
        
static struct status_t status;
static CURL* curl = NULL;

int http_init()
{
        curl = curl_easy_init();
        if (curl == NULL) {
                fprintf(stderr, "Network: Failed to initialise curl.\n");
                return -1;
        }
        return 0;
}

int http_cleanup()
{
        if (curl != NULL)
                curl_easy_cleanup(curl);
        curl = NULL;
        return 0;
}

size_t http_write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
        /* Ignore the data sent back from server */
        return size * nmemb;
}

int http_cmd(const char* url)
{
        char errmsg[CURL_ERROR_SIZE];
        CURLcode res;
        long response_code = -1;
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_write_callback);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L); // 5 minutes
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errmsg);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
                fprintf(stderr, "Network: Failed to get the data: %s\n", errmsg);
                return -1;
        }

        res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        if (res != CURLE_OK) {
                fprintf(stderr, "Network: Failed to obtain curl's response code.\n");
                return -1;
        }
        if (response_code != 200) {
                fprintf(stderr, "Network: HTTP GET request failed (HTTP code %d).\n",
                        response_code);
                return -1;
        }

        return 0;
}

int process_event(struct js_event* e)
{
        int res = 0;
        
        e->type &= ~JS_EVENT_INIT;	

        switch (e->type) {
        case JS_EVENT_BUTTON:
                if (e->number == 2 || e->number == 5 || e->number == 10) {
                        if (e->value == 1) {
                                if (status.z != 1) {
                                        status.z = 1;
                                        res = http_cmd("http://127.0.0.1/z/increase");
                                }
                        } else {
                                if (status.z != 0) {
                                        status.z = 0;
                                        res = http_cmd("http://127.0.0.1/z/stop");
                                }
                        }
                } else if (e->number == 1 || e->number == 6 || e->number == 9) {
                        if (e->value == 1) {
                                if (status.z != -1) {
                                        status.z = -1;
                                        res = http_cmd("http://127.0.0.1/z/decrease");
                                }
                        } else {
                                if (status.z != 0) {
                                        status.z = 0;
                                        res = http_cmd("http://127.0.0.1/z/stop");
                                }
                        }
                }
                break;

        case JS_EVENT_AXIS:
                if (e->number == 1) {
                        if (e->value > 10000) {
                                if (status.x != 1) {
                                        status.x = 1;
                                        res = http_cmd("http://127.0.0.1/x/decrease");
                                }
                        } else if (e->value < -10000) {
                                if (status.x != -1) {
                                        status.x = -1;
                                        res = http_cmd("http://127.0.0.1/x/increase");
                                }
                        } else {
                                if (status.x != 0) {
                                        status.x = 0;
                                        res = http_cmd("http://127.0.0.1/x/stop");
                                }
                        }
                } else if (e->number == 0) {
                        if (e->value > 10000) {
                                if (status.y != 1) {
                                        status.y = 1;
                                        res = http_cmd("http://127.0.0.1/y/increase");
                                }
                        } else if (e->value < -10000) {
                                if (status.y != -1) {
                                        status.y = -1;
                                        res = http_cmd("http://127.0.0.1/y/decrease");
                                }
                        } else {
                                if (status.y != 0) {
                                        status.y = 0;
                                        res = http_cmd("http://127.0.0.1/y/stop");
                                }
                        }
                }
                break;
        }

        return res;
}

int main(int argc, char** argv)
{
        int fd;
        struct js_event e;
        int count_errors = 0;
        
        fd = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
                fprintf(stderr, "Failed to open /dev/input/js0\n");
                exit(1);
        }

        if (http_init() != 0) {
                close(fd);
                exit(1);                
        }

        status.x = 0;
        status.y = 0;
        status.z = 0;
        
        while (1) {
		while (read(fd, &e, sizeof(e)) > 0) {
			if (process_event(&e) != 0)
                                count_errors++;
		}
		/* EAGAIN is returned when the queue is empty */
		if (errno != EAGAIN) {
                        fprintf(stderr, "Reading event failed\n");
                        count_errors++;
		}

                if (count_errors > 42) {
                        fprintf(stderr, "Too many errors. Quitting\n");
                        break;
                }
        }
        
        http_cleanup();

        return 0;
}
