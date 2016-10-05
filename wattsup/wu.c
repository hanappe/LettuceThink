#include "cpufreq_config.h"
#include "thread.h"
#include "wu_private.h"
#include "wu.h"

static struct {
        thread_t thread;
        int error;
        int started;
        wu_callback_t callback;
        void* callback_data;
        int stop_flag;
} wu = { 0 };

static int wu_call_listener(double energy)
{
        if (!wu.callback) {
                return 0;
        }

        return wu.callback(energy, wu.callback_data);
}

static void wu_routine(void)
{
        wu.started = 0;
        wu.stop_flag = 0;
        wu.error = 0;

        if (wu_open() != 0) {
                fprintf(stderr, "Failed to open wattsup device\n");
                wu.error = 1;
                return;
        }

        wu.started = 1;
        double time0 = thread_time();
        unsigned long count = 0;

        while (!wu.stop_flag) {
                double watt = wu_watts();
                count++;

                if (watt < 0.0) {
                        fprintf(stderr, "[%lu] failed to read the power consumption\n", count);
                        continue;
                }

                double time = thread_time();
                double dt = time - time0;
                double energy = watt * dt;
                //fprintf(stderr, "[%lu] energy %g = %gW x %gs\n", energy, watt, dt, count);

                int err = wu_call_listener(energy);
                if (err) break;

                time0 = time;
        }

        wu_close();
}

int wu_start(void)
{
        if (wu.started) {
                return -1;
        }

	wu.thread = thread_new();
	if (wu.thread == NULL) {
                fprintf(stderr, "Failed to create wattsup thread\n");
                return -1;
        }

        thread_set_routine(wu.thread, wu_routine);
        thread_set_low_priority(wu.thread);

        if (thread_start(wu.thread)) {
                fprintf(stderr, "Failed to start wattsup thread\n");
                return -1;
        }

        while (!wu.started && !wu.error) {
                thread_sleep(0.1);
        }

        if (wu.error) {
                return -1;
        }

        return 0;
}

int wu_stop(void)
{
        if (!wu.started) {
                return -1;
        }

        wu.stop_flag = 1;
        thread_join(wu.thread);
        thread_free(wu.thread);
        wu.thread = NULL;

        return 0;
}

int wu_set_listener(wu_callback_t callback, void* data)
{
        wu.callback = callback;
        wu.callback_data = data;
        return 0;
}

