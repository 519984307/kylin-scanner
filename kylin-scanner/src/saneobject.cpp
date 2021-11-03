/*
* Copyright (C) 2020, KylinSoft Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/
#include "saneobject.h"
#include <ukui-log4qt.h>

SaneObject * SaneObject::instance = new SaneObject;

#ifdef __cplusplus
extern "C" {
#endif

#define STRIP_HEIGHT 256
#define A4BRY 297 // Some scanner device will less than 297

typedef struct _Image {
    uint8_t *data;
    int width;
    int height;
    int x;
    int y;
} Image;

typedef struct _OptDescriptor {
    int numColorMode;
    int numSource;
    int numResolution;
    int numSizeBrX;
    int numSizeBrY;
} OptDescriptor;
static OptDescriptor g_optDesc = {2, 3, 6, 10, 11};

static double g_saneSizeA4BrY = 297;
static SANE_Handle g_device = nullptr;
static int g_verbose;
static SANE_Byte *g_buf;
static size_t g_BufSize;
static SANE_Device *g_saneDevice = nullptr;
const SANE_Device **g_deviceList = nullptr;

#define SET_1_BIT(n,i) ((1<<(i))|(n))
#define SET_0_BIT(n,i) ((~(1<<(i)))&(n))
#define SET_R_BIT(n,i) ((n)^(1<<(i)))
#define GET_i_BIT(n,i) (((n)>>(i))&1)


static void writePnmHeader (SANE_Frame format, int width, int height, int depth, FILE *ofp)
{
    switch (format) {
    case SANE_FRAME_RED:
    case SANE_FRAME_GREEN:
    case SANE_FRAME_BLUE:
    case SANE_FRAME_RGB:
        fprintf(ofp, "P6\n# SANE data follows\n%d %d\n%d\n",
                width, height, (depth <= 8) ? 255 : 65535);
        KyInfo() << "#P6 SANE data follows width = " << width << "height = " << height
                << "depth = " << ((depth <= 8) ? 255 : 65535);
        break;
    default:
        if (depth == 1) {
            fprintf(ofp, "P4\n# SANE data follows\n%d %d\n", width, height);
            KyInfo() << "#P4 SANE data follows width = " << width << "height = " << height;
        } else {
            fprintf(ofp, "P5\n# SANE data follows\n%d %d\n%d\n",
                    width, height, (depth <= 8) ? 255 : 65535);
            KyInfo() << "#P5 SANE data follows width = " << width << "height = " << height
                    << "depth = " << ((depth <= 8) ? 255 : 65535);
        }
        break;
    }
}

static void *advance (Image *image)
{
    if (++image->x >= image->width) {
        image->x = 0;
        if ((++image->y >= image->height) || !image->data) {
            size_t old_size = 0, new_size;

            if (image->data)
                old_size = static_cast<size_t>(image->height * image->width);

            image->height += STRIP_HEIGHT;
            new_size = static_cast<size_t>(image->height * image->width);

            if (image->data) {
                image->data = (uint8_t *) realloc (image->data, new_size);
            } else {
                image->data = (uint8_t *) malloc (new_size);
            }

            if (image->data)
                memset (image->data + old_size, 0, new_size - old_size);
        }
    }

    if (!image->data) {
        KyInfo() << "Can't allocate image buffer, width = " << image->width
                << "height = " << image->height;
    }

    return image->data;
}

static SANE_Status onScanning(FILE *ofp)
{
    int i, len, first_frame = 1, offset = 0, must_buffer = 0, hundred_percent;
    SANE_Byte min = 0xff, max = 0;
    SANE_Parameters parm;
    SANE_Status status;
    Image image = { nullptr, 0, 0, 0, 0 };
    SANE_Word total_bytes = 0;
    SANE_Int hang_over = -1;

    do {
        if (!first_frame) {
            status = sane_start (g_device);
            if (status != SANE_STATUS_GOOD)
                goto cleanup;
        }

        status = sane_get_parameters (g_device, &parm);
        KyInfo() << "Parm : status = " << sane_strstatus(status)
                << "format = " << parm.format
                << "last_frame = " << parm.last_frame
                << "bytes_per_line = " << parm.bytes_per_line
                << "pixels_per_line = " << parm.pixels_per_line
                << "lines = " << parm.lines
                << "depth = " << parm.depth;

        if (status != SANE_STATUS_GOOD)
            goto cleanup;

        if (first_frame) {
            if (parm.lines >= 0) {
                KyInfo() << "Image's size(pixels): " << parm.pixels_per_line << parm.lines
                        << "Bits/pixel: " <<  parm.depth * (SANE_FRAME_RGB == parm.format ? 3 : 1);
            } else {
                KyInfo() << "Image's wide pixels: " << parm.pixels_per_line
                        << "Height for bits/pixel: " << parm.depth * (SANE_FRAME_RGB == parm.format ? 3 : 1);
            }

            switch (parm.format) {
            case SANE_FRAME_RED:
            case SANE_FRAME_GREEN:
            case SANE_FRAME_BLUE:
                assert (parm.depth == 8);
                must_buffer = 1;
                offset = parm.format - SANE_FRAME_RED;
                break;

            case SANE_FRAME_RGB:
                KyInfo() << "SANE_FRAME_GRB";
            //assert ((parm.depth == 8) || (parm.depth == 16));

            case SANE_FRAME_GRAY:
                assert ((parm.depth == 1) || (parm.depth == 8) || (parm.depth == 16));
                if (parm.lines < 0) {
                    must_buffer = 1;
                    offset = 0;
                } else {
                    writePnmHeader (parm.format, parm.pixels_per_line, parm.lines, parm.depth, ofp);
                }
                break;
            default:
                break;
            }

            if (must_buffer) {
                /**
                 * We're either scanning a multi-frame image or the
                 * scanner doesn't know what the eventual image height
                 * will be (common for hand-held scanners).  In either
                 * case, we need to buffer all data before we can write
                 * the image.
                 */
                image.width = parm.bytes_per_line;
                if (parm.lines >= 0) {
                    image.height = parm.lines - STRIP_HEIGHT + 1;
                } else {
                    image.height = 0;
                }

                image.x = image.width - 1;
                image.y = -1;
                if (!advance (&image)) {
                    status = SANE_STATUS_NO_MEM;
                    goto cleanup;
                }
            }
        } else {
            assert (parm.format >= SANE_FRAME_RED && parm.format <= SANE_FRAME_BLUE);
            offset = parm.format - SANE_FRAME_RED;
            image.x = image.y = 0;
        }

        hundred_percent = parm.bytes_per_line * parm.lines \
                          *((parm.format == SANE_FRAME_RGB || parm.format == SANE_FRAME_GRAY) ? 1 : 3);

        while (1) {
            double progr;
            status = sane_read (g_device, g_buf, g_BufSize, &len);
            total_bytes += (SANE_Word) len;
            progr = ((total_bytes * 100.) / (double) hundred_percent);
            if (progr > 100.)
                progr = 100.;

            if (status != SANE_STATUS_GOOD) {
                if (status != SANE_STATUS_EOF)
                    return status;
                break;
            }

            if (must_buffer) {
                KyInfo() << "must_buffer = " << must_buffer;
                switch (parm.format) {
                case SANE_FRAME_RED:
                case SANE_FRAME_GREEN:
                case SANE_FRAME_BLUE:
                    for (i = 0; i < len; ++i) {
                        image.data[offset + 3 * i] = g_buf[i];
                        if (!advance (&image)) {
                            status = SANE_STATUS_NO_MEM;
                            goto cleanup;
                        }
                    }
                    offset += 3 * len;
                    break;
                case SANE_FRAME_RGB:
                    for (i = 0; i < len; ++i) {
                        image.data[offset + i] = g_buf[i];
                        if (!advance (&image)) {
                            status = SANE_STATUS_NO_MEM;
                            goto cleanup;
                        }
                    }
                    offset += len;
                    break;
                case SANE_FRAME_GRAY:
                    for (i = 0; i < len; ++i) {
                        image.data[offset + i] = g_buf[i];
                        if (!advance (&image)) {
                            status = SANE_STATUS_NO_MEM;
                            goto cleanup;
                        }
                    }
                    offset += len;
                    break;
                default:
                    break;
                }
            } else { /* ! must_buffer */
                if ((parm.depth != 16)) {
                    fwrite (g_buf, 1, len, ofp);
                } else {
#if !defined(WORDS_BIGENDIAN)
                    int start = 0;
                    /* check if we have saved one byte from the last sane_read */
                    if (hang_over > -1) {
                        if (len > 0) {
                            fwrite (g_buf, 1, 1, ofp);
                            g_buf[0] = (SANE_Byte) hang_over;
                            hang_over = -1;
                            start = 1;
                        }
                    }
                    /* now do the byte-swapping */
                    for (i = start; i < (len - 1); i += 2) {
                        unsigned char LSB;
                        LSB = g_buf[i];
                        g_buf[i] = g_buf[i + 1];
                        g_buf[i + 1] = LSB;
                    }
                    /* check if we have an odd number of bytes */
                    if (((len - start) % 2) != 0) {
                        hang_over = g_buf[len - 1];
                        len--;
                    }
#endif
                    fwrite (g_buf, 1, len, ofp);
                }
            }

            if (g_verbose && parm.depth == 8) {
                for (i = 0; i < len; ++i)
                    if (g_buf[i] >= max) {
                        max = g_buf[i];
                    } else if (g_buf[i] < min) {
                        min = g_buf[i];
                    }
            }
        }
        first_frame = 0;
    } while (!parm.last_frame);

    if (must_buffer) {
        image.height = image.y;
        writePnmHeader (parm.format, parm.pixels_per_line, image.height, parm.depth, ofp);

#if !defined(WORDS_BIGENDIAN)
        if (parm.depth == 16) {
            for (i = 0; i < image.height * image.width; i += 2) {
                unsigned char LSB;
                LSB = image.data[i];
                image.data[i] = image.data[i + 1];
                image.data[i + 1] = LSB;
            }
        }
#endif
        fwrite (image.data, 1, image.height * image.width, ofp);
    }

    fflush( ofp );

