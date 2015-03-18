/* boot.img unpacker
 * extracts zImage, initramfs.cpio.gz, second.img, and dt.img
 * second.img and dt.img are only extracted if they are present
 *
 * Copyright (C) 2015 Sultan Qasim Khan
 * All rights reserved.
 *
 * Licensed under the BSD 2-Clause License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bootimg.h"

static int extract(FILE *src_file, unsigned offset, unsigned size, char *out_name)
{
    FILE *out_file;
    void *buff;
    size_t n_bytes;

    out_file = fopen(out_name, "wb");
    if (out_file == NULL)
    {
        fprintf(stderr, "failed to create output file\n");
        return -11;
    }

    buff = malloc(size);
    if (buff == NULL)
    {
        fprintf(stderr, "failed to allocate memory for chunk\n");
        fclose(out_file);
        return -12;
    }

    fseek(src_file, (long)offset, SEEK_SET);

    n_bytes = fread(buff, 1, size, src_file);
    if (n_bytes != size)
    {
        fprintf(stderr, "failed to read chunk\n");
        fclose(out_file);
        free(buff);
        return -13;
    }

    n_bytes = fwrite(buff, 1, size, out_file);
    if (n_bytes != size)
    {
        fprintf(stderr, "failed to write chunk\n");
        fclose(out_file);
        free(buff);
        return -14;
    }

    free(buff);
    fclose(out_file);

    return 0;
}

int main(int argc, char **argv)
{
    boot_img_hdr hdr;
    int m, n, o, p, status;
    long img_size;
    unsigned kernel_loc, rd_loc, second_loc, dt_loc, end_loc;
    size_t n_read;
    FILE *boot_img;
    
    if (argc != 2)
    {
        fprintf(stderr, "Usage: unpack-bootimg boot.img\n");
        return -1;
    }

    boot_img = fopen(argv[1], "rb");
    if (boot_img == NULL)
    {
        fprintf(stderr, "failed to open boot image\n");
        return -2;
    }
    
    memset(&hdr, 0, sizeof(boot_img_hdr));
    n_read = fread(&hdr, sizeof(boot_img_hdr), 1, boot_img);
    if (n_read != 1)
    {
        fprintf(stderr, "failed to read boot image header\n");
        fclose(boot_img);
        return -3;
    }

    if (strncmp((char *)hdr.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE) != 0)
    {
        fprintf(stderr, "invalid boot header magic string\n");
        fclose(boot_img);
        return -4;
    }

    // compute file offsets to find the images
    n = (hdr.kernel_size + hdr.page_size - 1) / hdr.page_size;
    m = (hdr.ramdisk_size + hdr.page_size - 1) / hdr.page_size;
    o = (hdr.second_size + hdr.page_size - 1) / hdr.page_size;
    p = (hdr.dt_size + hdr.page_size - 1) / hdr.page_size;
    kernel_loc = hdr.page_size;
    rd_loc = kernel_loc + (n * hdr.page_size);
    second_loc = rd_loc + (m * hdr.page_size);
    dt_loc = second_loc + (o * hdr.page_size);
    end_loc = dt_loc + (p * hdr.page_size);

    // sanity check the offsets by making sure that the end location exists
    fseek(boot_img, 0, SEEK_END);
    img_size = ftell(boot_img);
    if (img_size < (long)end_loc)
    {
        fprintf(stderr, "boot image too small\n");
        fclose(boot_img);
        return -5;
    }

    if (hdr.kernel_size > 0)
    {
        status = extract(boot_img, kernel_loc, hdr.kernel_size, "zImage");
        if (status)
        {
            fclose(boot_img);
            return status;
        }
    }

    if (hdr.ramdisk_size > 0)
    {
        status = extract(boot_img, rd_loc, hdr.ramdisk_size, "initramfs.cpio.gz");
        if (status)
        {
            fclose(boot_img);
            return status;
        }
    }
    
    if (hdr.second_size > 0)
    {
        status = extract(boot_img, second_loc, hdr.second_size, "second.img");
        if (status)
        {
            fclose(boot_img);
            return status;
        }
    }
    
    if (hdr.dt_size > 0)
    {
        status = extract(boot_img, dt_loc, hdr.dt_size, "dt.img");
        if (status)
        {
            fclose(boot_img);
            return status;
        }
    }

    fclose(boot_img);

    return 0;
}

