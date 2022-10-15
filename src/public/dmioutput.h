/*
 *   ----------------------------
 *  |  dmioutput.h
 *   ----------------------------
 *   This file is part of BiosReader.
 *
 *   BiosReader is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   BiosReader is distributed in the hope and belief that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with BiosReader.  If not, see <https://www.gnu.org/licenses/>.
 */


#pragma once

#include "dmidecode.h"


/**********************************************************
 * Generic output functions needing careful tending since
 * dealing with output messages on console or applications.
 *
 **********************************************************
 */

void pr_comment(const char *format, ...);
void pr_info(const char *format, ...);
void pr_handle(const struct dmi_header *h);
void pr_handle_name(const char *format, ...);
void pr_attr(const char *name, const char *format, ...);
void pr_subattr(const char *name, const char *format, ...);
void pr_list_start(const char *name, const char *format, ...);
void pr_list_item(const char *format, ...);
void pr_list_end(void);
void pr_sep(void);
void pr_struct_err(const char *format, ...);