cleanup:
    if (image.data)
        free (image.data);

    return status;
}


SANE_Status getSaneParameters(SANE_Handle device)
{
    SANE_Status status = SANE_STATUS_INVAL;
    SANE_Parameters parm;

    status = sane_get_parameters (device, &parm);
    KyInfo() << "Parm : status = " << sane_strstatus(status)
            << "format = " << parm.format
            << "last_frame = " << parm.last_frame
            << "bytes_per_line = " << parm.bytes_per_line
            << "pixels_per_line = " << parm.pixels_per_line
            << "lines = " << parm.lines
            << "depth = " << parm.depth;

    return status;
}

SANE_Status doScan(const char *fileName)
{
    SANE_Status status = SANE_STATUS_GOOD;
    FILE *ofp = nullptr;
    char path[PATH_MAX] = {0};
    char part_path[PATH_MAX] = {0};
    g_BufSize = (32 * 1024);
    g_buf = static_cast<SANE_Byte *>(malloc(g_BufSize));

    do {
        sprintf (path, "%s", fileName);
        strncpy (part_path, path, sizeof(part_path) - 1);
        strcat (part_path, ".part");
        KyInfo() << "part_path = " << part_path;

        status = sane_start (g_device);
        KyInfo() << "`sane_start` status: " << sane_strstatus(status);
        if (status != SANE_STATUS_GOOD) {
            KyInfo() << "Cannot start scan devices, sane_status = " << status;
            g_sane_object->setSaneStatus(false);
            break;
        }

        if (nullptr == (ofp = fopen (part_path, "w"))) {
            status = SANE_STATUS_ACCESS_DENIED;
            break;
        }

        status = onScanning(ofp);

        switch (status) {
        case SANE_STATUS_GOOD:
        case SANE_STATUS_EOF: {
            status = SANE_STATUS_GOOD;
            if (!ofp || (0 != fclose(ofp))) {
                status = SANE_STATUS_ACCESS_DENIED;
                break;
            } else {
                ofp = nullptr;
                if (rename (part_path, path)) {
                    status = SANE_STATUS_ACCESS_DENIED;
                    break;
                }
            }
        }
        break;
        default:
            break;
        }
    } while (0);

#if 0
    if (SANE_STATUS_GOOD != status)
        sane_cancel (g_device);
#endif

    sane_cancel (g_device);
    if (ofp) {
        fclose (ofp);
        ofp = nullptr;
    }

    if (g_buf) {
        free (g_buf);
        g_buf = nullptr;
    }

    return status;
}

static void authCallback (SANE_String_Const resource, SANE_Char *username, SANE_Char *password)
{
    KyInfo() << "auth_callback" << resource << username << password;
}

static void saneInit()
{
    SANE_Int version_code = 0;

    sane_init (&version_code, authCallback);

    KyInfo() << "version_code = " << version_code;
}

static SANE_Status saneGetDevices(const SANE_Device ***device_list)
{
    SANE_Status status = SANE_STATUS_GOOD;

    KyInfo() << "Get all scan devices, please waiting ...";

    /// This will be crashed unexpectedly, samples can be followed:
    /// 1. Caused by specific drives, such as `lenovo-image-g-series_1.0-16_arm64`, so we need Vendor developers to handle it
    /// 2. This will take a long time
    status = sane_get_devices (device_list, SANE_FALSE);

    if (status) {
        KyInfo() << "status = " << sane_strstatus(status);
    }

    return status;
}

SANE_Status saneOpen(SANE_Device *device, SANE_Handle *sane_handle)
{
    SANE_Status status = SANE_STATUS_INVAL;

    KyInfo() << "name = " << device->name
            << "model = " << device->model
            << "vendor = " << device->vendor
            << "type = " << device->type;

    // For usbRemoved
    char name[512] = {0};
    snprintf(name, 512, "%s %s", device->vendor, device->model);
    g_sane_object->openSaneName = QString(name);

    KyInfo() << "Open device name:  " <<g_sane_object->openSaneName;

    /// Filter HP scanners: hpaio:/net/hp_laserjet_pro_mfp_m226dw?ip=192.168.195.5&queue=false
    /// This scanner will `sane_open` not stop forever, so we reture error immediately
    if (device->name) {
        if (strstr(device->name, "hpaio:/net/hp_laserjet_pro_mfp_m226dw?ip=192")) {
            KyInfo() << device->name << " cannot scan, so we just return SANE_STATUS_INVAL.";
            return status;
        }
    }

    status = sane_open(device->name, sane_handle);

    if (status) {
        /// status =  Error during device I/O: can be this scanner connected by usb is error,
        /// so check usb connected.
        KyInfo() << "status = " << sane_strstatus(status);
    }

    return status;
}

