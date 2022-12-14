/*
 * 	    algo0x13.cpp              (C) 2006-2008, Aurélien Croc (AP²C)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *  $Id: algo0x13.cpp 301 2012-02-29 14:11:09Z tillkamppeter $
 * 
 */
#include "algo0x13.h"
#include <string.h>
#include "errlog.h"
#include "request.h"
#include "printer.h"
#include "bandplane.h"

#ifndef DISABLE_JBIG

/*
 * Fonction de rappel
 * Callback
 */
void Algo0x13::_callback(unsigned char *data, size_t len, void *arg)
{
    info_t* info = (info_t *)arg;

    if (!len)
        return;

    // It's the first BIH
    if (!info->last) {
        bandList_t* bandList;
        unsigned char *tmp;

        tmp = new unsigned char[len];
        bandList = new bandList_t;
        memcpy(tmp, data, len);
        bandList->band = new BandPlane();
        bandList->band->setData(tmp, len);
        bandList->band->setEndian(BandPlane::BigEndian);
        bandList->band->setCompression(0x13);
        bandList->next = NULL;
        *(info->list) = bandList;
        info->last = bandList;
        if (len != 20)
            ERRORMSG(_("the first BIG *MUST* be 20 bytes long (currently=%zu"),
                len);
        info->data = NULL;
        info->size = 0;

    // Register the BIH
    } else {
        while (len) {
            unsigned long freeSpace, toCopy;

            // Full band: register it
            if (info->size == info->maxSize) {
                bandList_t* bandList;

                bandList = new bandList_t;
                bandList->band = new BandPlane();
                bandList->band->setData(info->data, info->size);
                bandList->band->setEndian(BandPlane::BigEndian);
                bandList->band->setCompression(0x13);
                bandList->next = NULL;
                info->last->next = bandList;
                info->last = bandList;
                info->data = NULL;
                info->size = 0;
            }

            // Allocate a new data buffer if needed
            if (!info->data)
                info->data = new unsigned char[info->maxSize];

            // Register data
            freeSpace = info->maxSize - info->size;
            toCopy = freeSpace < len ? freeSpace : len;
            memcpy(info->data + info->size, data, toCopy);
            info->size += toCopy;
            data += toCopy;
            len -= toCopy;
        }
    }
}



/*
 * Constructeur - Destructeur
 * Init - Uninit
 */
Algo0x13::Algo0x13()
{
    _compressed = false;
    _list = NULL;
}

Algo0x13::~Algo0x13()
{
}



/*
 * Routine de compression
 * Compression routine
 */
BandPlane* Algo0x13::compress(const Request& request, unsigned char *data, 
        unsigned long width, unsigned long height)
{
    jbg85_enc_state state;
    unsigned long i, wbytes;
    info_t info = {&_list, NULL, NULL, 0, 0};
    BandPlane *plane;
    bandList_t* tmp;

    if (!data || !width || !height) {
        ERRORMSG(_("Invalid given data for compression (0x13)"));
        return NULL;
    }

    // Compress if it's the first time
    if (!_compressed) {
        info.maxSize = request.printer()->packetSize();
        if (!info.maxSize) {
            ERRORMSG(_("PacketSize is set to 0!"));
            info.maxSize = 512*1024;
        }
        wbytes = (width + 7) / 8;
        jbg85_enc_init(&state, width, height, _callback, &info);
        jbg85_enc_options(&state, JBG_LRLTWO | JBG_TPBON, height, 0);
        for (i = 0; i < height; i++) {
            jbg85_enc_lineout(&state,
                              data + i * wbytes,
                              data + (i - 1) * wbytes,
                              data + (i - 2) * wbytes);
        }

        // Register the last band
        if (info.size) {
            bandList_t* bandList;

            bandList = new bandList_t;
            bandList->band = new BandPlane();
            bandList->band->setData(info.data, info.size);
            bandList->band->setEndian(BandPlane::BigEndian);
            bandList->band->setCompression(0x13);
            bandList->next = NULL;
            info.last->next = bandList;
        }
        _compressed = true;
    }

    if (!_list)
        return NULL;
    tmp = _list;
    plane = tmp->band;
    _list = _list->next;
    delete tmp;
    return plane;
}

#endif /* DISABLE_JBIG */

/* vim: set expandtab tabstop=4 shiftwidth=4 smarttab tw=80 cin enc=utf8: */

