/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <log.h>

void main(void)
{
	LOG(EDEBUG, "Hello World! %s", CONFIG_BOARD);
}