SANE_Status getOptionColors(SANE_Handle sane_handle, int optnum)
{
    QStringList colors;

    QVector<string> colorScanModes;
    colorScanModes.push_back(SANE_VALUE_SCAN_MODE_COLOR);
    colorScanModes.push_back("color"); /* 松下KV-N1058Y for kylin-scanner */
    colorScanModes.push_back("Color");
    colorScanModes.push_back("24bit Color[Fast]"); /* brother4 driver, Brother DCP-1622WE, ADS3600W; simple-scan(#134), kylin-scanner(#74173) */
    colorScanModes.push_back("24bit Color"); /* Seen in the proprietary brother3 driver */
    colorScanModes.push_back("Color - 16 Million Colors"); /*Samsung unified driver. simple-scan(LP: 892915) */

    QVector<string> grayScanModes;
    grayScanModes.push_back(SANE_VALUE_SCAN_MODE_GRAY);
    grayScanModes.push_back("Gray");
    grayScanModes.push_back("gray"); /* 松下KV-N1058Y for kylin-scanner */
    grayScanModes.push_back("Grayscale");
    grayScanModes.push_back(SANE_I18N("Grayscale"));
    grayScanModes.push_back("True Gray"); /* Seen in the proprietary brother3 driver, Brother ADS3600W; however, Gray[Error Diffusion] is error gray scan modes */
    grayScanModes.push_back("Grayscale - 256 Levels"); /* Samsung unified driver. simple-scan(LP: 892915) */

    QVector<string> lineartScanModes;
    lineartScanModes.push_back(SANE_VALUE_SCAN_MODE_LINEART);
    lineartScanModes.push_back("Lineart");
    lineartScanModes.push_back("LineArt");
    lineartScanModes.push_back(SANE_I18N("LineArt"));
    lineartScanModes.push_back("Black & White");
    lineartScanModes.push_back(SANE_I18N("Black & White"));
    lineartScanModes.push_back("Binary");
    lineartScanModes.push_back(SANE_I18N("Binary"));
    lineartScanModes.push_back("Thresholded");
    lineartScanModes.push_back("Black and White - Line Art"); /* Samsung unified driver. simple-scan(LP: 892915) */
    lineartScanModes.push_back("Black and White - Halftone");
    lineartScanModes.push_back("bw"); /* 松下KV-N1058Y for kylin-scanner */

    const SANE_Option_Descriptor *opt;
    SANE_Status status = SANE_STATUS_INVAL;

    KyInfo() << "Get colors option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    g_sane_object->colorModesMap.clear();

    for (int i = 0; opt->constraint.string_list[i] != nullptr; ++i) {
//        KyInfo() << "color strings = " << *(opt->constraint.string_list + i);

        const char *tmp = *(opt->constraint.string_list + i);
        QVector<string>::iterator it;

        status = SANE_STATUS_GOOD;

        it = find(colorScanModes.begin(), colorScanModes.end(), tmp);

        if (it != colorScanModes.end()) {
            if (! g_sane_object->colorModesMap.contains("Color")) {
                // Only the first key matched can be followed, which could avoid duplicate inserts bugs
                g_sane_object->colorModesMap.insert("Color", QString::fromStdString(*it));
                colors << QObject::tr("Color");
            }
        }

        it = find(grayScanModes.begin(), grayScanModes.end(), tmp);
        if ((it != grayScanModes.end()) && (! g_sane_object->colorModesMap.contains("Gray"))) {
            g_sane_object->colorModesMap.insert("Gray", QString::fromStdString(*it));
            colors << QObject::tr("Gray");
        }

        it = find(lineartScanModes.begin(), lineartScanModes.end(), tmp);
        if ((it != lineartScanModes.end()) && (! g_sane_object->colorModesMap.contains("Lineart"))) {
            g_sane_object->colorModesMap.insert("Lineart", QString::fromStdString(*it));
            colors << QObject::tr("Lineart");
        }
    }

    KyInfo() << "colors: " << colors;
    QMap<QString, QString>::iterator itMap;
    for (itMap=g_sane_object->colorModesMap.begin(); itMap != g_sane_object->colorModesMap.end(); ++itMap ) {
        KyInfo() << "Iterator for colorModesMap: " <<itMap.key() << itMap.value();
    }
    g_sane_object->setSaneColors(colors);
    return status;
}

SANE_Status setOptionColors(SANE_Handle sane_handle, SANE_String val_color)
{
    SANE_Status status = SANE_STATUS_INVAL;

    KyInfo() << "option: " << g_optDesc.numColorMode
            << "Set color option = " << val_color;
    int i = 0;
    const SANE_Option_Descriptor *opt;


    KyInfo() << "color handle: " << sane_handle;
    opt = sane_get_option_descriptor(sane_handle, g_optDesc.numColorMode);
    for (int i = 0; opt->constraint.string_list[i] != nullptr; ++i) {
        const char *tmp = *(opt->constraint.string_list + i);
        status = SANE_STATUS_GOOD;
        KyInfo() << "color strings = " << *(opt->constraint.string_list + i);
    }

    status = sane_control_option(sane_handle,  static_cast<SANE_Int>(g_optDesc.numColorMode),
                                 SANE_ACTION_SET_VALUE, val_color, nullptr);

    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    KyInfo() << "Set color success.";

    return status;
}

SANE_Status getOptionSourcesDefaultFailed()
{
    QStringList typeStringList;
    SANE_Status status = SANE_STATUS_GOOD;

    typeStringList << QObject::tr("Default Type");

    g_sane_object->haveSourceFlag = 0;

    KyInfo() << "Default Type: " << typeStringList;

    g_sane_object->setSaneTypes(typeStringList);

    return status;

}

