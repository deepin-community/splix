/*
 *      pjl.h                   (C) 2007, Aurélien Croc (AP²C)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *   $Id: pjl.h 302 2012-02-29 14:18:29Z tillkamppeter $
 */
#ifndef _PJL_H_
#define _PJL_H_

class QFile;
class QTextStream;

extern bool parsePJLHeader(QFile& qpdlDocument, bool quiet, QTextStream& out, 
    QTextStream& err);

#endif /* _PJL_H_ */

/* vim: set expandtab tabstop=4 shiftwidth=4 smarttab tw=80 cin enc=utf8 : */

