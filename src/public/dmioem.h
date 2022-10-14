/*
 *   ----------------------------
 *  |  dmioem.h
 *   ----------------------------
 *   This file is part of BiosReader.
 *
 *   BiosReader is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   BiosReader is distributed in the hope and belief that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with BiosReader.  If not, see <https://www.gnu.org/licenses/>.
 */

struct dmi_header;

void dmi_set_vendor(const char* s, const char* p);
int dmi_decode_oem(const struct dmi_header* h);