SANE_Status getOptionSources(SANE_Handle sane_handle, int optnum)
{
    QStringList typeStringList;
    SANE_Status status = SANE_STATUS_INVAL;
    const SANE_Option_Descriptor *opt;
    QVector<string>::iterator it;

    QVector<string> flatbedSources;
    flatbedSources.push_back("Auto");
    flatbedSources.push_back(SANE_I18N("Auto"));
    flatbedSources.push_back("Flatbed");
    flatbedSources.push_back(SANE_I18N("Flatbed"));
    flatbedSources.push_back("FlatBed");
    flatbedSources.push_back("Normal");
    flatbedSources.push_back(SANE_I18N("Normal"));

    QVector<string> adfSources;
    adfSources.push_back("Automatic Document Feeder");
    adfSources.push_back(SANE_I18N("Automatic Document Feeder"));
    adfSources.push_back("ADF");
    adfSources.push_back("adf"); /* 松下KV-N1058Y for kylin-scanner*/
    adfSources.push_back("Automatic Document Feeder(left aligned)"); /* Seen in the proprietary brother3 driver */
    adfSources.push_back("utomatic Document Feeder(centrally aligned)"); /* Seen in the proprietary brother3 driver */
    adfSources.push_back("ADF Simplex"); /* Samsung unified driver. LP: # 892915 */

    QVector<string> adfFrontSources;
    adfFrontSources.push_back("ADF Front");
    adfFrontSources.push_back(SANE_I18N("ADF Front"));

    QVector<string> adfBackSources;
    adfBackSources.push_back("ADF Back");
    adfBackSources.push_back(SANE_I18N("ADF Back"));

    QVector<string> adfDuplexSources;
    adfDuplexSources.push_back("ADF Duplex");
    adfDuplexSources.push_back(SANE_I18N("ADF Duplex"));
    adfDuplexSources.push_back("ADF Duplex - Long-Edge Binding"); /* Samsung unified driver. LP: # 892915 */
    adfDuplexSources.push_back("ADF Duplex - Short-Edge Binding");
    adfDuplexSources.push_back("Duplex"); /* HP duplex scan support. LP: #1353599 */
    adfDuplexSources.push_back(SANE_I18N("Duplex")); /* drc240 scanner */
    adfDuplexSources.push_back("Automatic Document Feeder(centrally aligned,Duplex)"); /* Brother duplex scan support. LP: #1343773 */
    adfDuplexSources.push_back("Automatic Document Feeder(left aligned,Duplex)");


    KyInfo() << "GetOptionSource: " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    g_sane_object->sourceModesMap.clear();
    g_sane_object->haveSourceFlag = 0;


    for (int i = 0; opt->constraint.string_list[i] != nullptr; ++i) {
        const char *tmp = *(opt->constraint.string_list + i);

        status = SANE_STATUS_GOOD;

//        KyInfo() << "Sources: " << tmp;

        it = find(flatbedSources.begin(), flatbedSources.end(), tmp);

        if (it != flatbedSources.end()) {
            if (! g_sane_object->sourceModesMap.contains("Flatbed")) {
                g_sane_object->sourceModesMap.insert("Flatbed", QString::fromStdString(*it));
                typeStringList << QObject::tr("Flatbed");

                ++g_sane_object->haveSourceFlag;
                continue;
            }
        }

        it = find(adfSources.begin(), adfSources.end(), tmp);
        if ((it != adfSources.end()) && (! g_sane_object->sourceModesMap.contains("ADF"))) {
            g_sane_object->sourceModesMap.insert("ADF", QString::fromStdString(*it));
            typeStringList << QObject::tr("ADF");

            ++g_sane_object->haveSourceFlag;
            continue;
        }

        it = find(adfFrontSources.begin(), adfFrontSources.end(), tmp);
        if ((it != adfFrontSources.end()) && (! g_sane_object->sourceModesMap.contains("ADF Front"))) {
            g_sane_object->sourceModesMap.insert("ADF Front", QString::fromStdString(*it));
            typeStringList << QObject::tr("ADF Front");

            ++g_sane_object->haveSourceFlag;
            continue;
        }

        it = find(adfBackSources.begin(), adfBackSources.end(), tmp);
        if ((it != adfBackSources.end()) && (! g_sane_object->sourceModesMap.contains("ADF Back"))) {
            g_sane_object->sourceModesMap.insert("ADF Back", QString::fromStdString(*it));
            typeStringList << QObject::tr("ADF Back");

            ++g_sane_object->haveSourceFlag;
            continue;
        }

        it = find(adfDuplexSources.begin(), adfDuplexSources.end(), tmp);
        if ((it != adfDuplexSources.end()) && (! g_sane_object->sourceModesMap.contains("ADF Duplex"))) {
            g_sane_object->sourceModesMap.insert("ADF Duplex", QString::fromStdString(*it));
            typeStringList << QObject::tr("ADF Duplex");

            ++g_sane_object->haveSourceFlag;
            continue;
        }
    }

    KyInfo() << "Type: " << typeStringList;
    QMap<QString, QString>::iterator itMap;
    for (itMap=g_sane_object->sourceModesMap.begin(); itMap != g_sane_object->sourceModesMap.end(); ++itMap ) {
        KyInfo() << "Iterator for sourceModesMap: " <<itMap.key() << itMap.value();
    }

    g_sane_object->setSaneTypes(typeStringList);

    return status;
}

SANE_Status setOptionSources(SANE_Handle sane_handle, int optnum, SANE_String val_source)
{
    SANE_Status status = SANE_STATUS_GOOD;

    KyInfo() << "Set source option = " << val_source;

    status = sane_control_option(sane_handle, optnum,
                                 SANE_ACTION_SET_VALUE, val_source, nullptr);
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    return status;
}

static SANE_Status getOptionResolutions(SANE_Handle sane_handle, int optnum)
{
    QStringList resolutions;
    SANE_Status status = SANE_STATUS_INVAL;
    const SANE_Option_Descriptor *opt;

    KyInfo() << "Get resolution option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    for (int i = 0; opt->constraint.word_list[i]; ++i) {
        int res = *(opt->constraint.word_list + i);
        status = SANE_STATUS_GOOD;
//        KyInfo() << "resolution int = " << res;

        switch (res) {
        case 4800:
            resolutions << QObject::tr("4800 dpi");
            break;
        case 2400:
            resolutions << QObject::tr("2400 dpi");
            break;
        case 1200:
            resolutions << QObject::tr("1200 dpi");
            break;
        case 600:
            resolutions << QObject::tr("600 dpi");
            break;
        case 300:
            resolutions << QObject::tr("300 dpi");
            break;
        case 200:
            resolutions << QObject::tr("200 dpi");
            break;
        case 150:
            resolutions << QObject::tr("150 dpi");
            break;
        case 100:
            resolutions << QObject::tr("100 dpi");
            break;
        case 75:
            resolutions << QObject::tr("75 dpi");
            break;
        default:
            break;
        }
    }

    // reverse sort
    qSort(resolutions.begin(), resolutions.end(), [](const QString & s1, const QString & s2) {
        // Get numbers from QString, and then sort

        int numberS1 = g_sane_object->getNumbersFromQString(s1);
        int numberS2 = g_sane_object->getNumbersFromQString(s2);

        return numberS1 < numberS2;
    });
    resolutions << QObject::tr("Auto");

    g_sane_object->setSaneResolutions(resolutions);

    return status;
}

SANE_Status setOptionResolutions(SANE_Handle sane_handle, SANE_Int val_resolution)
{
    SANE_Status status = SANE_STATUS_GOOD;

    KyInfo() << "Set resolution option = " << val_resolution;

    status = sane_control_option(sane_handle, g_optDesc.numResolution,
                                 SANE_ACTION_SET_VALUE, &val_resolution, nullptr);

    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    return status;
}


/**
 * @brief get_option_sizes
 *
 * Get all sizes[8,9,10,11] option: tl-x tl-y br-x br-y (etc.)
 *
 * A3: 0 0 297 420
 * A4: 0 0 210 297 (canon 210=> 0 0 216 297)
 *
 * Because sizes just can read normal, so we could directly to set general sizes
 *
 * @param sane_handle
 * @param optnum
 * @return sane status
 */
SANE_Status getOptionSizes(SANE_Handle sane_handle, int optnum)
{
    const SANE_Option_Descriptor *opt;
    SANE_Status status = SANE_STATUS_GOOD;

    KyInfo() << "Get size option = " << optnum;

    opt = sane_get_option_descriptor(sane_handle, optnum);

    for (int i = 0; opt->constraint.word_list[i]; ++i) {
        KyInfo() << "sizes = " << *(opt->constraint.word_list + i);
    }
    return status;
}

SANE_Status setOptionSizes(SANE_Handle sane_handle, int optnum, SANE_Int val_size)
{
    SANE_Status status = SANE_STATUS_GOOD;

    KyInfo() << "Set size option = " << val_size;

    status = sane_control_option(sane_handle, optnum,
                                 SANE_ACTION_SET_VALUE, &val_size, nullptr);

    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "status = " << status << "desc: " << sane_strstatus(status);
        return status;
    }

    return status;
}

