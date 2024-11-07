/*
 * Copyright (c) 2020 Peter Bigot Consulting, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/mspi.h>

#define MSPI_NODE DT_NODELABEL(exmif)
#define DISPLAY_NODE DT_NODELABEL(display)

#define BUF_LEN (10 * 1024)
static uint8_t buf[BUF_LEN];

static int test(const struct device *mspi_dev)
{
	const struct mspi_dev_id dev_id = MSPI_DEVICE_ID_DT(DISPLAY_NODE);
	int rc;

	struct mspi_dev_cfg dev_cfg = MSPI_DEVICE_CONFIG_DT(DISPLAY_NODE);
	struct mspi_xfer_packet packet = {
		.data_buf = buf,
	};
	struct mspi_xfer xfer = {
		.xfer_mode   = MSPI_PIO,
		.packets     = &packet,
		.num_packet  = 1,
		.timeout     = 100,
	};

	rc = mspi_dev_config(mspi_dev, &dev_id,
			     MSPI_DEVICE_CONFIG_ALL, &dev_cfg);
	if (rc < 0) {
		printk("mspi_dev_config() failed: %d\n", rc);
		return rc;
	}

	for (int i = 0; i < sizeof(buf); ++i) {
		buf[i] = (uint8_t)i;
	}

	packet.dir = MSPI_TX;
	packet.cmd = 0x00;
	packet.num_bytes = sizeof(buf);
	xfer.cmd_length = 1;
	xfer.addr_length = 3;
	xfer.tx_dummy = 0;
	rc = mspi_transceive(mspi_dev, &dev_id, &xfer);
	if (rc < 0) {
		printk("mspi_transceive() failed: %d\n", rc);
		return rc;
	}

	return 0;
}

int main(void)
{
	const struct device *mspi_dev = DEVICE_DT_GET(MSPI_NODE);
	int rc;

	printk("EXMIF test on %s\n", CONFIG_BOARD_TARGET);

	if (!device_is_ready(mspi_dev)) {
		printk("%s: device not ready\n", mspi_dev->name);
		return -1;
	}

	if (1) {
		rc = test(mspi_dev);
		if (rc < 0) {
			return -1;
		}
	}

	return 0;
}
