/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __H_VPU_DEBUG_H__
#define __H_VPU_DEBUG_H__

#include <linux/debugfs.h>

#include "vpu_v4l2.h"

static inline struct dentry *init_vpu_debugfs(struct vpu_dev_core *core)
{
	return NULL;
}

static inline void cleanup_vpu_debugfs(struct dentry *dir)
{ }

static inline void vpu_wakeup_fw_logging_wq(void)
{ }

#endif /* __H_VPU_DEBUG_H__ */