SANE_Status setOptionSizesReal(SANE_Handle sane_handle, SANE_Int val_size_br_x,
                                  SANE_Int val_size_br_y)
{
    SANE_Status status = SANE_STATUS_GOOD;
    KyInfo() << "Size bottom-right location(xy) = " << val_size_br_x << val_size_br_y;

    status = setOptionSizes(sane_handle, g_optDesc.numSizeBrX, SANE_FIX(val_size_br_x));
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "status = " << sane_strstatus(status);
        return status;
    }

    status = setOptionSizes(sane_handle, g_optDesc.numSizeBrY, SANE_FIX(val_size_br_y));
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "status = " << sane_strstatus(status);
        return status;
    }

    return status;
}

SANE_Status setOptionSizesAll(SANE_Handle sane_handle, int type)
{
    SANE_Status status = SANE_STATUS_GOOD;

    switch (type) {
    case A2:
        status = setOptionSizesReal(sane_handle, 420, 594);
        break;
    case A3:
        status = setOptionSizesReal(sane_handle, 297, 420);
        break;
    case A4:
        status = setOptionSizesReal(sane_handle, 210, g_saneSizeA4BrY);
        break;
    case A5:
        status = setOptionSizesReal(sane_handle, 148, 210);
        break;
    case A6:
        status = setOptionSizesReal(sane_handle, 105, 144);
        break;
    default:
        status = SANE_STATUS_UNSUPPORTED;
    }

    return status;
}

#define GUARDS_SIZE 4
#define GUARD1 ((SANE_Word)0x5abf8ea5)
#define GUARD2 ((SANE_Word)0xa58ebf5a)

static void *guardsMalloc(size_t size)
{
    unsigned char *ptr;

    size += 2 * GUARDS_SIZE;
    ptr = static_cast<unsigned char *>(malloc(size));

    assert(ptr);

    ptr += GUARDS_SIZE;

    return (ptr);
}

static void guardsFree(void *ptr)
{
    unsigned char *p = static_cast<unsigned char *>(ptr);

    p -= GUARDS_SIZE;
    free(p);
}

static const SANE_Option_Descriptor *getOptdescByName(SANE_Handle device, const char *name,
                                                         int *option_num)
{
    SANE_Int num_dev_options;
    SANE_Status status;

    /* Get the number of options. */
    status = sane_control_option (device, 0,
                                  SANE_ACTION_GET_VALUE, &num_dev_options, nullptr);

    KyInfo() << "\n\n\nGet Option name: " << name << status;

    for (*option_num = 0; *option_num < num_dev_options; (*option_num)++) {
        const SANE_Option_Descriptor *opt;

        opt = sane_get_option_descriptor (device, *option_num);

        if (opt->name) {
//            KyInfo() << opt->name;
        }

        if (opt->name && strcmp(opt->name, name) == 0) {
            KyInfo() << "Get option desc for " << *option_num << "opt->name = " << opt->name << "name" << name;
            return (opt);
        }
    }
    return (nullptr);
}

void displayOptionValue(SANE_Handle device, int optnum)
{
    const SANE_Option_Descriptor *opt;

    opt = sane_get_option_descriptor(device, optnum);

    KyInfo() << "Get options: optnum = " << optnum
            << "name = " << opt->name
            << "title = " << opt->title
            << "type = " << opt->type
            << "desc = " << opt->desc
            << "cap = " << opt->cap
            << "size = " << opt->size
            << "unit = " << opt->unit;
}

