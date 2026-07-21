// SPDX-License-Identifier: GPL-2.0-only
/*
 * Linux temperature monitor: a small educational kernel module.
 *
 * The module asks the Linux thermal framework for one thermal zone, checks
 * its temperature periodically, and exposes its current state through procfs.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/thermal.h>
#include <linux/workqueue.h>

#define PROC_NAME "temp_monitor"

enum temp_level {
	TEMP_NORMAL,
	TEMP_WARNING,
	TEMP_CRITICAL,
};

static char *zone = "x86_pkg_temp";
module_param(zone, charp, 0444);
MODULE_PARM_DESC(zone, "Thermal-zone type to monitor");

static unsigned int interval_ms = 2000;
module_param(interval_ms, uint, 0644);
MODULE_PARM_DESC(interval_ms, "Sampling interval in milliseconds");

static int warning_c = 75;
module_param(warning_c, int, 0644);
MODULE_PARM_DESC(warning_c, "Warning threshold in degrees Celsius");

static int critical_c = 85;
module_param(critical_c, int, 0644);
MODULE_PARM_DESC(critical_c, "Critical threshold in degrees Celsius");

static struct thermal_zone_device *thermal_zone;
static struct delayed_work monitor_work;
static struct proc_dir_entry *proc_entry;
static DEFINE_MUTEX(state_lock);

static int current_mc;
static int maximum_mc;
static unsigned long sample_count;
static unsigned long warning_count;
static int last_error;
static enum temp_level current_level = TEMP_NORMAL;

static const char *level_name(enum temp_level level)
{
	switch (level) {
	case TEMP_WARNING:
		return "warning";
	case TEMP_CRITICAL:
		return "critical";
	default:
		return "normal";
	}
}

static enum temp_level classify_temperature(int temperature_mc)
{
	if (temperature_mc >= critical_c * 1000)
		return TEMP_CRITICAL;
	if (temperature_mc >= warning_c * 1000)
		return TEMP_WARNING;
	return TEMP_NORMAL;
}

static void report_transition(enum temp_level previous, enum temp_level next,
			      int temperature_mc)
{
	/* Log only state changes so one hot sensor does not flood dmesg. */
	if (previous == next)
		return;

	if (next == TEMP_CRITICAL)
		pr_err("temp_monitor: critical temperature: %d.%03d C\n",
		       temperature_mc / 1000, temperature_mc % 1000);
	else if (next == TEMP_WARNING)
		pr_warn("temp_monitor: warning temperature: %d.%03d C\n",
			temperature_mc / 1000, temperature_mc % 1000);
	else
		pr_info("temp_monitor: temperature returned to normal: %d.%03d C\n",
			temperature_mc / 1000, temperature_mc % 1000);
}

static void monitor_temperature(struct work_struct *work)
{
	enum temp_level previous;
	enum temp_level next;
	int temperature_mc;
	int error;

	error = thermal_zone_get_temp(thermal_zone, &temperature_mc);

	mutex_lock(&state_lock);
	last_error = error;
	if (!error) {
		previous = current_level;
		next = classify_temperature(temperature_mc);
		current_mc = temperature_mc;
		if (!sample_count || temperature_mc > maximum_mc)
			maximum_mc = temperature_mc;
		sample_count++;
		if (next > TEMP_NORMAL && previous != next)
			warning_count++;
		current_level = next;
		report_transition(previous, next, temperature_mc);
	}
	mutex_unlock(&state_lock);

	if (error)
		pr_warn_ratelimited("temp_monitor: sensor read failed: %d\n", error);

	/* Requeue this function instead of blocking a kernel thread. */
	schedule_delayed_work(&monitor_work,
			      msecs_to_jiffies(max(interval_ms, 100U)));
}

static int status_show(struct seq_file *output, void *unused)
{
	mutex_lock(&state_lock);
	seq_printf(output, "zone: %s\n", zone);
	seq_printf(output, "state: %s\n", level_name(current_level));
	seq_printf(output, "current: %d.%03d C\n",
		   current_mc / 1000, current_mc % 1000);
	seq_printf(output, "maximum: %d.%03d C\n",
		   maximum_mc / 1000, maximum_mc % 1000);
	seq_printf(output, "warning threshold: %d C\n", warning_c);
	seq_printf(output, "critical threshold: %d C\n", critical_c);
	seq_printf(output, "samples: %lu\n", sample_count);
	seq_printf(output, "warnings: %lu\n", warning_count);
	seq_printf(output, "last sensor error: %d\n", last_error);
	mutex_unlock(&state_lock);
	return 0;
}

static int status_open(struct inode *inode, struct file *file)
{
	return single_open(file, status_show, NULL);
}

static const struct proc_ops status_operations = {
	.proc_open = status_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init temp_monitor_init(void)
{
	if (warning_c >= critical_c || interval_ms < 100) {
		pr_err("temp_monitor: require warning_c < critical_c and interval_ms >= 100\n");
		return -EINVAL;
	}

	thermal_zone = thermal_zone_get_zone_by_name(zone);
	if (IS_ERR(thermal_zone)) {
		pr_err("temp_monitor: thermal zone '%s' was not found\n", zone);
		return PTR_ERR(thermal_zone);
	}

	proc_entry = proc_create(PROC_NAME, 0444, NULL, &status_operations);
	if (!proc_entry)
		return -ENOMEM;

	INIT_DELAYED_WORK(&monitor_work, monitor_temperature);
	schedule_delayed_work(&monitor_work, 0);
	pr_info("temp_monitor: monitoring '%s' every %u ms (%d/%d C)\n",
		zone, interval_ms, warning_c, critical_c);
	return 0;
}

static void __exit temp_monitor_exit(void)
{
	cancel_delayed_work_sync(&monitor_work);
	proc_remove(proc_entry);
	pr_info("temp_monitor: stopped\n");
}

module_init(temp_monitor_init);
module_exit(temp_monitor_exit);

MODULE_AUTHOR("Hassan Roumani");
MODULE_DESCRIPTION("Educational Linux thermal-zone monitor");
MODULE_LICENSE("GPL");
