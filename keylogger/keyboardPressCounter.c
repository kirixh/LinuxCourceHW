#include <linux/module.h>
#include <linux/init.h>
#include <linux/semaphore.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Piatkovskyi");
MODULE_DESCRIPTION("");
MODULE_VERSION("0.0");

static int irq = 1;
static DEFINE_MUTEX(stats_mutex);
static unsigned int counter = 0;
static struct timer_list irq_timer;

/*-----------------------------------------------------------------------------*/
static irqreturn_t keyboard_interrupt(int irq, void *dev_id)
{
    mutex_lock(&stats_mutex);
    ++counter;
    mutex_unlock(&stats_mutex);

    return IRQ_HANDLED;
}

/*-----------------------------------------------------------------------------*/
static void onTimer(struct timer_list *t)
{
    pr_info("Number of keyboard presses for 60 sec: %d\n", counter);
    counter = 0;
    mod_timer(t, jiffies + msecs_to_jiffies(60000));
}

/*-----------------------------------------------------------------------------*/
static int __init keyboardPressCounter_module_added(void) {
    free_irq(irq, &irq);

    int err = request_irq(irq, keyboard_interrupt, IRQF_SHARED, "keyboardPressCounter", (&irq));
    if (err)
    {
        pr_alert("IRQ request failed\n");
        return err;
    }
    timer_setup(&irq_timer, onTimer, 0);
    mod_timer(&irq_timer, jiffies + msecs_to_jiffies(60000));

    pr_info("Module keyboardPressCounter has been installed\n");
    return 0;
} 

/*-----------------------------------------------------------------------------*/
static void __exit keyboardPressCounter_module_removed(void)
{
    synchronize_irq(irq);
    free_irq(irq, &irq);
    del_timer(&irq_timer);

    pr_info("Module keyboardPressCounter has been uninstalled\n");
}

module_init(keyboardPressCounter_module_added)
module_exit(keyboardPressCounter_module_removed)