static SANE_Status getOptionValue(SANE_Handle device, const char *option_name)
{
    QStringList sizes;

    SANE_Status status = SANE_STATUS_GOOD;

    const SANE_Option_Descriptor *opt;
    int optnum;
    static char str[100];

    SANE_String val_string_source;
    SANE_String val_string_color;
    SANE_Word val_size;
    SANE_Word val_resolution;

    opt = getOptdescByName(device, option_name, &optnum);

    if (opt) {
        void *optval;
        optval = guardsMalloc(opt->size);
        /* Get default optval(different format) */
        status = sane_control_option (device, optnum,
                                      SANE_ACTION_GET_VALUE, optval, nullptr);

        if (opt->desc) {
            KyInfo() << opt->desc;
        }

        KyInfo() << "opt->type = " << opt->type;
        switch (opt->type) {
        case SANE_TYPE_INT:
            KyInfo() << "type = int" << "size = " << opt->size;

            if (!strcmp(option_name, SANE_NAME_SCAN_RESOLUTION)) {
                val_resolution = *(SANE_Word *)optval;

                g_optDesc.numResolution = optnum;
                if (opt->constraint_type == SANE_CONSTRAINT_WORD_LIST)
                    status = getOptionResolutions(device, optnum);

                KyInfo() << "optnum = " << g_optDesc.numResolution
                        << "resolution = " << val_resolution
                        << "constraint_type = " << opt->constraint_type;
            }
            break;
        case SANE_TYPE_BOOL:
            if (*(SANE_Word *) optval == SANE_FALSE) {
                strcpy(str, "FALSE");
            } else {
                strcpy(str, "TRUE");
            }
            KyInfo() << "type = bool" << "size = " << opt->size << str;
            break;
        case SANE_TYPE_FIXED:
            KyInfo() << "type = fixed" << "size = " << opt->size;

            val_size = SANE_UNFIX(*(SANE_Word *) optval);
            if (opt->constraint_type == SANE_CONSTRAINT_RANGE)
                status = getOptionSizes(device, optnum);

            KyInfo() << "str_status = " << sane_strstatus(status);

            if (!strcmp(option_name, SANE_NAME_SCAN_TL_X)) {
                KyInfo() << "size tl_x = " << val_size << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_TL_Y)) {
                KyInfo() << "size tl_y = " << val_size << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_BR_X)) {
                g_optDesc.numSizeBrX = optnum;
                // Via br_x to decide scan sizes
                int size_range = static_cast<int>( SANE_UNFIX(opt->constraint.range->max) \
                                                   - SANE_UNFIX(opt->constraint.range->min));
                KyInfo() << "min = " << SANE_UNFIX(opt->constraint.range->min)
                        << "max = " << SANE_UNFIX(opt->constraint.range->max)
                        << "size_range = " << size_range;
                {
                    if (val_size >= 420)
                        sizes << "A2";
                    if (size_range >= 297)
                        sizes << "A3";
                    if (size_range >= 210)
                        sizes << "A4";
                    if (size_range >= 148)
                        sizes << "A5";
                    if (size_range >= 105)
                        sizes << "A6";
                }
                g_sane_object->setSaneSizes(sizes);
                KyInfo() << "size optnum = " << g_optDesc.numSizeBrX
                        << "br_x" << val_size
                        << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_BR_Y)) {
                g_optDesc.numSizeBrY = optnum;
                // Need via br_y to decide scan sizes after br_x, in case of (215.9, 296.926) to A4, which should be A5
                int size_range = static_cast<int>( SANE_UNFIX(opt->constraint.range->max) \
                                                   - SANE_UNFIX(opt->constraint.range->min));
                double min = SANE_UNFIX(opt->constraint.range->min);
                double max = SANE_UNFIX(opt->constraint.range->max);
                KyInfo() << "min = " << min
                        << "max = " << max
                        << "size_range = " << size_range;
                // Judge max(296.926) less than A4BRY(297)
                if ((qCeil(max) == A4BRY) && (qFloor(max) == A4BRY - 1)) {
                    KyInfo() << "A4 br_y max = " << max;
                    g_saneSizeA4BrY = max;
                } else {
                    g_saneSizeA4BrY = A4BRY;
                }
                KyInfo() << "size optnum = " << g_optDesc.numSizeBrY
                        << "br_y" << val_size
                        << "g_saneSizeA4Bry = " <<g_saneSizeA4BrY
                        << "constraint_type = " << opt->constraint_type;
            }

            break;
        case SANE_TYPE_STRING:
            KyInfo() << "type = string" << "size = " << opt->size;

            if (!strcmp(option_name, SANE_NAME_SCAN_MODE)) {
                val_string_color = static_cast<SANE_String>(optval);
                g_optDesc.numColorMode = optnum;
                status = getOptionColors(device, optnum);
                KyInfo() << "Default optnum = " << g_optDesc.numColorMode
                        << "color = " << val_string_color
                        << "constraint_type = " << opt->constraint_type;
            } else if (!strcmp(option_name, SANE_NAME_SCAN_SOURCE)) {
                val_string_source = static_cast<SANE_String>(optval);
                g_optDesc.numSource = optnum;
                status = getOptionSources(device, optnum);
                KyInfo() << "Default optnum = " << g_optDesc.numSource
                        << "source = " << val_string_source
                        << "constraint_type = " << opt->constraint_type;
            } else {
                // Canon Lide 400 local connect error
                status = SANE_STATUS_INVAL;
            }
            break;
        case SANE_TYPE_BUTTON:
            KyInfo() << "type = button" << "size = " << opt->size;
            break;
        case SANE_TYPE_GROUP:
            KyInfo() << "type = button" << "size = " << opt->size;
            break;
        default:
            KyInfo() << "type = %d" << opt->type << "size = " << opt->size;
            break;
        }

        switch (opt->unit) {
        case SANE_UNIT_NONE:
            KyInfo() << "unit = none";
            break;
        case SANE_UNIT_PIXEL:
            KyInfo() << "unit = pixel";
            break;
        case SANE_UNIT_BIT:
            KyInfo() << "unit = bit";
            break;
        case SANE_UNIT_MM:
            KyInfo() << "unit = mm";
            break;
        case SANE_UNIT_DPI:
            KyInfo() << "unit = dpi";
            break;
        case SANE_UNIT_PERCENT:
            KyInfo() << "unit = percent";
            break;
        case SANE_UNIT_MICROSECOND:
            KyInfo() << "unit = microsecond";
            break;
        default:
            KyInfo() << "unit = " << opt->unit;
            break;
        }

        switch (opt->constraint_type) {
        case SANE_CONSTRAINT_RANGE:
            if (opt->type == SANE_TYPE_FIXED) {
                KyInfo() << "min = " << SANE_UNFIX(opt->constraint.range->min) \
                        << "max = " << SANE_UNFIX(opt->constraint.range->max) \
                        << "quant = " << opt->constraint.range->quant;
            } else {
                KyInfo() << "min = " << opt->constraint.range->min \
                        << "max = " << opt->constraint.range->max \
                        << "quant = " << SANE_UNFIX(opt->constraint.range->quant);
            }
            break;
        case SANE_CONSTRAINT_WORD_LIST:
            for (int i = 0; i < opt->constraint.word_list[0]; ++i) {
                if (opt->type == SANE_TYPE_INT) {
                    KyInfo() << opt->constraint.word_list[i + 1];
                } else {
                    KyInfo() << SANE_UNFIX(opt->constraint.word_list[i + 1]);
                }
            }
            break;
        case SANE_CONSTRAINT_STRING_LIST:
            for (int i = 0; opt->constraint.string_list[i] != NULL; ++i) {
                KyInfo() << opt->constraint.string_list[i];
            }
            break;
        default:
            KyInfo() << "case = %d not found" ;
            break;
        }

        guardsFree(optval);
    } else {
        /* The option does not exists. */
        strcpy(str, "backend default");

        KyInfo() << "option_name = " << option_name << "str = " << str;
        if ((strcmp(str, "backend default") == 0)
                && (strcmp(option_name, SANE_NAME_SCAN_SOURCE) == 0)) {
            getOptionSourcesDefaultFailed();
        }
    }

    KyInfo() << "option_name = " << option_name << "str = " << str;

    return status;
}

static SANE_Status showAllSaneParameters(SANE_Handle device)
{
    static char str[150];
    char *p = str;

    SANE_Status  status = SANE_STATUS_GOOD;

    *p = 0;

    status = getOptionValue(device, SANE_NAME_SCAN_SOURCE);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "source parameters error!";
        return status;
    }

    status = getOptionValue(device, SANE_NAME_SCAN_MODE);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "color mode parameters error!";
        return status;
    }

    status = getOptionValue(device, SANE_NAME_SCAN_RESOLUTION);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "resolution parameters error!";
        return status;
    }

    // Default size coordination, top_left(x, y)
    getOptionValue(device, SANE_NAME_SCAN_TL_X);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "tl_x parameters error!";
        return status;
    }

    getOptionValue(device, SANE_NAME_SCAN_TL_Y);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "tl_y parameters error!";
        return status;
    }

    /* Default size coordination, botton_right(x, y)
     *
     * Refer to backend/sharp.c, for A4 size:
     * s->val[OPT_BR_X].w = SANE_FIX(210);
     * s->val[OPT_BR_Y].w = SANE_FIX(297);
     */
    getOptionValue(device, SANE_NAME_SCAN_BR_X);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "br_x parameters error!";
        return status;
    }

    status = getOptionValue(device, SANE_NAME_SCAN_BR_Y);
    if (status != SANE_STATUS_GOOD) {
        status = SANE_STATUS_INVAL;
        KyInfo() << "br_y parameters error!";
        return status;
    }

    return status;
}

static SANE_Status startSaneScan(SANE_Handle sane_handle, SANE_String_Const fileName)
{
    g_device = sane_handle;

    SANE_Status status = SANE_STATUS_GOOD;
    status = doScan(fileName);

    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "start scan error, status = " << status;
    }

    return status;
}

void saneCancel(SANE_Handle sane_handle)
{
    KyInfo() << "saneCancel()";

    if (g_sane_object->getSaneHaveHandle()) {
        g_sane_object->setSaneHaveHandle(false);

        KyInfo() << "sane_cancel and sane_close";

        sane_cancel(sane_handle);
        sane_close(sane_handle);
    }
}

void detectSaneDevices()
{
    KyInfo() << "detectSaneDevices();";

    QStringList names;
    QString type;

    SANE_Status sane_status;
    char name[512] = {0};

    saneInit();

    do {
        //const SANE_Device ** device_list = nullptr;
        sane_status = saneGetDevices((&g_deviceList));
        if (sane_status) {
            KyInfo() << "Cannot get scan devices, sane_status = " << sane_status;
            g_sane_object->setSaneStatus(false);
            break;
        }

        int i = 0;
        unsigned int column = 80;

        for (i = 0; g_deviceList[i]; ++i) {
            if (column + strlen (g_deviceList[i]->name) + 1 >= 80)
                column = 4;
            if (column > 4)
                column += 1;

            column += strlen (g_deviceList[i]->name);
        }

        for (i = 0; g_deviceList[i]; ++i) {
            KyInfo() << "Name: " << g_deviceList[i]->name
                    << "Vendor: " << g_deviceList[i]->vendor
                    << "Model: " << g_deviceList[i]->model
                    << "Type: " << g_deviceList[i]->type;

            snprintf(name, 512, "%s %s", g_deviceList[i]->vendor, g_deviceList[i]->model);

            names << name;
        }

        KyInfo() << names;
        g_sane_object->setSaneNames(names);

        if (!g_deviceList[0]) {
            KyInfo() << "No scan devices !";
            sane_status = SANE_STATUS_UNSUPPORTED;
            g_sane_object->setSaneStatus(false);
            break;
        }

        sane_status = SANE_STATUS_GOOD;

    } while (0);

    if (sane_status) {
        g_sane_object->setSaneStatus(false);
        KyInfo() << "find device set status false";
    } else {
        g_sane_object->setSaneStatus(true);
    }
}

void openSaneDevice(int index)
{
    KyInfo() << "openSaneDevice";

    QStringList names;
    SANE_Status sane_status;
    char name[512] = {0};

    do {
        /**
         * Deal with open same scanner device again meeting `SANE_STATUS_DEVICE_BUSY`,
         * Thus, should `sane_close` the open handle before next `sane_open`
         */
        if (g_sane_object->getSaneHaveHandle()) {
            // Avoid SANE_STATUS_BUSY status
            KyInfo() << "begin to sane_close()";;
            sane_close(g_sane_object->handle);
            g_sane_object->setSaneHaveHandle(false);
        }
        g_sane_object->setSaneHaveHandle(false);

        for (int i = 0; g_deviceList[i]; ++i) {
            snprintf(name, 512, "%s %s", g_deviceList[i]->vendor, g_deviceList[i]->model);

            names << name;
        }

        if (names.isEmpty()) {
            KyInfo() << "No scan devices !";

        }
        KyInfo() << names;
        g_sane_object->setSaneNames(names);

        if (!g_deviceList[0]) {
            KyInfo() << "No scan devices !";
            sane_status = SANE_STATUS_UNSUPPORTED;
            g_sane_object->setSaneStatus(false);
            break;
        }

        KyInfo() << "Open a scan device, plese waiting ...";
        SANE_Handle sane_handle;
        g_saneDevice = const_cast<SANE_Device *>(*(g_deviceList + index));
        if (!g_saneDevice) {
            KyInfo() << "No device connected!";
            sane_status = SANE_STATUS_UNSUPPORTED;
            g_sane_object->setSaneStatus(false);
            break;
        }

        sane_status = saneOpen(g_saneDevice, &sane_handle);
        if (sane_status) {
            KyInfo() << "First open a device failed!";

            g_sane_object->setSaneStatus(false);
            g_sane_object->setSaneHaveHandle(false);

            break;
        }
        g_sane_object->handle = sane_handle;
        g_sane_object->setSaneHaveHandle(true);

        KyInfo() << "Start scanning, please waiting ...";

        sane_status = showAllSaneParameters(sane_handle);

    } while (0);

    if (sane_status) {
        g_sane_object->setSaneStatus(false);
        KyInfo() << "open device set status false";
    } else {
        g_sane_object->setSaneStatus(true);
    }

}
#ifdef __cplusplus
}
#endif

SaneObject::SaneObject(QObject *parent) : QObject(parent)
{
    devicesInfo.have_handle = false;
    devicesInfo.status = false;
    devicesInfo.haveOpenSaneDevice = false;
    devicesInfo.name << "";
    devicesInfo.type << "";
    devicesInfo.color << "";
    devicesInfo.resolution << "";
    devicesInfo.size << "";
}

SaneObject::~SaneObject()
{

}

SaneObject *SaneObject::getInstance()
{
    if (! instance) {
        instance = new SaneObject;
    }
    return instance;
}

bool SaneObject::getSaneStatus()
{
    return devicesInfo.status;
}

bool SaneObject::getSaneHaveOpenDevice()
{
    return devicesInfo.haveOpenSaneDevice;
}

bool SaneObject::getSaneHaveHandle()
{
    return devicesInfo.have_handle;
}

QStringList SaneObject::getSaneNames()
{
    return devicesInfo.name;
}

QStringList SaneObject::getSaneTypes()
{
    return devicesInfo.type;
}

QStringList SaneObject::getSaneResolutions()
{
    return devicesInfo.resolution;

}

int SaneObject::getNumbersFromQString(const QString &qstring)
{
    QString numbers;
    foreach (QChar ch, qstring) {
        if (ch.isDigit()) {
            numbers += ch;
        }
    }
    return numbers.toInt();
}

QStringList SaneObject::getSaneSizes()
{
    return devicesInfo.size;
}

QStringList SaneObject::getSaneColors()
{
    return devicesInfo.color;
}

void SaneObject::setSaneHaveOpenSaneDevice(bool haveOpenSaneDevice)
{
    devicesInfo.haveOpenSaneDevice = haveOpenSaneDevice;
}

void SaneObject::setSaneHaveHandle(bool have_handle)
{
    devicesInfo.have_handle = have_handle;
}

void SaneObject::setSaneStatus(bool status)
{
    devicesInfo.status = status;
}

void SaneObject::setSaneTypes(QStringList type)
{
    devicesInfo.type = type;
}

void SaneObject::setSaneNames(QStringList name)
{
    devicesInfo.name = name;
}

void SaneObject::setSaneResolutions(QStringList resolution)
{
    devicesInfo.resolution = resolution;
}

void SaneObject::setSaneSizes(QStringList size)
{
    devicesInfo.size = size;
}

void SaneObject::setSaneColors(QStringList color)
{
    devicesInfo.color = color;
}

void SaneObject::saneExit()
{
    if (g_sane_object->getSaneHaveHandle()) {
        saneCancel(g_sane_object->handle);
    }

    sane_exit();
}

void SaneObject::saneClose()
{
    if (g_sane_object->getSaneHaveHandle()) {
        saneCancel(g_sane_object->handle);
    }
}

void SaneObject::setSaneAllParametersByUser()
{
    setSaneNameByUser();
    setSanePageNumberByUser();
    setSaneTimeByUser();
    setSaneTypeByUser();
    setSaneColorByUser();
    setSaneResolutionByUser();
    setSaneSizeByUser();
    setSaneFormatByUser();
}

void SaneObject::setSaneNameByUser()
{

}

void SaneObject::setSanePageNumberByUser()
{

}

void SaneObject::setSaneTimeByUser()
{
    // used in ScanThread run(), so we not set hear
}

void SaneObject::setSaneTypeByUser()
{
    SANE_Status status = SANE_STATUS_GOOD;
    string strType;
    SANE_String s_type;
    QMap<QString, QString>::iterator it;

    QString type = getSaneTypeByUser(g_sane_object->userInfo.type);
    if (type == "") {
        // avoid crashed unexpected
        return ;
    }

    it = g_sane_object->sourceModesMap.find(type);
    if (it != g_sane_object->sourceModesMap.end()) {
        // `type` is the key of `sourceModesMap`, we need to get the value of `sourceModesMap`,
        KyInfo() << "sourceModesMap value: " << it.value();
        strType = it.value().toStdString();
    }

    s_type = const_cast<SANE_String>(strType.c_str());

    status = setOptionSources(g_sane_object->handle, g_optDesc.numSource, s_type);
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "Failed to set option color: " << type
                << "status = " << sane_strstatus(status);
    }
}

QString SaneObject::getSaneTypeByUser(QString type)
{
    if (QString::compare(type, tr("Flatbed"), Qt::CaseInsensitive) == 0) {
        return QString("Flatbed");
    } else if (QString::compare(type, tr("ADF"), Qt::CaseInsensitive) == 0) {
        return QString("ADF");
    } else if (QString::compare(type, tr("ADF Front"), Qt::CaseInsensitive) == 0) {
        return QString("ADF Front");
    } else if (QString::compare(type, tr("ADF Back"), Qt::CaseInsensitive) == 0) {
        return QString("ADF Back");
    } else if (QString::compare(type, tr("Default Type"), Qt::CaseInsensitive) == 0) {
        return QString("");
    } else {
        return QString("");
    }
}

void SaneObject::setSaneColorByUser()
{
    SANE_Status status = SANE_STATUS_GOOD;
    string strColor;
    SANE_String s_color;
    QMap<QString, QString>::iterator it;

    QString userColor = g_sane_object->userInfo.color;

    QString color = getSaneColorByUser(userColor);
    KyInfo() << "userColor: " << color;

    it = g_sane_object->colorModesMap.find(color);
    if (it != g_sane_object->colorModesMap.end()) {
        strColor = it.value().toStdString();
    }

    s_color = const_cast<SANE_String>(strColor.c_str());

    status = setOptionColors(g_sane_object->handle, s_color);
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "Failed to set option color: " << color
                << "status = " << sane_strstatus(status);
    }
}

QString SaneObject::getSaneColorByUser(QString color)
{
    if ((QString::compare(color, "Lineart", Qt::CaseInsensitive) == 0)
            || (QString::compare(color, "黑白", Qt::CaseInsensitive) == 0)) {
        return QString("Lineart");
    } else if ((QString::compare(color, "Gray", Qt::CaseInsensitive) == 0)
               || (QString::compare(color, "Gray", Qt::CaseInsensitive) == 0)) {
        return QString("Gray");
    } else if ((QString::compare(color, "Color", Qt::CaseInsensitive) == 0)
               || (QString::compare(color, "彩色", Qt::CaseInsensitive) == 0)) {
        // Cannot match tr("Color"), maybe segment fault
        return QString("Color");
    }
}

void SaneObject::setSaneResolutionByUser()
{
    SANE_Status status = SANE_STATUS_GOOD;
    QString resolution = g_sane_object->userInfo.resolution;
    SANE_Int i_resolution;

    KyInfo() << "resolution = " << resolution;

    resolutionValue = getSaneResolutionByUser(resolution);
    i_resolution = static_cast<SANE_Int>(resolutionValue);

    status = setOptionResolutions(g_sane_object->handle, i_resolution);
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "Failed to set option resolution: " << resolution
                << "status = " << sane_strstatus(status);
    }
}

int SaneObject::getSaneResolutionByUser(QString resolution)
{
    if (QString::compare(resolution, tr("Auto"), Qt::CaseInsensitive) == 0
            || QString::compare(resolution, "自动", Qt::CaseInsensitive) == 0
            || QString::compare(resolution, "Auto", Qt::CaseInsensitive) == 0) {
        return 300;
    } else if (QString::compare(resolution, tr("75 dpi"), Qt::CaseInsensitive) == 0) {
        return 75;
    } else if (QString::compare(resolution, tr("150 dpi"), Qt::CaseInsensitive) == 0) {
        return 150;
    } else if (QString::compare(resolution, tr("200 dpi"), Qt::CaseInsensitive) == 0) {
        return 200;
    } else if (QString::compare(resolution, tr("300 dpi"), Qt::CaseInsensitive) == 0) {
        return 300;
    } else if (QString::compare(resolution, tr("600 dpi"), Qt::CaseInsensitive) == 0) {
        return 600;
    } else if (QString::compare(resolution, tr("1200 dpi"), Qt::CaseInsensitive) == 0) {
        return 1200;
    } else if (QString::compare(resolution, tr("2400 dpi"), Qt::CaseInsensitive) == 0) {
        return 2400;
    } else if (QString::compare(resolution, tr("4800 dpi"), Qt::CaseInsensitive) == 0) {
        return 4800;
    }
}

void SaneObject::setSaneSizeByUser()
{
    SANE_Status status = SANE_STATUS_GOOD;
    string strSize;
    SANE_String s_size;
    int type = 0;

    strSize = g_sane_object->userInfo.size.toStdString();

    s_size = const_cast<SANE_String>(strSize.c_str());
    if (strcmp("A2", s_size) == 0) {
        type = A2;
    } else if (strcmp("A3", s_size) == 0) {
        type = A3;
    } else if (strcmp("A4", s_size) == 0) {
        type = A4;
    } else if (strcmp("A5", s_size) == 0) {
        type = A5;
    } else if (strcmp("A6", s_size) == 0) {
        type = A6;
    }
    status = setOptionSizesAll(g_sane_object->handle, type);
    if (status != SANE_STATUS_GOOD) {
        KyInfo() << "Failed to set option size: " << s_size
                << "status = " << sane_strstatus(status);
    }
}

void SaneObject::setSaneFormatByUser()
{

}

QString SaneObject::getSaneFormatByUser()
{
    QString format = g_sane_object->userInfo.format;

    return format;
}

QString SaneObject::getSaneSaveNameByUser()
{
    QString saveName = g_sane_object->userInfo.saveName;

    return saveName;
}

QString SaneObject::getSaneSaveDirectoryByUser()
{
    QString saveDirectory = g_sane_object->userInfo.saveDirectory;

    return saveDirectory;
}

void SaneObject::detectSaneDeviceForPage()
{
    detectSaneDevices();
}

void SaneObject::openSaneDeviceForPage(int index)
{
    openSaneDevice(index);
}

QString SaneObject::getFullScanFileNameExceptFormat()
{
    QString saveFormat = getSaneFormatByUser();
    QString saveName = getSaneSaveNameByUser();
    QString saveDirectory = getSaneSaveDirectoryByUser();

    QString saveFullName = saveDirectory + "/" + saveName;
    KyInfo() << "saveFullName = " << saveFullName;

    return saveFullName;
}

/**
 * @brief Sane::startScanning
 * Through parameters setted by user, we could do this:
 *      - 1. Set parameters which are setted by user;
 *      - 2. Do scanning actually.
 *
 * @param info parameters setted by user
 * @return scanning result
 */
int SaneObject::startScanning(UserSelectedInfo info)
{
    KyInfo() << "startScanning";

    SANE_Status status = SANE_STATUS_GOOD;

    setSaneAllParametersByUser();

    QString saveFullName = getFullScanFileNameExceptFormat();
    if (g_sane_object->scanPageNumber == 0) {
        loadFullScanFileName = saveFullName + QString(".pnm");
        saveFullScanFileName = saveFullName + QString(".") + g_sane_object->userInfo.format;
    } else {
        QString suffix = QString::number(g_sane_object->scanPageNumber);

        loadFullScanFileName = saveFullName + QString("-") + suffix + QString(".pnm");
        saveFullScanFileName = saveFullName + QString("-") + suffix + QString(".") + g_sane_object->userInfo.format;
    }

    KyInfo() << "loadFullScanFileName = " << loadFullScanFileName
             << "saveFullScanFileName = " << saveFullScanFileName;

    KyInfo() << "Start scanning, please waiting ...";
    status = startSaneScan(g_sane_object->handle, loadFullScanFileName.toStdString().c_str());

    if (g_sane_object->getSaneHaveHandle()) {
        sane_cancel(g_sane_object->handle);
    }

    return status;
}

bool SaneObject::testScannerIsAlive(QString deviceName)
{
    KyInfo() << "deviceName= " << deviceName;
    return true;
}
